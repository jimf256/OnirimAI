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

struct SemaphoreWrapper
{
	SemaphoreWrapper(const std::string& name)
		: m_name(name)
		, m_handle(NULL)
	{
		m_handle = CreateSemaphoreA(NULL, 0, 1, name.c_str());
		assert(m_handle != NULL);
	}

	~SemaphoreWrapper()
	{
		bool result = CloseHandle(m_handle);
		assert(result);
	}

	void Wait()
	{
		WaitForSingleObject(m_handle, INFINITE);
	}

	void Release()
	{
		ReleaseSemaphore(m_handle, 1, NULL);
	}

private:
	std::string m_name;
	HANDLE m_handle;
};

static SemaphoreWrapper g_cppDataAccess("onirim_cpp_data_semaphore");
static SemaphoreWrapper g_pythonDataAccess("onirim_python_data_semaphore");

// -------------------------------------------------------------------------------------------------

extern "C" __declspec(dllexport) void WaitForCppData()
{
	g_cppDataAccess.Wait();
}

// -------------------------------------------------------------------------------------------------

extern "C" __declspec(dllexport) void SignalPythonData()
{
	g_pythonDataAccess.Release();
}

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
	WriteCppData(sstream);

	g_cppDataAccess.Release();
	g_pythonDataAccess.Wait();

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
