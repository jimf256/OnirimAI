#include <iostream>
#include "GameLogic.h"
#include "PlayerInterface.h"
#include "Logging.h"
#include "CommandLine.h"
#include "Random.h"
#include <string>
#include <cassert>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// -------------------------------------------------------------------------------------------------

// define function pointer types for the player dll functions
typedef PlayerInterface* (*CreatePlayerFunc)();
typedef void (*DestroyPlayerFunc)(PlayerInterface*);
static CreatePlayerFunc g_createPlayer = nullptr;
static DestroyPlayerFunc g_destroyPlayer = nullptr;

// -------------------------------------------------------------------------------------------------

bool LoadPlayerLibrary(std::string moduleName)
{
	#if _DEBUG
	moduleName += "_d";
	#endif
	moduleName += ".dll";

	SetDllDirectoryA("Libraries");
	HMODULE dll = LoadLibraryA(moduleName.c_str());
	if (dll != NULL)
	{
		g_createPlayer = (CreatePlayerFunc)GetProcAddress(dll, "CreatePlayerInstance");
		g_destroyPlayer = (DestroyPlayerFunc)GetProcAddress(dll, "DestroyPlayerInstance");
		assert(g_createPlayer != nullptr);
		assert(g_destroyPlayer != nullptr);
	}

	return (g_createPlayer != nullptr && g_destroyPlayer != nullptr);
}

// -------------------------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
	CommandLine::Set(argc, argv);

	// read the player module name from the commandline
	std::string playerModuleName = "";
	if (CommandLine::GetNumArgs() > 1)
	{
		playerModuleName = CommandLine::GetArg(1);
	}

	// set the random seed from the commandline
	// if not specified it will use a non-deterministic seed
	int randomSeed = 0;
	if (CommandLine::FindValue("seed", randomSeed))
	{
		Random::SetSeed(randomSeed);
	}

	// attempt to create a player instance then start game logic
	if (LoadPlayerLibrary(playerModuleName))
	{
		PlayerInterface* player = g_createPlayer();
		if (player != nullptr)
		{
			auto logLambda = [](const std::string& str) { OutputDebugStringA(str.c_str()); };
			GameLogic logic(*player, logLambda);
			logic.Run();
			g_destroyPlayer(player);

			// return the game result as the exit code
			return static_cast<int>(logic.GetResult());
		}
		else
		{
			std::cout << "failed to create player instance\n";
			system("pause");
		}
	}
	else
	{
		std::cout << "failed to load player library: \"" << playerModuleName << "\"\n";
		system("pause");
	}

	return 0;
}

// -------------------------------------------------------------------------------------------------
