"""Index du dump SDK (SDK_classes.hpp / SDK_structs.hpp).

Usage :
    python -I tools/sdkindex.py <dossier SDK> find <Membre> [...]
    python -I tools/sdkindex.py <dossier SDK> show <Classe> [debut] [fin]
    python -I tools/sdkindex.py <dossier SDK> at <Classe> <offset>
"""
import os
import re
import sys

RE_STRUCT = re.compile(r'^(?:struct|class)\s+(\w+)(?:\s*:\s*(?:public\s+)?(\w+))?\s*$')
RE_MEMBER = re.compile(
    r'^\t(//\s*)?(.+?)\s+(\w+)(\[[^\]]*\])?(\s*:\s*\d+)?;\s*//\s*0x([0-9A-Fa-f]+)\(0x([0-9A-Fa-f]+)\)'
    r'(?:\s*mask\s*0x([0-9A-Fa-f]+))?\s*(.*)$')
RE_OBF = re.compile(r'^_[0-9a-f]{10}$')


class Member:
    __slots__ = ('type', 'name', 'offset', 'size', 'mask', 'flags', 'ghost', 'pad', 'owner')

    def __init__(self, type_, name, offset, size, mask, flags, ghost, owner):
        self.type, self.name, self.offset, self.size = type_, name, offset, size
        self.mask, self.flags, self.ghost, self.owner = mask, flags, ghost, owner
        self.pad = name.startswith('pad_')

    @property
    def obfuscated(self):
        return bool(RE_OBF.match(self.name))

    def __str__(self):
        m = ' mask 0x%02X' % self.mask if self.mask is not None else ''
        g = ' [ghost]' if self.ghost else ''
        return '0x%04X(0x%04X)%s  %s %s%s' % (self.offset, self.size, m, self.type, self.name, g)


class Struct:
    __slots__ = ('name', 'parent', 'members', 'file')

    def __init__(self, name, parent, file):
        self.name, self.parent, self.members, self.file = name, parent, [], file


class SDK:
    def __init__(self, folder):
        self.structs = {}
        self.by_member = {}
        for fn in ('SDK_structs.hpp', 'SDK_classes.hpp'):
            self._parse(os.path.join(folder, fn), fn)
        self.children = {}
        for s in self.structs.values():
            self.children.setdefault(s.parent, []).append(s.name)

    def _parse(self, path, fn):
        cur = None
        with open(path, encoding='utf-8', errors='replace') as f:
            for line in f:
                line = line.rstrip('\r\n')
                if cur is None:
                    m = RE_STRUCT.match(line)
                    if m:
                        cur = Struct(m.group(1), m.group(2), fn)
                    continue
                if line.startswith('};'):
                    self.structs[cur.name] = cur
                    cur = None
                    continue
                m = RE_MEMBER.match(line)
                if not m:
                    continue
                mem = Member(m.group(2).strip(), m.group(3), int(m.group(6), 16), int(m.group(7), 16),
                             int(m.group(8), 16) if m.group(8) else None, m.group(9).strip(),
                             bool(m.group(1)), cur.name)
                cur.members.append(mem)
                if not mem.pad:
                    self.by_member.setdefault(mem.name, []).append(mem)

    def chain(self, name):
        """La classe puis ses ancetres."""
        out = []
        while name and name in self.structs and name not in out:
            out.append(name)
            name = self.structs[name].parent
        return out

    def descendants(self, name):
        out, todo = [], [name]
        while todo:
            n = todo.pop()
            for c in self.children.get(n, []):
                out.append(c)
                todo.append(c)
        return out

    def related(self, a, b):
        return a in self.chain(b) or b in self.chain(a)

    def lookup(self, cls, member):
        """Membre `member` dans `cls` ou un de ses ancetres."""
        for c in self.chain(cls):
            for m in self.structs[c].members:
                if m.name == member and not m.pad:
                    return m
        return None

    def all_members(self, cls):
        out = []
        for c in reversed(self.chain(cls)):
            out.extend(self.structs[c].members)
        return out

    def at(self, cls, offset):
        """Membres de `cls` (ancetres compris) couvrant `offset`."""
        return [m for m in self.all_members(cls) if m.offset <= offset < m.offset + max(m.size, 1)]


def main():
    sdk = SDK(sys.argv[1])
    cmd = sys.argv[2]
    if cmd == 'find':
        for name in sys.argv[3:]:
            for m in sdk.by_member.get(name, []):
                print('%-45s %s' % (m.owner, m))
    elif cmd == 'show':
        lo = int(sys.argv[4], 16) if len(sys.argv) > 4 else 0
        hi = int(sys.argv[5], 16) if len(sys.argv) > 5 else 1 << 62
        print(' > '.join(sdk.chain(sys.argv[3])))
        for m in sdk.all_members(sys.argv[3]):
            if lo <= m.offset <= hi:
                print('%-28s %s' % (m.owner, m))
    elif cmd == 'at':
        for m in sdk.at(sys.argv[3], int(sys.argv[4], 16)):
            print('%-28s %s' % (m.owner, m))
    elif cmd == 'grep':
        rx = re.compile(sys.argv[3], re.I)
        for s in sdk.structs.values():
            if rx.search(s.name):
                print(s.name, ':', s.parent, len(s.members))


if __name__ == '__main__':
    main()
