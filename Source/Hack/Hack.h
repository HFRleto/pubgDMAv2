#pragma once

#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <Common/Data.h>
#include <DMALibrary/Memory/Memory.h>
#include <thread>
#include <Common/Constant.h>
#include <Utils/Utils.h>
#include <Utils/Throttler.h>
#include <Common/Entitys.h>
#include <Common/Offset.h>
#include <Common/VectorHelper.h>
#include <cstdint>
#include <psapi.h>
#include <Utils/Engine.h>
#include <Utils/ue4math/rotator.h>
#include <Utils/ue4math/transform.h>
#include <Utils/Timer.h>
#include <Hack/KeyState.h>
#include <Hack/Decrypt.h>
#include <Hack/GNames.h>
#include <Hack/AimBot.h>
#include <Hack/Players.h>
#include <Hack/Actors.h>
#include <Hack/Vehicles.h>
#include <Hack/Items.h>
#include <Hack/Projects.h>
#include <Hack/Radar.h>
#include <Hack/Segment.h>
//#include <Hack/Process.h>
#include <Common/Config.h>

#include <Utils/KmBox.h>
#include <Utils/KmBoxNet.h>
#include <Utils/Lurker.h>
#include <Utils/MoBox.h>
#include "Webpageradar.h"
#include "autoRecoil.h"

#include <Hack/VisibleCheck.h>

class Hack
{
public:

	static const char* SceneName(Scene scene)
	{
		switch (scene) {
		case Scene::FindProcess: return "RechercheProcessus";
		case Scene::Lobby: return "Lobby";
		case Scene::Gaming: return "EnPartie";
		}
		return "?";
	}

	// Journalise la chaîne de pointeurs quand la validité d'un maillon change, puis toutes les 10 s
	// tant qu'un maillon est invalide ou qu'on est en partie (avec les compteurs).
	static void LogPointerChain(bool gaming)
	{
		struct Link { const char* Name; uint64_t Value; };
		const Link links[] = {
			{ "UWorld", GameData.UWorld },
			{ "GameInstance", GameData.GameInstance },
			{ "GNames", GameData.GNames },
			{ "GameState", GameData.GameState },
			{ "LocalPlayer", GameData.LocalPlayer },
			{ "PlayerController", GameData.PlayerController },
			{ "AcknowledgedPawn", GameData.AcknowledgedPawn },
			{ "CurrentLevel", GameData.CurrentLevel },
			{ "ActorArray", GameData.ActorArray },
			{ "PlayerCameraManager", GameData.PlayerCameraManager },
		};
		// hors partie, seuls les trois premiers maillons sont lus
		const size_t count = gaming ? std::size(links) : 3;

		static uint32_t lastMask = ~0u;
		static auto lastLog = std::chrono::steady_clock::now();

		uint32_t mask = gaming ? 0x80000000u : 0;
		for (size_t i = 0; i < count; i++) {
			// Utils::ValidPtr renvoie true quand le pointeur est invalide
			if (Utils::ValidPtr(links[i].Value))
				mask |= 1u << i;
		}
		const bool broken = (mask & 0x7FFFFFFFu) != 0;

		const auto now = std::chrono::steady_clock::now();
		if (mask == lastMask && (now - lastLog < 10s || (!broken && !gaming)))
			return;
		lastMask = mask;
		lastLog = now;

		std::string line;
		char buffer[96];
		for (size_t i = 0; i < count; i++) {
			snprintf(buffer, sizeof(buffer), " %s=0x%llX%s", links[i].Name, (unsigned long long)links[i].Value,
				Utils::ValidPtr(links[i].Value) ? "(INVALIDE)" : "");
			line += buffer;
		}
		Utils::Log(broken ? 2 : 1, U8("[CHAINE] scène=%s%s"), SceneName(GameData.Scene), line.c_str());
		if (gaming) {
			Utils::Log(broken ? 2 : 1, U8("[ETAT] entités=%zu joueurs=%zu carte=%s FOV caméra=%.1f"),
				Data::GetCacheEntitys().size(), Data::GetCachePlayers().size(), GameData.MapName.c_str(), GameData.Camera.FOV);
		}
	}

