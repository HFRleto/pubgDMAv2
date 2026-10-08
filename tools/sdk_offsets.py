"""Recalcule les offsets de Source/Common/Offset.h a partir d'un dump SDK.

Usage :
    python -I tools/sdk_offsets.py <dossier SDK> [Offset.h] [offsetTest.txt] [rapport.txt]

Pour chaque cle de la map GameData.Offset, on tente dans l'ordre :
  1. SDK     : chemin explicite de SPEC resolu dans le SDK (nom, type ou voisinage)
  2. AUTO    : nom de membre unique dans tout le SDK
     NOMS    : indice FName lu dans NamesDump.txt
  3. EXTERNE : valeur du fichier offsetTest.txt (globales, membres non reflechis)
  4. INFERE  : ancienne valeur + decalage des offsets voisins deja resolus dans la meme classe
Le SDK ne contient ni les adresses globales ni les cles de dechiffrement.

Syntaxe des chemins de SPEC :  Proprietaire::etape[.etape | ->etape]...
  Proprietaire : Classe | @chemin (type pointe par un membre) | * (toutes les classes)
  etape        : Nom|Alias   membre par nom
                 <regex>     membre par type
                 ?Nom        membre dont le type (struct) contient un membre Nom
     suffixes  : #n (n-ieme correspondance), ~+n / ~-n (n-ieme voisin), +0xN (constante)
  '.' entre dans une struct imbriquee (offsets cumules), '->' suit un pointeur (offset remis a zero)
"""
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from sdkindex import SDK  # noqa: E402

