#pragma once
#include <cstdint>
#include "common/Data.h"

// ============================================================
//  Offsets — fournis localement, sans dépendre du cloud
//  Heure de Pékin actuelle : 19 juin 2026
//  Version : 2609.1.3.1, partielle : les lignes « PAS À JOUR » sont restées en 2605
//
//  Note : Decrypt::CIndex / Decrypt::Xe / Decrypt::DestroyXe sont définis dans Hack/Decrypt.h
// ============================================================
namespace Offset
{

	constexpr uint64_t XenuineDecrypt = 0x10EC3828;           // 2609 : offsetTest
	constexpr uint64_t UWorld = 0x128271E8;                   // 2609 : offsetTest
	constexpr uint64_t GNames = 0x12ABD1B0;                   // 2609 : offsetTest
	constexpr uint64_t GNamesPtr = 0x10;                      // 2609 : offsetTest
	constexpr uint64_t ChunkSize = 0x4164;                    // 2609 : offsetTest
	constexpr uint64_t GObjects = 0x127E7F70;                 // 2609 : script
	constexpr uint64_t GObjectsCount = 0x127E7F60;            // 2609 : script
	constexpr uint64_t CurrentLevel = 0x8D0;                  // 2609 : SDK
	constexpr uint64_t Actors = 0xB0;                         // 2609 : offsetTest
	//constexpr uint64_t ActorsForGC = 0x358; // inutilisé
	constexpr uint64_t GameInstance = 0x7F8;                  // 2609 : script, vérifié en jeu
	constexpr uint64_t GameState = 0x278;                     // 2609 : script, signature figée ; FAUX en jeu (pointeur nul), à retrouver
	constexpr uint64_t LocalPlayer = 0x40;                    // 2609 : script, vérifié en jeu
	constexpr uint64_t PlayerController = 0x38;               // 2609 : offsetTest
	constexpr uint64_t AcknowledgedPawn = 0x4A8;              // 2609 : SDK
	constexpr uint64_t PlayerCameraManager = 0x4D0;           // 2609 : SDK

	constexpr uint64_t ObjID = 0x28;                          // 2609 : SDK
	//constexpr uint64_t DecryptNameIndexRor = 0x1; // inutilisé
	//constexpr uint64_t DecryptNameIndexXorKey1 = 0xD286E6F2; // 2609 : script // inutilisé
	//constexpr uint64_t DecryptNameIndexXorKey2 = 0x4CC9D1B1; // 2609 : script // inutilisé
	//constexpr uint64_t DecryptNameIndexXorKey3 = 0xFFFF0000; // 2609 : script // inutilisé
	//constexpr uint64_t DecryptNameIndexRval = 0x7; // 2609 : script // inutilisé
	//constexpr uint64_t DecryptNameIndexSval = 0x4; // inutilisé
	//constexpr uint64_t DecryptNameIndexDval = 0x9; // 2609 : script // inutilisé

	constexpr uint64_t ViewTarget = 0x1090;                   // 2609 : SDK
	constexpr uint64_t CameraCacheLocation = 0x474;           // 2609 : SDK
	constexpr uint64_t CameraCacheRotation = 0x464;           // 2609 : SDK
	constexpr uint64_t CameraCacheFOV = 0x470;                // 2609 : SDK

	constexpr uint64_t LastTeamNum = 0x1E10;                  // 2609 : SDK ; valeur déduite par adjacency, source la moins fiable
	constexpr uint64_t TeamNumber = 0xB80;                    // 2609 : SDK ; TslPlayerState::TeamNumber, source la plus fiable
	constexpr uint64_t TeamPtr = 0x1E00;                      // 2609 : nouvelle liste ; pointeur d'objet Team partagé par une même équipe

	constexpr uint64_t MyHUD = 0x4C8;                         // 2609 : SDK
	constexpr uint64_t BlockInputWidgetList = 0x5C8;          // 2609 : SDK
	constexpr uint64_t bShowMouseCursor = 0x658;              // 2609 : SDK
	constexpr uint64_t ComponentLocation = 0x330;             // 2609 : offsetTest
	constexpr uint64_t ComponentToWorld = 0x320;              // 2609 : offsetTest
	constexpr uint64_t CharacterState = 0x114C;               // 2609 : SDK
	constexpr uint64_t CharacterName = 0x16E0;                // 2609 : SDK
	constexpr uint64_t CharacterMovement = 0x598;             // 2609 : SDK
	constexpr uint64_t WorldToMap = 0x114;                    // 2609 : script, signature générique, à vérifier
	constexpr uint64_t LayoutData = 0x40;                     // 2609 : SDK
	constexpr uint64_t Offsets = 0x0;                         // 2609 : SDK
	constexpr uint64_t Alignment = 0x20;                      // 2609 : SDK
	constexpr uint64_t Visibility = 0xA9;                     // 2609 : SDK
	constexpr uint64_t SelectMinimapSizeIndex = 0x5C8;        // PAS À JOUR (valeur 2605)
	constexpr uint64_t Slot = 0x38;                           // 2609 : SDK
	constexpr uint64_t WidgetStateMap = 0x550;                // 2609 : SDK

