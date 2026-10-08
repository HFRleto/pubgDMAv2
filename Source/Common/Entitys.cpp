#include "common/Entitys.h"
#include "common/Data.h";
#include <Utils/FNVHash.h>

std::unordered_map<std::string, EntityInfo, FnvHash> EntityPlayerLists = {
	//玩家
	{"PlayerMale_A", {"Player", EntityType::Player, 0}},
	{"PlayerMale_A_C", {"Player", EntityType::Player, 0}},
	{"PlayerFemale_A", {"Player", EntityType::Player, 0}},
	{"PlayerFemale_A_C", {"Player", EntityType::Player, 0}},
	{"AIPawn_Base_C", {"AI", EntityType::AI, 0}},
	{"AIPawn_Base_Female_C", {"AI", EntityType::AI, 0}},
	{"AIPawn_Base_Male_C", {"AI", EntityType::AI, 0}},
	{"AIPawn_Base_Pillar_C", {"AI", EntityType::AI, 0}},
	{"AIPawn_Base_Female_Pillar_C", {"AI", EntityType::AI, 0}},
	{"AIPawn_Base_Male_Pillar_C", {"AI", EntityType::AI, 0}},
	{"UltAIPawn_Base_C", {"AI", EntityType::AI, 0}},
	{"UltAIPawn_Base_Female_C", {"AI", EntityType::AI, 0}},
	{"UltAIPawn_Base_Male_C", {"AI", EntityType::AI, 0}},
	{"ZDF2_NPC_Runner_C", {"AI", EntityType::AI, 0}},
	{"ZDF2_NPC_Burning_C", {"AI", EntityType::AI, 0}},
	{"ZDF2_NPC_Tanker_C", {"AI", EntityType::AI, 0}},
	{"ZDF2_NPC_Female_C", {"AI", EntityType::AI, 0}},
	{"ZombieNpcNewPawn_Tanker_C", {"AI", EntityType::AI, 0}},
	{"UltAIPawn_Base_Pillar_C", {"AI", EntityType::AI, 0}},
    {"UltAIPawn_Base_Female_Pillar_C", {"AI", EntityType::AI, 0}},
    {"UltAIPawn_Base_Male_Pillar_C", {"AI", EntityType::AI, 0}},
    {"BP_MarketAI_Pawn_C", {"AI", EntityType::AI, 0}},
};