# cle -> chemin SDK
SPEC = {
    # moteur
    'CurrentLevel': 'UWorld::CurrentLevel',
    'ObjID': 'UObject::NameIndex_enc',
    'AcknowledgedPawn': 'APlayerController::AcknowledgedPawn',
    'PlayerCameraManager': 'APlayerController::PlayerCameraManager',
    'MyHUD': 'APlayerController::MyHUD',
    'PlayerInput': 'APlayerController::PlayerInput',
    'bShowMouseCursor': 'APlayerController::bShowMouseCursor',
    'PlayerArray': 'AGameStateBase::PlayerArray',
    'NumAliveTeams': 'ATslGameStateBase::NumAliveTeams',
    'FeatureRepObject': r'ATslGameStateBase::<^TArray<UTslModeFeatureRepObject\*>$>',
    'SafetyZonePosition': '*::SafetyZonePosition/UTslModeFeatureRepObject',
    'SafetyZoneRadius': '*::SafetyZoneRadius/UTslModeFeatureRepObject',
    'BlueZoneRadius': '*::BlueZoneRadius|BluezoneRadius/UTslModeFeatureRepObject',
    'BlueZonePosition': '*::BlueZoneRadius|BluezoneRadius~-1/UTslModeFeatureRepObject',
    # camera
    'ViewTarget': 'APlayerCameraManager::ViewTarget',
    'CameraCacheRotation': 'APlayerCameraManager::?Timestamp#0.?FOV.Rotation',
    'CameraCacheFOV': 'APlayerCameraManager::?Timestamp#0.?FOV.FOV',
    'CameraCacheLocation': 'APlayerCameraManager::?Timestamp#0.?FOV.Location',
    # personnage
    'RootComponent': 'AActor::RootComponent',
    'ReplicatedMovement': 'AActor::ReplicatedMovement',
    'PlayerState': 'APawn::PlayerState',
    'Mesh': 'ACharacter::Mesh',
    'CharacterMovement': 'ACharacter::CharacterMovement',
    'WeaponProcessor': 'ATslCharacter::WeaponProcessor',
    'Gender': 'ATslCharacter::Gender',
    'LastTeamNum': 'ATslCharacter::LastTeamNum',
    'CharacterName': 'ATslCharacter::CharacterName',
    'CharacterState': 'ATslCharacter::CharacterState',
    'SpectatedCount': 'ATslCharacter::SpectatedCount',
    'AimOffsets': 'ATslCharacter::AimOffsets',
    'InventoryFacade': 'ATslCharacter::InventoryFacade',
    'VehicleRiderComponent': 'ATslCharacter::VehicleRiderComponent',
    'GroggyHealth': 'ATslCharacter::GroggyHealth|DBNOHealth',
    # etat du joueur
    'PlayerName': 'APlayerState::PlayerName',
    'AccountId': 'ATslPlayerStateBase::AccountId',
    'TeamNumber': 'ATslPlayerStateBase::TeamNumber',
    'SquadMemberIndex': 'ATslPlayerStateBase::SquadMemberIndex',
    'DamageDealtOnEnemy': 'ATslPlayerStateBase::DamageDealtOnEnemy',
    'PartnerLevel': 'ATslPlayerStateBase::PartnerLevel',
    'CharacterClanInfo': 'ATslPlayerStateBase::CharacterClanInfo|ClanInfo',
    'PubgIdData': 'ATslPlayerStateBase::PubgIdData',
    # composants
    'ComponentVelocity': 'USceneComponent::ComponentVelocity',
    'bAlwaysCreatePhysicsState': 'UPrimitiveComponent::bAlwaysCreatePhysicsState',
    'StaticMesh': 'UStaticMeshComponent::StaticMesh',
    'SkeletalMesh': 'USkinnedMeshComponent::SkeletalMesh',
    'Skeleton': 'USkeletalMesh::Skeleton',
    'SkeletalSockets': 'USkeleton::Sockets',
    'SkeletalSocketName': 'USkeletalMeshSocket::SocketName',
    'AnimScriptInstance': 'USkeletalMeshComponent::AnimScriptInstance',
    'StaticSockets': 'UStaticMesh::Sockets',
    'StaticSocketName': '@UStaticMesh::Sockets::SocketName',
    'StaticRelativeLocation': '@UStaticMesh::Sockets::RelativeLocation',
    'StaticRelativeRotation': '@UStaticMesh::Sockets::RelativeRotation',
    'StaticRelativeScale': '@UStaticMesh::Sockets::RelativeScale',
    # interface
    'Slot': 'UWidget::Slot',
    'Visibility': 'UWidget::Visibility',
    'LayoutData': 'UCanvasPanelSlot::LayoutData',
    'Offsets': 'FAnchorData::Offsets',
    'Alignment': 'FAnchorData::Alignment',
    'WidgetStateMap': r'ATslBaseHUD::<^TMap<FString, F_\w+>$>',
    'BlockInputWidgetList': r'ATslBaseHUD::<^TArray<UBlockInputUserWidget\*>$>',
    # animation
    'PreEvalPawnState': 'UTslAnimInstance::PreEvalPawnState',
    'ControlRotation_CP': 'UTslAnimInstance::ControlRotation_CP',
    'LeanLeftAlpha_CP': 'UTslAnimInstance::LeanLeftAlpha_CP',
    'LeanRightAlpha_CP': 'UTslAnimInstance::LeanRightAlpha_CP',
    'bIsScoping_CP': 'UTslAnimInstance::bIsScoping_CP',
    'bIsReloading_CP': 'UTslAnimInstance::bIsReloading_CP',
    # vehicules
    'VehicleMovement': 'AWheeledVehicle::VehicleMovement',
    'Wheels': 'UWheeledVehicleMovementComponent::Wheels',
    'WheelLocation': 'UVehicleWheel::Location',
    'WheelOldLocation': 'UVehicleWheel::OldLocation',
    'WheelVelocity': 'UVehicleWheel::Velocity',
    'SeatIndex': 'UVehicleRiderComponent::SeatIndex',
    'LastVehiclePawn': r'UVehicleRiderComponent::<^APawn\*$>',
    'VehicleCommonComponent': 'ATslWheeledVehicle::VehicleCommonComponent',
    'FloatingComponent': 'ATslFloatingVehicle::VehicleCommonComponent',
    'VehicleHealth': '@ATslWheeledVehicle::VehicleCommonComponent::Health',
    'VehicleHealthMax': '@ATslWheeledVehicle::VehicleCommonComponent::Health~+1',
    'VehicleFuel': '@ATslWheeledVehicle::VehicleCommonComponent::Fuel',
    'VehicleFuelMax': '@ATslWheeledVehicle::VehicleCommonComponent::Fuel~+1',
    # objets / inventaire
    'DroppedItem': 'ADroppedItem::Item',
    'ItemPackageItems': 'AItemPackage::Items',
    'Inventory': 'AInventoryFacade::Inventory',
    'InventoryItems': 'AInventory::Items',
    'InventoryItemTagItemCount': 'UItem::StackCount',
    # armes
    'EquippedWeapons': r'@ATslCharacter::WeaponProcessor::<^TArray<ATslWeapon\*>$>',
    'AttachedItems': 'ATslWeapon::AttachedItems',
    'ScopingAttachPoint': 'ATslWeapon_Gun::ScopingAttachPoint',
    'WeaponTrajectoryData': 'ATslWeapon_Trajectory::WeaponTrajectoryData',
    'TrajectoryConfig': 'UWeaponTrajectoryData::?InitialSpeed',
    'BallisticCurve': '@UWeaponTrajectoryData::?InitialSpeed::RangeModifier~-1',
    'FloatCurves': 'UCurveVector::FloatCurves',
    'WeaponConfig_WeaponClass': r'ATslWeapon::WeaponConfig.<^EWeaponClass$>#0',
    'Mesh3P': r'ATslWeapon::<^U(Skeletal)?MeshComponent\*$>',
    'AttachedStaticComponentMap': r'*::<^TMap<.*EWeaponAttachmentSlotID.*, U\w*Component\*>$>',
    # projectiles
    'TimeTillExplosion': 'ATslProjectile::TimeTillExplosion',
    'ExplodeState': r'*::<^EProjectileExplodeState$>',
}

