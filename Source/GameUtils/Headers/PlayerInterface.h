#pragma once
#include "Enums.h"
#include "Card.h"
#include <vector>

class CardCollection;
class PublicGameState;

// -------------------------------------------------------------------------------------------------

class PlayerInterface
{
public:
	PlayerInterface() {}
	virtual ~PlayerInterface() {}

	virtual void OnGameStarted(const PublicGameState& state) = 0;
	virtual void OnGameEnded(const PublicGameState& state, EGameResult result) = 0;

	virtual void OnCardDrawn(const Card& card) = 0;
	virtual void OnLabrynthModified(const CardCollection& labrynth) = 0;
	virtual void OnDoorModified(EColor color, EDoorModification modification) = 0;

	virtual void ResolveTurnAction(const PublicGameState& state, ETurnAction& choice, std::size_t& handIndex) = 0;
	virtual void ResolveNightmareCard(const PublicGameState& state, EResolveNightmareAction& choice, EColor& color) = 0;
	virtual void ResolveDoorCard(const PublicGameState& state, const Card& doorCard, EResolveDoorAction& choice) = 0;
	virtual void ResolvePremonition(const PublicGameState& state, std::vector<Card>& reorderedCards) = 0;
};

// -------------------------------------------------------------------------------------------------
