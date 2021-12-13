#include "PythonPlayer.h"
#include "Semaphore.h"
#include "PublicGameState.h"
#include "LogUtils.h"
#include <iostream>
#include <sstream>
#include <string>
#include <cassert>

// -------------------------------------------------------------------------------------------------

static Semaphore g_cppDataAccess("onirim_cpp_data_semaphore");
static Semaphore g_pythonDataAccess("onirim_python_data_semaphore");
static std::stringstream g_sstream;

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
	: m_dataFile("shared_data.txt", 1024)
{
}

// -------------------------------------------------------------------------------------------------

PythonPlayer::~PythonPlayer()
{
}

// -------------------------------------------------------------------------------------------------

void PythonPlayer::ExchangeData(std::stringstream& sstream)
{
	m_dataFile.WriteFromStringStream(sstream);
	g_cppDataAccess.Release();
	g_pythonDataAccess.Wait();
	m_dataFile.ReadToStringStream(sstream);
}

// -------------------------------------------------------------------------------------------------

void PythonPlayer::OnGameStarted(const PublicGameState& state)
{
	g_sstream.str("");
	g_sstream << "OnGameStarted\n";
	ExchangeData(g_sstream);
}

// -------------------------------------------------------------------------------------------------

void PythonPlayer::OnGameEnded(const PublicGameState& state, EGameResult result)
{
	g_sstream.str("");
	g_sstream << "OnGameEnded\n" << LogUtils::GetGameResult(result);
	ExchangeData(g_sstream);
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
	g_sstream.str("");
	g_sstream << "ResolveTurn\n";
	ExchangeData(g_sstream);

	// discard the first card in hand
	choice = ETurnAction::DiscardCard;
	handIndex = 0;
}

// -------------------------------------------------------------------------------------------------

void PythonPlayer::ResolveNightmareCard(const PublicGameState& state, EResolveNightmareAction& choice, EColor& color)
{
	g_sstream.str("");
	g_sstream << "ResolveNightmare\n";
	ExchangeData(g_sstream);

	// discard hand
	choice = EResolveNightmareAction::DiscardHand;
}

// -------------------------------------------------------------------------------------------------

void PythonPlayer::ResolveDoorCard(const PublicGameState& state, const Card& doorCard, EResolveDoorAction& choice)
{
	g_sstream.str("");
	g_sstream << "ResolveDoorCard\n";
	ExchangeData(g_sstream);

	// always discard a key to get a door
	choice = EResolveDoorAction::DiscardKeyCard;
}

// -------------------------------------------------------------------------------------------------

void PythonPlayer::ResolvePremonition(const PublicGameState& state, std::vector<Card>& reorderedCards)
{
	g_sstream.str("");
	g_sstream << "ResolvePremonition\n";
	ExchangeData(g_sstream);

	// keep the current order
}

// -------------------------------------------------------------------------------------------------
