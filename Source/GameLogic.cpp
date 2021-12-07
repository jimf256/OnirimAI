#include "GameLogic.h"
#include "Card.h"
#include "PlayerInterface.h"
#include "InternalGameState.h"
#include "Logging.h"
#include "LogUtils.h"
#include <unordered_map>
#include <cassert>

// -------------------------------------------------------------------------------------------------

GameLogic::GameLogic(PlayerInterface& player)
	: m_player(player)
	, m_inProgress(false)
{
	m_gameState.reset(new InternalGameState());
}

// -------------------------------------------------------------------------------------------------

GameLogic::~GameLogic()
{
}

// -------------------------------------------------------------------------------------------------

void GameLogic::Run()
{
	m_inProgress = true;

	// create the deck
	InitializeDeck();

	// draw initial cards
	DrawCards(true);
	ShuffleLimboCards();

	while (m_inProgress)
	{
		ResolveTurn();
		DrawCards(false);
		ShuffleLimboCards();
	}
}

// -------------------------------------------------------------------------------------------------

void GameLogic::InitializeDeck()
{
	CardCollection& deck = m_gameState->GetDeck();

	// add nightmare cards
	for (int i = 0; i < 10; ++i)
	{
		deck.AddToTop(Card(EColor::None, ECardType::Nightmare));
	}

	// set up card counts for all colors
	std::unordered_map<EColor, std::unordered_map<ECardType, int>> cardCounts;
	cardCounts[EColor::Red] = std::unordered_map<ECardType, int>();
	cardCounts[EColor::Red][ECardType::Sun] = 9;
	cardCounts[EColor::Red][ECardType::Moon] = 4;
	cardCounts[EColor::Red][ECardType::Key] = 3;
	cardCounts[EColor::Red][ECardType::Door] = 2;
	cardCounts[EColor::Blue] = cardCounts[EColor::Red];
	cardCounts[EColor::Blue][ECardType::Sun] = 8;
	cardCounts[EColor::Green] = cardCounts[EColor::Red];
	cardCounts[EColor::Green][ECardType::Sun] = 7;
	cardCounts[EColor::Yellow] = cardCounts[EColor::Red];
	cardCounts[EColor::Yellow][ECardType::Sun] = 6;

	for (auto color_it = cardCounts.begin(); color_it != cardCounts.end(); ++color_it)
	{
		for (auto type_it = color_it->second.begin(); type_it != color_it->second.end(); ++type_it)
		{
			for (int i = 0; i < type_it->second; ++i)
			{
				deck.AddToTop(Card(color_it->first, type_it->first));
			}
		}
	}

	m_gameState->UpdateDeckSize();

	// randomize
	deck.Shuffle();

	// log initial deck state
	LOG("Initial Deck Contents:");
	LOG(LogUtils::GetCollectionContents(deck, false, false));
}

// -------------------------------------------------------------------------------------------------

void GameLogic::DrawCards(bool nonLocationCardsToLimbo)
{
	// draw cards from the deck until the hand is full
	// if nonLocationCardsToLimbo is true, non-location cards are added to limbo instead
	// if nonLocationCardsToLimbo is false, non-location cards will trigger an event that must be resolved

	CardCollection& deck = m_gameState->GetDeck();
	CardCollection& hand = m_gameState->GetHand();
	CardCollection& limbo = m_gameState->GetLimbo();
	DiscardTracker& discard = m_gameState->GetDiscard();
	DoorProgress& doorProgress = m_gameState->GetDoorProgress();

	LOG("");
	LOG("starting draw cards step...");

	const std::size_t kMaxHandSize = 5;
	while (deck.Size() > 0 && hand.Size() < kMaxHandSize && !doorProgress.HasAllDoors() && !discard.DoorWasDiscarded())
	{
		Card card;
		if (deck.RemoveFromTop(card))
		{
			LOG("draw card: " + LogUtils::GetCardName(card));
			m_gameState->UpdateDeckSize();
			m_player.CardDrawn(card);

			if (nonLocationCardsToLimbo)
			{
				if (card.Type() == ECardType::Nightmare || card.Type() == ECardType::Door)
				{
					limbo.AddToBottom(card);
					LOG(LogUtils::GetCardName(card) + " added to limbo");
				}
				else
				{
					hand.AddToBottom(card);
					LOG(LogUtils::GetCardName(card) + " added to hand");
				}
			}
			else
			{
				if (card.Type() == ECardType::Nightmare)
				{
					ResolveNightmareCard(card);

					if (hand.Size() == 0)
					{
						nonLocationCardsToLimbo = true;
					}
				}
				else if (card.Type() == ECardType::Door)
				{
					ResolveDoorCard(card);
				}
				else
				{
					hand.AddToBottom(card);
					LOG(LogUtils::GetCardName(card) + " added to hand");
				}
			}
		}
		else
		{
			assert(!"should never get here - deck is empty");
		}
	}

	// check for game over - out of cards
	if (hand.Size() < kMaxHandSize && deck.Size() == 0)
	{
		m_inProgress = false;
		m_player.GameOver(false);
	}

	// check for game over - a door was discarded
	if (discard.DoorWasDiscarded())
	{
		m_inProgress = false;
		m_player.GameOver(true);
	}

	// check for game over - all doors completed
	if (doorProgress.HasAllDoors())
	{
		m_inProgress = false;
		m_player.GameOver(true);
	}
}

