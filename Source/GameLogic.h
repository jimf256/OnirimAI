#pragma once
#include "Enums.h"
#include <memory>
#include <functional>

class Card;
class PlayerInterface;
class InternalGameState;
class PublicGameState;

// -------------------------------------------------------------------------------------------------

typedef std::function<void()> ResolveTurnEvent;
typedef std::function<void()> ResolveNightmareCardEvent;
typedef std::function<void()> ResolveDoorCardEvent;
typedef std::function<void()> ResolvePremonitionEvent;

// -------------------------------------------------------------------------------------------------

class GameLogic
{
public:
	GameLogic(PlayerInterface& player);
	~GameLogic();

	const PublicGameState* GetGameState() const;

	void Run();

private:
	void InitializeDeck();
	void DrawCards(bool nonLocationCardsToLimbo);
	void ShuffleLimboCards();
	void ResolveTurn();
	void ResolveNightmareCard(const Card& card);
	void ResolveDoorCard(const Card& card);
	void ResolvePremonition();
	void CheckForLabrynthDoor();

private:
	bool m_inProgress = false;
	PlayerInterface& m_player;
	std::unique_ptr<InternalGameState> m_gameState;
};

// -------------------------------------------------------------------------------------------------
