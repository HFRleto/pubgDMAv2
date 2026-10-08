# Mettre à jour les offsets après une mise à jour du jeu

Deux outils dans ce dossier, qui se complètent :

| Outil | Source | Ce qu'il donne |
|---|---|---|
| `dumper.py` | la mémoire du jeu, lue par la carte DMA | adresses globales, clés de santé, formule `CIndex`, une partie des offsets de membres |
| `sdk_offsets.py` | un dump SDK (`SDK_classes.hpp`, `SDK_structs.hpp`) | les offsets de membres, par nom de classe et de membre |

Aucun des deux n'écrit dans la mémoire du jeu ni dans `Offset.h`.

## 1. Le script DMA (`dumper.py`)

### Prérequis

- Python 3 en 64 bits sur le PC qui a la carte DMA branchée en USB.
- Le pilote FTDI installé (la carte apparaît comme « FTDI FT601 USB 3.0 Bridge Device »).
- Les deux bibliothèques Python :

```
python -m pip install memprocfs iced-x86
```

Aucune DLL à copier : `pip` installe `vmm.dll`, `leechcore.dll` et `FTD3XX.dll` avec les paquets.

### Lancer

Le jeu doit être lancé sur l'autre PC, de préférence dans le lobby ou en partie.

```
python tools\dumper.py --no-dump --json -o tools\dumper_out
```

- `--no-dump` : ne copie pas l'exécutable du jeu (long et inutile pour les offsets).
- `--json` : écrit aussi `offsets.json`.
- `-o` : dossier de sortie.

Le balayage dure environ 15 minutes. Pour suivre la sortie pendant ce temps, redirigez-la dans un fichier :

```
python -u tools\dumper.py --no-dump --json -o tools\dumper_out > tools\dumper_out\run.log 2>&1
```

Ne lancez pas deux exécutions en même temps, ni le script en même temps que le programme principal : une seule connexion à la carte à la fois.

### Résultats

Dans le dossier de sortie :

- `offsets.hpp` : les constantes trouvées, les constantes de déchiffrement et les fonctions `CIndexLo` / `CIndexHi`.
- `offsets.json` : les mêmes valeurs, plus faciles à comparer par script.

Dans la sortie console, `[+] Nom = 0x...` est une valeur trouvée et `[-] Nom MISS` une signature qui n'a rien donné.

### Pièges connus

- **Blocage au démarrage, sans aucune sortie.** MemProcFS attend le téléchargement des symboles Windows. Cette copie du script passe déjà `-disable-symbolserver` (ligne 82) ; si vous repartez du script d'origine, il faut le rajouter.
- **Signatures à valeur figée.** 27 signatures contiennent l'offset en dur (par exemple `48 8B 8B 78 02 00 00` pour `GameState`). Elles ne peuvent renvoyer que cette valeur : un `[+]` ne prouve rien. Sont concernées notamment `GameState`, `ViewTarget`, `MyHUD`, `CharacterMovement`, `ScopingAttachPoint`, `PreEvalPawnState`, `VehicleHealth`, `InventoryItems`, `TrajectoryGravityZ`, `PlayerName`.
- **Plages de validation périmées.** Chaque signature a un minimum et un maximum. Quand l'offset réel sort de la plage, le script affiche `MISS` alors que la signature est peut-être bonne.
- **Délai de 5 secondes.** `Search timeout` suivi de `MISS` signifie que la recherche n'a pas eu le temps de finir, pas que la signature est fausse (`SEARCH_TIMEOUT`, ligne 72).
- **Valeurs absurdes.** Un résultat à `0x0` (vu sur `ItemID` et `DroppedItemGroup`) est une erreur de décodage.
- **`GroggyHealth`.** La signature renvoie l'offset de `BlueBlockerGaugeTotalMax`. La bonne valeur est celle de `DBNOHealth` dans le SDK.

## 2. L'outil SDK (`sdk_offsets.py`)

```
python -I tools\sdk_offsets.py C:\chemin\vers\SDKdump\SDK
```

Il lit `Source\Common\Offset.h` et `offsetTest.txt`, puis écrit `tools\offsets_report.txt` : une ligne par clé avec l'ancienne valeur, la nouvelle, la méthode et le détail.

