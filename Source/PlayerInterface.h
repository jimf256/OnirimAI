#pragma once
#include "Enums.h"
#include "Card.h"
#include <vector>

class PublicGameState;

// -------------------------------------------------------------------------------------------------

class PlayerInterface
{
public:
	PlayerInterface() {}

	virtual void GameOver(bool success) = 0;
	virtual void CardDrawn(const Card& card) = 0;
	virtual void DoorModified(EColor color, bool added) = 0;
	virtual void ResolveTurnAction(const PublicGameState& state, ETurnAction& choice, std::size_t& handIndex) = 0;
	virtual void ResolveNightmareCard(const PublicGameState& state, EResolveNightmareAction& choice, EColor& color) = 0;
	virtual void ResolveDoorCard(const PublicGameState& state, const Card& doorCard, EResolveDoorAction& choice) = 0;
	virtual void ResolvePremonition(const PublicGameState& state, std::vector<Card>& reorderedCards) = 0;
};

// -------------------------------------------------------------------------------------------------
