#define _PHYSX_DEBUG
#include <winsock2.h>
#include <windows.h>
#include <shellapi.h>
#include <Overlay/Overlay.h>
#include <Common/Data.h>
#include <Utils/Utils.h>
#include <Hack/Hack.h>
#include <Common/Offset.h>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <thread>
#include <vector>
#include <DMALibrary/Memory/Memory.h>
#include <cstdint>
#include <cmath>
#include <CommCtrl.h>
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "Msimg32.lib")

FGameData GameData;
using namespace std;

namespace
{

	std::string Utf8FromWide(const wchar_t* text)
	{
		if (text == nullptr) {
			return {};
		}

		const int size = WideCharToMultiByte(CP_UTF8, 0, text, -1, nullptr, 0, nullptr, nullptr);
		if (size <= 1) {
			return {};
		}

		std::string utf8(static_cast<size_t>(size - 1), '\0');
		WideCharToMultiByte(CP_UTF8, 0, text, -1, utf8.data(), size - 1, nullptr, nullptr);
		return utf8;
	}

	std::string Utf8FromWide(const std::wstring& text)
	{
		return Utf8FromWide(text.c_str());
	}

}

void SetConsoleStyle()
{
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleTitle(L"\u6B22\u8FCE\u4F7F\u7528\u54D2\u54D2\u54D2\uFF0C\u516C\u76CA\u8F6F\u4EF6\uFF0C\u4EC5\u4F9B\u5A31\u4E50\u3002\u4E00\u5207\u540E\u679C\u81EA\u884C\u627F\u62C5\uFF01\uFF01");

	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD dwMode = 0;
	GetConsoleMode(hOut, &dwMode);
	if (!(dwMode & ENABLE_VIRTUAL_TERMINAL_PROCESSING))
	{
		dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		SetConsoleMode(hOut, dwMode);
	}
}

int DENGLU;
//
//static int Refresh()
//{
//	while (true)
//	{
//		mem.RefreshAll();
//
//		Sleep(1000 * 60 * 15);
//	}
//}

BOOL WINAPI ConsoleHandler(DWORD event) {
	if (event == CTRL_CLOSE_EVENT) {
		HWND Progman = FindWindowA("Progman", NULL);
		HWND TrayWnd = FindWindowA("Shell_TrayWnd", NULL);
		ShowWindow(Progman, SW_SHOW);
		ShowWindow(TrayWnd, SW_SHOW);
		return TRUE;
	}
	return FALSE;
}

