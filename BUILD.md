# Compiler KAKA-PRO

Ce document décrit la compilation du projet tel qu'il est dans ce dépôt. Le `README.md` d'origine
(en chinois) décrit une version antérieure et n'est plus à jour sur plusieurs points.

## Prérequis

- Windows 10 ou 11, 64 bits.
- Visual Studio 2022 ou « Build Tools pour Visual Studio 2022 », avec la charge de travail
  **Développement Desktop en C++** :
  - jeu d'outils MSVC **v143** (testé avec 14.44) ;
  - SDK Windows 10/11 (testé avec 10.0.26100).

Aucune dépendance n'est à télécharger : toutes les bibliothèques sont dans le dépôt
(`Include\`, `lib\`, `freetype\`, `Runtime\x64\`, `ThirdParty\`).

## Compilation

Seule la configuration **Release | x64** est prévue.

En ligne de commande (PowerShell), depuis n'importe quel dossier :

```powershell
& "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\MSBuild\Current\Bin\MSBuild.exe" "<chemin du dépôt>\Jax Kane.sln" /p:Configuration=Release /p:Platform=x64 /m
```

- Le chemin de `MSBuild.exe` ci-dessus est celui des Build Tools. Avec Visual Studio Community,
  remplacer `BuildTools` par `Community` et `Program Files (x86)` par `Program Files`.
- `/m` compile sur plusieurs cœurs. Il est facultatif.
- Sans `/p:Configuration=Release /p:Platform=x64`, MSBuild choisit Debug, qui ne copie pas les
  fichiers nécessaires à l'exécution.

Depuis Visual Studio : ouvrir `Jax Kane.sln`, choisir **Release** et **x64**, puis
*Générer > Générer la solution*.

Une compilation complète prend environ 20 à 30 secondes sur une machine récente.
Un avertissement `C4244` (conversion `wchar_t` vers `char`) est attendu et sans conséquence.

## Résultat

Tout est produit dans `x64\Release\` :

| Élément | Origine |
|---|---|
| `KAKA-PRO.exe` | Compilation |
| `freetype.dll` | Copié depuis `freetype\win64\` |
| `vmm.dll`, `leechcore.dll`, `FTD3XX.dll`, `embree4.dll`, `tbb12.dll`, `zlib1.dll`, `zstd.dll`, `boost_random-vc143-mt-x64-1_86.dll` | Copiés depuis `Runtime\x64\` |
| `D3DCompiler_43.dll`, `d3dx9_43.dll`, `d3dx10_43.dll`, `d3dx11_43.dll` | Copiés depuis `ThirdParty\Microsoft.DXSDK.D3DX\` |
| `Assets\` | Copie de `Assets\` |
| `config\` | Copie de `config\` |
| `obj\`, `KAKA-PRO.lib`, `KAKA-PRO.exp` | Fichiers intermédiaires, inutiles à l'exécution |

Le dossier `x64\Release\` ne contient rien d'autre que des fichiers régénérables : il peut être
supprimé entièrement, la compilation le recrée.

## Recompiler depuis zéro

Supprimer `x64\Release\` puis relancer la commande de compilation.

Attention : si le programme a déjà été lancé depuis ce dossier, il y a écrit ses réglages
(`config\dadaConfig.bak`, `config\BlackLists.txt`, `config\WhiteLists.txt`). Les copier ailleurs
avant de supprimer le dossier.

## Déplacer le programme

Pour l'utiliser depuis un autre dossier ou un autre PC, copier depuis `x64\Release\` :

- `KAKA-PRO.exe` et les 13 DLL ;
- le dossier `Assets\` (icônes ; sans lui le programme tourne mais sans images) ;
- le dossier `config\` (le programme ne le crée pas lui-même : sans lui, les réglages ne sont
  pas enregistrés).

Sur la machine cible, il faut aussi :

- le **Redistribuable Microsoft Visual C++ 2015-2022 (x64)**, requis par plusieurs DLL ;
- le pilote de la carte DMA.

Le programme lit et écrit ses fichiers par rapport au **dossier de travail**. Le lancer par
double-clic, ou depuis un raccourci dont le champ « Démarrer dans » pointe sur son dossier.
