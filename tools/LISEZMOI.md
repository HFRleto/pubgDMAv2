# Mettre à jour les offsets après une mise à jour du jeu

Ce guide décrit comment tirer le maximum d'offsets **corrects** des deux sources disponibles, puis
comment les vérifier en jeu. Il se termine par l'état des connaissances pour la version 2609.1.3.1.

Aucun outil de ce dossier n'écrit dans la mémoire du jeu ni dans `Offset.h`.

## En bref

1. Vérifier que le dump SDK et le jeu sont de la même version.
2. Lancer `sdk_offsets.py` : il donne les offsets de membres par leur nom.
3. Lancer `dumper.py --image` : il donne les adresses globales, les clés de santé et `CIndex`.
4. Croiser les deux résultats avec la règle de la section 4.
5. Reporter dans `Offset.h`, recompiler, puis vérifier en jeu avec le mode diagnostic.

## 1. Les sources et ce qu'elles valent

| Source | Outil | Ce qu'elle donne | Ce qu'elle ne donne pas |
|---|---|---|---|
| Dump SDK (`SDK_classes.hpp`, `SDK_structs.hpp`) | `sdk_offsets.py`, `sdkindex.py` | offsets de membres, par nom de classe et de membre | adresses globales, clés, membres au nom masqué, zones non décrites |
| Mémoire du jeu, lue par la carte DMA | `dumper.py` | adresses globales (`UWorld`, `GNames`…), clés de santé, formule `CIndex`, quelques offsets de membres | la plupart des membres : beaucoup de signatures sont périmées |
| Listes d'offsets publiées, messages de forum | aucun | des pistes pour les membres au nom masqué | aucune garantie : à vérifier une par une |
| Le jeu lui-même | mode diagnostic du programme | la preuve qu'une valeur est bonne, et le sens d'un champ | rien tant qu'on ne provoque pas la situation en jeu |

Ordre de confiance : **vérifié en jeu** > **SDK par nom** > **SDK et balayage d'accord** > balayage
seul à correspondance unique > valeur déduite ou liste publiée.

Le dump SDK utilisé pour la 2609 est dans `C:\Users\devOne\Downloads\SDKdump\SDK`. Il n'est pas produit
par les outils de ce dossier : il faut en obtenir un nouveau à chaque version du jeu.

## 2. Le SDK (`sdk_offsets.py`)

### Vérifier la version

`TslGame.version.json`, dans le dossier du SDK, donne la version du dump (`FileVersion`). `dumper.py`
affiche celle du jeu en cours (`Process Version`). Si elles diffèrent, les offsets du SDK sont ceux
d'une autre version.

### Lancer

```
python -I tools\sdk_offsets.py C:\chemin\vers\SDKdump\SDK
```

L'outil lit `Source\Common\Offset.h` et `offsetTest.txt`, puis écrit `tools\offsets_report.txt` : une
ligne par clé avec l'ancienne valeur, la nouvelle, la méthode et le détail.

| Méthode | Signification | Confiance |
|---|---|---|
| `SDK` | trouvé dans le SDK par nom, type ou voisinage | bonne |
| `NOMS` | indice de nom lu dans `NamesDump.txt` (`MouseX`, `MouseY`) | bonne |
| `EXTERNE` | pris dans `offsetTest.txt` | à croiser avec le balayage |
| `INFERE` | ancienne valeur plus le décalage d'un voisin | faible, à vérifier en jeu |
| `AUTO` | nom unique dans tout le SDK | faible |
| `INCONNU` | rien trouvé ; le détail donne les candidats quand il y en a | aucune |

### Chercher à la main (`sdkindex.py`)

```
python -I tools\sdkindex.py <SDK> find NomDuMembre          # où ce nom existe-t-il ?
python -I tools\sdkindex.py <SDK> at NomDeClasse 0x1B70     # qu'y a-t-il à cet offset ?
python -I tools\sdkindex.py <SDK> show NomDeClasse 0x400 0x500
```