# cle -> classe proprietaire, pour l'inference par decalage quand le SDK ne nomme pas le membre
INFER = {
    'GameInstance': 'UWorld', 'GameState': 'UWorld', 'TimeSeconds': 'UWorld', 'WorldToMap': 'UWorld',
    'LocalPlayer': 'UGameInstance',
    'AntiCheatCharacterSyncManager': 'ATslPlayerController',
    'Health': 'ATslCharacter', 'bEncryptedHealth': 'ATslCharacter',
    'EncryptedHealthOffset': 'ATslCharacter', 'DecryptedHealthOffset': 'ATslCharacter',
    'PlayerStatistics': 'ATslPlayerStateBase', 'PlayerStatusType': 'ATslPlayerStateBase',
    'SurvivalTier': 'ATslPlayerStateBase', 'SurvivalLevel': 'ATslPlayerStateBase',
    'LastUpdateVelocity': 'UCharacterMovementComponent',
    'Eyes': 'USkeletalMeshComponent',
    'InputAxisProperties': 'UPlayerInput',
    'RecoilADSRotation_CP': 'UTslAnimInstance',
    'DampingRate': 'UVehicleWheel', 'ShapeRadius': 'UVehicleWheel',
    'ItemTable': 'UItem',
    'DroppedItemGroup': 'ADroppedItemGroup',
    'CurrentWeaponIndex': '@ATslCharacter::WeaponProcessor',
    'FiringAttachPoint': 'ATslWeapon_Trajectory', 'CurrentAmmoData': 'ATslWeapon_Trajectory',
    'TrajectoryGravityZ': 'ATslWeapon_Trajectory',
}

