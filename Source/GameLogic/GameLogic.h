#pragma once
#include "Enums.h"
#include "Logging.h"
#include "PublicGameState.h"
#include <memory>

class Card;
class PlayerInterface;
class InternalGameState;

// -------------------------------------------------------------------------------------------------

class GameLogic
{
public:
	GameLogic(PlayerInterface& player, Logging::PlatformLogHook logHook);
	~GameLogic();

	void Run();
	EGameResult GetResult() const { return m_result; }

private:
	void InitializeDeck();
	void DrawCards(bool nonLocationCardsToLimbo);
	void ShuffleLimboCards();
	void ResolveTurn();
	void ResolveNightmareCard(const Card& card);
	void ResolveDoorCard(const Card& card);
	void ResolvePremonition();
	void CheckForLabrynthDoor();

	void OnGameStarted();
	void OnGameEnded();

private:
	bool m_inProgress;
	EGameResult m_result;
	PlayerInterface& m_player;
	std::unique_ptr<InternalGameState> m_gameState;
	PublicGameState m_gameStateCopy;
};

// -------------------------------------------------------------------------------------------------
