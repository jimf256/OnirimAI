#include "PythonPlayer.h"
#include "PublicGameState.h"
#include "LogUtils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <streambuf>
#include <cassert>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// -------------------------------------------------------------------------------------------------

class ostreambuf : public std::basic_streambuf<char, std::char_traits<char>>
{
public:
	ostreambuf(char_type* buffer, std::streamsize size)
	{
		setp(buffer, buffer, buffer + size);
	}
};

static HANDLE g_fileHandle = INVALID_HANDLE_VALUE;
static HANDLE g_fileMapping = INVALID_HANDLE_VALUE;
static char* g_data = nullptr;

const std::size_t kDataSize = 1024;
const std::string kDataFilename = "shared_data.txt";
const std::string kCppWaitingSignalFilename = "cpp_waiting_signal.txt";
const std::string kPythonWaitingSignalFilename = "python_waiting_signal.txt";
const std::string kEventStartedSignalFilename = "event_started_signal.txt";
const std::string kEventCompletedSignalFilename = "event_completed_signal.txt";
const std::string kGameEndedSignalFilename = "game_over_signal.txt";

// -------------------------------------------------------------------------------------------------

PythonPlayer::PythonPlayer()
{
	// create the file handle, file mapping and mapped view
	g_fileHandle = CreateFileA(kDataFilename.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	assert(g_fileHandle != INVALID_HANDLE_VALUE);
	if (g_fileHandle != INVALID_HANDLE_VALUE)
	{
		g_fileMapping = CreateFileMappingA(g_fileHandle, NULL, PAGE_READWRITE, 0, kDataSize, NULL);
		assert(g_fileMapping != INVALID_HANDLE_VALUE);
		if (g_fileMapping != INVALID_HANDLE_VALUE)
		{
			g_data = reinterpret_cast<char*>(MapViewOfFile(g_fileMapping, FILE_MAP_ALL_ACCESS, 0, 0, kDataSize));
			assert(g_data != nullptr);
		}
	}
}

// -------------------------------------------------------------------------------------------------

PythonPlayer::~PythonPlayer()
{
	// clean up file handle, file mapping and mapped view
	if (g_data != nullptr)
	{
		bool result = UnmapViewOfFile(g_data);
		assert(result);
	}
	if (g_fileMapping != INVALID_HANDLE_VALUE)
	{
		bool result = CloseHandle(g_fileMapping);
	}
	if (g_fileHandle != INVALID_HANDLE_VALUE)
	{
		bool result = CloseHandle(g_fileHandle);
	}
}

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
	if (g_data != nullptr)
	{
		memset(g_data, 0, kDataSize);
		ostreambuf buffer(g_data, kDataSize);
		std::ostream file(&buffer);

		file << sstream.rdbuf();
		file.flush();
	}
}

// -------------------------------------------------------------------------------------------------

void ReadPythonData(std::stringstream& sstream)
{
	if (g_data != nullptr)
	{
		sstream.str("");
		for (int i = 0; i < kDataSize; ++i)
		{
			sstream.put(g_data[i]);
			if (g_data[i] == '\n')
			{
				break;
			}
		}
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
	sstream << "OnGameStarted\n";
	ExchangeData(sstream);
	//std::cout << "python sent: " << sstream.rdbuf() << "\n";
}

// -------------------------------------------------------------------------------------------------

void PythonPlayer::OnGameEnded(const PublicGameState& state, EGameResult result)
{
	std::stringstream sstream;
	sstream << "OnGameEnded\n" << LogUtils::GetGameResult(result);
	ExchangeData(sstream);
	//std::cout << "python sent: " << sstream.rdbuf() << "\n";

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
	sstream << "ResolveTurn\n";
	ExchangeData(sstream);
	//std::cout << "python sent: " << sstream.rdbuf() << "\n";

	// discard the first card in hand
	choice = ETurnAction::DiscardCard;
	handIndex = 0;
}

// -------------------------------------------------------------------------------------------------

void PythonPlayer::ResolveNightmareCard(const PublicGameState& state, EResolveNightmareAction& choice, EColor& color)
{
	std::stringstream sstream;
	sstream << "ResolveNightmare\n";
	ExchangeData(sstream);
	//std::cout << "python sent: " << sstream.rdbuf() << "\n";

	// discard hand
	choice = EResolveNightmareAction::DiscardHand;
}

// -------------------------------------------------------------------------------------------------

void PythonPlayer::ResolveDoorCard(const PublicGameState& state, const Card& doorCard, EResolveDoorAction& choice)
{
	std::stringstream sstream;
	sstream << "ResolveDoorCard\n";
	ExchangeData(sstream);
	//std::cout << "python sent: " << sstream.rdbuf() << "\n";

	// always discard a key to get a door
	choice = EResolveDoorAction::DiscardKeyCard;
}

// -------------------------------------------------------------------------------------------------

void PythonPlayer::ResolvePremonition(const PublicGameState& state, std::vector<Card>& reorderedCards)
{
	std::stringstream sstream;
	sstream << "ResolvePremonition\n";
	ExchangeData(sstream);
	//std::cout << "python sent: " << sstream.rdbuf() << "\n";

	// keep the current order
}

// -------------------------------------------------------------------------------------------------