# nom dans offsetTest.txt -> cle de la map
EXTERNAL = {
    'uworld': 'UWorld', 'decrypt': 'XenuineDecrypt', 'gnames': 'GNames', 'gname_second': 'GNamesPtr',
    'chunk_size': 'ChunkSize', 'actor_id': 'ObjID', 'actor_array': 'Actors',
    'persistent_level': 'CurrentLevel', 'controller': 'PlayerController', 'pawn': 'AcknowledgedPawn',
    'camera_manager': 'PlayerCameraManager', 'camera_fov': 'CameraCacheFOV',
    'camera_rot': 'CameraCacheRotation', 'camera_loc': 'CameraCacheLocation',
    'mesh': 'Mesh', 'last_team_num': 'LastTeamNum', 'static_mesh': 'StaticMesh',
    'skeletal_mesh': 'SkeletalMesh', 'groggy_health': 'GroggyHealth', 'gender': 'Gender',
    'root_component': 'RootComponent', 'component_to_world': 'ComponentToWorld',
    'absolute_location': 'ComponentLocation', 'character_name': 'CharacterName',
    'spectator_count': 'SpectatedCount', 'always_create_physics_state': 'bAlwaysCreatePhysicsState',
}


# cle -> nom dont on veut l'indice FName (lu dans NamesDump.txt)
NAME_INDEX = {'MouseX': 'MouseX', 'MouseY': 'MouseY'}


def read_name_indices(path, wanted):
    out = {}
    if os.path.exists(path):
        with open(path, encoding='utf-8', errors='replace') as f:
            for line in f:
                m = re.match(r'\[(\d+)\] (.+)$', line.strip())
                if m and m.group(2) in wanted and m.group(2) not in out:
                    out[m.group(2)] = int(m.group(1))
                    if len(out) == len(wanted):
                        break
    return out


class Unresolved(Exception):
    pass


def pointee(type_):
    """Classe/struct designee par un type de membre."""
    ptrs = re.findall(r'(\w+)\s*\*', type_)
    if ptrs:
        return ptrs[-1]
    inner = re.findall(r'<\s*(\w+)\s*>', type_)
    return inner[-1] if inner else type_.strip()


def split_path(path):
    """Decoupe 'a.b->c' en [('', 'a'), ('.', 'b'), ('->', 'c')] sans couper dans <...>."""
    out, cur, sep, depth, i = [], '', '', 0, 0
    while i < len(path):
        ch = path[i]
        if ch == '<':
            depth += 1
        elif ch == '>' and depth:
            depth -= 1
        if depth == 0 and ch == '.':
            out.append((sep, cur))
            cur, sep = '', '.'
        elif depth == 0 and path.startswith('->', i):
            out.append((sep, cur))
            cur, sep = '', '->'
            i += 1
        else:
            cur += ch
        i += 1
    out.append((sep, cur))
    return out


