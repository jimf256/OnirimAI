#include "AIPlayer.h"
#include "PublicGameState.h"
#include "Logging.h"
#include "LogUtils.h"
#include <iostream>
#include <unordered_map>

// -------------------------------------------------------------------------------------------------

AIPlayer::AIPlayer()
{
}

// -------------------------------------------------------------------------------------------------

void AIPlayer::GameOver(bool success)
{
}

// -------------------------------------------------------------------------------------------------

void AIPlayer::CardDrawn(const Card& card)
{
}

// -------------------------------------------------------------------------------------------------

void AIPlayer::DoorModified(EColor color, bool added)
{
}

// -------------------------------------------------------------------------------------------------

void AIPlayer::ResolveTurnAction(const PublicGameState& state, ETurnAction& choice, std::size_t& handIndex)
{
	// always discard first card
	choice = ETurnAction::DiscardCard;
	handIndex = 0;
}

// -------------------------------------------------------------------------------------------------

void AIPlayer::ResolveNightmareCard(const PublicGameState& state, EResolveNightmareAction& choice, EColor& color)
{
	// always discard hand
	choice = EResolveNightmareAction::DiscardDeck;
}

// -------------------------------------------------------------------------------------------------

void AIPlayer::ResolveDoorCard(const PublicGameState& state, const Card& doorCard, EResolveDoorAction& choice)
{
	// always discard a key
	choice = EResolveDoorAction::DiscardKeyCard;
}

// -------------------------------------------------------------------------------------------------

void AIPlayer::ResolvePremonition(const PublicGameState& state, std::vector<Card>& reorderedCards)
{
	// keep the same order, but avoid discarding a door
	if (reorderedCards[reorderedCards.size() - 1].Type() == ECardType::Door)
	{
		// find the first non-door card
		std::size_t i = 0;
		for (; i < reorderedCards.size(); ++i)
		{
			if (reorderedCards[i].Type() != ECardType::Door)
			{
				break;
			}
		}

		if (i < reorderedCards.size())
		{
			Card temp = reorderedCards[i];
			reorderedCards[i] = reorderedCards[reorderedCards.size() - 1];
			reorderedCards[reorderedCards.size() - 1] = temp;
		}
	}
}

// -------------------------------------------------------------------------------------------------
