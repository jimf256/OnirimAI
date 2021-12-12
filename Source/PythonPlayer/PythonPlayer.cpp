#include "PythonPlayer.h"
#include "PublicGameState.h"
#include "LogUtils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

// -------------------------------------------------------------------------------------------------

PythonPlayer::PythonPlayer()
{
}

// -------------------------------------------------------------------------------------------------

PythonPlayer::~PythonPlayer()
{
}

// -------------------------------------------------------------------------------------------------

const std::string kCppFilename = "cpp_data.txt";
const std::string kPythonFilename = "python_data.txt";
const std::string kCppWaitingSignalFilename = "cpp_waiting_signal.txt";
const std::string kPythonWaitingSignalFilename = "python_waiting_signal.txt";
const std::string kEventStartedSignalFilename = "event_started_signal.txt";
const std::string kEventCompletedSignalFilename = "event_completed_signal.txt";
const std::string kGameEndedSignalFilename = "game_over_signal.txt";

// -------------------------------------------------------------------------------------------------

void CreateSignalFile(const std::string& filename)
{
	std::ofstream file(filename);
	if (file.is_open())
	{
		file.flush();
		file.close();
	}
}

// -------------------------------------------------------------------------------------------------

void WaitForSignalFile(const std::string& filename)
{
	while (true)
	{
		std::ifstream file(filename);
		if (file.is_open())
		{
			file.close();
			break;
		}
	}
}

// -------------------------------------------------------------------------------------------------

void DeleteSignalFile(const std::string& filename)
{
	std::remove(filename.c_str());
}

// -------------------------------------------------------------------------------------------------

void WriteCppData(std::stringstream& sstream)
{
	std::ofstream file(kCppFilename);
	if (file.is_open())
	{
		file << sstream.rdbuf();
		file.flush();
		file.close();
	}
}

// -------------------------------------------------------------------------------------------------

void ReadPythonData(std::stringstream& sstream)
{
	sstream.str("");
	std::ifstream file(kPythonFilename);
	if (file.is_open())
	{
		sstream << file.rdbuf();
		file.close();
	}
}

// -------------------------------------------------------------------------------------------------

void ExchangeData(std::stringstream& sstream)
{
	CreateSignalFile(kEventStartedSignalFilename);
	WaitForSignalFile(kPythonWaitingSignalFilename);
	DeleteSignalFile(kEventStartedSignalFilename);

	WriteCppData(sstream);

	CreateSignalFile(kCppWaitingSignalFilename);
	WaitForSignalFile(kEventCompletedSignalFilename);
	DeleteSignalFile(kCppWaitingSignalFilename);

	ReadPythonData(sstream);
}

// -------------------------------------------------------------------------------------------------

void PythonPlayer::OnGameStarted(const PublicGameState& state)
{
	std::stringstream sstream;
	sstream << "OnGameStarted";
	ExchangeData(sstream);
	std::cout << "python sent: " << sstream.rdbuf() << "\n";
}

// -------------------------------------------------------------------------------------------------

void PythonPlayer::OnGameEnded(const PublicGameState& state, EGameResult result)
{
	std::stringstream sstream;
	sstream << "OnGameEnded\n" << LogUtils::GetGameResult(result);
	ExchangeData(sstream);
	std::cout << "python sent: " << sstream.rdbuf() << "\n";

	CreateSignalFile(kGameEndedSignalFilename);
}

// -------------------------------------------------------------------------------------------------

void PythonPlayer::OnCardDrawn(const Card& card)
{
}

// -------------------------------------------------------------------------------------------------

void PythonPlayer::OnLabrynthModified(const CardCollection& labrynth)
{
}

// -------------------------------------------------------------------------------------------------

void PythonPlayer::OnDoorModified(EColor color, EDoorModification modification)
{
}

// -------------------------------------------------------------------------------------------------

void PythonPlayer::ResolveTurnAction(const PublicGameState& state, ETurnAction& choice, std::size_t& handIndex)
{
	std::stringstream sstream;
	sstream << "ResolveTurn";
	ExchangeData(sstream);
	std::cout << "python sent: " << sstream.rdbuf() << "\n";

	// discard the first card in hand
	choice = ETurnAction::DiscardCard;
	handIndex = 0;
}

// -------------------------------------------------------------------------------------------------

void PythonPlayer::ResolveNightmareCard(const PublicGameState& state, EResolveNightmareAction& choice, EColor& color)
{
	std::stringstream sstream;
	sstream << "ResolveNightmare";
	ExchangeData(sstream);
	std::cout << "python sent: " << sstream.rdbuf() << "\n";

	// discard hand
	choice = EResolveNightmareAction::DiscardHand;
}

// -------------------------------------------------------------------------------------------------

void PythonPlayer::ResolveDoorCard(const PublicGameState& state, const Card& doorCard, EResolveDoorAction& choice)
{
	std::stringstream sstream;
	sstream << "ResolveDoorCard";
	ExchangeData(sstream);
	std::cout << "python sent: " << sstream.rdbuf() << "\n";

	// always discard a key to get a door
	choice = EResolveDoorAction::DiscardKeyCard;
}

// -------------------------------------------------------------------------------------------------

void PythonPlayer::ResolvePremonition(const PublicGameState& state, std::vector<Card>& reorderedCards)
{
	std::stringstream sstream;
	sstream << "ResolvePremonition";
	ExchangeData(sstream);
	std::cout << "python sent: " << sstream.rdbuf() << "\n";

	// keep the current order
}

// -------------------------------------------------------------------------------------------------