// -------------------------------------------------------------------------------------------------

void GameLogic::ShuffleLimboCards()
{
	// move cards from limbo to deck, then shuffle the deck
	CardCollection& limbo = m_gameState->GetLimbo();
	CardCollection& deck = m_gameState->GetDeck();

	if (limbo.Size() > 0)
	{
		LOG("");
		LOG("shuffling limbo cards into the deck");

		while (limbo.Size() > 0)
		{
			Card card;
			if (limbo.RemoveFromTop(card))
			{
				deck.AddToTop(card);
			}
		}

		deck.Shuffle();
		m_gameState->UpdateDeckSize();
	}
}

// -------------------------------------------------------------------------------------------------

void GameLogic::ResolveTurn()
{
	CardCollection& hand = m_gameState->GetHand();
	CardCollection& labrynth = m_gameState->GetLabrynth();
	DiscardTracker& discard = m_gameState->GetDiscard();

	LOG("");
	LOG("starting player turn action...");
	LOG("Hand Contents:");
	LOG(LogUtils::GetCollectionContents(hand, false, false));

	// query player
	ETurnAction choice = ETurnAction::PlayCard;
	std::size_t choiceIndex = 0;
	m_player.ResolveTurnAction(*m_gameState, choice, choiceIndex);

	LOG("");
	assert(hand.Size() > 0);
	assert(choiceIndex < hand.Size());

	// get the card from the hand (don't remove it yet)
	Card handCard;
	if (hand.GetAtIndex(choiceIndex, handCard))
	{
		if (choice == ETurnAction::PlayCard)
		{
			// get the bottom labrynth card (if it exists)
			Card labCard;
			labrynth.GetBottom(labCard);

			// play the card to the labrynth and check if it causes a door to be gained
			assert(labrynth.Size() == 0 || labCard.Type() != handCard.Type());
			if (labrynth.Size() == 0 || labCard.Type() != handCard.Type())
			{
				if (hand.RemoveFromIndex(choiceIndex, handCard))
				{
					LOG("played card: " + LogUtils::GetCardName(handCard));
					labrynth.AddToBottom(handCard);
					CheckForLabrynthDoor();
				}
			}
		}

		else if (choice == ETurnAction::DiscardCard)
		{
			// discard from hand
			if (hand.RemoveFromIndex(choiceIndex, handCard))
			{
				LOG("discarded card: " + LogUtils::GetCardName(handCard));
				discard.DiscardCard(handCard);

				// if we discarded a key, resolve a premonition
				if (handCard.Type() == ECardType::Key)
				{
					ResolvePremonition();
				}
			}
		}
	}
	else
	{
		assert(!"should never get here - invalid hand card index");
	}
}

// -------------------------------------------------------------------------------------------------