std::unordered_map<std::string, EntityInfo, FnvHash> EntityItemLists = {
	//盒子
	{"DeathDropItemPackage_C", {"Death Box", EntityType::DeadBox, 0}},
	{"Carapackage_RedBox_C", {"Air Drop", EntityType::AirDrop, 0}},
	{"Carapackage_SmallPackage_C", {"Small Air Drop", EntityType::AirDrop, 0}},
	{"Carapackage_FlareGun_C", {"Air Drop", EntityType::AirDrop, 0}},
	{"Carapackage_SmallPackage_SLB_C", {"Air Drop", EntityType::AirDrop, 0}},
	{"Carapackage_SmallPackage_DihorOtok_C", {"Air Drop", EntityType::AirDrop, 0}},
	{"Carapackage_RedBox_Subzero_C", {"Air Drop", EntityType::AirDrop, 0}},
	{"Carapackage_FlareGun_Subzero_C", {"Air Drop", EntityType::AirDrop, 0}},

	//物品
	{"DroppedItem", {"DroppedItem", EntityType::DroppedItem, 0}},
	{"DroppedItemGroup", {"DroppedItemGroup", EntityType::DroppedItemGroup, 0}},

	//刷新物品
	{"Item_Weapon_Groza_C", {"Groza", EntityType::Item, 0, WeaponType::AR}},
	{"Item_Weapon_BerylM762_C", {"Beryl M762", EntityType::Item, 0, WeaponType::AR}},
	{"Item_Weapon_ACE32_C", {"ACE32", EntityType::Item, 0, WeaponType::AR}},
	{"Item_Weapon_HK416_C", {"M416", EntityType::Item, 0, WeaponType::AR}},
	{"Item_Weapon_FAMASG2_C", {"FAMASI", EntityType::Item, 0, WeaponType::AR}},
	{"Item_Weapon_AUG_C", {"AUG", EntityType::Item, 0, WeaponType::AR}},
	{"Item_Weapon_AK47_C", {"AKM", EntityType::Item, 0, WeaponType::AR}},
	{"Item_Weapon_SCAR-L_C", {"SCAR-L", EntityType::Item, 0, WeaponType::AR}},
	{"Item_Weapon_G36C_C", {"G36C", EntityType::Item, 0, WeaponType::AR}},
	{"Item_Weapon_QBZ95_C", {"QBZ95", EntityType::Item, 0, WeaponType::AR}},
	{"Item_Weapon_K2_C", {"K2", EntityType::Item, 0, WeaponType::AR}},
	{"Item_Weapon_Mk47Mutant_C", {"Mk47", EntityType::Item, 0, WeaponType::AR}},
	{"Item_Weapon_M16A4_C", {"M16A4", EntityType::Item, 0, WeaponType::AR}},

	{"Item_Weapon_MG3_C", {"MG3", EntityType::Item, 0, WeaponType::LMG}},
	{"Item_Weapon_DP28_C", {"DP28", EntityType::Item, 0, WeaponType::LMG}},
	{"Item_Weapon_M249_C", {"M249", EntityType::Item, 0, WeaponType::LMG}},

	{"Item_Weapon_L6_C", {"Barrett", EntityType::Item, 0, WeaponType::SR}},
	{"Item_Weapon_AWM_C", {"AWM", EntityType::Item, 0, WeaponType::SR}},
	{"Item_Weapon_M24_C", {"M24", EntityType::Item, 0, WeaponType::SR}},
	{"Item_Weapon_Kar98k_C", {"Kar98k", EntityType::Item, 0, WeaponType::SR}},
	{"Item_Weapon_Mosin_C", {"Mosin Nagant", EntityType::Item, 0, WeaponType::SR}},
	{"Item_Weapon_Win1894_C", {"Win94", EntityType::Item, 0, WeaponType::SR}},
	{"Item_Weapon_Crossbow_C", {"Crossbow", EntityType::Item, 0, WeaponType::SR}},

	{"Item_Weapon_Mk14_C", {"Mk14", EntityType::Item, 0, WeaponType::DMR}},
	{"Item_Weapon_FNFal_C", {"SLR", EntityType::Item, 0, WeaponType::DMR}},
	{"Item_Weapon_Mk12_C", {"Mk12", EntityType::Item, 0, WeaponType::DMR}},
	{"Item_Weapon_SKS_C", {"SKS", EntityType::Item, 0, WeaponType::DMR}},
	{"Item_Weapon_QBU88_C", {"QBU", EntityType::Item, 0, WeaponType::DMR}},
	{"Item_Weapon_Dragunov_C", {"Dragunov", EntityType::Item, 0, WeaponType::DMR}},
	{"Item_Weapon_Mini14_C", {"Mini14", EntityType::Item, 0, WeaponType::DMR}},
	{"Item_Weapon_VSS_C", {"VSS", EntityType::Item, 0, WeaponType::DMR}},

	{"Item_Weapon_OriginS12_C", {"O12", EntityType::Item, 0, WeaponType::SG}},
	{"Item_Weapon_DP12_C", {"DBS", EntityType::Item, 0, WeaponType::SG}},
	{"Item_Weapon_Saiga12_C", {"S12K", EntityType::Item, 0, WeaponType::SG}},
	{"Item_Weapon_Winchester_C", {"S1897", EntityType::Item, 0, WeaponType::SG}},
	{"Item_Weapon_Berreta686_C", {"S686", EntityType::Item, 0, WeaponType::SG}},


	{"Item_Weapon_P90_C", {"P90", EntityType::Item, 0, WeaponType::SMG}},
	{"Item_Weapon_Vector_C", {"Vector", EntityType::Item, 0, WeaponType::SMG}},
	{"Item_Weapon_UZI_C", {"UZI", EntityType::Item, 0, WeaponType::SMG}},
	{"Item_Weapon_UMP_C", {"UMP", EntityType::Item, 0, WeaponType::SMG}},
	{"Item_Weapon_Thompson_C", {"Tommy Gun", EntityType::Item, 0, WeaponType::SMG}},
	{"Item_Weapon_BizonPP19_C", {"PP-19 Bizon", EntityType::Item, 0, WeaponType::SMG}},
	{"Item_Weapon_JS9_C", {"JS9", EntityType::Item, 0, WeaponType::SMG}},
	{"Item_Weapon_MP5K_C", {"MP5K", EntityType::Item, 0, WeaponType::SMG}},
	{"Item_Weapon_MP9_C", {"MP9", EntityType::Item, 0, WeaponType::SMG}},


	{"Item_Weapon_G18_C", {"P18C", EntityType::Item, 0, WeaponType::HG}},
	{"Item_Weapon_StunGun_C", {"Stun Gun", EntityType::Item, 0, WeaponType::HG}},
	{"Item_Weapon_M1911_C", {"P1911", EntityType::Item, 0, WeaponType::HG}},
	{"Item_Weapon_M9_C", {"P92", EntityType::Item, 0, WeaponType::HG}},
	{"Item_Weapon_NagantM1895_C", {"R1895", EntityType::Item, 0, WeaponType::HG}},
	{"Item_Weapon_Rhino_C", {"R45", EntityType::Item, 0, WeaponType::HG}},
	{"Item_Weapon_DesertEagle_C", {"Deagle", EntityType::Item, 0, WeaponType::HG}},
	{"Item_Weapon_vz61Skorpion_C", {"Skorpion", EntityType::Item, 0, WeaponType::HG}},
	{"Item_Weapon_Sawnoff_C", {"Sawed-Off", EntityType::Item, 0, WeaponType::HG}},


	//药品
	{"Item_Heal_MedKit_C", {"Med Kit", EntityType::Item, 0, WeaponType::Drug}},
	{"Item_Heal_FirstAid_C", {"First Aid Kit", EntityType::Item, 0, WeaponType::Drug}},
	{"Item_Heal_Bandage_C", {"Bandage", EntityType::Item, 0, WeaponType::Drug}},
	{"Item_Weapon_TraumaBag_C", {"Medical Kit", EntityType::Item, 0, WeaponType::Drug}},
	{"Item_Weapon_TacPack_C", {"Tactical Kit", EntityType::Item, 0, WeaponType::Drug}},
	{"Item_Boost_AdrenalineSyringe_C", {"Adrenaline", EntityType::Item, 0, WeaponType::Drug}},
	{"Item_Boost_EnergyDrink_C", {"Energy Drink", EntityType::Item, 0, WeaponType::Drug}},
	{"Item_Boost_PainKiller_C", {"Painkiller", EntityType::Item, 0, WeaponType::Drug}},

	//装备
	{"Item_Head_E_01_Lv1_C", {"Helmet Lv1", EntityType::Item, 0, WeaponType::Armor}},
	{"Item_Head_E_02_Lv1_C", {"Helmet Lv1", EntityType::Item, 0, WeaponType::Armor}},
	{"Item_Head_F_01_Lv2_C", {"Helmet Lv2", EntityType::Item, 0, WeaponType::Armor}},
	{"Item_Head_F_02_Lv2_C", {"Helmet Lv2", EntityType::Item, 0, WeaponType::Armor}},
	{"Item_Head_G_01_Lv3_C", {"Helmet Lv3", EntityType::Item, 0, WeaponType::Armor}},

	{"Item_Armor_E_01_Lv1_C", {"Vest Lv1", EntityType::Item, 0, WeaponType::Armor}},
	{"Item_Armor_D_01_Lv2_C", {"Vest Lv2", EntityType::Item, 0, WeaponType::Armor}},
	{"Item_Armor_C_01_Lv3_C", {"Vest Lv3", EntityType::Item, 0, WeaponType::Armor}},

	{"Item_Back_BlueBlocker_Lv1", {"Jammer Pack Lv1", EntityType::Item, 0, WeaponType::Armor}},
	{"Item_Back_BlueBlocker", {"Jammer Pack Lv2", EntityType::Item, 0, WeaponType::Armor}},
	{"Item_Back_BlueBlocker_Lv3", {"Jammer Pack Lv3", EntityType::Item, 0, WeaponType::Armor}},
	{"Item_Back_E_02_Lv1_C", {"Backpack Lv1", EntityType::Item, 0, WeaponType::Armor}},
	{"Item_Back_E_01_Lv1_C", {"Backpack Lv1", EntityType::Item, 0, WeaponType::Armor}},
	{"Item_Back_F_02_Lv2_C", {"Backpack Lv2", EntityType::Item, 0, WeaponType::Armor}},
	{"Item_Back_F_01_Lv2_C", {"Backpack Lv2", EntityType::Item, 0, WeaponType::Armor}},
	{"Item_Back_C_02_Lv3_C", {"Backpack Lv3", EntityType::Item, 0, WeaponType::Armor}},
	{"Item_Back_C_01_Lv3_C", {"Backpack Lv3", EntityType::Item, 0, WeaponType::Armor}},
	{"Item_Back_B_08_Lv3_C", {"Backpack Lv3", EntityType::Item, 0, WeaponType::Armor}},

	{"Item_Ghillie_01_C", {"Ghillie Suit", EntityType::Item, 0, WeaponType::Armor}},
	{"Item_Ghillie_02_C", {"Ghillie Suit", EntityType::Item, 0, WeaponType::Armor}},
	{"Item_Ghillie_03_C", {"Ghillie Suit", EntityType::Item, 0, WeaponType::Armor}},
	{"Item_Ghillie_04_C", {"Ghillie Suit", EntityType::Item, 0, WeaponType::Armor}},
	{"Item_Ghillie_05_C", {"Ghillie Suit", EntityType::Item, 0, WeaponType::Armor}},
	{"Item_Ghillie_06_C", {"Ghillie Suit", EntityType::Item, 0, WeaponType::Armor}},

	//枪口
	{"Item_Attach_Weapon_Muzzle_Compensator_Large_C", {"Compensator (AR)", EntityType::Item, 0, WeaponType::Muzzle} },
	{"Item_Attach_Weapon_Muzzle_FlashHider_Large_C", {"Flash Hider (AR)", EntityType::Item, 0, WeaponType::Muzzle} },
	{"Item_Attach_Weapon_Muzzle_Suppressor_Large_C", {"Suppressor (AR/DMR)", EntityType::Item, 0, WeaponType::Muzzle} },
	{"Item_Attach_Weapon_Muzzle_AR_MuzzleBrake_C", {"Muzzle Brake (AR/DMR)", EntityType::Item, 0, WeaponType::Muzzle} },

	{"Item_Attach_Weapon_Muzzle_Compensator_SniperRifle_C", {"Compensator (SR)", EntityType::Item, 0, WeaponType::Muzzle} },
	{"Item_Attach_Weapon_Muzzle_FlashHider_SniperRifle_C", {"Flash Hider (SR)", EntityType::Item, 0, WeaponType::Muzzle} },
	{"Item_Attach_Weapon_Muzzle_Suppressor_SniperRifle_C", {"Suppressor (SR)", EntityType::Item, 0, WeaponType::Muzzle} },

	{"Item_Attach_Weapon_Muzzle_Compensator_Medium_C", {"Compensator (SMG)", EntityType::Item, 0, WeaponType::Muzzle} },
	{"Item_Attach_Weapon_Muzzle_FlashHider_Medium_C", {"Flash Hider (SMG)", EntityType::Item, 0, WeaponType::Muzzle} },
	{"Item_Attach_Weapon_Muzzle_Suppressor_Medium_C", {"Suppressor (SMG)", EntityType::Item, 0, WeaponType::Muzzle} },

	{"Item_Attach_Weapon_Muzzle_Choke_C", {"Choke (Shotgun)", EntityType::Item, 0, WeaponType::Muzzle} },
	{"Item_Attach_Weapon_Muzzle_Duckbill_C", {"Duckbill (Shotgun)", EntityType::Item, 0, WeaponType::Muzzle} },

	//瞄具
	{"Item_Attach_Weapon_SideRail_DotSight_RMR_C", {"Canted Sight", EntityType::Item, 0, WeaponType::Sight} },
	{"Item_Attach_Weapon_Upper_DotSight_01_C", {"Red Dot", EntityType::Item, 0, WeaponType::Sight} },
	{"Item_Attach_Weapon_Upper_Holosight_C", {"Holo Sight", EntityType::Item, 0, WeaponType::Sight} },
	{"Item_Attach_Weapon_Upper_Aimpoint_C", {"2x Scope", EntityType::Item, 0, WeaponType::Sight} },
	{"Item_Attach_Weapon_Upper_Scope3x_C", {"3x Scope", EntityType::Item, 0, WeaponType::Sight} },
	{"Item_Attach_Weapon_Upper_ACOG_01_C", {"4x Scope", EntityType::Item, 0, WeaponType::Sight} },
	{"Item_Attach_Weapon_Upper_DualOptic_4x1x_C", {"Hybrid Scope", EntityType::Item, 0, WeaponType::Sight} },
	{"Item_Attach_Weapon_Upper_Scope6x_C", {"6x Scope", EntityType::Item, 0, WeaponType::Sight} },
	{"Item_Attach_Weapon_Upper_CQBSS_C", {"8x Scope", EntityType::Item, 0, WeaponType::Sight} },
	{"Item_Attach_Weapon_Upper_PM2_01_C", {"15x Scope", EntityType::Item, 0, WeaponType::Sight} },
	{"Item_Attach_Weapon_Upper_Thermal_C", {"Thermal Scope", EntityType::Item, 0, WeaponType::Sight} },

	//枪托
	{"Item_Attach_Weapon_Stock_AR_Composite_C", {"Tactical Stock", EntityType::Item, 0, WeaponType::GunButt} },
	{"Item_Attach_Weapon_Stock_AR_HeavyStock_C", {"Heavy Stock", EntityType::Item, 0, WeaponType::GunButt} },
	{"Item_Attach_Weapon_Stock_SniperRifle_CheekPad_C", {"Cheek Pad", EntityType::Item, 0, WeaponType::GunButt} },
	{"Item_Attach_Weapon_Stock_SniperRifle_BulletLoops_C", {"Bullet Loops", EntityType::Item, 0, WeaponType::GunButt} },
	{"Item_Attach_Weapon_Stock_UZI_C", {"Folding Stock", EntityType::Item, 0, WeaponType::GunButt} },
	//握把
	{"Item_Attach_Weapon_Lower_Foregrip_C", {"Vertical Grip", EntityType::Item, 0, WeaponType::Grip}},
	{"Item_Attach_Weapon_Lower_AngledForeGrip_C", {"Angled Grip", EntityType::Item, 0, WeaponType::Grip} },
	{"Item_Attach_Weapon_Lower_TiltedGrip_C", {"Tilted Grip", EntityType::Item, 0, WeaponType::Grip} },
	{"Item_Attach_Weapon_Lower_HalfGrip_C", {"Half Grip", EntityType::Item, 0, WeaponType::Grip} },
	{"Item_Attach_Weapon_Lower_LightweightForeGrip_C", {"Light Grip", EntityType::Item, 0, WeaponType::Grip} },
	{"Item_Attach_Weapon_Lower_ThumbGrip_C", {"Thumb Grip", EntityType::Item, 0, WeaponType::Grip} },
	{"Item_Attach_Weapon_Lower_QuickDraw_Large_Crossbow_C", {"Quiver", EntityType::Item, 0, WeaponType::Grip} },
	{"Item_Attach_Weapon_Lower_LaserPointer_C", {"Laser Sight", EntityType::Item, 0, WeaponType::Grip} },

	//弹匣
	{"Item_Attach_Weapon_Magazine_ExtendedQuickDraw_Large_C", {"Ext. QuickDraw Mag (AR)", EntityType::Item, 0, WeaponType::Magazine}},
	{"Item_Attach_Weapon_Magazine_Extended_Large_C", {"Extended Mag (AR)", EntityType::Item, 0, WeaponType::Magazine}},
	{"Item_Attach_Weapon_Magazine_QuickDraw_Large_C", {"QuickDraw Mag (AR)", EntityType::Item, 0, WeaponType::Magazine} },

	{"Item_Attach_Weapon_Magazine_ExtendedQuickDraw_SniperRifle_C", {"Ext. QuickDraw Mag (SR)", EntityType::Item, 0, WeaponType::Magazine} },
	{"Item_Attach_Weapon_Magazine_Extended_SniperRifle_C", {"Extended Mag (SR)", EntityType::Item, 0, WeaponType::Magazine} },
	{"Item_Attach_Weapon_Magazine_QuickDraw_SniperRifle_C", {"QuickDraw Mag (SR)", EntityType::Item, 0, WeaponType::Magazine} },

	{"Item_Attach_Weapon_Magazine_ExtendedQuickDraw_Medium_C", {"Ext. QuickDraw Mag (SMG)", EntityType::Item, 0, WeaponType::Magazine} },
	{"Item_Attach_Weapon_Magazine_Extended_Medium_C", {"Extended Mag (SMG)", EntityType::Item, 0, WeaponType::Magazine} },
	{"Item_Attach_Weapon_Magazine_QuickDraw_Medium_C", {"QuickDraw Mag (SMG)", EntityType::Item, 0, WeaponType::Magazine} },


	//子弹
	{"Item_Ammo_Mortar_C", {"60mm", EntityType::Item, 0, WeaponType::Bullet} },
	{"Item_Ammo_Bolt_C", {"Bolt", EntityType::Item, 0, WeaponType::Bullet} },
	{"Item_Ammo_Flare_C", {"Flare", EntityType::Item, 0, WeaponType::Bullet} },
	{"Item_Ammo_57mm_C", {"5.7mm", EntityType::Item, 0, WeaponType::Bullet} },
	{"Item_Ammo_300Magnum_C", {".300 Magnum", EntityType::Item, 0, WeaponType::Bullet} },
	{"Item_Ammo_556mm_C", {"5.56mm", EntityType::Item, 0, WeaponType::Bullet} },
	{"Item_Ammo_762mm_C", {"7.62mm", EntityType::Item, 0, WeaponType::Bullet} },
	{"Item_Ammo_40mm_C", {"40mm", EntityType::Item, 0, WeaponType::Bullet} },
	{"Item_Ammo_40mmBluezone_C", {"40mm Bluezone Round", EntityType::Item, 0, WeaponType::Bullet} },
	{"Item_Ammo_9mm_C", {"9mm", EntityType::Item, 0, WeaponType::Bullet} },
	{"Item_Ammo_12Guage_C", {"12 Gauge", EntityType::Item, 0, WeaponType::Bullet} },
	{"Item_Ammo_12GuageSlug_C", {"12 Gauge Slug", EntityType::Item, 0, WeaponType::Bullet} },
	{"Item_Ammo_45ACP_C", {".45", EntityType::Item, 0, WeaponType::Bullet} },
	{"Item_Ammo_ZiplinegunHook_C", {"Rope", EntityType::Item, 0, WeaponType::Bullet} },

	//投掷物
	{"Item_Weapon_C4_C", {"C4", EntityType::Item, 0, WeaponType::Grenade} },
	{"Item_Weapon_BluezoneGrenade_C", {"Bluezone Grenade", EntityType::Item, 0, WeaponType::Grenade} },
	{"Item_Weapon_Grenade_C", {"Grenade", EntityType::Item, 0, WeaponType::Grenade} },
	{"Item_Weapon_FlashBang_C", {"Flashbang", EntityType::Item, 0, WeaponType::Grenade} },
	{"Item_Weapon_StickyGrenade_C", {"Sticky Bomb", EntityType::Item, 0, WeaponType::Grenade} },
	{"Item_Weapon_Molotov_C", {"Molotov", EntityType::Item, 0, WeaponType::Grenade} },
	{"Item_Weapon_SmokeBomb_C", {"Smoke Grenade", EntityType::Item, 0, WeaponType::Grenade} },
	{"Item_Weapon_SpikeTrap_C", {"Spike Trap", EntityType::Item, 0, WeaponType::Grenade} },
	{"Item_Weapon_DecoyGrenade_C", {"Decoy Grenade", EntityType::Item, 0, WeaponType::Grenade} },

	//其它
	{"Item_Weapon_Pan_C", { "Pan", EntityType::Item, 0, WeaponType::Other } },
	{"Item_Weapon_Cowbar_C", { "Crowbar", EntityType::Item, 0, WeaponType::Other } },
	{"Item_Weapon_Sickle_C", { "Sickle", EntityType::Item, 0, WeaponType::Other } },
	{"Item_Weapon_Machete_C", { "Machete", EntityType::Item, 0, WeaponType::Other } },
	{"Item_Weapon_Pickaxe_C", { "Pickaxe", EntityType::Item, 0, WeaponType::Other } },
	{"Item_Weapon_Mortar_C", {"Mortar", EntityType::Item, 0, WeaponType::Other} },
	{"Item_Weapon_Ziplinegun_C", {"Ascender Launcher", EntityType::Item, 0, WeaponType::Other} },
	{"Item_Heal_BattleReadyKit_C", {"Battle Ready Kit", EntityType::Item, 0, WeaponType::Other} },
	{"Item_Weapon_M79_C", {"M79", EntityType::Item, 0, WeaponType::Other} },
	{"Item_Weapon_BZGL_C", {"Bluezone Launcher", EntityType::Item, 0, WeaponType::Other} },
	{"Item_Weapon_PanzerFaust100M_C", {"Panzerfaust", EntityType::Item, 0, WeaponType::Other} },
	{"Item_Weapon_FlareGun_C", {"Flare Gun", EntityType::Item, 0, WeaponType::Other} },
	{"Item_Weapon_PackageFlare_C", {"Emergency Supply Flare", EntityType::Item, 0, WeaponType::Other} },
	{"Item_JerryCan_C", {"Gas Can", EntityType::Item, 0, WeaponType::Other} },
	{"Item_EmergencyPickup_C", {"Emergency Pickup", EntityType::Item, 0, WeaponType::Other} },
	{"Item_Bluechip_C", {"Blue Chip", EntityType::Item, 0, WeaponType::Other} },
	{"Item_Revival_Transmitter_C", {"Blue Chip Transmitter", EntityType::Item, 0, WeaponType::Other} },
	{"Item_BulletproofShield_C", {"Folded Shield", EntityType::Item, 0, WeaponType::Other} },
	{"Item_Tiger_SelfRevive_C", {"Self AED", EntityType::Item, 0, WeaponType::Other} },
	{"InstantRevivalKit_C", {"Instant Revive", EntityType::Item, 0, WeaponType::Other} },
	{"Item_Mountainbike_C", {"Folding Bike", EntityType::Item, 0, WeaponType::Other} },
	{"Item_Rubberboat_C", { "Kayak", EntityType::Item, 0, WeaponType::Other } },
	{"Item_Weapon_Drone_C", {"Drone", EntityType::Item, 0, WeaponType::Other} },
	{"Item_Weapon_Bluebomb_Subzero_C", {"Bluezone Diffuser", EntityType::Item, 0, WeaponType::Other} },
	{"Item_Ghillie_BlueBlocker_C", {"Cold Suit", EntityType::Item, 0, WeaponType::Other} },
	{"Vehicle_Repair_Kit_C", {"Vehicle Repair Kit", EntityType::Item, 0, WeaponType::Other} },
	{"Helmet_Repair_Kit_C", {"Helmet Repair Kit", EntityType::Item, 0, WeaponType::Other} },
	{"Armor_Repair_Kit_C", {"Armor Repair Kit", EntityType::Item, 0, WeaponType::Other} },
	{"Item_Weapon_IntegratedRepair_C", {"All-in-One Repair Kit", EntityType::Item, 0, WeaponType::Other} },
	{"Item_Weapon_Spotter_Scope_C", {"Spotter Scope", EntityType::Item, 0, WeaponType::Other} },
	{"Item_Weapon_TacPack_C", {"Tactical Pack", EntityType::Item, 0, WeaponType::Other} },
	{"Item_Neon_Gold_C", {"Gold Bar", EntityType::Item, 0, WeaponType::Other} },
	{"Item_Neon_Coin_C", {"Trade Coin", EntityType::Item, 0, WeaponType::Other} },

	//钥匙、
	{"Item_Heaven_Key_C", {"Haven Key", EntityType::Item, 0, WeaponType::key} },
	{"Item_Chimera_Key_C", {"Paramo Key", EntityType::Item, 0, WeaponType::key} },
	{"Item_Tiger_Key_C", {"Taego Key", EntityType::Item, 0, WeaponType::key} },
	{"Item_BTSecretRoom_Key_C", {"Erangel Key", EntityType::Item, 0, WeaponType::key} },
	{"Item_DihorOtok_Key_C", {"Vikendi Keycard", EntityType::Item, 0, WeaponType::key} },
	{"Item_Secuity_Keycard_C", {"Deston Keycard", EntityType::Item, 0, WeaponType::key} },
	{"Item_Neon_Key_C", {"Rondo Key", EntityType::Item, 0, WeaponType::key} },
};

