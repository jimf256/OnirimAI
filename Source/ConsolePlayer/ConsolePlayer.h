#pragma once
#include "PlayerInterface.h"

// -------------------------------------------------------------------------------------------------

class ConsolePlayer : public PlayerInterface
{
public:
	ConsolePlayer();
	virtual ~ConsolePlayer();

	virtual void OnGameStarted(const PublicGameState& state);
	virtual void OnGameEnded(const PublicGameState& state, EGameResult result);

	virtual void OnCardDrawn(const Card& card);
	virtual void OnLabrynthModified(const CardCollection& labrynth);
	virtual void OnDoorModified(EColor color, EDoorModification modification);

	virtual void ResolveTurnAction(const PublicGameState& state, ETurnAction& choice, std::size_t& handIndex);
	virtual void ResolveNightmareCard(const PublicGameState& state, EResolveNightmareAction& choice, EColor& color);
	virtual void ResolveDoorCard(const PublicGameState& state, const Card& doorCard, EResolveDoorAction& choice);
	virtual void ResolvePremonition(const PublicGameState& state, std::vector<Card>& reorderedCards);
};

// -------------------------------------------------------------------------------------------------

extern "C" __declspec(dllexport) PlayerInterface* CreatePlayerInstance()
{
	return new ConsolePlayer();
}

// -------------------------------------------------------------------------------------------------

extern "C" __declspec(dllexport) void DestroyPlayerInstance(PlayerInterface* player)
{
	delete player;
}

// -------------------------------------------------------------------------------------------------