`find` cherche un nom exact. `at` est la commande la plus utile pour juger une valeur venue d'ailleurs :
si l'offset tombe au milieu d'un autre membre, sur un type incompatible ou dans un `pad_`, la valeur est
fausse ou le membre n'est pas décrit.

### Ajouter une clé au SDK

Quand `at` donne un nom pour une clé jusque-là `INCONNU`, l'ajouter à la table `SPEC` en tête de
`sdk_offsets.py`. Exemple fait pour la 2609 :

```python
'StateBits': 'ATslCharacter::BitContainerNormalValue',
```

La syntaxe des chemins (struct imbriquée, pointeur, recherche par type) est décrite en tête du fichier.

### Limites du SDK

- **Noms masqués.** Un membre affiché `_3ab4eb7b2a` existe, mais le SDK ne dit pas ce qu'il est. Son
  offset ne peut pas être recalculé par nom à la version suivante.
- **Zones non décrites.** Certaines parties de `UWorld`, `ATslCharacterBase` ou `ATslPlayerStateBase`
  apparaissent comme `pad_` : les membres qui s'y trouvent (`GameState`, la santé…) sont invisibles.
- **Structures opaques.** `PubgIdData` est décrit comme un bloc de 0x90 octets sans contenu : les offsets
  à l'intérieur (`SurvivalTier`, `SurvivalLevel`) restent des déductions.
- **Le SDK donne l'offset, pas le sens.** `BitContainerNormalValue` est un entier de 32 bits ; quel bit
  veut dire quoi ne se trouve qu'en jeu.
- **Pointeurs chiffrés.** Un type `TEncryptedObjectPtr<...>` (16 octets) ne se lit pas comme un pointeur
  ordinaire. `ATslCharacter::Team` (0x1E00) en est un : lu tel quel, il renvoie une valeur sans rapport
  avec l'équipe.

## 3. Le balayage mémoire (`dumper.py`)

### Prérequis

- Python 3 en 64 bits sur le PC qui a la carte DMA branchée en USB.
- Le pilote FTDI installé (la carte apparaît comme « FTDI FT601 USB 3.0 Bridge Device »).
- Les deux bibliothèques Python, sans option `-I` au lancement car elles sont installées par utilisateur :

```
python -m pip install memprocfs iced-x86
```

### Lancer

Le jeu doit tourner sur l'autre PC, dans le lobby ou en partie, et **le programme principal doit être
fermé** : la carte n'accepte qu'une connexion à la fois.

```
python -u tools\dumper.py --no-dump --json --image C:\temp\tsl_image.bin -o tools\dumper_out > tools\dumper_out\run.log 2>&1
```

- `--image` : copie d'abord le code du jeu dans un fichier (environ 420 Mo, une minute), puis fait toutes
  les recherches sur cette copie. Le balayage prend alors moins d'une minute au lieu d'un quart d'heure.
  Si le fichier existe déjà, il est réutilisé sans toucher à la carte DMA : on peut donc relancer autant
  de fois que voulu, jeu fermé. Supprimer le fichier (et son `.json`) après une mise à jour du jeu.
- `--no-dump` : ne reconstruit pas l'exécutable du jeu (inutile pour les offsets).
- `--json` : écrit aussi `offsets.json`.
- Sans `--image`, les recherches passent par la carte une à une : plus lent, et sujet aux délais dépassés.

Sorties : `offsets.hpp` (constantes, constantes de déchiffrement, fonctions `CIndexLo` / `CIndexHi`) et
`offsets.json`.

### Lire la sortie

| Ligne | Signification |
|---|---|
| `[*] TslGame.exe PID=… pages lisibles=7/100` | un des deux processus du jeu ; l'outil garde le plus lisible |
| `[+] Nom = 0x…` | valeur trouvée, signature à correspondance unique |
| `[+] Nom = 0x… (valeur figee)` | la signature contient déjà la valeur : **ce `[+]` ne prouve rien** |
| `[+] Nom = 0x… (N correspondances)` | la signature existe à N endroits ; seule la première est lue |
| `[-] Nom MISS` | rien trouvé, ou valeur hors de la plage attendue |

### Pièges connus

