#include "PythonPlayer.h"
#include "Semaphore.h"
#include "PublicGameState.h"
#include "LogUtils.h"
#include <sstream>
#include <string>
#include <vector>
#include <cassert>

// -------------------------------------------------------------------------------------------------

static Semaphore g_cppDataAccess("onirim_cpp_data_semaphore");
static Semaphore g_pythonDataAccess("onirim_python_data_semaphore");
static std::stringstream g_cppData;
static std::vector<uint8_t> g_pythonData;

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
	, m_result(EGameResult::Unknown)
{
}

// -------------------------------------------------------------------------------------------------

PythonPlayer::~PythonPlayer()
{
}

// -------------------------------------------------------------------------------------------------

void PythonPlayer::ExchangeData(std::stringstream& cppData, std::vector<std::uint8_t>& pythonData)
{
	m_dataFile.WriteFromStringStream(cppData);
	g_cppDataAccess.Release();
	g_pythonDataAccess.Wait();
	m_dataFile.ReadBytes(pythonData);
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

	sstream << "result=" << static_cast<int>(m_result) << "\n";
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
	g_cppData.str("");
	g_cppData << "OnGameStarted\n";
	SerializeGameState(g_cppData, state);
	ExchangeData(g_cppData, g_pythonData);
}

// -------------------------------------------------------------------------------------------------

void PythonPlayer::OnGameEnded(const PublicGameState& state, EGameResult result)
{
	g_cppData.str("");
	g_cppData << "OnGameEnded\n";
	m_result = result;
	SerializeGameState(g_cppData, state);
	ExchangeData(g_cppData, g_pythonData);
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
	g_cppData.str("");
	g_cppData << "ResolveTurn\n";
	SerializeGameState(g_cppData, state);
	ExchangeData(g_cppData, g_pythonData);

	// extract the turn action and card index from the returned python data
	assert(g_pythonData.size() == 2);
	if (g_pythonData.size() == 2)
	{
		assert(g_pythonData[0] < 2);
		assert(g_pythonData[1] < 5);
		choice = static_cast<ETurnAction>(g_pythonData[0]);
		handIndex = static_cast<std::size_t>(g_pythonData[1]);
	}
	else
	{
		// backup default
		choice = ETurnAction::DiscardCard;
		handIndex = 0;
	}
}

// -------------------------------------------------------------------------------------------------

void PythonPlayer::ResolveNightmareCard(const PublicGameState& state, EResolveNightmareAction& choice, EColor& color)
{
	g_cppData.str("");
	g_cppData << "ResolveNightmare\n";
	SerializeGameState(g_cppData, state);
	ExchangeData(g_cppData, g_pythonData);

	// extract the nightmare action and door/key color from the returned python data
	assert(g_pythonData.size() == 2);
	if (g_pythonData.size() == 2)
	{
		assert(g_pythonData[0] < 4);
		assert(g_pythonData[1] < 4);
		choice = static_cast<EResolveNightmareAction>(g_pythonData[0]);
		color = static_cast<EColor>(g_pythonData[1]);
	}
	else
	{
		// backup default
		choice = EResolveNightmareAction::DiscardHand;
	}
}

// -------------------------------------------------------------------------------------------------

void PythonPlayer::ResolveDoorCard(const PublicGameState& state, const Card& doorCard, EResolveDoorAction& choice)
{
	g_cppData.str("");
	g_cppData << "ResolveDoorCard\n";
	SerializeGameState(g_cppData, state);
	ExchangeData(g_cppData, g_pythonData);

	// extract the door card action from the returned python data
	assert(g_pythonData.size() == 1);
	if (g_pythonData.size() == 1)
	{
		assert(g_pythonData[0] < 2);
		choice = static_cast<EResolveDoorAction>(g_pythonData[0]);
	}
	else
	{
		// backup default
		choice = EResolveDoorAction::DiscardKeyCard;
	}
}

// -------------------------------------------------------------------------------------------------

void PythonPlayer::ResolvePremonition(const PublicGameState& state, std::vector<Card>& reorderedCards)
{
	g_cppData.str("");
	g_cppData << "ResolvePremonition\n";
	SerializeGameState(g_cppData, state);

	// also serialize the revealed cards from the deck
	g_cppData << "premonition=";
	SerializeCardVector(reorderedCards, g_cppData);
	g_cppData << "\n";

	ExchangeData(g_cppData, g_pythonData);

	// extract the premonition card order from the returned python data
	assert(g_pythonData.size() == 5);
	if (g_pythonData.size() == 5)
	{
		std::vector<Card> copy;
		for (std::size_t i = 0; i < g_pythonData.size(); ++i)
		{
			assert(g_pythonData[i] < 5);
			if (g_pythonData[i] < 5)
			{
				copy.push_back(reorderedCards[g_pythonData[i]]);
			}
		}
		if (copy.size() == reorderedCards.size())
		{
			reorderedCards = copy;
		}
	}
	else
	{
		// backup default
	}
}

// -------------------------------------------------------------------------------------------------
