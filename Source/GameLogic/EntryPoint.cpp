#include "GameLogic.h"
#include "Enums.h"
#include "PlayerInterface.h"
#include "Logging.h"
#include "LogUtils.h"
#include "CommandLine.h"
#include "Random.h"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
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
	else
	{
		Random::SetNonDeterministicSeed();
	}

	// load the player dll
	int returnCode = 0;
	if (LoadPlayerLibrary(playerModuleName))
	{
		// read how many runs of the game logic to do from the commandline
		int runs = 1;
		if (CommandLine::FindValue("runs", runs) && runs < 1)
		{
			runs = 1;
		}

		std::cout << "using player library: " << playerModuleName << "\n";
		std::cout << "running the game " << runs << ((runs > 1) ? " times" : " time") << "\n\n";

		// run multiple times and cache the results
		std::map<EGameResult, int> results;
		for (int i = 0; i < runs; ++i)
		{
			PlayerInterface* player = g_createPlayer();
			if (player != nullptr)
			{
				// create and run the game logic
				auto logLambda = [](const std::string& str) { OutputDebugStringA(str.c_str()); };
				GameLogic logic(*player, logLambda);
				logic.Run();
				g_destroyPlayer(player);

				// if we were only running a single run we also return the result as the return code, as
				// well as caching it to the results log file
				results[logic.GetResult()] += 1;
				if (runs == 1)
				{
					returnCode = static_cast<int>(logic.GetResult());
				}
			}
			else
			{
				std::cout << "failed to create player instance\n";
				system("pause");
				break;
			}
		}

		// save the results to file and print them to the console
		const std::string kResultsFileName = "results.log";
		std::ofstream resultsFile(kResultsFileName);
		if (resultsFile.is_open())
		{
			for (auto it = results.begin(); it != results.end(); ++it)
			{
				resultsFile << static_cast<int>(it->first) << ": " << it->second << "\n";
				std::cout << LogUtils::GetGameResult(it->first) << ": " << it->second << "\n";
			}
			resultsFile.flush();
			resultsFile.close();
		}
	}
	else
	{
		std::cout << "failed to load player library: \"" << playerModuleName << "\"\n";
		system("pause");
	}

	return returnCode;
}

// -------------------------------------------------------------------------------------------------
