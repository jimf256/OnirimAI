#include "PythonPlayer.h"
#include "Semaphore.h"
#include "PublicGameState.h"
#include "LogUtils.h"
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

void SerializeCardVector(const std::vector<Card>& vec, std::stringstream& sstream)
{
	for (auto it = vec.begin(); it != vec.end(); ++it)
	{
		if (it->Type() == ECardType::Nightmare)
		{
			sstream << "NM,";
		}
		else
		{
			sstream << LogUtils::GetColorChar(it->Color()) << LogUtils::GetCardTypeChar(it->Type()) << ",";
		}
	}
}

// -------------------------------------------------------------------------------------------------

void PythonPlayer::SerializeGameState(std::stringstream& sstream, const PublicGameState& state)
{
	static const std::vector<EColor> colors { EColor::Red, EColor::Blue, EColor::Green, EColor::Yellow };
	static const std::vector<ECardType> types { ECardType::Sun, ECardType::Moon, ECardType::Key, ECardType::Door };

	sstream << "deck_remaining=" << state.GetDeckRemaining() << "\n";
	sstream << "nm_remaining=" << state.GetNightmaresRemaining() << "\n";

	// labrynth cards
	sstream << "labrynth=";
	SerializeCardVector(state.GetLabrynth().GetCards(), sstream);
	sstream << "\n";

	// limbo cards
	sstream << "limbo=";
	SerializeCardVector(state.GetLimbo().GetCards(), sstream);
	sstream << "\n";

	// hand cards
	sstream << "hand=";
	SerializeCardVector(state.GetHand().GetCards(), sstream);
	sstream << "\n";

	// door progress
	sstream << "doors=";
	const DoorProgress& doors = state.GetDoorProgress();
	for (auto it = colors.begin(); it != colors.end(); ++it)
	{
		sstream << LogUtils::GetColorChar(*it) << doors.CountDoors(*it) << ",";
	}
	sstream << "\n";

	// discard contents
	sstream << "discard=";
	const CardCounter& discard = state.GetDiscard();
	for (auto colIt = colors.begin(); colIt != colors.end(); ++colIt)
	{
		for (auto typeIt = types.begin(); typeIt != types.end(); ++typeIt)
		{
			sstream << LogUtils::GetColorChar(*colIt) << LogUtils::GetCardTypeChar(*typeIt) << ":"
				<< discard.Get(*colIt, *typeIt) << ",";
		}
	}
	sstream << "NM:" << discard.Get(ECardType::Nightmare) << "\n";
}

// -------------------------------------------------------------------------------------------------

void PythonPlayer::OnGameStarted(const PublicGameState& state)
{
	g_sstream.str("");
	g_sstream << "OnGameStarted\n";
	SerializeGameState(g_sstream, state);
	ExchangeData(g_sstream);
}

// -------------------------------------------------------------------------------------------------

void PythonPlayer::OnGameEnded(const PublicGameState& state, EGameResult result)
{
	g_sstream.str("");
	g_sstream << "OnGameEnded\n" << LogUtils::GetGameResult(result);
	SerializeGameState(g_sstream, state);
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
	SerializeGameState(g_sstream, state);
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
	SerializeGameState(g_sstream, state);
	ExchangeData(g_sstream);

	// discard hand
	choice = EResolveNightmareAction::DiscardHand;
}

// -------------------------------------------------------------------------------------------------

void PythonPlayer::ResolveDoorCard(const PublicGameState& state, const Card& doorCard, EResolveDoorAction& choice)
{
	g_sstream.str("");
	g_sstream << "ResolveDoorCard\n";
	SerializeGameState(g_sstream, state);
	ExchangeData(g_sstream);

	// always discard a key to get a door
	choice = EResolveDoorAction::DiscardKeyCard;
}

// -------------------------------------------------------------------------------------------------

void PythonPlayer::ResolvePremonition(const PublicGameState& state, std::vector<Card>& reorderedCards)
{
	g_sstream.str("");
	g_sstream << "ResolvePremonition\n";
	SerializeGameState(g_sstream, state);

	// also serialize the revealed cards from the deck
	g_sstream << "premonition=";
	SerializeCardVector(reorderedCards, g_sstream);
	g_sstream << "\n";

	ExchangeData(g_sstream);

	// keep the current order
}

// -------------------------------------------------------------------------------------------------
