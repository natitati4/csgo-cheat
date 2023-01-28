#include <Windows.h>
#include <iostream>
#include "offsets.h"

DWORD WINAPI MainThread(HMODULE hModule)
{
	char home_pressed = 0;

	DWORD imageBase = (DWORD)GetModuleHandle("client.dll");

	while (!GetAsyncKeyState(VK_END))
	{
		if (GetAsyncKeyState(VK_HOME) & 1)
			home_pressed = home_pressed ^ 1; // 1 if wasn't pressed, 0 if was pressed (basically changing, toggling).

		if (!home_pressed)
			continue;

		DWORD LocalPlayer = *(DWORD*)(imageBase + dwLocalPlayer);
		DWORD GlowObjectManager = *(DWORD*)(imageBase + dwGlowObjectManager);
		DWORD EntityList = *(DWORD*)(imageBase + dwEntityList);

		int localTeam = *(int*)(LocalPlayer + m_iTeamNum);

		for (int i = 1; i < 32; i++)
		{
			DWORD entity = *(DWORD*)(imageBase + dwEntityList + i * 0x10);

			if (entity == NULL)
				continue;

			int glowIndex = *(int*)(entity + m_iGlowIndex);
			int entityTeam = *(int*)(entity + m_iTeamNum);

			if (entityTeam == localTeam)
			{
				// local team (rgb)
				*(float*)((GlowObjectManager + glowIndex * 0x38 + 0x8)) = 0.f; // red
				*(float*)((GlowObjectManager + glowIndex * 0x38 + 0xC)) = 1.f; // green
				*(float*)((GlowObjectManager + glowIndex * 0x38 + 0x10)) = 0.f; // blue
				*(float*)((GlowObjectManager + glowIndex * 0x38 + 0x14)) = 2.0f; // alpha
			}

			else
			{
				// enemy team (rgb)
				*(float*)((GlowObjectManager + glowIndex * 0x38 + 0x8)) = 1.f; // red
				*(float*)((GlowObjectManager + glowIndex * 0x38 + 0xC)) = 0.f; // green
				*(float*)((GlowObjectManager + glowIndex * 0x38 + 0x10)) = 0.f; // blue
				*(float*)((GlowObjectManager + glowIndex * 0x38 + 0x14)) = 2.0f; // alpha
			}

			*(bool*)((GlowObjectManager + glowIndex * 0x38 + 0x28)) = true;
			*(bool*)((GlowObjectManager + glowIndex * 0x38 + 0x29)) = false;

		}
	}

	FreeLibraryAndExitThread(hModule, 0);
	return 0;
}

BOOL APIENTRY DllMain(
	HINSTANCE hinstDLL,  // handle to DLL module
	DWORD fdwReason,     // reason for calling function
	LPVOID lpvReserved)  // reserved
{
	// Perform actions based on the reason for calling.
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		// Initialize once for each new process.
		// Return FALSE to fail DLL load.

		CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)MainThread, hinstDLL, 0, nullptr));
		break;

	case DLL_THREAD_ATTACH:
		// Do thread-specific initialization.
		break;

	case DLL_THREAD_DETACH:
		// Do thread-specific cleanup.
		break;

	case DLL_PROCESS_DETACH:
		// Perform any necessary cleanup.
		break;
	}
	return TRUE;  // Successful DLL_PROCESS_ATTACH.
}