	constexpr uint64_t FeatureRepObject = 0xD10;              // 2609 : SDK
	constexpr uint64_t SafetyZonePosition = 0xB0;             // 2609 : SDK
	constexpr uint64_t SafetyZoneRadius = 0xBC;               // 2609 : SDK
	constexpr uint64_t BlueZoneRadius = 0xCC;                 // 2609 : SDK
	constexpr uint64_t BlueZonePosition = 0xC0;               // 2609 : SDK
	constexpr uint64_t NumAliveTeams = 0x4E0;                 // 2609 : SDK

	//constexpr uint64_t HeaFlag = 0x110; // 2609 : script // inutilisé
	//constexpr uint64_t Health1 = 0x9A0; // 2609 : script // inutilisé
	//constexpr uint64_t Health2 = 0x980; // 2609 : script // inutilisé
	//constexpr uint64_t Health3 = 0x974; // 2609 : script // inutilisé
	constexpr uint64_t Health4 = 0x960;                       // 2609 : script, confirmé par la structure du SDK
	//constexpr uint64_t Health5 = 0x975; // 2609 : script // inutilisé
	//constexpr uint64_t Health6 = 0x970; // 2609 : script // inutilisé
	constexpr uint64_t Health_keys0 = 0xCEC7A58E;             // 2609 : script
	constexpr uint64_t Health_keys1 = 0x9B63B202;             // 2609 : script
	constexpr uint64_t Health_keys2 = 0xCAC166A5;             // 2609 : script
	constexpr uint64_t Health_keys3 = 0x2384859;              // 2609 : script
	constexpr uint64_t Health_keys4 = 0xDE911D0A;             // 2609 : script
	constexpr uint64_t Health_keys5 = 0x23DDBC20;             // 2609 : script
	constexpr uint64_t Health_keys6 = 0x94559C8;              // 2609 : script
	constexpr uint64_t Health_keys7 = 0xA521A721;             // 2609 : script
	constexpr uint64_t Health_keys8 = 0xBBC7A58;              // 2609 : script
	constexpr uint64_t Health_keys9 = 0xB0EF0287;             // 2609 : script
	constexpr uint64_t Health_keys10 = 0xE27517A7;            // 2609 : script
	constexpr uint64_t Health_keys11 = 0x878ADBC2;            // 2609 : script
	constexpr uint64_t Health_keys12 = 0xBDDEC7D5;            // 2609 : script
	constexpr uint64_t Health_keys13 = 0x2935907;             // 2609 : script
	constexpr uint64_t Health_keys14 = 0x59099E38;            // 2609 : script
	constexpr uint64_t Health_keys15 = 0xF3C62AD8;            // 2609 : script
	constexpr uint64_t GroggyHealth = 0x18C0;                 // 2609 : SDK (= DBNOHealth, seul float de vie encore lisible côté client)
	constexpr uint64_t DBNOHealthMax = 0x1724;                // 2609 : nouvelle liste
	constexpr uint64_t HealthMax = 0xA50;                     // 2609 : nouvelle liste
	constexpr uint64_t StateBits = 0x1B70;                    // 2609 : trouvé à la main, absent des deux listes ; bit0 = DBNO, bit1 = dead
	//constexpr uint64_t BlueBlockerGaugeTotalMax = 0x10; // inutilisé

	constexpr uint64_t PlayerArray = 0x418;                   // 2609 : SDK
	constexpr uint64_t AccountId = 0xB70;                     // 2609 : SDK
	constexpr uint64_t PlayerName = 0x420;                    // 2609 : SDK
	constexpr uint64_t PlayerStatusType = 0x440;              // PAS À JOUR (valeur 2605)
	constexpr uint64_t SquadMemberIndex = 0xBA8;              // 2609 : SDK
	constexpr uint64_t PlayerState = 0x428;                   // 2609 : SDK
	constexpr uint64_t KilledBits = 0xAD8;                    // 2609 : nouvelle liste ; TslPlayerStateBase::@OnRep_Killed, lisible même après despawn du corps
	constexpr uint64_t KilledMask = 0x1;                      // 2609 : nouvelle liste
	constexpr uint64_t PlayerStatistics = 0xA2C;              // PAS À JOUR (valeur 2605)
	constexpr uint64_t DamageDealtOnEnemy = 0xAE4;            // 2609 : SDK
	constexpr uint64_t SpectatedCount = 0x16C4;               // 2609 : SDK
	constexpr uint64_t ping = 0x41C;                          // 2609 : SDK
	//constexpr uint64_t MatchId = 0x488; // inutilisé
	//constexpr uint64_t CapsuleComponent = 0x6E8; // inutilisé
	//constexpr uint64_t CustomTimeDilation = 0x198; // inutilisé

	constexpr uint64_t PartnerLevel = 0x630;                  // 2609 : SDK
	constexpr uint64_t SurvivalTier = 0xE58;                  // 2609 : déduit de PubgIdData + 0x20 (SDK), à vérifier
	constexpr uint64_t SurvivalLevel = 0xE5C;                 // 2609 : déduit de PubgIdData + 0x24 (SDK), à vérifier
	constexpr uint64_t PubgIdData = 0xE38;                    // 2609 : SDK

	constexpr uint64_t CharacterClanInfo = 0xB08;             // 2609 : SDK