	static void Update()
	{
		auto hScatter = mem.CreateScatterHandle();
		while (true)
		{
			mem.RefreshTLB();

			GameData.UWorld = Decrypt::Xe(mem.Read<uint64_t>(GameData.GameBase + GameData.Offset["UWorld"]));
			GameData.GameInstance = Decrypt::Xe(mem.Read<uint64_t>(GameData.UWorld + GameData.Offset["GameInstance"]));
			GameData.GNames = GNames::GetGNamesPtr();

			if (Utils::ValidPtr(GameData.UWorld) || GameData.Scene != Scene::Gaming)
			{
				LogPointerChain(false);
				Sleep(GameData.ThreadSleep);
				continue;
			}

			GameData.GameState = Decrypt::Xe(mem.Read<uint64_t>(GameData.UWorld + GameData.Offset["GameState"]));
			GameData.LocalPlayer = Decrypt::Xe(mem.Read<uint64_t>(mem.Read<uint64_t>(GameData.GameInstance + GameData.Offset["LocalPlayer"])));
			GameData.PlayerController = Decrypt::Xe(mem.Read<uint64_t>(GameData.LocalPlayer + GameData.Offset["PlayerController"]));
			GameData.AcknowledgedPawn = Decrypt::Xe(mem.Read<uint64_t>(GameData.PlayerController + GameData.Offset["AcknowledgedPawn"]));
			GameData.CurrentLevel = Decrypt::Xe(mem.Read<uint64_t>(GameData.UWorld + GameData.Offset["CurrentLevel"]));
			GameData.ActorArray = Decrypt::Xe(mem.Read<uint64_t>(GameData.CurrentLevel + GameData.Offset["Actors"]));

			const uint64_t viewTargetOffset = GameData.Offset["ViewTarget"];

			uint64_t PlayerCameraManager;
			uint64_t MyHUD;
			BYTE bShowMouseCursor;
			uint64_t PlayerInput;
			uint64_t AntiCheatCharacterSyncManager;
			uint64_t CacheCameraViewTarget = 0;

			mem.AddScatterRead(hScatter, GameData.PlayerController + GameData.Offset["PlayerCameraManager"], &PlayerCameraManager);
			mem.AddScatterRead(hScatter, GameData.PlayerController + GameData.Offset["MyHUD"], &MyHUD);
			mem.AddScatterRead(hScatter, GameData.PlayerController + GameData.Offset["bShowMouseCursor"], &bShowMouseCursor);
			mem.AddScatterRead(hScatter, GameData.PlayerController + GameData.Offset["PlayerInput"], &PlayerInput);
			mem.AddScatterRead(hScatter, GameData.PlayerController + GameData.Offset["AntiCheatCharacterSyncManager"], &AntiCheatCharacterSyncManager);
			if (viewTargetOffset != 0) {
				mem.AddScatterRead(hScatter, GameData.PlayerCameraManager + viewTargetOffset, &CacheCameraViewTarget);
			}
			mem.ExecuteReadScatter(hScatter);

			GameData.PlayerCameraManager = PlayerCameraManager;
			GameData.MyHUD = MyHUD;
			GameData.bShowMouseCursor = bShowMouseCursor == 0x25 ? true : false;
			GameData.PlayerInput = PlayerInput;
			GameData.AntiCheatCharacterSyncManager = AntiCheatCharacterSyncManager;
			LogPointerChain(true);
			GameData.CameraViewTarget = 0;


			if (viewTargetOffset != 0 && !Utils::ValidPtr(CacheCameraViewTarget)) {
				int ViewTargetID = Decrypt::CIndex(mem.Read<int>(CacheCameraViewTarget + GameData.Offset["ObjID"]));
				auto ViewTargetEntityInfo = Data::GetGNameListsByIDItem(ViewTargetID);
				if (ViewTargetEntityInfo.Type == EntityType::Player || ViewTargetEntityInfo.Type == EntityType::AI)
				{
					int TeamID = mem.Read<int>(CacheCameraViewTarget + GameData.Offset["LastTeamNum"]);
					GameData.CameraViewTarget = CacheCameraViewTarget;
					GameData.LocalPlayerTeamID = (TeamID >= 100000) ? (TeamID - 100000) : TeamID;
				}
			}

			Players::UpdatePlayerLists();
			Radar::UpdateWorldMapInfo();

			Sleep(300);
		}
		mem.CloseScatterHandle(hScatter);
	}