void GameLogic::ResolveNightmareCard(const Card& card)
{
	CardCollection& hand = m_gameState->GetHand();
	CardCollection& deck = m_gameState->GetDeck();
	CardCollection& limbo = m_gameState->GetLimbo();
	DiscardTracker& discard = m_gameState->GetDiscard();
	DoorProgress& doorProgress = m_gameState->GetDoorProgress();

	LOG("");
	LOG("resolving nightmare card...");

	// query player
	EResolveNightmareAction choice = EResolveNightmareAction::DiscardDeck;
	EColor choiceColor = EColor::None;
	m_player.ResolveNightmareCard(*m_gameState, choice, choiceColor);

	if (choice == EResolveNightmareAction::DiscardDeck)
	{
		// discard five cards from the top of the deck, doors and nighmares go to the limbo instead
		LOG("discarding five cards from the deck...");
		for (int i = 0; i < 5; ++i)
		{
			Card deckCard;
			if (deck.RemoveFromTop(deckCard))
			{
				m_gameState->UpdateDeckSize();

				if (deckCard.Type() == ECardType::Door || deckCard.Type() == ECardType::Nightmare)
				{
					LOG("limbo'd card: " + LogUtils::GetCardName(deckCard));
					limbo.AddToTop(deckCard);
				}
				else
				{
					LOG("limbo'd card: " + LogUtils::GetCardName(deckCard));
					discard.DiscardCard(deckCard);
				}
			}
		}
	}

	else if (choice == EResolveNightmareAction::DiscardHand)
	{
		// discard all cards in hand
		LOG("discarding hand...");
		while (hand.Size() > 0)
		{
			Card handCard;
			if (hand.RemoveFromTop(handCard))
			{
				LOG("discarded card: " + LogUtils::GetCardName(handCard));
				discard.DiscardCard(handCard);
			}
		}
	}

	else if (choice == EResolveNightmareAction::DiscardKey)
	{
		// find the chosen key among the hand cards
		LOG("discarding key...");
		auto keyIt = hand.GetCards().end();
		for (auto it = hand.GetCards().begin(); it != hand.GetCards().end(); ++it)
		{
			if (it->Type() == ECardType::Key && it->Color() == choiceColor)
			{
				keyIt = it;
				break;
			}
		}
		// discard the key from the hand
		if (keyIt != hand.GetCards().end())
		{
			Card handCard;
			if (hand.RemoveFromIndex(keyIt - hand.GetCards().begin(), handCard))
			{
				LOG("discarded card: " + LogUtils::GetCardName(handCard));
				discard.DiscardCard(handCard);
			}
		}
		else
		{
			assert(!"there was no key to discard");
		}
	}

	else if (choice == EResolveNightmareAction::RemoveDoor)
	{
		// remove a door from door progress, add it to limbo
		LOG("removing door...");
		assert(doorProgress.CountDoors(choiceColor) > 0);
		if (doorProgress.CountDoors(choiceColor) > 0)
		{
			LOG("\nlost a " + LogUtils::GetColor(choiceColor) + " door!\n");
			m_player.DoorModified(choiceColor, false);

			doorProgress.RemoveDoor(choiceColor);
			limbo.AddToTop(Card(choiceColor, ECardType::Door));
		}
		else
		{
			assert(!"there was no to destroy");
		}
	}

	// discard the nightmare card
	discard.DiscardCard(card);
}

// -------------------------------------------------------------------------------------------------

void GameLogic::ResolveDoorCard(const Card& card)
{
	CardCollection& hand = m_gameState->GetHand();
	CardCollection& limbo = m_gameState->GetLimbo();
	DiscardTracker& discard = m_gameState->GetDiscard();
	DoorProgress& doorProgress = m_gameState->GetDoorProgress();

	// search for a matching key among the hand cards
	auto keyIt = hand.GetCards().end();
	for (auto it = hand.GetCards().begin(); it != hand.GetCards().end(); ++it)
	{
		if (it->Type() == ECardType::Key && it->Color() == card.Color())
		{
			keyIt = it;
			break;
		}
	}

	if (keyIt != hand.GetCards().end())
	{
		LOG("");
		LOG("resolving door card...");

		// query player
		EResolveDoorAction choice = EResolveDoorAction::DiscardKeyCard;
		m_player.ResolveDoorCard(*m_gameState, card, choice);

		if (choice == EResolveDoorAction::DiscardKeyCard)
		{
			// discard the key from the hand and add door progress
			LOG("discarding key to unlock door");
			if (keyIt != hand.GetCards().end())
			{
				Card handCard;
				if (hand.RemoveFromIndex(keyIt - hand.GetCards().begin(), handCard))
				{
					LOG("discarded card: " + LogUtils::GetCardName(handCard));
					discard.DiscardCard(handCard);

					LOG("\ngained a " + LogUtils::GetColor(card.Color()) + " door!\n");
					m_player.DoorModified(card.Color(), true);
					doorProgress.AddDoor(card.Color());
				}
			}
		}
		else if (choice == EResolveDoorAction::KeepKeyCard)
		{
			// door card goes to limbo
			limbo.AddToBottom(card);
			LOG(LogUtils::GetCardName(card) + " added to limbo");
		}
	}
	else
	{
		// door card goes to limbo
		limbo.AddToBottom(card);
		LOG(LogUtils::GetCardName(card) + " added to limbo");
	}
}