| Méthode | Signification |
|---|---|
| `SDK` | trouvé dans le SDK par nom, type ou voisinage |
| `NOMS` | indice de nom lu dans `NamesDump.txt` (`MouseX`, `MouseY`) |
| `EXTERNE` | pris dans `offsetTest.txt` |
| `INFERE` | ancienne valeur plus le décalage d'un voisin, à vérifier |
| `AUTO` | nom unique dans tout le SDK, peu fiable |
| `INCONNU` | rien trouvé ; le détail donne les candidats quand il y en a |

La table `SPEC` en tête du fichier dit où chercher chaque clé. Quand une clé passe en `INCONNU` après une mise à jour, c'est en général qu'un membre a été renommé : `sdkindex.py` aide à le retrouver.

```
python -I tools\sdkindex.py <SDK> find NomDuMembre
python -I tools\sdkindex.py <SDK> show NomDeClasse 0x400 0x500
python -I tools\sdkindex.py <SDK> at NomDeClasse 0x960
```

## 3. Reporter les valeurs dans le programme

Les noms du script sont les mêmes que ceux des constantes de `Source\Common\Offset.h`.

1. **Croiser les sources.** Une valeur donnée à l'identique par le SDK et par le script est sûre. En cas de désaccord, le SDK a raison quand il nomme le membre.
2. **Vérifier une valeur douteuse** avec `sdkindex.py at Classe 0xValeur` : si l'offset tombe au milieu d'un autre membre ou sur un membre d'un autre type, elle est fausse.
3. **Modifier la constante** dans `Offset.h` et noter la source en commentaire, comme les lignes existantes (`// 2609 : SDK`, `// 2609 : script, à vérifier`).
4. **Santé.** Seul `Health4` est à reporter ; `bEncryptedHealth`, `EncryptedHealthOffset` et `DecryptedHealthOffset` en sont calculés (`+0x15`, `+0x14`, `+0x10`). Les 16 `Health_keys` se reportent une par une.
5. **Déchiffrement des noms.** Recopier le corps de `CIndexHi` de `offsets.hpp` dans `Decrypt::CIndex` (`Source\Hack\Decrypt.cpp`), en gardant le retour à 0 quand la valeur est nulle.
6. **Nouvelle clé.** Il faut la constante en haut de `Offset.h` et la ligne `GameData.Offset["Nom"] = Nom;` dans `Sever_Init()`. Une clé lue sans être enregistrée vaut 0 sans aucune erreur.
7. **Recompiler**, puis tester en jeu les lignes marquées `à vérifier`.

## 4. Diagnostiquer un plantage ou un offset faux

```
powershell -ExecutionPolicy Bypass -File tools\debug_run.ps1 -Seconds 60
```

Le script recompile avec les symboles, lance le programme sous le débogueur Windows (`cdb`), puis affiche la pile d'appels en cas de plantage et la sortie console. `-NoBuild` saute la compilation.

Dans la console du programme, deux lignes reviennent toutes les 10 secondes en partie :

- `[CHAINE]` : la valeur de chaque pointeur de la chaîne `UWorld` → `PlayerController` → caméra. Un maillon marqué `(INVALIDE)` désigne l'offset à corriger dans `Offset.h`.
- `[ETAT]` : nombre d'entités et de joueurs lus, nom de la carte et champ de vision de la caméra. Une carte au nom lisible prouve que `GNames`, `ObjID` et `CIndex` sont bons.

## Ce qu'aucune source ne donne aujourd'hui

`PlayerStatusType`, `PlayerStatistics`, `SelectMinimapSizeIndex`, `ItemID`, `DroppedItemGroup`, `DroppedItemGroupUItem`, `CurrentAmmoData`, `FiringAttachPoint`, `AttachedStaticComponentMap`, `WeaponAttachmentData`, `ElapsedCookingTime`, `TimeSeconds`, `ExplodeState`, `MortarLocation`, `MortarRotation`, `MapGrid_Map`, `DormantCharacterClientList`.

Pour ceux-là il faut corriger les signatures du script (motif, plage ou valeur figée) à partir du binaire de la version en cours.