	constexpr uint64_t EquippedWeapons = 0x210;               // 2609 : SDK
	constexpr uint64_t WeaponProcessor = 0x958;               // 2609 : SDK
	constexpr uint64_t CurrentWeaponIndex = 0x319;            // 2609 : script (inchangé), à vérifier
	constexpr uint64_t WeaponTrajectoryData = 0x1220;         // 2609 : SDK
	constexpr uint64_t TrajectoryGravityZ = 0x10EC;           // 2609 : script, signature figée, à vérifier
	constexpr uint64_t FiringAttachPoint = 0x8E0;             // PAS À JOUR (valeur 2605) ; candidats SDK : 0x8D0, 0x8E0
	constexpr uint64_t ScopingAttachPoint = 0xED8;            // 2609 : SDK
	constexpr uint64_t TrajectoryConfig = 0x108;              // 2609 : SDK
	constexpr uint64_t BallisticCurve = 0x28;                 // 2609 : SDK
	constexpr uint64_t FloatCurves = 0x38;                    // 2609 : SDK (inchangé)
	constexpr uint64_t Mesh3P = 0xA78;                        // 2609 : SDK, à vérifier

	constexpr uint64_t Keys = 0x60;                           // 2609 : SDK (inchangé)

	constexpr uint64_t AttachedStaticComponentMap = 0x1508;   // PAS À JOUR (valeur 2605)

	constexpr uint64_t WeaponConfig_WeaponClass = 0x720;      // 2609 : SDK, à vérifier
	constexpr uint64_t ElapsedCookingTime = 0xB30;            // PAS À JOUR (valeur 2605)

	constexpr uint64_t PlayerInput = 0x548;                   // 2609 : SDK
	constexpr uint64_t InputAxisProperties = 0x138;           // 2609 : déduit du SDK (zone non décrite, inchangé), à vérifier

	constexpr uint64_t LastUpdateVelocity = 0x3E0;            // 2609 : script + SDK (inchangé)
	constexpr uint64_t ComponentVelocity = 0x230;             // 2609 : SDK
	constexpr uint64_t Mesh = 0x5B0;                          // 2609 : SDK
	constexpr uint64_t RootComponent = 0x3E8;                 // 2609 : SDK
	constexpr uint64_t StaticMesh = 0xAE8;                    // 2609 : SDK
	constexpr uint64_t Eyes = 0x75C;                          // 2609 : script + SDK (inchangé)
	constexpr uint64_t bAlwaysCreatePhysicsState = 0x498;     // 2609 : SDK
	constexpr uint64_t SkeletalMesh = 0xAD8;                  // 2609 : SDK
	constexpr uint64_t Skeleton = 0x50;                       // 2609 : SDK
	constexpr uint64_t SkeletalSockets = 0x198;               // 2609 : SDK
	constexpr uint64_t SkeletalSocketName = 0x30;             // 2609 : SDK

	constexpr uint64_t VehicleMovement = 0x470;               // 2609 : SDK
	constexpr uint64_t VehicleRiderComponent = 0x21B0;        // 2609 : SDK
	constexpr uint64_t ReplicatedMovement = 0x80;             // 2609 : SDK
	constexpr uint64_t LastVehiclePawn = 0x270;               // 2609 : SDK
	constexpr uint64_t SeatIndex = 0x238;                     // 2609 : SDK

	constexpr uint64_t Wheels = 0x330;                        // 2609 : SDK
	constexpr uint64_t WheelLocation = 0x100;                 // 2609 : SDK
	constexpr uint64_t WheelOldLocation = 0x10C;              // 2609 : SDK
	constexpr uint64_t WheelVelocity = 0x118;                 // 2609 : SDK
	constexpr uint64_t DampingRate = 0x54;                    // 2609 : déduit, à vérifier
	constexpr uint64_t ShapeRadius = 0x48;                    // 2609 : déduit, à vérifier

	constexpr uint64_t DroppedItemGroup = 0x130;              // 2609 : nouvelle liste (structurel, à vérifier)
	constexpr uint64_t ItemPackageItems = 0x580;              // 2609 : SDK
	constexpr uint64_t DroppedItemGroupUItem = 0x888;         // 2609 : nouvelle liste

	constexpr uint64_t AttachedItems = 0x868;                 // 2609 : SDK
	constexpr uint64_t WeaponAttachmentData = 0x290;          // 2609 : nouvelle liste
	constexpr uint64_t ItemTable = 0xB0;                      // 2609 : déduit du SDK (zone non décrite, inchangé), à vérifier
	constexpr uint64_t ItemID = 0x274;                        // 2609 : nouvelle liste (champ FName)
	constexpr uint64_t ItemIDIndexOffset = 0x4;               // 2609 : dword ComparisonIndex a +4 du champ FName (mettre 0x0 si les items ressortent vides)
	constexpr uint64_t DroppedItem = 0x470;                   // 2609 : SDK

	constexpr uint64_t AnimScriptInstance = 0xE30;            // 2609 : SDK
	constexpr uint64_t PreEvalPawnState = 0x658;              // 2609 : SDK