std::unordered_map<std::string, EntityInfo, FnvHash> EntityWeaponLists = {
	//武器
	{"WeapFlashBang_C", {"Flashbang", EntityType::Weapon, 0}},
	{"WeapBluezoneGrenade_C", {"Bluezone Grenade", EntityType::Weapon, 0}},
	{"WeapGrenade_C", {"Grenade", EntityType::Weapon, 0,WeaponType::Grenade}},
	{"WeapMortar_C", {"Mortar", EntityType::Weapon, 0,WeaponType::Other}},
	{"WeapStickyGrenade_C", {"Sticky Bomb", EntityType::Weapon, 0}},
	{"WeapC4_C", {"C4", EntityType::Weapon, 0}},
	{"WeapMolotov_C", {"Molotov", EntityType::Weapon, 0}},
	{"WeapSmokeBomb_C", {"Smoke Grenade", EntityType::Weapon, 0}},
	//{"WeapC4_C", {"C4", EntityType::Weapon, 0}},
	{"WeapDecoyGrenade_C", {"Decoy Grenade", EntityType::Weapon, 0}},
	{"WeapBluezoneGrenade_C", {"Zone Grenade", EntityType::Weapon, 0}},
	{"WeapSpikeTrap_C", {"Spike Trap", EntityType::Weapon, 0}},

	// Melee
	{"WeapCowbar_C", {"Crowbar", EntityType::Weapon, 0}},
	{"WeapPan_C", {"Pan", EntityType::Weapon, 0}},
	{"WeapSickle_C", {"Sickle", EntityType::Weapon, 0}},
	{"WeapMachete_C", {"Machete", EntityType::Weapon, 0}},
	{"WeapCowbarProjectile_C", {"Crowbar", EntityType::Weapon, 0}},
	{"WeapMacheteProjectile_C", {"Machete", EntityType::Weapon, 0}},
	{"WeapPanProjectile_C", {"Pan", EntityType::Weapon, 0}},
	{"WeapSickleProjectile_C", {"Sickle", EntityType::Weapon, 0}},

	// AR
	{"WeapLunchmeatsAK47_C", {"AKM", EntityType::Weapon, 0, WeaponType::AR}},
	{"WeapAK47_C", {"AKM", EntityType::Weapon, 0, WeaponType::AR}},
	{"WeapGroza_C", {"Groza", EntityType::Weapon, 0, WeaponType::AR}},
	{"WeapDuncansHK416_C", {"M416", EntityType::Weapon, 0, WeaponType::AR}},
	{"WeapHK416_C", {"M416", EntityType::Weapon, 0, WeaponType::AR}},
	{"WeapM16A4_C", {"M16A4", EntityType::Weapon, 0, WeaponType::AR}},
	{"WeapSCAR-L_C", {"SCAR-L", EntityType::Weapon, 0, WeaponType::AR}},
	{"WeapACE32_C", {"ACE", EntityType::Weapon, 0, WeaponType::AR}},
	{"WeapAUG_C", {"AUG", EntityType::Weapon, 0, WeaponType::AR}},
	{"WeapBerylM762_C", {"BerylM762", EntityType::Weapon, 0, WeaponType::AR}},
	{"WeapG36C_C", {"G36C", EntityType::Weapon, 0, WeaponType::AR}},
	{"WeapQBZ95_C", {"QBZ95", EntityType::Weapon, 0, WeaponType::AR}},
	{"WeapK2_C", {"K2", EntityType::Weapon, 0, WeaponType::AR}},
	{"WeapMk47Mutant_C", {"Mk47", EntityType::Weapon, 0, WeaponType::AR}},
	{"WeapFamasG2_C", {"FAMASI", EntityType::Weapon, 0, WeaponType::AR}},
	// SR
	{"WeapAWM_C", {"AWM", EntityType::Weapon, 0, WeaponType::SR}},
	{"WeapJuliesM24_C", {"M24", EntityType::Weapon, 0, WeaponType::SR}},
	{"WeapM24_C", {"M24", EntityType::Weapon, 0, WeaponType::SR}},
	{"WeapJuliesKar98k_C", {"Kar98k", EntityType::Weapon, 0, WeaponType::SR}},
	{"WeapKar98k_C", {"Kar98k", EntityType::Weapon, 0, WeaponType::SR}},
	{"WeapWin94_C", {"Win94", EntityType::Weapon, 0, WeaponType::SR}},
	{"WeapL6_C", {"Lynx", EntityType::Weapon, 0, WeaponType::SR}},
	{"WeapMosinNagant_C", {"Mosin Nagant", EntityType::Weapon, 0, WeaponType::SR}},
	{"WeapCrossbow_1_C", {"Crossbow", EntityType::Weapon, 0, WeaponType::SR}},

	// SG
	{"WeapOriginS12_C", {"O12", EntityType::Weapon, 0, WeaponType::SG}},
	{"WeapBerreta686_C", {"S686", EntityType::Weapon, 0, WeaponType::SG}},
	{"WeapSaiga12_C", {"S12K", EntityType::Weapon, 0, WeaponType::SG}},
	{"WeapWinchester_C", {"S1897", EntityType::Weapon, 0, WeaponType::SG}},
	{"WeapDP12_C", {"DBS", EntityType::Weapon, 0, WeaponType::SG}},


	// PISTOL
	{"WeapG18_C", {"P18C", EntityType::Weapon, 0, WeaponType::HG}},
	{"WeapM1911_C", {"P1911", EntityType::Weapon, 0, WeaponType::HG}},
	{"WeapM9_C", {"P92", EntityType::Weapon, 0, WeaponType::HG}},
	{"WeapNagantM1895_C", {"R1895", EntityType::Weapon, 0, WeaponType::HG}},
	{"WeapRhino_C", {"R45", EntityType::Weapon, 0, WeaponType::HG}},
	{"WeapDesertEagle_C", {"Deagle", EntityType::Weapon, 0, WeaponType::HG}},
	{"Weapvz61Skorpion_C", {"Skorpion", EntityType::Weapon, 0, WeaponType::HG}},
	{"WeapStunGun_C", {"Stun Gun", EntityType::Weapon, 0, WeaponType::HG}},
	{"WeapSawnoff_C", {"Sawed-Off", EntityType::Weapon, 0, WeaponType::HG}},

	// LMG
	{"WeapM249_C", {"M249", EntityType::Weapon, 0, WeaponType::LMG}},
	{"WeapMG3_C", {"MG3", EntityType::Weapon, 0, WeaponType::LMG}},
	{"WeapDP28_C", {"DP28", EntityType::Weapon, 0, WeaponType::LMG}},

	// DMR
	{"WeapDragunov_C", {"Dragunov", EntityType::Weapon, 0, WeaponType::DMR}},
	{"WeapMini14_C", {"Mini14", EntityType::Weapon, 0, WeaponType::DMR}},
	{"WeapMk14_C", {"Mk14", EntityType::Weapon, 0, WeaponType::DMR}},
	{"WeapSKS_C", {"SKS", EntityType::Weapon, 0, WeaponType::DMR}},
	{"WeapFNFal_C", {"SLR", EntityType::Weapon, 0, WeaponType::DMR}},
	{"WeapMadsFNFal_C", {"SLR", EntityType::Weapon, 0, WeaponType::DMR}},
	{"WeapMadsQBU88_C", {"QBU", EntityType::Weapon, 0, WeaponType::DMR}},
	{"WeapQBU88_C", {"QBU", EntityType::Weapon, 0, WeaponType::DMR}},
	{"WeapMk12_C", {"Mk12", EntityType::Weapon, 0, WeaponType::DMR}},
	{"WeapVSS_C", {"VSS", EntityType::Weapon, 0, WeaponType::DMR}},

	// SMG
	{"WeapThompson_C", {"Tommy Gun", EntityType::Weapon, 0, WeaponType::SMG}},
	{"WeapUMP_C", {"UMP", EntityType::Weapon, 0, WeaponType::SMG}},
	{"WeapUZI_C", {"UZI", EntityType::Weapon, 0, WeaponType::SMG}},
	{"WeapUziPro_C", {"UZI", EntityType::Weapon, 0, WeaponType::SMG}},
	{"WeapVector_C", {"Vector", EntityType::Weapon, 0, WeaponType::SMG}},
	{"WeapBizonPP19_C", {"PP-19 Bizon", EntityType::Weapon, 0, WeaponType::SMG}},
	{"WeapMP5K_C", {"MP5K", EntityType::Weapon, 0, WeaponType::SMG}},
	{"WeapP90_C", {"P90", EntityType::Weapon, 0, WeaponType::SMG}},
	{"WeapJS9_C", {"JS9", EntityType::Weapon, 0, WeaponType::SMG}},
	{"WeapMP9_C", {"MP9", EntityType::Weapon, 0, WeaponType::SMG}},

	// Special
	//{"WeapMortar_C", {"Mortar", EntityType::Weapon, 0}},
	{"WeapFlareGun_C", {"Flare Gun", EntityType::Weapon, 0}},
	{"WeapPanzerFaust100M1_C", {"Panzerfaust", EntityType::Weapon, 0, WeaponType::PanzerFaust100M1} },
	{"WeapJerryCan_C", {"Gas Can", EntityType::Weapon, 0}},
	{"WeapDrone_C", {"Drone", EntityType::Weapon, 0}},
	{"WeapTraumaBag_C", {"Medical Kit", EntityType::Weapon, 0}},
	{"WeapSpotterScope_C", {"Spotter Scope", EntityType::Weapon, 0}},
	{"WeapTacPack_C", {"Tactical Pack", EntityType::Weapon, 0}},
	{"WeapM79_C", {"M79", EntityType::Weapon, 0}},
	{"WeapIntegratedRepair_C", {"All-in-One Repair Kit", EntityType::Weapon, 0} },
	{"WeapZiplinegun_C", {"Ascender Launcher", EntityType::Weapon, 0} },
	{"WeapBZGL_Subzero_C", {"Bluezone Launcher", EntityType::Weapon, 0} },
	{"WeapBluebomb_Subzero_C", {"Bluezone Diffuser", EntityType::Weapon, 0} },

	{"WeapSnowball_C", {"Snowball", EntityType::Weapon, 0} },
	{"WeapApple_C", {"Apple", EntityType::Weapon, 0} },
	{"WeapRock_C", {"Rock", EntityType::Weapon, 0} },

		
};

