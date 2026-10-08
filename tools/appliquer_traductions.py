"""Remplace les textes chinois des sources par leur traduction (tools/traductions_zh_en.json).

    python -I tools/appliquer_traductions.py          # liste ce qui serait fait
    python -I tools/appliquer_traductions.py apply    # modifie les fichiers

Un texte n'est remplace que si le litteral entier figure dans le dictionnaire : les noms qui servent
de cles (noms d'objets compares entre fichiers, noms d'images) restent donc coherents partout.
Les images de Assets/image/Weapon au nom chinois sont renommees avec la meme table.
Sont laisses tels quels : les commentaires, les lignes qui ont deja une alternative anglaise
(Languages ==, Localize), les journaux, et les fichiers de SKIP_FILES.
"""
import io
import json
import os
import re
import sys

sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8', errors='replace')
here = os.path.dirname(os.path.abspath(__file__))
root = os.path.dirname(here)
apply = len(sys.argv) > 1 and sys.argv[1] == 'apply'
table = json.load(open(os.path.join(here, 'traductions_zh_en.json'), encoding='utf-8'))

ZH = re.compile('[一-鿿　-〿＀-￯]')
LITERAL = re.compile(r'"((?:[^"\\]|\\.)*)"')
WEAPON_ICON = re.compile(r'(Assets/image/Weapon/)([^"/]+)(\.png)')
SKIP_FILES = ('Webpageradar.h', 'kmboxNet', 'MeshPatcher')
SKIP_LINES = ('Languages ==', 'Localize(', 'Utils::Log(')

replaced, left = 0, {}
for dp, dn, fn in os.walk(os.path.join(root, 'Source')):
    for f in fn:
        if not f.endswith(('.h', '.cpp')) or any(s in f for s in SKIP_FILES):
            continue
        path = os.path.join(dp, f)
        # surrogateescape : certains fichiers contiennent des octets invalides en UTF-8, on les conserve
        text = open(path, encoding='utf-8', errors='surrogateescape', newline='').read()
        lines = text.split('\n')
        changed = False
        for i, line in enumerate(lines):
            if line.strip().startswith('//') or any(s in line for s in SKIP_LINES):
                continue
            if 'Assets/' in line:
                new = WEAPON_ICON.sub(lambda m: m.group(1) + table.get(m.group(2), m.group(2)) + m.group(3), line)
            elif f == 'Overlay.cpp':
                continue  # fenetre de connexion, jamais affichee
            else:
                def sub(m):
                    return '"' + table[m.group(1)] + '"' if m.group(1) in table else m.group(0)
                new = LITERAL.sub(sub, line)
                for lit in LITERAL.findall(new):
                    if ZH.search(lit):
                        left.setdefault(lit, set()).add(f)
            if new != line:
                lines[i] = new
                replaced += 1
                changed = True
        if changed and apply:
            open(path, 'w', encoding='utf-8', errors='surrogateescape', newline='').write('\n'.join(lines))

print('lignes %s : %d' % ('modifiees' if apply else 'a modifier', replaced))
print('textes chinois restants (hors dictionnaire) : %d' % len(left))
for lit, files in left.items():
    print('   %s   [%s]' % (lit[:90], ', '.join(sorted(files))))

weapons = os.path.join(root, 'Assets', 'image', 'Weapon')
for name in sorted(os.listdir(weapons)):
    stem, ext = os.path.splitext(name)
    if all(ord(c) < 128 for c in name):
        continue
    if stem not in table:
        print('image sans traduction :', name)
        continue
    target = os.path.join(weapons, table[stem] + ext)
    if os.path.exists(target):
        print('image deja presente, non renommee : %s -> %s' % (name, table[stem] + ext))
        continue
    print('image : %s -> %s' % (name, table[stem] + ext))
    if apply:
        os.rename(os.path.join(weapons, name), target)
