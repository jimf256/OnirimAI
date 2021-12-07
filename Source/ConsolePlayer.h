#pragma once
#include "PlayerInterface.h"

// -------------------------------------------------------------------------------------------------

class ConsolePlayer : public PlayerInterface
{
public:
	ConsolePlayer();

	virtual void GameOver(bool success);
	virtual void CardDrawn(const Card& card);
	virtual void DoorModified(EColor color, bool added);
	virtual void ResolveTurnAction(const PublicGameState& state, ETurnAction& choice, std::size_t& handIndex);
	virtual void ResolveNightmareCard(const PublicGameState& state, EResolveNightmareAction& choice, EColor& color);
	virtual void ResolveDoorCard(const PublicGameState& state, const Card& doorCard, EResolveDoorAction& choice);
	virtual void ResolvePremonition(const PublicGameState& state, std::vector<Card>& reorderedCards);
};

// -------------------------------------------------------------------------------------------------

