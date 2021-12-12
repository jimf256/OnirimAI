#include "PlayerTemplate.h"
#include "PublicGameState.h"
#include "LogUtils.h"
#include <iostream>
#include <string>

// -------------------------------------------------------------------------------------------------

PlayerTemplate::PlayerTemplate()
{
}

// -------------------------------------------------------------------------------------------------

PlayerTemplate::~PlayerTemplate()
{
}

// -------------------------------------------------------------------------------------------------

void PlayerTemplate::OnGameStarted(const PublicGameState& state)
{
}

// -------------------------------------------------------------------------------------------------

void PlayerTemplate::OnGameEnded(const PublicGameState& state, EGameResult result)
{
	std::cout << "Game Result: " << LogUtils::GetGameResult(result) << "\n";
	system("pause");
}

// -------------------------------------------------------------------------------------------------

void PlayerTemplate::OnCardDrawn(const Card& card)
{
}

// -------------------------------------------------------------------------------------------------

void PlayerTemplate::OnLabrynthModified(const CardCollection& labrynth)
{
}

// -------------------------------------------------------------------------------------------------

void PlayerTemplate::OnDoorModified(EColor color, EDoorModification modification)
{
}

// -------------------------------------------------------------------------------------------------

void PlayerTemplate::ResolveTurnAction(const PublicGameState& state, ETurnAction& choice, std::size_t& handIndex)
{
	// discard the first card in hand
	choice = ETurnAction::DiscardCard;
	handIndex = 0;
}

// -------------------------------------------------------------------------------------------------

void PlayerTemplate::ResolveNightmareCard(const PublicGameState& state, EResolveNightmareAction& choice, EColor& color)
{
	// discard hand
	choice = EResolveNightmareAction::DiscardHand;
}

// -------------------------------------------------------------------------------------------------

void PlayerTemplate::ResolveDoorCard(const PublicGameState& state, const Card& doorCard, EResolveDoorAction& choice)
{
	// always discard a key to get a door
	choice = EResolveDoorAction::DiscardKeyCard;
}

// -------------------------------------------------------------------------------------------------

void PlayerTemplate::ResolvePremonition(const PublicGameState& state, std::vector<Card>& reorderedCards)
{
	// keep the current order
}

// -------------------------------------------------------------------------------------------------