std::unordered_map<std::string, EntityInfo, FnvHash> EntityVehicleLists = {
	//车辆
	{"BP_EmPickup_Aircraft_C", {"Helicopter", EntityType::Vehicle, 0}},
	{"BP_EmergencyPickupVehicle_C", {"Emergency Pickup", EntityType::Vehicle, 0}},
	{"TransportAircraft_Chimera_C", {"Plane", EntityType::Vehicle, 0}},
	{"BP_Bicycle_C", {"Bicycle", EntityType::Vehicle, 0}},
	{"BP_BRDM_C", {"Armored Vehicle", EntityType::Vehicle, 0}},//不缺
	{"Uaz_Armored_C", {"Jeep", EntityType::Vehicle, 0}},
	{"Uaz_Pillar_C", {"Armored Jeep", EntityType::Vehicle, 0}},
	{"BP_Motorglider_C", {"Motor Glider", EntityType::Vehicle, 0}},
	{"BP_Motorglider_Blue_C", {"Motor Glider", EntityType::Vehicle, 0}},//不缺
	{"BP_Motorglider_Green_C", {"Motor Glider", EntityType::Vehicle, 0}},
	{"BP_Motorglider_Orange_C", {"Motor Glider", EntityType::Vehicle, 0}},
	{"BP_Motorglider_Red_C", {"Motor Glider", EntityType::Vehicle, 0}},
	{"BP_Motorglider_Teal_C", {"Motor Glider", EntityType::Vehicle, 0}},
	{"BP_LootTruck_C", {"Loot Truck", EntityType::Vehicle, 0}},
	{"AquaRail_A_01_C", {"Aquarail", EntityType::Vehicle, 0}},
	{"AquaRail_A_02_C", {"Aquarail", EntityType::Vehicle, 0}},
	{"AquaRail_A_03_C", {"Aquarail", EntityType::Vehicle, 0}},
	{"Boat_PG117_C", {"Speedboat", EntityType::Vehicle, 0}},
	{"PG117_A_01_C", {"Speedboat", EntityType::Vehicle, 0}},
	{"AirBoat_V2_C", {"Motorboat", EntityType::Vehicle, 0}},
	{"BP_M_Rony_A_01_C", {"Pickup Truck", EntityType::Vehicle, 0}},
	{"BP_M_Rony_A_02_C", {"Pickup Truck", EntityType::Vehicle, 0}},
	{"BP_M_Rony_A_03_C", {"Pickup Truck", EntityType::Vehicle, 0}},
	{"BP_Mirado_C", {"Sports Car", EntityType::Vehicle, 0}},
	{"BP_Mirado_A_01_C", {"Sports Car", EntityType::Vehicle, 0}},
	{"BP_Mirado_A_02_C", {"Sports Car", EntityType::Vehicle, 0}},
	{"BP_Mirado_A_03_C", {"Sports Car", EntityType::Vehicle, 0}},
	{"BP_Mirado_A_03_Esports_C", {"Sports Car", EntityType::Vehicle, 0}},
	{"BP_Mirado_A_04_C", {"Sports Car", EntityType::Vehicle, 0}},
	{"BP_Mirado_Open_05_C", {"Sports Car", EntityType::Vehicle, 0}},
	{"BP_Mirado_Open_C", {"Supercar", EntityType::Vehicle, 0}},
	{"BP_Mirado_Open_01_C", {"Supercar", EntityType::Vehicle, 0}},
	{"BP_Mirado_Open_02_C", {"Supercar", EntityType::Vehicle, 0}},
	{"BP_Mirado_Open_03_C", {"Supercar", EntityType::Vehicle, 0}},
	{"BP_Mirado_Open_04_C", {"Supercar", EntityType::Vehicle, 0}},
	{"BP_Motorbike_04_C", {"Motorcycle", EntityType::Vehicle, 0}},
	{"BP_Motorbike_04_Desert_C", {"Motorcycle", EntityType::Vehicle, 0}},
	{"BP_Motorbike_04_SideCar_C", {"Motorcycle", EntityType::Vehicle, 0}},
	{"BP_Motorbike_04_SideCar_Desert_C", {"Motorcycle", EntityType::Vehicle, 0}},
	{"BP_Motorbike_Solitario_C", {"Motorcycle", EntityType::Vehicle, 0}},
	{"BP_Niva_01_C", {"Snow Jeep", EntityType::Vehicle, 0}},
	{"BP_Niva_02_C", {"Snow Jeep", EntityType::Vehicle, 0}},
	{"BP_Niva_03_C", {"Snow Jeep", EntityType::Vehicle, 0}},
	{"BP_Niva_04_C", {"Snow Jeep", EntityType::Vehicle, 0}},
	{"BP_Niva_05_C", {"Snow Jeep", EntityType::Vehicle, 0}},
	{"BP_Niva_06_C", {"Snow Jeep", EntityType::Vehicle, 0}},
	{"BP_Niva_07_C", {"Snow Jeep", EntityType::Vehicle, 0}},
	{"BP_Niva_Esports_C", {"Snow Jeep", EntityType::Vehicle, 0}},
	{"BP_PickupTruck_A_01_C", {"Pickup", EntityType::Vehicle, 0}},
	{"BP_PickupTruck_A_02_C", {"Pickup", EntityType::Vehicle, 0}},
	{"BP_PickupTruck_A_03_C", {"Pickup", EntityType::Vehicle, 0}},
	{"BP_PickupTruck_A_04_C", {"Pickup", EntityType::Vehicle, 0}},
	{"BP_PickupTruck_A_05_C", {"Pickup", EntityType::Vehicle, 0}},
	{"BP_PickupTruck_A_esports_C", {"Pickup", EntityType::Vehicle, 0}},
	{"BP_PickupTruck_B_01_C", {"Pickup", EntityType::Vehicle, 0}},
	{"BP_PickupTruck_B_02_C", {"Pickup", EntityType::Vehicle, 0}},
	{"BP_PickupTruck_B_03_C", {"Pickup", EntityType::Vehicle, 0}},
	{"BP_PickupTruck_B_04_C", {"Pickup", EntityType::Vehicle, 0}},
	{"BP_PickupTruck_B_05_C", {"Pickup", EntityType::Vehicle, 0}},
	{"BP_TukTukTuk_A_01_C", {"Three-Wheeler", EntityType::Vehicle, 0}},
	{"BP_TukTukTuk_A_02_C", {"Three-Wheeler", EntityType::Vehicle, 0}},
	{"BP_TukTukTuk_A_03_C", {"Three-Wheeler", EntityType::Vehicle, 0}},
	{"BP_Van_A_01_C", {"Bus", EntityType::Vehicle, 0}},
	{"BP_Van_A_02_C", {"Bus", EntityType::Vehicle, 0}},
	{"BP_Van_A_03_C", {"Bus", EntityType::Vehicle, 0}},
	{"BP_MiniBus_C", {"Bus", EntityType::Vehicle, 0}},
	{"BP_Scooter_01_A_C", {"Scooter", EntityType::Vehicle, 0}},
	{"BP_Scooter_02_A_C", {"Scooter", EntityType::Vehicle, 0}},
	{"BP_Scooter_03_A_C", {"Scooter", EntityType::Vehicle, 0}},
	{"BP_Scooter_04_A_C", {"Scooter", EntityType::Vehicle, 0}},
	{"BP_Snowbike_01_C", {"Snowmobile", EntityType::Vehicle, 0}},
	{"BP_Snowbike_02_C", {"Snowmobile", EntityType::Vehicle, 0}},
	{"BP_Snowmobile_01_C", {"Snowmobile", EntityType::Vehicle, 0}},
	{"BP_Snowmobile_02_C", {"Snowmobile", EntityType::Vehicle, 0}},
	{"BP_Snowmobile_03_C", {"Snowmobile", EntityType::Vehicle, 0}},
	{"Buggy_A_01_C", {"Buggy", EntityType::Vehicle, 0}},
	{"Buggy_A_02_C", {"Buggy", EntityType::Vehicle, 0}},
	{"Buggy_A_03_C", {"Buggy", EntityType::Vehicle, 0}},
	{"Buggy_A_04_C", {"Buggy", EntityType::Vehicle, 0}},
	{"Buggy_A_05_C", {"Buggy", EntityType::Vehicle, 0}},
	{"Buggy_A_06_C", {"Buggy", EntityType::Vehicle, 0}},
	{"Dacia_A_01_v2_C", {"Sedan", EntityType::Vehicle, 0}},
	{"Dacia_A_01_v2_snow_C", {"Sedan", EntityType::Vehicle, 0}},
	{"Dacia_A_02_v2_C", {"Sedan", EntityType::Vehicle, 0}},
	{"Dacia_A_03_v2_C", {"Sedan", EntityType::Vehicle, 0}},
	{"Dacia_A_03_v2_Esports_C", {"Sedan", EntityType::Vehicle, 0}},
	{"Dacia_A_04_v2_C", {"Sedan", EntityType::Vehicle, 0}},
	{"Uaz_A_01_C", {"Jeep", EntityType::Vehicle, 0}},
	{"Uaz_B_01_C", {"Jeep", EntityType::Vehicle, 0}},
	{"Uaz_C_01_C", {"Jeep", EntityType::Vehicle, 0}},
	{"Uaz_B_01_esports_C", {"Jeep", EntityType::Vehicle, 0}},
	{"BP_Dirtbike_C", {"Dirt Bike", EntityType::Vehicle, 0}},
	{"BP_CoupeRB_C", {"Coupe RB", EntityType::Vehicle, 0}},
	{"BP_ATV_C", {"Quad", EntityType::Vehicle, 0}},
	{"BP_PonyCoupe_C", {"Sports Car", EntityType::Vehicle, 0}},
	{"BP_Porter_C", {"Truck", EntityType::Vehicle, 0}},
	{"BP_Pillar_Car_C", {"Police Car", EntityType::Vehicle, 0}},
	{"BP_Food_Truck_C", {"Food Truck", EntityType::Vehicle, 0}},
	{"BP_Blanc_C", {"Blanc (white)", EntityType::Vehicle, 0}},
	{"BP_Blanc_Esports_C", {"Blanc (black)", EntityType::Vehicle, 0}},
	{"BP_McLarenGT_C", {"McLaren", EntityType::Vehicle, 0}},
	{"ABP_McLarenGT_C", {"McLaren", EntityType::Vehicle, 0} },
	{"BP_McLarenGT_Lx_Yellow_C", {"McLaren", EntityType::Vehicle, 0} },
	{"BP_McLarenGT_St_black_C", {"McLaren", EntityType::Vehicle, 0} },
	{"BP_McLarenGT_St_white_C", {"McLaren", EntityType::Vehicle, 0} },
	{"BP_DBX_LGD_C", {"Aston Martin (luxury SUV)", EntityType::Vehicle, 0} },
	{"BP_Vantage_EP_C", {"Aston Martin (coupe)", EntityType::Vehicle, 0} },
	{"BP_Vantage_LGD_C", {"Aston Martin (luxury coupe)", EntityType::Vehicle, 0} },
	{"BP_PicoBus_C", {"Electric Bus", EntityType::Vehicle, 0} },
	{"BP_PanigaleV4S_EP01_C", {"Ducati (red)", EntityType::Vehicle, 0} },
	{"BP_PanigaleV4S_EP02_C", {"Ducati (black)", EntityType::Vehicle, 0} },
	{"BP_PanigaleV4S_LGD01_C", {"Ducati (green)", EntityType::Vehicle, 0} },
	{"BP_PanigaleV4S_LGD02_C", {"Ducati (rose gold)", EntityType::Vehicle, 0} },
	{"BP_PanigaleV4S_LGD03_C", {"Ducati (twilight pink)", EntityType::Vehicle, 0} },
	{"BP_PanigaleV4S_LGD04_C", {"Ducati (gold)", EntityType::Vehicle, 0} },
	{"BP_Urus_EP_C", {"Lamborghini (SUV)", EntityType::Vehicle, 0} },
	{"BP_Urus_LGD_C", {"Lamborghini (luxury SUV)", EntityType::Vehicle, 0} },
	{"BP_Countach_ULT_C", {"Lamborghini (luxury sports car)", EntityType::Vehicle, 0} },
	{"BP_Classic_01_C", {"Gang", EntityType::Vehicle, 0} },
	{"BP_Classic_02_C", {"Escape", EntityType::Vehicle, 0} },
	{"BP_Rubber_boat_C", {"Kayak", EntityType::Vehicle, 0} },
	{"BP_BearV2_C", {"Bear", EntityType::Vehicle, 0} },
	{"StrongBoxBP_C", {"Safe", EntityType::Vehicle, 0} },
	{"BP_Chiron_LGD_C", {"Bugatti Chiron", EntityType::Vehicle, 0} },
	{"BP_Cayenne_EP_C", {"Porsche Cayenne", EntityType::Vehicle, 0} },
	{"BP_Carrera_LGD_C", {"Porsche 911", EntityType::Vehicle, 0} },
	{"BP_Panamera_ULT_C", {"Porsche Panamera", EntityType::Vehicle, 0} },
	{"BP_Chiron_LGD_C", {"Bugatti", EntityType::Vehicle, 0} },
	{"BP_Special_Sedan_01_C", {"Countach (black)", EntityType::Vehicle, 0} },
	{"BP_Special_Sedan_02_C", {"Countach (white)", EntityType::Vehicle, 0} },
	{"BP_Cayenne_TransiX_C", {"Porsche Cayenne", EntityType::Vehicle, 0} },
	{"BP_Football_C", {"Dummy", EntityType::Vehicle, 0} },
	{ "BP_Vending_machine_Boost_01_C", {"Vending Machine", EntityType::Vehicle, 0} },
	{ "BP_VendingMachine_Heal_01_C", {"Health Station", EntityType::Vehicle, 0} }


};

