#include <iostream>
#include "GameLogic.h"
#include "ConsolePlayer.h"
#include "Logging.h"
#include <Windows.h>

// -------------------------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
	auto logLambda = [](const std::string& str)
	{
		OutputDebugStringA(str.c_str());
	};
	Logging::Initialize("debug.log", logLambda);

	ConsolePlayer player;
	GameLogic logic(player);
	logic.Run();
	return 0;
}

// -------------------------------------------------------------------------------------------------