	static void ReleaseLoadedModel() {
		PROCESS_MEMORY_COUNTERS_EX pmc;
		GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
		SIZE_T initialMemory = pmc.WorkingSetSize / (1024 * 1024);

		GameData.PhysxLoaderStopRequested = true;
		{
			std::lock_guard<std::mutex> lock(GameData.physxLoaderMutex);
			if (GameData.PhysxLoaderThread && GameData.PhysxLoaderThread->joinable()) {
				GameData.PhysxLoaderThread->join();
			}
			GameData.PhysxLoaderThread.reset();
		}

		{
			std::lock_guard<std::mutex> lock(GameData.physxSceneMutex);
			std::atomic_store(&GameData.PhysxScene, std::shared_ptr<Physics::VisibleScene>{});
		}
		std::atomic_store(&GameData.NextHintMeshData, std::shared_ptr<const Physics::SceneGeometryData>{});

		EmptyWorkingSet(GetCurrentProcess());

		GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
		SIZE_T finalMemory = pmc.WorkingSetSize / (1024 * 1024);

		Utils::Log(1, U8("[SCENE] Ressources de la scène libérées. Mémoire : %zu Mo -> %zu Mo (%zd Mo libérés)"),
			initialMemory, finalMemory, initialMemory - finalMemory);
	}

	static void StartLoadMapModel() {
		ReleaseLoadedModel();
		Utils::Log(1, U8("[SCENE] Chargement du modèle de la carte..."));

		if (GameData.Config.ESP.LowModel)
		{
			GameData.Config.ESP.PhysxLoadRadius = 350;
			GameData.Config.ESP.PhysxDynamicRefreshInterval = 900;
			GameData.Config.ESP.HeightFieldMutex = 1800;
			GameData.Config.ESP.PhysxStaticRefreshInterval = 1400;
			GameData.Config.ESP.PhysxRefreshLimit = 96;
		}

		if (GameData.Config.ESP.MediumModel)
		{
			GameData.Config.ESP.PhysxLoadRadius = 500;
			GameData.Config.ESP.PhysxDynamicRefreshInterval = 700;
			GameData.Config.ESP.HeightFieldMutex = 1500;
			GameData.Config.ESP.PhysxStaticRefreshInterval = 1200;
			GameData.Config.ESP.PhysxRefreshLimit = 192;
		}

		if (GameData.Config.ESP.HighModel)
		{
			GameData.Config.ESP.PhysxLoadRadius = 700;
			GameData.Config.ESP.PhysxDynamicRefreshInterval = 500;
			GameData.Config.ESP.HeightFieldMutex = 1200;
			GameData.Config.ESP.PhysxStaticRefreshInterval = 900;
			GameData.Config.ESP.PhysxRefreshLimit = 320;
		}

		{
			std::lock_guard<std::mutex> lock(GameData.physxSceneMutex);
			std::atomic_store(&GameData.PhysxScene, std::make_shared<Physics::VisibleScene>());
		}

		GameData.PhysxLoaderStopRequested = false;
		const auto generation = GameData.PhysxLoaderGeneration.fetch_add(1) + 1;
		{
			std::lock_guard<std::mutex> lock(GameData.physxLoaderMutex);
			GameData.PhysxLoaderThread = std::make_unique<std::thread>(VisibleCheck::RunPhysxLoader, generation);
		}
	}