std::unordered_map<std::string, EntityInfo, FnvHash> EntityProjectLists = {
	//投掷物
	{"ProjGrenade_C", {"Grenade", EntityType::Project, 0}},
	{"ProjBluezoneGrenade_C", {"Bluezone Grenade", EntityType::Project, 0}},
	{"ProjBZGrenade_C", {"Bluezone Grenade", EntityType::Project, 0}},
	{"ProjFlashBang_C", {"Flashbang", EntityType::Project, 0}},
	{"ProjMolotov_C", {"Molotov", EntityType::Project, 0}},
	{"ProjC4_C", {"C4", EntityType::Project, 0}},
};

std::unordered_map<std::string, EntityInfo, FnvHash> EntityOtherLists = {
	{"ScopeAimCamera", {"ScopeAimCamera", EntityType::Other, 0} },
	{"MouseX", {"MouseX", EntityType::Other, 0} },
	{"MouseY", {"MouseY", EntityType::Other, 0} },
};

std::unordered_map<std::string, EntityInfo, FnvHash> EntityLists = {


};


EntityInfo findEntityInfoByID(int id)
{
	return EntityInfo();
}

bool hasEntityWithIDZero()
{
	return false;
}

void EntityInit()
{
	EntityLists.clear();
	EntityLists.insert(EntityOtherLists.begin(), EntityOtherLists.end());
	EntityLists.insert(EntityPlayerLists.begin(), EntityPlayerLists.end());
	EntityLists.insert(EntityWeaponLists.begin(), EntityWeaponLists.end());
	EntityLists.insert(EntityItemLists.begin(), EntityItemLists.end());
	EntityLists.insert(EntityVehicleLists.begin(), EntityVehicleLists.end());
	EntityLists.insert(EntityProjectLists.begin(), EntityProjectLists.end());
}