- **Deux processus `TslGame.exe`.** Le jeu lance un parent presque vide et le vrai processus. L'outil
  choisit maintenant le plus lisible ; si toutes les signatures échouent d'un coup, c'est le signe que le
  mauvais a été pris.
- **Valeurs figées non signalées.** La mention `valeur figee` n'apparaît que si la signature n'a aucun
  octet libre. Certaines signatures figent l'offset mais laissent un autre octet libre
  (`PreEvalPawnState`, `bIsDBNO_CP`, `LeanActiveFlag_CP`…) : elles ne peuvent renvoyer que l'ancienne
  valeur. En cas de doute, lire la signature dans la table `SIGNATURES` de `dumper.py`.
- **Plages de validation périmées.** Chaque signature a un minimum et un maximum. Quand l'offset réel
  sort de la plage, l'outil affiche `MISS` alors que la signature est peut-être bonne.
- **Valeurs absurdes.** Un résultat à `0x0` (`ItemID`, `DroppedItemGroup`) est une erreur de décodage.
- **Même valeur pour deux clés.** `CurrentLevel` et `RootComponent` sortent tous deux à `0x3E8` : la
  signature de `CurrentLevel` tombe sur le mauvais code.
- **`GroggyHealth`.** La signature renvoie un autre champ (`0x1FA0`). La bonne valeur est
  `ATslCharacter::DBNOHealth` dans le SDK.
- **Blocage au démarrage, sans aucune sortie.** MemProcFS attend le téléchargement des symboles Windows.
  L'outil passe déjà `-disable-symbolserver`.

### Juger une signature trouvée ailleurs

Une signature publiée sur un forum se teste avant d'être adoptée :

1. Elle doit avoir **une seule correspondance** dans le code.
2. Le registre de base de l'instruction doit pointer sur l'objet, pas sur la pile. Si la fonction
   commence par `lea rbp, [rsp-…]` ou si les lignes voisines font `lea rax, [rbp-60h]`, alors `rbp` est
   la pile et la valeur extraite est une position dans la pile.
3. La valeur doit coïncider avec le SDK (`sdkindex.py at`).

Cas réel en 2609 : la signature `48 8B 85 ?? ?? ?? ?? 48 89 44 24 20 C6 85 ?? ?? ?? ?? 01`, publiée
pour le champ d'état `0x1B70`, a bien une correspondance unique et renvoie `0x1B70`. Mais
`[rbp+0x1B70]` y est l'emplacement de pile du premier argument de la fonction : la valeur vient de la
taille de la pile et sa coïncidence avec l'offset du champ est fortuite. Elle n'a pas été retenue.

## 4. Croiser les sources

| SDK | Balayage | Décision |
|---|---|---|
| nommé | même valeur | sûr |
| nommé | autre valeur | le SDK a raison ; la signature est figée, ambiguë ou périmée |
| nommé | `MISS` | prendre le SDK |
| `INFERE` ou `AUTO` | même valeur, correspondance unique | probable, à vérifier en jeu |
| `INCONNU` | valeur à correspondance unique, non figée | probable, à vérifier en jeu |
| `INCONNU` | figée, multiple ou `MISS` | inconnu : chercher ailleurs (section 7) |

Les adresses globales et les clés ne viennent que du balayage. Elles sont fiables quand elles sont
stables d'une exécution à l'autre et que le programme lit ensuite un nom de carte correct.

## 5. Vérifier en jeu

Lancer le programme avec un argument quelconque active le mode diagnostic :

```
x64\Release\KAKA-PRO.exe diag
```

- La console reste affichée. Deux lignes y reviennent toutes les 10 secondes en partie :
  - `[CHAINE]` : chaque pointeur de la chaîne `UWorld` → `PlayerController` → caméra. Un maillon marqué
    `(INVALIDE)` désigne l'offset à corriger.
  - `[ETAT]` : nombre d'entités et de joueurs, nom de la carte, champ de vision. Un nom de carte lisible
    prouve que `GNames`, `ObjID` et `CIndex` sont bons.