class Resolver:
    def __init__(self, sdk):
        self.sdk = sdk

    def pick(self, struct, step):
        """Membre de `struct` (ancetres compris) designe par `step`, plus constante a ajouter."""
        add = 0
        m = re.search(r'\+0x([0-9A-Fa-f]+)$', step)
        if m:
            add, step = int(m.group(1), 16), step[:m.start()]
        near = 0
        m = re.search(r'~([+-]\d+)$', step)
        if m:
            near, step = int(m.group(1)), step[:m.start()]
        index = None
        m = re.search(r'#(\d+)$', step)
        if m:
            index, step = int(m.group(1)), step[:m.start()]
        members = [x for x in self.sdk.all_members(struct) if not x.pad]
        if step.startswith('<') and step.endswith('>'):
            rx = re.compile(step[1:-1])
            found = [x for x in members if rx.search(x.type)]
        elif step.startswith('?'):
            found = [x for x in members
                     if any(y.name == step[1:] for y in self.sdk.all_members(pointee(x.type)))]
        else:
            names = step.split('|')
            found = [x for x in members if x.name in names]
        if not found:
            raise Unresolved('%s introuvable dans %s' % (step, struct))
        if index is None and len(found) > 1:
            raise Unresolved('%s ambigu dans %s : %s' % (
                step, struct, ', '.join('0x%X' % x.offset for x in found[:8])))
        member = found[index or 0]
        if near:
            i = members.index(member) + near
            if not 0 <= i < len(members):
                raise Unresolved('pas de voisin %+d pour %s' % (near, step))
            member = members[i]
        return member, add

    def owner(self, spec):
        if spec.startswith('@'):
            return pointee(self.resolve(spec[1:])[1].type)
        if spec not in self.sdk.structs:
            raise Unresolved('classe %s absente du SDK' % spec)
        return spec

    def resolve(self, path):
        """-> (offset, dernier membre, description)."""
        own, _, rest = path.rpartition('::')
        if own == '*':
            return self.resolve_any(rest)
        struct = self.owner(own)
        offset, member, desc = 0, None, []
        for sep, step in split_path(rest):
            if sep == '->':
                struct, offset = pointee(member.type), 0
            elif sep == '.':
                struct = pointee(member.type)
            member, add = self.pick(struct, step)
            offset += member.offset + add
            desc.append('%s::%s' % (member.owner, member.name))
        return offset, member, ' . '.join(desc)

    def resolve_any(self, rest):
        """Cherche l'etape dans toutes les classes, filtrees par 'etape/ClasseDeBase'."""
        step, _, base = rest.partition('/')
        hits = []
        for name in self.sdk.structs:
            if base and base not in self.sdk.chain(name):
                continue
            try:
                member, add = self.pick(name, step)
            except Unresolved:
                continue
            if all(h[1] is not member for h in hits):
                hits.append((member.offset + add, member))
        if not hits:
            raise Unresolved('%s introuvable dans tout le SDK' % step)
        if len(hits) > 1:
            raise Unresolved('%s ambigu : %s' % (step, ', '.join(
                '%s@0x%X' % (m.owner, o) for o, m in hits[:6])))
        offset, member = hits[0]
        return offset, member, '%s::%s' % (member.owner, member.name)


def read_old(path):
    """-> ({cle: ancienne valeur}, [cles dans l'ordre]) depuis Offset.h (lignes actives)."""
    consts, keys, order = {}, {}, []
    with open(path, encoding='utf-8-sig') as f:
        for line in f:
            m = re.match(r'\s*constexpr uint64_t (\w+) = (0x[0-9A-Fa-f]+);', line)
            if m:
                consts[m.group(1)] = int(m.group(2), 16)
            m = re.match(r'\s*GameData\.Offset\["([^"]+)"\]\s*=\s*([^;]+);', line)
            if m:
                total = 0
                for term in m.group(2).split('+'):
                    term = term.strip()
                    total += consts[term] if term in consts else int(term, 16)
                keys[m.group(1)] = total
                order.append(m.group(1))
    return keys, order


def read_external(path):
    out = {}
    if path and os.path.exists(path):
        with open(path, encoding='utf-8', errors='replace') as f:
            for line in f:
                m = re.match(r'\s*constexpr auto (\w+) = (0x[0-9A-Fa-f]+);', line)
                if m:
                    out[m.group(1)] = int(m.group(2), 16)
    return out


def describe(sdk, cls, offset):
    """Ce que le SDK place a `offset` dans `cls`."""
    hit = sdk.at(cls, offset)
    if not hit:
        return 'hors de la classe'
    m = hit[-1]
    if m.pad:
        return 'zone non reflechie (%s)' % m.owner
    if m.offset != offset:
        return 'au milieu de %s %s' % (m.type, m.name)
    return '%s %s' % (m.type, m.name)