// -------------------------------------------------------------------------------------------------

void GameLogic::ResolvePremonition()
{
	CardCollection& deck = m_gameState->GetDeck();
	DiscardTracker& discard = m_gameState->GetDiscard();

	// read the top five cards from the deck
	std::vector<Card> deckCards;
	for (int i = 0; i < 5; ++i)
	{
		Card deckCard;
		if (deck.GetAtIndex(i, deckCard))
		{
			deckCards.push_back(deckCard);
		}
	}

	// if there are no cards in the deck to read, bail out early
	if (deckCards.size() == 0)
	{
		return;
	}

	LOG("");
	LOG("resolving premonition...");

	// query player
	std::size_t choiceIndex = 0;
	std::vector<Card> reordered = deckCards;
	m_player.ResolvePremonition(*m_gameState, reordered);

	// validate reordered vector contains the same cards
	assert(reordered.size() == deckCards.size());
	bool foundAll = true;
	for (std::size_t i = 0; i < deckCards.size(); ++i)
	{
		bool found = false;
		for (std::size_t j = 0; j < reordered.size(); ++j)
		{
			if (deckCards[i] == reordered[j])
			{
				found = true;
				break;
			}
		}

		if (!found)
		{
			foundAll = false;
			break;
		}
	}
	assert(foundAll);
	if (!foundAll)
	{
		return;
	}

	// however many cards we read, remove them from the deck
	const std::size_t cardCount = deckCards.size();
	for (std::size_t i = 0; i < cardCount; ++i)
	{
		Card card;
		deck.RemoveFromTop(card);
	}

	// discard the last card we read
	discard.DiscardCard(reordered[reordered.size() - 1]);
	LOG("discarded card: " + LogUtils::GetCardName(reordered[reordered.size() - 1]));

	// add the reordered cards back onto the top of the deck
	for (std::size_t i = 0; i < reordered.size() - 1; ++i)
	{
		std::size_t index = reordered.size() - i - 2;
		deck.AddToTop(reordered[index]);
	}

	m_gameState->UpdateDeckSize();
}

// -------------------------------------------------------------------------------------------------

void GameLogic::CheckForLabrynthDoor()
{
	CardCollection& deck = m_gameState->GetDeck();
	CardCollection& labrynth = m_gameState->GetLabrynth();
	DoorProgress& doorProgress = m_gameState->GetDoorProgress();

	// find how many cards of the same color are currently at the bottom of the labrynth (in a row)
	EColor color = EColor::None;
	int count = 0;
	for (std::size_t i = 0; i < labrynth.Size(); ++i)
	{
		std::size_t index = labrynth.Size() - i - 1;
		Card card;
		if (labrynth.GetAtIndex(index, card))
		{
			if (i == 0)
			{
				color = card.Color();
				count = 1;
			}
			else
			{
				if (card.Color() != color)
				{
					break;
				}
				else
				{
					count++;
				}
			}
		}
	}

	// if there is a multiple of three, we unlock a door, remove it from the deck and shuffle the deck
	if (color != EColor::None && count > 0 && (count % 3) == 0)
	{
		if (doorProgress.HasAllDoors(color) == false)
		{
			m_player.DoorModified(color, true);
			LOG("\ngained a " + LogUtils::GetColor(color) + " door!\n");

			doorProgress.AddDoor(color);

			// find the door in the deck so we can remove it
			auto doorIt = deck.GetCards().end();
			for (auto it = deck.GetCards().begin(); it != deck.GetCards().end(); ++it)
			{
				if (it->Type() == ECardType::Door && it->Color() == color)
				{
					doorIt = it;
					break;
				}
			}
			if (doorIt != deck.GetCards().end())
			{
				Card card;
				deck.RemoveFromIndex(doorIt - deck.GetCards().begin(), card);
				m_gameState->UpdateDeckSize();
			}
		}

		deck.Shuffle();
	}
}

// -------------------------------------------------------------------------------------------------