- Le fichier `players_debug.log`, à côté de l'exécutable, reçoit une ligne par joueur **chaque fois
  qu'une valeur change** :
  - `[TEAM]` : `LastTeamNum` et `TeamNumber` bruts, équipe retenue, équipe locale, coéquipier ou non.
  - `[STATE]` : champ d'état, drapeaux `bIsDBNO`, `GroggyHealth`, santé, et l'état décidé.
  - `[COLOR]` : type (joueur ou bot), visible ou caché, ce qui fixe la couleur à l'écran.

Le fichier est lisible pendant l'exécution. Pour qu'un outil lise la console, lancer avec la sortie
redirigée dans un fichier.

Méthode : provoquer la situation en jeu, puis regarder quel champ a bougé à cet instant.

| Ce qu'on veut vérifier | Situation à provoquer |
|---|---|
| l'équipe | une partie en duo ou en escouade, près d'un coéquipier |
| « à terre » | un joueur mis à terre : impossible en solo |
| « relevé » | un joueur à terre réanimé |
| « mort » | n'importe quelle élimination, y compris la sienne |
| un nouveau champ | ajouter sa valeur brute à une ligne du journal dans `Players.h` |

Une valeur identique chez tous les joueurs, ou qui ne change jamais, signale un offset faux ou un champ
inutile (cas de `GroggyHealth`, bloqué à 100).

Pour un plantage plutôt qu'un offset faux :

```
powershell -ExecutionPolicy Bypass -File tools\debug_run.ps1 -Seconds 60
```

Le script recompile avec les symboles, lance le programme sous le débogueur Windows (`cdb`), puis
affiche la pile d'appels en cas de plantage. `-NoBuild` saute la compilation.

## 6. Reporter les valeurs dans le programme

Les noms des outils sont ceux des constantes de `Source\Common\Offset.h`.

1. **Modifier la constante** et noter la source en commentaire, comme les lignes existantes
   (`// 2609 : SDK`, `// 2609 : SDK (Classe::Membre), vérifié en jeu`).
2. **Nouvelle clé.** Il faut la constante en haut de `Offset.h` **et** la ligne
   `GameData.Offset["Nom"] = Nom;` dans `Sever_Init()`. Une clé lue sans être enregistrée vaut 0 sans
   aucune erreur.
3. **Santé.** Seul `Health4` est à reporter ; `bEncryptedHealth`, `EncryptedHealthOffset` et
   `DecryptedHealthOffset` en sont calculés (`+0x15`, `+0x14`, `+0x10`). Les 16 `Health_keys` se
   reportent une par une.
4. **Déchiffrement des noms.** Recopier le corps de `CIndexHi` de `offsets.hpp` dans `Decrypt::CIndex`
   (`Source\Hack\Decrypt.cpp`), en gardant le retour à 0 quand la valeur est nulle.
5. **Recompiler**, puis vérifier en jeu les lignes marquées `à vérifier`.

Piège du code : `Utils::ValidPtr` renvoie `true` quand le pointeur est **invalide**. Une lecture gardée
par `if (Utils::ValidPtr(p))` ne se fait donc que sur les mauvais pointeurs.

## 7. État des connaissances en 2609.1.3.1

Résultat des outils : SDK 95 clés par nom, 14 déduites, 45 inconnues ; balayage 87 signatures sur 139.

### Vérifié en jeu

| Clé | Valeur | Source | Constat |
|---|---|---|---|
| `LastTeamNum` | `0x1E10` | SDK + balayage | équipe correcte, vaut l'équipe + 100000 |
| `TeamNumber` | `0xB80` | SDK | identique à `LastTeamNum` à chaque lecture |
| `PlayerState` | `0x428` | SDK | pointeur chiffré, valide une fois déchiffré |
| `StateBits` | `0x1B70` | SDK | bit 0 = à terre, bit 1 = mort ; les bits 3 et 5 existent, sens inconnu |
| `bIsDBNO1` / `bIsDBNO2` | `0x3588` / `0x3589` | liste publiée | up/alive : 1/1 debout, 0/1 à terre, 0/0 mort |
| `CharacterName` | `0x16E0` | SDK + balayage | noms lisibles |
| chaîne `UWorld` → caméra | voir `Offset.h` | SDK + balayage | tous les maillons valides sauf `GameState` |