	static void UpdateCamera()
	{
		Throttler Throttlered;
		auto hScatter = mem.CreateScatterHandle();
		CameraData Camera;
		float DeltaSeconds = 0.f;
		float TimeSeconds = 0.f;

		while (true)
		{
			if (GameData.Scene != Scene::Gaming || !GameData.Config.Overlay.UseThread)
			{
				Sleep(GameData.ThreadSleep);
				continue;
			}

			Throttlered.executeTaskWithSleep("UpdateCameraSleep", std::chrono::milliseconds(4), [] {});

			if (!GameData.Config.Overlay.UseLastFrameCameraCache)
			{
				mem.AddScatterRead(hScatter, GameData.PlayerCameraManager + GameData.Offset["CameraCacheLocation"], &Camera.Location);
				mem.AddScatterRead(hScatter, GameData.PlayerCameraManager + GameData.Offset["CameraCacheRotation"], &Camera.Rotation);
				mem.AddScatterRead(hScatter, GameData.PlayerCameraManager + GameData.Offset["CameraCacheFOV"], &Camera.FOV);
			}
			mem.AddScatterRead(hScatter, GameData.UWorld + GameData.Offset["TimeSeconds"], &TimeSeconds);
			mem.ExecuteReadScatter(hScatter);

			GameData.Camera = Camera;
			GameData.WorldTimeSeconds = TimeSeconds;
		}
		mem.CloseScatterHandle(hScatter);
	}