	//constexpr uint64_t bIsInVehicle_CP = 0x63C; // inutilisé
	//constexpr uint64_t bIsParachuting_CP = 0x92E; // inutilisé
	//constexpr uint64_t bIsFreefalling_CP = 0x92D; // inutilisé
	//constexpr uint64_t bEmergencyPickup_Flying_CP = 0x92F; // inutilisé
	//constexpr uint64_t bIsReviving_CP = 0x932; // inutilisé
	//constexpr uint64_t bIsSwimming_CP = 0x935; // inutilisé

	//constexpr uint64_t VTable = 0xA68; // inutilisé
	//constexpr uint64_t bIsDBNO_CP = 0x931; // inutilisé
	constexpr uint64_t bIsDBNO1 = 0x3588;                   // 2609 : nouvelle liste (structurel, à vérifier) ; up
	constexpr uint64_t bIsDBNO2 = 0x3589;                   // 2609 : nouvelle liste (structurel, à vérifier) ; alive
	constexpr uint64_t bIsDBNO0 = 0x358A;                   // 2609 : nouvelle liste (structurel, à vérifier) ; validator
	constexpr uint64_t bIsScoping_CP = 0x865;                 // 2609 : SDK
	//constexpr uint64_t bIsPreparingThrow_CP = 0x540; // inutilisé
	//constexpr uint64_t bIsThrowing_CP = 0x938; // inutilisé
	//constexpr uint64_t bIsFlashed_CP = 0x63F; // inutilisé
	constexpr uint64_t bIsReloading_CP = 0x75D;               // 2609 : SDK
	constexpr uint64_t RecoilADSRotation_CP = 0x824;          // 2609 : script (inchangé), douteux d'après le SDK
	constexpr uint64_t ControlRotation_CP = 0x674;            // 2609 : SDK
	constexpr uint64_t LeanLeftAlpha_CP = 0x6BC;              // 2609 : SDK
	constexpr uint64_t LeanRightAlpha_CP = 0x6C0;             // 2609 : SDK
	constexpr uint64_t CurrentAmmoData = 0xCA8;               // PAS À JOUR (valeur 2605)

	constexpr uint64_t StaticSockets = 0xC8;                  // 2609 : SDK
	constexpr uint64_t StaticSocketName = 0x30;               // 2609 : SDK
	constexpr uint64_t StaticRelativeScale = 0x50;            // 2609 : SDK
	constexpr uint64_t StaticRelativeLocation = 0x38;         // 2609 : SDK
	constexpr uint64_t StaticRelativeRotation = 0x44;         // 2609 : SDK

	//constexpr uint64_t InputYawScale = 0x66C; // inutilisé

	constexpr uint64_t AimOffsets = 0x1BD0;                   // 2609 : SDK

	constexpr uint64_t AntiCheatCharacterSyncManager = 0x11F8;// 2609 : script, à vérifier

	constexpr uint64_t TimeSeconds = 0x954;                   // PAS À JOUR (valeur 2605)
	constexpr uint64_t TimeTillExplosion = 0x834;             // 2609 : SDK
	constexpr uint64_t ExplodeState = 0x648;                  // PAS À JOUR (valeur 2605)
	constexpr uint64_t TrainingMapGrid = 0x608;               // 2609 : script + SDK
	//constexpr uint64_t RecentlyRendered = 0xBD8; // inutilisé
	//constexpr uint64_t LastSubmitTime = 0x758; // 2609 : script // inutilisé
	//constexpr uint64_t LastRenderTimeOnScreen = 0x77C; // inutilisé
	constexpr uint64_t MortarRotation = 0x540;                // PAS À JOUR (valeur 2605)
	constexpr uint64_t MortarEntity = 0xA8;                   // PAS À JOUR (valeur 2605)

	constexpr uint64_t MapGrid_Map = 0x4A8;                   // PAS À JOUR (valeur 2605)
	constexpr uint64_t Gender = 0xB40;                        // 2609 : SDK

	constexpr uint64_t MouseX = 0x5212;                       // 2609 : NamesDump
	constexpr uint64_t MouseY = 0x5213;                       // 2609 : NamesDump

	//constexpr uint64_t LineTraceSingle = 0xCBED628; // inutilisé
	//constexpr uint64_t HOOK = 0x117F9B58; // inutilisé
	//constexpr uint64_t HOOK_TWO = 0xCEDB7E4; // inutilisé

	constexpr uint64_t InventoryFacade = 0x1290;              // 2609 : SDK
	constexpr uint64_t Inventory = 0x408;                     // 2609 : SDK
	constexpr uint64_t InventoryItems = 0x600;                // 2609 : SDK
	//constexpr uint64_t InventoryItemCount = 0x6B0; // inutilisé
	constexpr uint64_t InventoryItemTagItemCount = 0x40;      // 2609 : SDK

	//constexpr uint64_t Equipment = 0x430; // inutilisé
	//constexpr uint64_t ItemsArray = 0x588; // inutilisé
	//constexpr uint64_t Durability = 0x1E4; // inutilisé
	//constexpr uint64_t Durabilitymax = 0x1E0; // inutilisé