int main(int argc, char* argv[]) {
	Utils::SetDiagnosticLogging(argc > 1);



	if (!SetConsoleCtrlHandler(ConsoleHandler, TRUE))
	{
		return -1;
	}

	HWND hwnd = GetConsoleWindow();
	if (hwnd != NULL)
	{
		ShowWindow(hwnd, SW_SHOW);
	}

	std::thread OverlayloginThread(&Overlay::login);

	while (!GameData.Config.Window.IsLogin) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	SetConsoleStyle();

	Utils::Log(0, " ");
	Utils::Log(0, U8("  ====================================================================================================="));
	Utils::Log(0, U8("  =                                                                                                   ="));
	Utils::Log(0, U8("  =                           ██    ██░      ██████░       ██   ██░                                  ="));
	Utils::Log(0, U8("  =                           ██    ██░      ██░░░░       ██    ██░                                  ="));
	Utils::Log(0, U8("  =                           ██    ██░      ██████░        ██ ██░░                                  ="));
	Utils::Log(0, U8("  =                           ██    ██░      ██░░░░         ████░                                    ="));
	Utils::Log(0, U8("  =                            ██  ██░       ██████░         ████░                                   ="));
	Utils::Log(0, U8("  =                             ████░        ██░░░░        ██  ██░                                   ="));
	Utils::Log(0, U8("  =                              ██░         ██████░      ██    ██░                                  ="));
	Utils::Log(0, U8("  =                                                                                                   ="));
	Utils::Log(0, U8("  =                                 >>>   V E X  <<<   PUBG                                           ="));
	Utils::Log(0, U8("  ====================================================================================================="));
	Utils::Log(1, Utf8FromWide(L"[-] [OUTPUT] 欢迎使用VEX公益DMA----功能正在启动中........").c_str());


	GameData.Config.Window.IsLogin = true;
	ShowWindow(hwnd, SW_SHOW);
	Config::Load();

	std::thread HackThread(Hack::Init);
	std::thread([] {
		while (true)
		{
			const uint64_t controller = GameData.PlayerController;
			const auto players = Data::GetCachePlayers();
			if (controller >= 0x10000 && controller <= 0x00007FFFFFFFFFFFULL && !players.empty())
			{
				for (uint64_t offset = 0x400; offset <= 0x520; offset += 8)
				{
					const uint64_t candidate = mem.Read<uint64_t>(controller + offset);
					if (players.find(candidate) != players.end())
					{
						if (GameData.Offset["AcknowledgedPawn"] != offset)
							Utils::Log(1, "[OFFSET] AcknowledgedPawn auto-selected: 0x%llX", offset);
						GameData.Offset["AcknowledgedPawn"] = offset;
						GameData.AcknowledgedPawn = candidate;
						break;
					}
				}
			}

			for (uint64_t offset = 0x4C0; offset <= 0x510; offset += 8)
			{
				const uint64_t candidate = mem.Read<uint64_t>(controller + offset);
				if (candidate < 0x10000 || candidate > 0x00007FFFFFFFFFFFULL)
					continue;
				const float fov = mem.Read<float>(candidate + GameData.Offset["CameraCacheFOV"]);
				if (std::isfinite(fov) && fov >= 20.0f && fov <= 180.0f)
				{
					if (GameData.Offset["PlayerCameraManager"] != offset)
						Utils::Log(1, "[OFFSET] PlayerCameraManager auto-selected: 0x%llX (FOV %.1f)", offset, fov);
					GameData.Offset["PlayerCameraManager"] = offset;
					GameData.PlayerCameraManager = candidate;
					break;
				}
			}

			const uint64_t pawn = GameData.AcknowledgedPawn;
			if (pawn >= 0x10000 && pawn <= 0x00007FFFFFFFFFFFULL)
				GameData.CameraViewTarget = pawn;
			Sleep(50);
		}
	}).detach();
	if (Utils::IsDiagnosticLoggingEnabled())
	{
		std::thread([] {
			while (true)
			{
				const auto entities = Data::GetCacheEntitys();
				const auto players = Data::GetCachePlayers();
				static uint64_t dumpedPawn = 0;
				const uint64_t currentPawn = GameData.AcknowledgedPawn;
				if (currentPawn >= 0x10000 && currentPawn <= 0x00007FFFFFFFFFFFULL &&
					currentPawn != dumpedPawn)
				{
					dumpedPawn = currentPawn;
					std::vector<unsigned char> pawnDump(0x4000);
					if (mem.Read(currentPawn, pawnDump.data(), static_cast<DWORD>(pawnDump.size())))
						std::ofstream("pawn_runtime_dump.bin", std::ios::binary | std::ios::trunc)
							.write(reinterpret_cast<const char*>(pawnDump.data()), pawnDump.size());

					const uint64_t weaponProcessor = mem.Read<uint64_t>(currentPawn + GameData.Offset["WeaponProcessor"]);
					if (weaponProcessor >= 0x10000 && weaponProcessor <= 0x00007FFFFFFFFFFFULL)
					{
						std::vector<unsigned char> weaponDump(0x1000);
						if (mem.Read(weaponProcessor, weaponDump.data(), static_cast<DWORD>(weaponDump.size())))
							std::ofstream("weapon_processor_runtime_dump.bin", std::ios::binary | std::ios::trunc)
								.write(reinterpret_cast<const char*>(weaponDump.data()), weaponDump.size());
					}
				}
				const uint64_t giA70 = mem.Read<uint64_t>(GameData.UWorld + 0xA70);
				const uint64_t gi888 = mem.Read<uint64_t>(GameData.UWorld + 0x888);
				const uint64_t level228 = mem.Read<uint64_t>(GameData.UWorld + 0x228);
				const uint64_t level850 = mem.Read<uint64_t>(GameData.UWorld + 0x850);
				Utils::Log(1,
					"[DRAW] scene=%d base=0x%llX world=0x%llX gameInstance=0x%llX gameState=0x%llX level=0x%llX actorArray=0x%llX gnames=0x%llX entities=%zu players=%zu objId=0x%llX localPlayer=0x%llX controller=0x%llX pawn=0x%llX camera=0x%llX selfEntity=0x%llX mesh=0x%llX weaponProcessor=0x%llX weapon=0x%llX health=%.1f weaponIndex=%u giA70=0x%llX gi888=0x%llX level228=0x%llX level850=0x%llX",
					(int)GameData.Scene, GameData.GameBase, GameData.UWorld,
					GameData.GameInstance, GameData.GameState, GameData.CurrentLevel,
					GameData.ActorArray, GameData.GNames, entities.size(), players.size(),
					GameData.Offset["ObjID"], GameData.LocalPlayer, GameData.PlayerController,
					GameData.AcknowledgedPawn, GameData.PlayerCameraManager,
					GameData.LocalPlayerInfo.Entity, GameData.LocalPlayerInfo.MeshComponent,
					GameData.LocalPlayerInfo.WeaponProcessor, GameData.LocalPlayerInfo.CurrentWeapon,
					GameData.LocalPlayerInfo.Health, (unsigned int)GameData.LocalPlayerInfo.CurrentWeaponIndex,
					giA70, gi888, level228, level850);
				Sleep(3000);
			}
		}).detach();
	}

	Overlay::Init(0, DrawMain);
	while (true)
	{
		if (GameData.Config.Window.Main) {
			Sleep(500);
			continue;
		}
		break;
	}

	return 0;
}