	static void UpdatePID()
	{
		Throttler throttler;
		bool gameProcessFound = false;
		bool prevGameProcessFound = false;
		int pidFailureCount = 0;
		const int PID_FAILURE_TOLERANCE = 10;
		auto lastHealthyProcessSignal = std::chrono::steady_clock::now();

		GameData.Scene = Scene::FindProcess;

		while (true)
		{
			throttler.executeTask("UpdatePID", 500ms, [&] {
				auto hasCachedProcessSignal = [&]() -> bool {
					if (GameData.PID == 0 || GameData.GameBase == 0 || GameData.Offset["UWorld"] == 0) {
						return false;
					}

					const uint64_t cachedUWorldToken = mem.Read<uint64_t>(GameData.GameBase + GameData.Offset["UWorld"], GameData.PID);
					return cachedUWorldToken > 0;
				};

				DWORD PID = mem.GetTslGamePID();
				if (PID == 0 && hasCachedProcessSignal())
				{
					PID = GameData.PID;
				}

				if (PID == 0)
				{
					pidFailureCount++;
					const bool processSignalExpired = (std::chrono::steady_clock::now() - lastHealthyProcessSignal) >= 8s;
					if (pidFailureCount >= PID_FAILURE_TOLERANCE && processSignalExpired)
					{
						gameProcessFound = false;
						if (prevGameProcessFound) {
							Utils::Log(2, U8("[PROCESSUS] Processus du jeu perdu, réinitialisation de la carte DMA..."));
							mem.Init("", false, false);
							GameData.GameBase = 0;
							GameData.PID = 0;
							Decrypt::DestroyXe();
						}
						mem.RefreshAll();
					}
				}
				else
				{
					lastHealthyProcessSignal = std::chrono::steady_clock::now();
					if (pidFailureCount > 0) {
						pidFailureCount = 0;
					}

					gameProcessFound = true;
					if (!prevGameProcessFound) {
						Utils::Log(1, U8("[PROCESSUS] Processus du jeu détecté, attachement..."));
						EntityLists.clear();
						EntityInit();
						Data::SetGNameLists(EntityLists);
						Data::SetGNameListsByID({});
						GameData.HookBase = mem.GetHookModuleBase();
						KeyState::Init();
						Utils::Log(1, U8("[PROCESSUS] Attaché au jeu : PID=%u base=0x%llX"), (unsigned)GameData.PID, (unsigned long long)GameData.GameBase);
					}
				}
				prevGameProcessFound = gameProcessFound;
			});


			// =========================================================================
			// 锟斤拷锟斤拷锟\u573A\u666F\u72B6\u6001锟斤拷锟竭硷拷锟斤拷锟街诧拷锟戒，锟斤拷锟窖撅拷锟角筹拷锟斤拷壮锟斤拷
			// =========================================================================
			Scene currentRealScene = GameData.Scene;
			if (gameProcessFound && !Utils::ValidPtr(GameData.UWorld))
			{
				int mapID = Decrypt::CIndex(mem.Read<uint64_t>(GameData.UWorld + GameData.Offset["ObjID"]));
				GameData.MapName = GNames::GetNameByID(mapID);
				currentRealScene = Utils::IsLobby(GameData.MapName) ? Scene::Lobby : Scene::Gaming;
			}
			else if (!gameProcessFound)
			{
				currentRealScene = Scene::FindProcess;
			}


			if (currentRealScene != GameData.Scene)
			{
				Scene oldScene = GameData.Scene;
				GameData.Scene = currentRealScene;

				Utils::Log(1, U8("[SCENE] Changement de scène : %s -> %s (carte : %s)"), SceneName(oldScene), SceneName(GameData.Scene), GameData.MapName.c_str());

				if (oldScene == Scene::Gaming && GameData.Scene != Scene::Gaming)
				{
					Utils::Log(1, U8("[SCENE] Sortie de partie détectée, libération du modèle..."));
					ReleaseLoadedModel();

				}

				if (GameData.Scene == Scene::Gaming)
				{
					Utils::Log(0, U8("[SCENE] Entrée en partie : lecture de la liste des joueurs, puis chargement du modèle..."));
					Players::ReadPlayerLists();
					const auto loadDelay = 1s;
					std::this_thread::sleep_for(loadDelay);

					// --- 锟睫改碉\u4FEE\u6539\u70B9 ---
					// 锟狡筹拷锟剿筡u79FB\u9664\u4E86\u5173\u4E8E GameData.Config.ESP.UseEngineVisibilityCheck 锟斤拷锟叫讹拷
					// 锟斤拷锟节伙\u7ACB\u5373\u52A0\u8F7D\u6A21\u578B
					GameData.ReloadModelRequested.store(false);
					StartLoadMapModel();
				}

				if (GameData.Scene == Scene::Lobby || GameData.Scene == Scene::FindProcess)
				{
					if (oldScene == Scene::Lobby) {
						ReleaseLoadedModel();
						//锟狡革拷锟铰\u5730\u56FE\u66F4\u65B0\u540E
						std::thread([]() {
							}).detach();

					}
				}
			}

			if (GameData.ReloadModelRequested.exchange(false))
			{
				if (GameData.Scene == Scene::Gaming)
				{
					Utils::Log(1, U8("[SCENE] Rechargement manuel du modèle de la carte demandé..."));
					StartLoadMapModel();
				}
				else
				{
					Utils::Log(2, U8("[SCENE] Pas en partie : impossible de recharger le modèle."));
				}
			}
			//if (currentRealScene != GameData.Scene)
			//{
			//    Scene oldScene = GameData.Scene;
			//    GameData.Scene = currentRealScene;

			//    Utils::Log(1, U8("[-] [OUTPUT] \u573A\u666F\u72B6\u6001\u5207\u6362: \u4ECE %d -> \u5230 %d"), (int)oldScene, (int)GameData.Scene);

			//    if (oldScene == Scene::Gaming && GameData.Scene != Scene::Gaming)
			//    {
			//        Utils::Log(1, U8("[-] [OUTPUT] \u68C0\u6D4B\u5230\u73A9\u5BB6\u9000\u51FA\u6E38\u620F\uFF0C\u91CA\u653E\u573A\u666F\u6A21\u578B锟斤拷..."));
			//        ReleaseLoadedModel();
			//    }

			//    if (GameData.Scene == Scene::Gaming)
			//    {
			//        Utils::Log(0, U8("[-] [OUTPUT] \u5DF2\u8BFB\u53D6\u5230\u5730\u56FE\uFF0C\u51C6\u5907\u52A0\u8F7D\u6A21\u578B..."));
			//        Players::ReadPlayerLists();
			//        const auto loadDelay = 10s;
			//        std::this_thread::sleep_for(loadDelay);
			//        if (!GameData.Config.ESP.UseEngineVisibilityCheck)
			//        {
			//            StartLoadMapModel();
			//        }
			//        else
			//        {
			//            Utils::Log(1, U8("[-] [OUTPUT] 锟斤拷锟杰匡拷锟斤拷模式锟窖匡拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷模锟酵★拷"));
			//        }
			//    }





			//    if (GameData.Scene == Scene::Lobby || GameData.Scene == Scene::FindProcess)
			//    {
			//        if (oldScene == Scene::Lobby) {
			//            ReleaseLoadedModel();
			//        }
			//    }
			//}
			std::this_thread::sleep_for(50ms);
		}
	}