	constexpr uint64_t VehicleCommonComponent = 0xB30;        // 2609 : SDK
	//constexpr uint64_t FloatingVehicleCommonComponent = 0x4F8; // inutilisé
	constexpr uint64_t VehicleFuel = 0x2F8;                   // 2609 : SDK
	constexpr uint64_t VehicleFuelMax = 0x2FC;                // 2609 : SDK
	constexpr uint64_t VehicleHealth = 0x2F0;                 // 2609 : SDK
	constexpr uint64_t VehicleHealthMax = 0x2F4;              // 2609 : SDK

	//constexpr uint64_t PhysxSDK = 0x123BE088; // 2609 : script // inutilisé
	//constexpr uint64_t PhysicsScene = 0x368; // inutilisé
	//constexpr uint64_t mPhysXScene = 0xD0; // inutilisé
	//constexpr uint64_t rigid_dynamics = 0x3B98; // inutilisé
	//constexpr uint64_t Unreal_Engine = 0x11DF6510; // inutilisé

	//constexpr uint64_t CurrentMinimapViewScale1D = 0x4A4; // inutilisé
	//constexpr uint64_t LastMinimapPos = 0x4B8; // inutilisé
	//constexpr uint64_t Minimap = 0x2D8; // inutilisé
	constexpr uint64_t GlobalAnimRateScale = 0xED8;           // 2609 : SDK (inchangé)
	constexpr uint64_t LastDamage = 0x1B60;                   // PAS À JOUR (valeur 2605)
	//constexpr uint64_t RecoilValueVector = 0x1154; // inutilisé
	//constexpr uint64_t VerticalRecovery = 0x1168; // inutilisé

	// Entrées des anciennes données non écrasées, valeurs d'origine conservées
	constexpr uint64_t Health = 0x970;                        // PAS À JOUR (valeur 2605)
	constexpr uint64_t Offsets_ = 0x0;                        // 2609 : SDK
	constexpr uint64_t Physx_sdk = 0x12270548;                // PAS À JOUR (valeur 2605)
	constexpr uint64_t MortarLocation = 0xB0;                 // PAS À JOUR (valeur 2605)
	//constexpr uint64_t MapMarker = 0xD0; // inutilisé
	constexpr uint64_t SPOOFCALL = 0x0;                       // PAS À JOUR (valeur 2605)
	constexpr uint64_t bIsDead = 0x3538;                      // PAS À JOUR (valeur 2605)
	constexpr uint64_t FloatingComponent = 0x4E0;             // 2609 : SDK
	//constexpr uint64_t Ping = 0x448; // inutilisé
	