def infer(sdk, res, cls, old, anchors):
    """Ancienne valeur + decalage des ancres voisines de la meme hierarchie."""
    try:
        cls = res.owner(cls)
    except Unresolved as e:
        return None, str(e)
    chain = sdk.chain(cls)
    near = sorted((a for a in anchors if a[2] in chain and a[2] != 'UObject'), key=lambda a: a[0])
    below = [a for a in near if a[0] <= old]
    above = [a for a in near if a[0] > old]
    preds = []
    for a, side in ((below[-1] if below else None, 'dessous'), (above[0] if above else None, 'dessus')):
        if a:
            preds.append((old + a[1] - a[0], side, a[3]))
    if not preds:
        return None, 'aucune ancre dans %s' % cls
    text = ' ; '.join('0x%X via %s (%s) -> %s' % (p, key, side, describe(sdk, cls, p))
                      for p, side, key in preds)
    agree = len(preds) == 2 and preds[0][0] == preds[1][0]
    # une seule ancre : on ne retient la valeur que si elle tombe pile sur un membre reflechi
    alone = len(preds) == 1 and any(
        m.offset == preds[0][0] and not m.pad for m in sdk.at(cls, preds[0][0]))
    return (preds[0][0] if agree or alone else None), text


def main():
    here = os.path.dirname(os.path.abspath(__file__))
    root = os.path.dirname(here)
    sdk_dir = sys.argv[1]
    offset_h = sys.argv[2] if len(sys.argv) > 2 else os.path.join(root, 'Source', 'Common', 'Offset.h')
    ext_path = sys.argv[3] if len(sys.argv) > 3 else os.path.join(root, 'offsetTest.txt')
    report = sys.argv[4] if len(sys.argv) > 4 else os.path.join(here, 'offsets_report.txt')

    sdk = SDK(sdk_dir)
    res = Resolver(sdk)
    old, order = read_old(offset_h)
    for key in list(SPEC) + list(INFER):
        if key not in old:
            old[key] = None
            order.append(key)
    ext = {EXTERNAL[k]: v for k, v in read_external(ext_path).items() if k in EXTERNAL}

    names = read_name_indices(os.path.join(sdk_dir, 'NamesDump.txt'), set(NAME_INDEX.values()))
    rows, anchors = {}, []
    for key in order:
        value = why = None
        method = ''
        if NAME_INDEX.get(key) in names:
            value, method, why = names[NAME_INDEX[key]], 'NOMS', 'indice FName dans NamesDump.txt'
        elif key in SPEC:
            try:
                value, member, why = res.resolve(SPEC[key])
                method = 'SDK'
                if old[key] is not None:
                    anchors.append((old[key], value, member.owner, key))
            except Unresolved as e:
                why = str(e)
        elif key not in INFER and len(sdk.by_member.get(key, [])) == 1:
            member = sdk.by_member[key][0]
            value, method, why = member.offset, 'AUTO', '%s::%s' % (member.owner, member.name)
        rows[key] = [value, method, why or '']

    for key in order:
        value, method, why = rows[key]
        if value is None and key in ext:
            rows[key] = [ext[key], 'EXTERNE', (why + ' | ' if why else '') + 'offsetTest.txt']
        elif value is not None and key in ext:
            rows[key][2] += ' | offsetTest %s' % ('OK' if ext[key] == value else 'DIFFERENT 0x%X' % ext[key])
        elif value is None and key in INFER and old[key] is not None:
            guess, text = infer(sdk, res, INFER[key], old[key], anchors)
            rows[key] = [guess, 'INFERE' if guess is not None else '', text]

    counts = {}
    lines = ['%-30s %-10s %-10s %-8s %s' % ('cle', 'ancien', 'nouveau', 'methode', 'detail')]
    for key in order:
        value, method, why = rows[key]
        method = method or 'INCONNU'
        counts[method] = counts.get(method, 0) + 1
        lines.append('%-30s %-10s %-10s %-8s %s' % (
            key, '-' if old[key] is None else '0x%X' % old[key],
            '?' if value is None else '0x%X' % value, method, why))
    summary = '  '.join('%s=%d' % kv for kv in sorted(counts.items()))
    with open(report, 'w', encoding='utf-8') as f:
        f.write('\n'.join(lines) + '\n\n' + summary + '\n')
    print(summary)
    print('rapport :', report)


if __name__ == '__main__':
    main()