	//static void UpdatePID()
	//{
	//	Throttler Throttlered;

	//	bool GameProcessFound = false;
	//	bool PrevGameProcessFound = false;

	//	while (true)
	//	{
	//		Throttlered.executeTask("UpdatePID", std::chrono::milliseconds(1500), [&GameProcessFound, &PrevGameProcessFound] {
	//			DWORD PID = mem.GetTslGamePID();
	//			int PIDi = 0;
	//			while (PID == 0)
	//			{
	//				PID = mem.GetTslGamePID();
	//				Sleep(100);
	//				PIDi++;
	//				if (PIDi > 200)
	//				{
	//					break;
	//				}
	//			}
	//			if (PID == 0)
	//			{
	//				mem.RefreshAll();
	//				GameData.Scene = Scene::FindProcess;
	//				GameProcessFound = false;
	//				GameData.GameBase = 0;
	//				GameData.PID = 0;
	//				Decrypt::DestroyXe();

	//				if (GameProcessFound != PrevGameProcessFound) {
	//					Utils::Log(2, U8("[-] [OUTPUT] \u672A\u68C0\u6D4B\u5230\u6E38\u620F\uFF0C\u91CD\u7F6E\u6E38\u620F\u72B6\u6001"));
	//				}
	//			}
	//			else {
	//				GameProcessFound = true;
	//				if (GameProcessFound != PrevGameProcessFound) {
	//					GameData.HookBase = mem.GetHookModuleBase();
	//					Utils::Log(1, U8("[-] [OUTPUT] \u573A\u666F\u8D44\u6E90\u91CA\u653E\u5B8C\u6210[1/3]"));
	//					Utils::Log(1, U8("[-] [OUTPUT] \u573A\u666F\u8D44\u6E90\u91CA\u653E\u5B8C\u6210[2/3]"));
	//					Utils::Log(1, U8("[-] [OUTPUT] \u573A\u666F\u8D44\u6E90\u91CA\u653E\u5B8C\u6210[3/3]"));
	//					Utils::Log(1, U8("[-] [OUTPUT] \u573A\u666F\u8D44\u6E90\u91CA\u653E\u5B8C\u6210\u5931\u8D25\uFF0C\u53EF\u80FD\u5DF2\u9644\u52A0\u6E38\u620F"));
	//					Utils::Log(1, U8("[-] [OUTPUT] \u6E38\u620F\u52A0\u8F7D\u6210\u529F\uFF0C\u5DF2\u9644\u52A0\u5230\u6E38\u620F"));
	//					EntityInit();
	//					Data::SetGNameLists(EntityLists);
	//					Data::SetGNameListsByID({});
	//				}
	//			}
	//			PrevGameProcessFound = GameProcessFound;
	//			});

