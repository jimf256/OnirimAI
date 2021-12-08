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

void AIPlayer::GameOver(EGameResult result)
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
	const std::vector<Card> hand = state.GetHand().GetCards();
	const std::vector<Card>& labrynth = state.GetLabrynth().GetCards();

	// find the last card in the labrynth (if any) and how many cards of the same color have been played
	Card lastCard;
	int colorCount = 0;
	if (labrynth.size() > 0)
	{
		lastCard = *labrynth.rbegin();
		for (auto it = labrynth.rbegin(); it != labrynth.rend(); ++it)
		{
			if (it->Color() != lastCard.Color())
			{
				break;
			}
			colorCount++;
		}
	}

	// for the below: ignore colors for which we already have all doors
	// 1. can we play a card to get a door right now (third in a series)?
	// 2. can we play a sun/moon to get 2/3 of a color
	// 3. do we have a sun/moon of one color in hand and can play one of them?
	// 4. can we discard a sun/moon
	// 5. discard a key

	// check if we can play a card for an immediate door
	if (colorCount % 3 == 2)
	{
		// find a card that matches the current color and has a different type
		for (auto it = hand.begin(); it != hand.end(); ++it)
		{
			if (it->Color() == lastCard.Color() && it->Type() != lastCard.Type())
			{
				// play the card and return
				handIndex = (it - hand.begin());
				choice = ETurnAction::PlayCard;
				return;
			}
		}
	}

	// check if we can play a sun/moon to get two of the same color in the labrynth (2/3 door)
	if (colorCount % 3 == 1)
	{
		// find a card that matches the current color, has a different type, and is a sun or moon
		for (auto it = hand.begin(); it != hand.end(); ++it)
		{
			if (it->Color() == lastCard.Color() && it->Type() != lastCard.Type() && it->Type() != ECardType::Key)
			{
				// play the card and return
				handIndex = (it - hand.begin());
				choice = ETurnAction::PlayCard;
				return;
			}
		}
	}

	// do we have a sun and moon in the same color in hand?  Play one of them
	std::unordered_map<EColor, bool> hasSun;
	std::unordered_map<EColor, bool> hasMoon;
	for (auto it = hand.begin(); it != hand.end(); ++it)
	{
		if (it->Type() == ECardType::Sun)
		{
			hasSun[it->Color()] = true;
		}
		else if (it->Type() == ECardType::Moon)
		{
			hasMoon[it->Color()] = true;
		}
	}
	std::vector<EColor> colors { EColor::Red, EColor::Blue, EColor::Green, EColor::Yellow };
	for (auto it = colors.begin(); it != colors.end(); ++it)
	{
		if (hasSun[*it] && hasMoon[*it])
		{
			ECardType type = (lastCard.Type() == ECardType::Sun) ? ECardType::Moon : ECardType::Sun;
			for (auto it2 = hand.begin(); it2 != hand.end(); ++it2)
			{
				if (it2->Color() == *it && it2->Type() == type)
				{
					// play the card and return
					handIndex = (it2 - hand.begin());
					choice = ETurnAction::PlayCard;
					return;
				}
			}
			break;
		}
	}

	// can we discard a sun?
	for (auto it = hand.begin(); it != hand.end(); ++it)
	{
		if (it->Type() == ECardType::Sun)
		{
			// discard the card and return
			handIndex = (it - hand.begin());
			choice = ETurnAction::DiscardCard;
			return;
		}
	}

	// can we discard a key?
	for (auto it = hand.begin(); it != hand.end(); ++it)
	{
		if (it->Type() == ECardType::Key)
		{
			// discard the card and return
			handIndex = (it - hand.begin());
			choice = ETurnAction::DiscardCard;
			return;
		}
	}

	// just discard the first card
	choice = ETurnAction::DiscardCard;
	handIndex = 0;
}

// -------------------------------------------------------------------------------------------------

void AIPlayer::ResolveNightmareCard(const PublicGameState& state, EResolveNightmareAction& choice, EColor& color)
{
	const std::vector<Card> hand = state.GetHand().GetCards();
	const DoorProgress& doors = state.GetDoorProgress();

	// check if we have any keys in hand
	std::unordered_map<EColor, int> keyCounts;
	int totalKeys = 0;
	int mostKeys = 0;
	EColor keyColor = EColor::None;
	for (auto it = hand.begin(); it != hand.end(); ++it)
	{
		if (it->Type() == ECardType::Key)
		{
			keyCounts[it->Color()]++;
			totalKeys++;

			if (keyCounts[it->Color()] > mostKeys)
			{
				mostKeys = keyCounts[it->Color()];
				keyColor = it->Color();
			}
		}
	}
	if (totalKeys > 0)
	{
		// pick a key to discard
		choice = EResolveNightmareAction::DiscardKey;
		color = keyColor;
	}
	else
	{
		// otherwise discard hand
		choice = EResolveNightmareAction::DiscardDeck;
	}
}

// -------------------------------------------------------------------------------------------------

void AIPlayer::ResolveDoorCard(const PublicGameState& state, const Card& doorCard, EResolveDoorAction& choice)
{
	// always discard a key to get a door
	choice = EResolveDoorAction::DiscardKeyCard;
}

// -------------------------------------------------------------------------------------------------

void AIPlayer::ResolvePremonition(const PublicGameState& state, std::vector<Card>& reorderedCards)
{
	// keep the same order, but avoid discarding a door
	if (reorderedCards[reorderedCards.size() - 1].Type() == ECardType::Door)
	{
		// the fifth card is a door, find the first non-door card to swap with it
		std::size_t i = 0;
		for (; i < reorderedCards.size(); ++i)
		{
			if (reorderedCards[i].Type() != ECardType::Door)
			{
				break;
			}
		}

		// swap the non-door card to the end
		if (i < reorderedCards.size())
		{
			Card temp = reorderedCards[i];
			reorderedCards[i] = reorderedCards[reorderedCards.size() - 1];
			reorderedCards[reorderedCards.size() - 1] = temp;
		}
	}
}

// -------------------------------------------------------------------------------------------------