	inline void Sever_Init()
	{
		// --- Déchiffrement de la santé ---
		GameData.Offset["Health"] = Health4;
		GameData.Offset["bEncryptedHealth"] = Health4 + 0x15;
		GameData.Offset["EncryptedHealthOffset"] = Health4 + 0x14;
		GameData.Offset["DecryptedHealthOffset"] = Health4 + 0x10;

		//GameData.Offset["HeaFlag"] = HeaFlag; // inutilisé
		//GameData.Offset["Health1"] = Health1; // inutilisé
		//GameData.Offset["Health2"] = Health2; // inutilisé
		//GameData.Offset["Health3"] = Health3; // inutilisé
		//GameData.Offset["Health4"] = Health4; // inutilisé
		//GameData.Offset["Health5"] = Health5; // inutilisé
		//GameData.Offset["Health6"] = Health6; // inutilisé
		GameData.Offset["GroggyHealth"] = GroggyHealth;
		GameData.Offset["DBNOHealthMax"] = DBNOHealthMax;
		GameData.Offset["HealthMax"] = HealthMax;
		GameData.Offset["StateBits"] = StateBits;
		//GameData.Offset["BlueBlockerGaugeTotalMax"] = BlueBlockerGaugeTotalMax; // inutilisé

		GameData.Offset["DecryptedHealthOffsets0"] = Health_keys0;
		GameData.Offset["DecryptedHealthOffsets1"] = Health_keys1;
		GameData.Offset["DecryptedHealthOffsets2"] = Health_keys2;
		GameData.Offset["DecryptedHealthOffsets3"] = Health_keys3;
		GameData.Offset["DecryptedHealthOffsets4"] = Health_keys4;
		GameData.Offset["DecryptedHealthOffsets5"] = Health_keys5;
		GameData.Offset["DecryptedHealthOffsets6"] = Health_keys6;
		GameData.Offset["DecryptedHealthOffsets7"] = Health_keys7;
		GameData.Offset["DecryptedHealthOffsets8"] = Health_keys8;
		GameData.Offset["DecryptedHealthOffsets9"] = Health_keys9;
		GameData.Offset["DecryptedHealthOffsets10"] = Health_keys10;
		GameData.Offset["DecryptedHealthOffsets11"] = Health_keys11;
		GameData.Offset["DecryptedHealthOffsets12"] = Health_keys12;
		GameData.Offset["DecryptedHealthOffsets13"] = Health_keys13;
		GameData.Offset["DecryptedHealthOffsets14"] = Health_keys14;
		GameData.Offset["DecryptedHealthOffsets15"] = Health_keys15;

		// --- Cœur du moteur ---
		GameData.Offset["XenuineDecrypt"] = XenuineDecrypt;
		GameData.Offset["UWorld"] = UWorld;
		GameData.Offset["GNames"] = GNames;
		GameData.Offset["GNamesPtr"] = GNamesPtr;
		GameData.Offset["ChunkSize"] = ChunkSize;
		GameData.Offset["ObjID"] = ObjID;
		GameData.Offset["GameInstance"] = GameInstance;
		GameData.Offset["LocalPlayer"] = LocalPlayer;
		GameData.Offset["Actors"] = Actors;
		GameData.Offset["CurrentLevel"] = CurrentLevel;
		GameData.Offset["PlayerController"] = PlayerController;
		GameData.Offset["AcknowledgedPawn"] = AcknowledgedPawn;
		GameData.Offset["PlayerCameraManager"] = PlayerCameraManager;
		GameData.Offset["GameState"] = GameState;
		GameData.Offset["NumAliveTeams"] = NumAliveTeams;
		//GameData.Offset["ActorsForGC"] = ActorsForGC; // inutilisé

		// --- Paramètres de déchiffrement GNames ---
		//GameData.Offset["DecryptNameIndexRor"] = DecryptNameIndexRor; // inutilisé
		//GameData.Offset["DecryptNameIndexXorKey1"] = DecryptNameIndexXorKey1; // inutilisé
		//GameData.Offset["DecryptNameIndexXorKey2"] = DecryptNameIndexXorKey2; // inutilisé
		//GameData.Offset["DecryptNameIndexXorKey3"] = DecryptNameIndexXorKey3; // inutilisé
		//GameData.Offset["DecryptNameIndexRval"] = DecryptNameIndexRval; // inutilisé
		//GameData.Offset["DecryptNameIndexSval"] = DecryptNameIndexSval; // inutilisé
		//GameData.Offset["DecryptNameIndexDval"] = DecryptNameIndexDval; // inutilisé

		// --- Caméra ---
		GameData.Offset["ViewTarget"] = ViewTarget;
		GameData.Offset["CameraCacheLocation"] = CameraCacheLocation;
		GameData.Offset["CameraCacheRotation"] = CameraCacheRotation;
		GameData.Offset["CameraCacheFOV"] = CameraCacheFOV;

		// --- Équipe / infos joueur ---
		GameData.Offset["LastTeamNum"] = LastTeamNum;
		GameData.Offset["TeamNumber"] = TeamNumber;
		GameData.Offset["TeamPtr"] = TeamPtr;
		GameData.Offset["PlayerArray"] = PlayerArray;
		GameData.Offset["AccountId"] = AccountId;
		GameData.Offset["PlayerName"] = PlayerName;
		GameData.Offset["PlayerStatusType"] = PlayerStatusType;
		GameData.Offset["SquadMemberIndex"] = SquadMemberIndex;
		GameData.Offset["PlayerState"] = PlayerState;
		GameData.Offset["KilledBits"] = KilledBits;
		GameData.Offset["KilledMask"] = KilledMask;
		GameData.Offset["PlayerStatistics"] = PlayerStatistics;
		GameData.Offset["DamageDealtOnEnemy"] = DamageDealtOnEnemy;
		GameData.Offset["SpectatedCount"] = SpectatedCount;
		GameData.Offset["PartnerLevel"] = PartnerLevel;
		GameData.Offset["SurvivalTier"] = SurvivalTier;
		GameData.Offset["SurvivalLevel"] = SurvivalLevel;
		GameData.Offset["CharacterClanInfo"] = CharacterClanInfo;
		//GameData.Offset["CustomTimeDilation"] = CustomTimeDilation; // inutilisé

		// --- HUD / composants ---
		GameData.Offset["MyHUD"] = MyHUD;
		GameData.Offset["bShowMouseCursor"] = bShowMouseCursor;
		GameData.Offset["BlockInputWidgetList"] = BlockInputWidgetList;
		GameData.Offset["ComponentLocation"] = ComponentLocation;
		GameData.Offset["ComponentToWorld"] = ComponentToWorld;
		GameData.Offset["CharacterState"] = CharacterState;
		GameData.Offset["CharacterName"] = CharacterName;
		GameData.Offset["CharacterMovement"] = CharacterMovement;
		GameData.Offset["WorldToMap"] = WorldToMap;
		GameData.Offset["LayoutData"] = LayoutData;
		GameData.Offset["Offsets"] = Offsets_;
		GameData.Offset["Alignment"] = Alignment;
		GameData.Offset["Visibility"] = Visibility;
		GameData.Offset["SelectMinimapSizeIndex"] = SelectMinimapSizeIndex;
		GameData.Offset["Slot"] = Slot;
		GameData.Offset["WidgetStateMap"] = WidgetStateMap;

		// --- Zone de sécurité ---
		GameData.Offset["FeatureRepObject"] = FeatureRepObject;
		GameData.Offset["SafetyZonePosition"] = SafetyZonePosition;
		GameData.Offset["SafetyZoneRadius"] = SafetyZoneRadius;
		GameData.Offset["BlueZoneRadius"] = BlueZoneRadius;
		GameData.Offset["BlueZonePosition"] = BlueZonePosition;

		// --- Mesh / squelette ---
		GameData.Offset["Mesh"] = Mesh;
		GameData.Offset["StaticMesh"] = StaticMesh;
		GameData.Offset["RootComponent"] = RootComponent;
		GameData.Offset["LastUpdateVelocity"] = LastUpdateVelocity;
		GameData.Offset["ComponentVelocity"] = ComponentVelocity;
		GameData.Offset["Eyes"] = Eyes;
		GameData.Offset["bAlwaysCreatePhysicsState"] = bAlwaysCreatePhysicsState;
		GameData.Offset["SkeletalMesh"] = SkeletalMesh;
		GameData.Offset["Skeleton"] = Skeleton;
		GameData.Offset["SkeletalSockets"] = SkeletalSockets;
		GameData.Offset["SkeletalSocketName"] = SkeletalSocketName;

		// --- Entrées ---
		GameData.Offset["PlayerInput"] = PlayerInput;
		GameData.Offset["InputAxisProperties"] = InputAxisProperties;
		//GameData.Offset["InputYawScale"] = InputYawScale; // inutilisé
		GameData.Offset["MouseX"] = MouseX;
		GameData.Offset["MouseY"] = MouseY;

		// --- Animation ---
		GameData.Offset["AnimScriptInstance"] = AnimScriptInstance;
		GameData.Offset["PreEvalPawnState"] = PreEvalPawnState;

		// --- Visée automatique ---
		GameData.Offset["AimOffsets"] = AimOffsets;

		// --- Véhicules ---
		GameData.Offset["VehicleMovement"] = VehicleMovement;
		GameData.Offset["VehicleRiderComponent"] = VehicleRiderComponent;
		GameData.Offset["ReplicatedMovement"] = ReplicatedMovement;
		GameData.Offset["LastVehiclePawn"] = LastVehiclePawn;
		GameData.Offset["SeatIndex"] = SeatIndex;
		GameData.Offset["Wheels"] = Wheels;
		GameData.Offset["WheelLocation"] = WheelLocation;
		GameData.Offset["WheelOldLocation"] = WheelOldLocation;
		GameData.Offset["WheelVelocity"] = WheelVelocity;
		GameData.Offset["DampingRate"] = DampingRate;
		GameData.Offset["ShapeRadius"] = ShapeRadius;
		GameData.Offset["VehicleCommonComponent"] = VehicleCommonComponent;
		GameData.Offset["FloatingComponent"] = FloatingComponent;
		GameData.Offset["VehicleFuel"] = VehicleFuel;
		GameData.Offset["VehicleFuelMax"] = VehicleFuelMax;
		GameData.Offset["VehicleHealth"] = VehicleHealth;
		GameData.Offset["VehicleHealthMax"] = VehicleHealthMax;
		//GameData.Offset["FloatingVehicleCommonComponent"] = FloatingVehicleCommonComponent; // inutilisé

		// --- Objets ---
		GameData.Offset["ItemID"] = ItemID;
		GameData.Offset["ItemIDIndexOffset"] = ItemIDIndexOffset;
		GameData.Offset["ItemTable"] = ItemTable;
		GameData.Offset["ItemPackageItems"] = ItemPackageItems;
		GameData.Offset["DroppedItemGroup"] = DroppedItemGroup;
		GameData.Offset["DroppedItem"] = DroppedItem;
		GameData.Offset["DroppedItemGroupUItem"] = DroppedItemGroupUItem;

		// --- Armes ---
		GameData.Offset["WeaponProcessor"] = WeaponProcessor;
		GameData.Offset["CurrentAmmoData"] = CurrentAmmoData;
		GameData.Offset["CurrentWeaponIndex"] = CurrentWeaponIndex;
		GameData.Offset["EquippedWeapons"] = EquippedWeapons;
		GameData.Offset["WeaponTrajectoryData"] = WeaponTrajectoryData;
		GameData.Offset["TrajectoryGravityZ"] = TrajectoryGravityZ;
		GameData.Offset["TrajectoryConfig"] = TrajectoryConfig;
		GameData.Offset["BallisticCurve"] = BallisticCurve;
		GameData.Offset["FloatCurves"] = FloatCurves;
		GameData.Offset["Keys"] = Keys;
		GameData.Offset["WeaponConfig_WeaponClass"] = WeaponConfig_WeaponClass;
		GameData.Offset["Mesh3P"] = Mesh3P;
		GameData.Offset["FiringAttachPoint"] = FiringAttachPoint;
		GameData.Offset["AttachedStaticComponentMap"] = AttachedStaticComponentMap;
		GameData.Offset["AttachedItems"] = AttachedItems;
		GameData.Offset["WeaponAttachmentData"] = WeaponAttachmentData;
		GameData.Offset["ScopingAttachPoint"] = ScopingAttachPoint;
		GameData.Offset["ElapsedCookingTime"] = ElapsedCookingTime;
		//GameData.Offset["RecoilValueVector"] = RecoilValueVector; // inutilisé
		//GameData.Offset["VerticalRecovery"] = VerticalRecovery; // inutilisé

		// --- Inventaire ---
		GameData.Offset["InventoryFacade"] = InventoryFacade;
		GameData.Offset["Inventory"] = Inventory;
		GameData.Offset["InventoryItems"] = InventoryItems;
		//GameData.Offset["InventoryItemCount"] = InventoryItemCount; // inutilisé
		GameData.Offset["InventoryItemTagItemCount"] = InventoryItemTagItemCount;
		//GameData.Offset["Equipment"] = Equipment; // inutilisé
		//GameData.Offset["ItemsArray"] = ItemsArray; // inutilisé
		//GameData.Offset["Durability"] = Durability; // inutilisé
		//GameData.Offset["Durabilitymax"] = Durabilitymax; // inutilisé

		// --- États CP ---
		GameData.Offset["ControlRotation_CP"] = ControlRotation_CP;
		GameData.Offset["RecoilADSRotation_CP"] = RecoilADSRotation_CP;
		GameData.Offset["LeanLeftAlpha_CP"] = LeanLeftAlpha_CP;
		GameData.Offset["LeanRightAlpha_CP"] = LeanRightAlpha_CP;
		GameData.Offset["bIsScoping_CP"] = bIsScoping_CP;
		GameData.Offset["bIsReloading_CP"] = bIsReloading_CP;
		//GameData.Offset["bIsInVehicle_CP"] = bIsInVehicle_CP; // inutilisé
		//GameData.Offset["bIsParachuting_CP"] = bIsParachuting_CP; // inutilisé
		//GameData.Offset["bIsFreefalling_CP"] = bIsFreefalling_CP; // inutilisé
		//GameData.Offset["bEmergencyPickup_Flying_CP"] = bEmergencyPickup_Flying_CP; // inutilisé
		//GameData.Offset["bIsReviving_CP"] = bIsReviving_CP; // inutilisé
		//GameData.Offset["bIsSwimming_CP"] = bIsSwimming_CP; // inutilisé
		//GameData.Offset["bIsDBNO_CP"] = bIsDBNO_CP; // inutilisé
		//GameData.Offset["bIsPreparingThrow_CP"] = bIsPreparingThrow_CP; // inutilisé
		//GameData.Offset["bIsThrowing_CP"] = bIsThrowing_CP; // inutilisé
		//GameData.Offset["bIsFlashed_CP"] = bIsFlashed_CP; // inutilisé
		//GameData.Offset["VTable"] = VTable; // inutilisé
		GameData.Offset["bIsDBNO0"] = bIsDBNO0;
		GameData.Offset["bIsDBNO1"] = bIsDBNO1;
		GameData.Offset["bIsDBNO2"] = bIsDBNO2;

		// --- Sockets statiques ---
		GameData.Offset["StaticSockets"] = StaticSockets;
		GameData.Offset["StaticSocketName"] = StaticSocketName;
		GameData.Offset["StaticRelativeLocation"] = StaticRelativeLocation;
		GameData.Offset["StaticRelativeRotation"] = StaticRelativeRotation;
		GameData.Offset["StaticRelativeScale"] = StaticRelativeScale;

		// --- Anti-triche ---
		GameData.Offset["AntiCheatCharacterSyncManager"] = AntiCheatCharacterSyncManager;

		// --- Grenades / mortier ---
		GameData.Offset["TimeSeconds"] = TimeSeconds;
		GameData.Offset["TimeTillExplosion"] = TimeTillExplosion;
		GameData.Offset["ExplodeState"] = ExplodeState;
		GameData.Offset["MortarLocation"] = MortarLocation;
		GameData.Offset["MortarRotation"] = MortarRotation;

		// --- Carte / terrain d'entraînement ---
		GameData.Offset["TrainingMapGrid"] = TrainingMapGrid;
		//GameData.Offset["MapMarker"] = MapMarker; // inutilisé
		GameData.Offset["MapGrid_Map"] = MapGrid_Map;
		GameData.Offset["Gender"] = Gender;
		//GameData.Offset["CurrentMinimapViewScale1D"] = CurrentMinimapViewScale1D; // inutilisé
		//GameData.Offset["LastMinimapPos"] = LastMinimapPos; // inutilisé
		//GameData.Offset["Minimap"] = Minimap; // inutilisé

		// --- PhysX ---
		//GameData.Offset["PhysxSDK"] = PhysxSDK; // inutilisé
		//GameData.Offset["PhysicsScene"] = PhysicsScene; // inutilisé
		//GameData.Offset["mPhysXScene"] = mPhysXScene; // inutilisé
		//GameData.Offset["rigid_dynamics"] = rigid_dynamics; // inutilisé
		//GameData.Offset["Unreal_Engine"] = Unreal_Engine; // inutilisé

		// --- Divers ---
		//GameData.Offset["LineTraceSingle"] = LineTraceSingle; // inutilisé
		//GameData.Offset["HOOK"] = HOOK; // inutilisé
		//GameData.Offset["HOOK_TWO"] = HOOK_TWO; // inutilisé
		//GameData.Offset["RecentlyRendered"] = RecentlyRendered; // inutilisé
		//GameData.Offset["LastSubmitTime"] = LastSubmitTime; // inutilisé
		//GameData.Offset["LastRenderTimeOnScreen"] = LastRenderTimeOnScreen; // inutilisé
		//GameData.Offset["CapsuleComponent"] = CapsuleComponent; // inutilisé
		//GameData.Offset["Ping"] = Ping; // inutilisé
		//GameData.Offset["MatchId"] = MatchId; // inutilisé

		return;
	}
}