	//		if (GameData.PID != 0) {
	//			int MapID = Decrypt::CIndex(mem.Read<uint64_t>(GameData.UWorld + GameData.Offset["ObjID"]));
	//			GameData.MapName = GNames::GetNameByID(MapID);

	//			if (Utils::IsLobby(GameData.MapName)) {
	//				GameData.Scene = Scene::Lobby;
	//			}
	//			else {
	//				GameData.Scene = Scene::Gaming;
	//			}
	//		}

	//		if (GameData.Scene != GameData.PreviousScene) {
	//			GameData.PreviousScene = GameData.Scene;
	//			switch (GameData.Scene) {
	//			case Scene::FindProcess:
	//				break;
	//			case Scene::Lobby:
	//				Sleep(GameData.ThreadSleep);
	//				ReleaseLoadedModel();
	//				break;
	//			case Scene::Gaming:
	//				Utils::Log(0, U8("[-] [OUTPUT] 锟窖讹拷取锟斤拷锟斤拷图锟斤拷锟诫开始锟斤拷锟斤拷"));
	//				//Utils::Log(0, "Entered the game scene, World name is [%s]", GameData.MapName.c_str());
	//				Players::ReadPlayerLists();
	//				StartLoadMapModel();
	//				break;
	//			default:
	//				break;
	//			}
	//		}

	//		Sleep(1500);
	//	}
	//}