### Champs lus mais inutilisables

| Clé | Valeur | Constat |
|---|---|---|
| `GroggyHealth` (`DBNOHealth`) | `0x18C0` | reste à 100, joueur debout, à terre ou mort |
| drapeau `0xAD8` de `ATslPlayerStateBase` | nom masqué | allumé sur des joueurs vivants, éteint sur des morts : ce n'est pas « tué » |
| `ATslCharacter::Team` | `0x1E00` | pointeur chiffré, illisible tel quel |
| `Health` | `0x960` | affiche 100 pour tout le monde |

### Concordant entre SDK et balayage, non observé en jeu

`AcknowledgedPawn`, `PlayerCameraManager`, les trois `CameraCache…`, `PlayerName`, `SpectatedCount`,
`BlockInputWidgetList`, `WidgetStateMap`, `FeatureRepObject`, `SafetyZoneRadius`, `Mesh`, `StaticMesh`,
`RootComponent`, `ComponentVelocity`, `bAlwaysCreatePhysicsState`, `AnimScriptInstance`,
`VehicleRiderComponent`, `ReplicatedMovement`, `SeatIndex`, `VehicleCommonComponent`, `WeaponProcessor`,
`EquippedWeapons`, `TrajectoryConfig`, `InventoryFacade`, `ControlRotation_CP`, `LeanLeftAlpha_CP`,
`LeanRightAlpha_CP`, `Gender`.

### Désaccords : le SDK a raison

| Clé | SDK | Balayage | Cause |
|---|---|---|---|
| `GroggyHealth` | `0x18C0` | `0x1FA0` | signature sur un autre champ |
| `CurrentLevel` | `0x8D0` | `0x3E8` | 5 correspondances, mauvaise prise |
| `MyHUD` | `0x4C8` | `0x4D0` | valeur figée |
| `CharacterMovement` | `0x598` | `0x720` | valeur figée |
| `PreEvalPawnState` | `0x658` | `0x638` | valeur figée |
| `VehicleHealth` | `0x2F0` | `0x2D8` | valeur figée |
| `ScopingAttachPoint` | `0xED8` | `0xB28` | valeur figée |
| `InventoryItems` | `0x600` | `0x690` | valeur figée |

### Ce qu'aucune source ne donne

- **`GameState`** : seul maillon de la chaîne signalé invalide en jeu. La valeur `0x278` vient d'une
  signature figée, et le SDK ne décrit pas cette zone de `UWorld`. Pistes non testées, tirées du SDK :
  `UWorld::LevelCollections` (`0x788`) puis `FLevelCollection::GameState` (`+0x8`), ou
  `UTslAnimInstance::GameState` (`0xB88`) depuis l'animation du joueur local.
- **Santé** : le balayage trouve `Health4` (`0x960`) et les 16 clés, mais la valeur lue reste à 100 pour
  tout le monde ; la cause n'a pas été cherchée. La zone est non décrite dans le SDK, `HealthMax`
  (`0xA50`) y est un `float` au nom masqué.
- **Déduits, jamais vérifiés** : `SurvivalTier`, `SurvivalLevel`, `PlayerStatusType`, `PlayerStatistics`,
  `DampingRate`, `ShapeRadius`, `CurrentAmmoData`, `FiringAttachPoint`, `DroppedItemGroup`,
  `RecoilADSRotation_CP`, `WeaponAttachmentData`.
- **Ni SDK ni balayage** : `SelectMinimapSizeIndex`, `ItemID`, `DroppedItemGroupUItem`,
  `AttachedStaticComponentMap`, `ElapsedCookingTime`, `TimeSeconds`, `ExplodeState`, `MortarLocation`,
  `MortarRotation`, `MapGrid_Map`, `InputAxisProperties`.

Pour ces derniers, il faut soit retrouver le nom du membre dans le SDK (`sdkindex.py find`, puis ajouter
la clé à `SPEC`), soit corriger la signature du balayage à partir du code de la version en cours.