	static void Init()
	{
		//Offset::Init();

		Offset::Sever_Init();
		Utils::Log(1, U8("[OFFSET] %zu offsets chargés. UWorld=0x%llX GNames=0x%llX XenuineDecrypt=0x%llX"),
			GameData.Offset.size(), (unsigned long long)GameData.Offset["UWorld"],
			(unsigned long long)GameData.Offset["GNames"], (unsigned long long)GameData.Offset["XenuineDecrypt"]);
		Utils::Log(1, U8("[OFFSET] GameInstance=0x%llX GameState=0x%llX LocalPlayer=0x%llX PlayerController=0x%llX CurrentLevel=0x%llX Actors=0x%llX"),
			(unsigned long long)GameData.Offset["GameInstance"], (unsigned long long)GameData.Offset["GameState"],
			(unsigned long long)GameData.Offset["LocalPlayer"], (unsigned long long)GameData.Offset["PlayerController"],
			(unsigned long long)GameData.Offset["CurrentLevel"], (unsigned long long)GameData.Offset["Actors"]);
		if (GameData.Offset["UWorld"] == 0 || GameData.Offset["GameInstance"] == 0 || GameData.Offset["PlayerController"] == 0) {
			Utils::Log(2, U8("[OFFSET] Initialisation échouée : UWorld, GameInstance ou PlayerController vaut 0 dans Offset.h."));
			system("pause");
			return;
		}

		EntityInit();
		Utils::Log(1, U8("[INIT] %zu types d'entités connus."), EntityLists.size());

		for (const auto& item : EntityLists)
		{
			if (item.second.Type == EntityType::Item)
			{
				ItemDetail Detail;
				Detail.Name = item.first;
				Detail.DisplayName = item.second.DisplayName;
				Detail.Type = item.second.WeaponType;

				std::unordered_map<std::string, int> AWeaponGroupOverrides = {
					{"Groza", 1}, {"Beryl M762", 1}, {"M416", 1}, {"ACE32", 1}, {"FAMASI", 1},
					{"AUG", 1}, {"Barrett", 1}, {"M24", 1}, {"AWM", 1}, {"Kar98k", 1},
					{"Mk14", 1}, {"Mk12", 1}, {"SKS", 1}, {"S1897", 1}, {"P90", 1},
					{"SLR", 1}, {"Flare Gun", 1}, {"MG3", 1}
				};

				std::unordered_map<std::string, int> BWeaponGroupOverrides = {
					{"Compensator (AR)", 2}, {"Suppressor (SR)", 2}, {"Red Dot", 2}, {"6x Scope", 2},
					{"8x Scope", 2}, {"Thermal Scope", 2}, {"15x Scope", 2}, {"Tactical Stock", 2},
					{"Vertical Grip", 2}, {"Half Grip", 2}, {"Ext. QuickDraw Mag (AR)", 2}, {"Ext. QuickDraw Mag (SR)", 2},
					{"Extended Mag (SR)", 2}, {"Tactical Stock", 2}, {"Cheek Pad", 2}, {"Bullet Loops", 2}
				};

				std::unordered_map<std::string, int> CWeaponGroupOverrides = {
					{"Vest Lv3", 3}, {"Helmet Lv3", 3}, {"Helmet Lv2", 3}, {"Backpack Lv3", 3},
					{"Jammer Pack", 3}, {"Backpack Lv2", 3}, {"Vest Lv2", 3}, {"Ghillie Suit", 3},
					{"Grenade", 3}, {"Smoke Grenade", 3}, {"Pan", 3}, {"Self AED", 3},
					{"Emergency Pickup", 3}, {"Blue Chip Transmitter", 3}, {"Blue Chip", 3}, {"3-in-1 Repair Kit", 3}
				};

				std::unordered_map<std::string, int> DWeaponGroupOverrides = {
					{"Haven Key", 4}, {"Paramo Key", 4}, {"Taego Key", 4}, {"Erangel Key", 4},
					{"Vikendi Keycard", 4}, {"Deston Keycard", 4}, {"First Aid Kit", 4}, {"Energy Drink", 4},
					{"Painkiller", 4}
				};

				// **锟斤拷始锟斤拷为未锟斤拷锟斤拷**
				int group = 0;

				auto it = AWeaponGroupOverrides.find(Detail.DisplayName);
				if (it != AWeaponGroupOverrides.end()) group = it->second;

				it = BWeaponGroupOverrides.find(Detail.DisplayName);
				if (it != BWeaponGroupOverrides.end()) group = it->second;

				it = CWeaponGroupOverrides.find(Detail.DisplayName);
				if (it != CWeaponGroupOverrides.end()) group = it->second;

				it = DWeaponGroupOverrides.find(Detail.DisplayName);
				if (it != DWeaponGroupOverrides.end()) group = it->second;

				// **直锟斤拷锟斤拷锟斤拷锟斤拷锟秸的凤拷锟斤拷**
				Detail.Group = group;
				GameData.Config.Item.Lists[item.first] = Detail;
			}
		}

		Config::Load();

		if (!mem.Init("TslGame.exe", false, false))
		{
			Utils::Log(2, U8("[DMA] Initialisation échouée : carte DMA absente, pilote manquant ou PC de jeu injoignable."));
			system("pause");
		}
		else {
			Utils::Log(1, U8("[DMA] Carte DMA initialisée, en attente du processus du jeu."));
		}

		KeyState::Init();

		Data::SetGNameLists(EntityLists);
		Data::SetGNameListsByID({});


		Utils::Log(1, U8("[INIT] Démarrage des threads de lecture."));
		std::thread UpdatePIDThread(UpdatePID);
		std::thread UpdateThread(Update);
		std::thread UpdateKeyStateThread(KeyState::Update);
		std::thread UpdateActorsThread(Actors::Update);
		std::thread UpdatePlayersThread(Players::Update);
		//std::thread UpdatePlayerFogsThread(Players::UpdateFogPlayers);
		std::thread UpdateVehiclesThread(Vehicles::Update);
		std::thread AimBotRunThread(AimBot::Run);
		std::thread UpdateCameraThread(UpdateCamera);
		std::thread UpdateItemsThread(Items::Update);
		std::thread UpdateProjectsThread(Projects::Update);
		std::thread UpdateRadarThread(Radar::Update);
		std::thread UpdateSegmentThread(Segment::Update);
		std::thread UpdateWebRadarThread(WebRadar::Rundata);
		std::thread recoilThread(Recoil::autoRecoil);
		UpdatePIDThread.join();

	}
};
