#include "GameLogic.h"
#include "Card.h"
#include "PlayerInterface.h"
#include "InternalGameState.h"
#include "LogUtils.h"
#include <unordered_map>
#include <cassert>

// -------------------------------------------------------------------------------------------------

GameLogic::GameLogic(PlayerInterface& player, Logging::PlatformLogHook logHook)
	: m_inProgress(false)
	, m_result(EGameResult::Unknown)
	, m_player(player)
{
	// initialize the log system
	Logging::Initialize("debug.log", logHook);

	// create the game state
	m_gameState.reset(new InternalGameState());
}

// -------------------------------------------------------------------------------------------------

GameLogic::~GameLogic()
{
	// shut down the log system (close the file)
	Logging::ShutDown();
}

// -------------------------------------------------------------------------------------------------

void GameLogic::Run()
{
	// create the deck
	InitializeDeck();

	// draw initial cards
	DrawCards(true);
	ShuffleLimboCards();

	OnGameStarted();
	while (m_result == EGameResult::Unknown)
	{
		ResolveTurn();
		DrawCards(false);
		ShuffleLimboCards();
	}
	OnGameEnded();
}

// -------------------------------------------------------------------------------------------------

void GameLogic::OnGameStarted()
{
	m_result = EGameResult::Unknown;
	m_inProgress = true;

	// pass a copy of the gamestate to prevent cheating
	PublicGameState gameStateCopy = *m_gameState;
	m_player.OnGameStarted(gameStateCopy);
}

// -------------------------------------------------------------------------------------------------

void GameLogic::OnGameEnded()
{
	assert(m_result != EGameResult::Unknown);
	m_inProgress = false;

	// pass a copy of the gamestate to prevent cheating
	PublicGameState gameStateCopy = *m_gameState;
	m_player.OnGameEnded(gameStateCopy, m_result);

	// print end-of-game state to the log
	LOG("\n\n::: game ended :::\n");
	LOG("game result: " + LogUtils::GetGameResult(result) + "\n");
	LOG("doors:\n" + LogUtils::GetDoorState(m_gameState->GetDoorProgress()));
	LOG("labrynth: " + LogUtils::GetCollectionContents(m_gameState->GetLabrynth(), true, true));
	LOG("hand: " + LogUtils::GetCollectionContents(m_gameState->GetHand(), true, true));
	LOG("discard:\n" + LogUtils::GetCardCounterState(m_gameState->GetDiscard()));
	LOG("limbo: " + LogUtils::GetCollectionContents(m_gameState->GetLimbo(), true, true));
	LOG("deck: " + LogUtils::GetCollectionContents(m_gameState->GetDeck(), true, true));

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
	CardCounter& discard = m_gameState->GetDiscard();
	DoorProgress& doorProgress = m_gameState->GetDoorProgress();

	LOG("");
	LOG("starting draw cards step...");

	const std::size_t kMaxHandSize = 5;
	while (deck.Size() > 0 && hand.Size() < kMaxHandSize && !doorProgress.HasAllDoors() && discard.Get(ECardType::Door) == 0)
	{
		Card card;
		if (deck.RemoveFromTop(card))
		{
			LOG("draw card: " + LogUtils::GetCardName(card));
			m_gameState->UpdateDeckSize();
			m_player.OnCardDrawn(card);

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
		m_result = EGameResult::Loss_OutOfCards;
	}

	// check for game over - a door was discarded
	if (discard.Get(ECardType::Door) > 0)
	{
		m_result = EGameResult::Loss_DiscardedDoor;
	}

	// check for game over - all doors completed
	if (doorProgress.HasAllDoors())
	{
		m_result = EGameResult::Win;
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
	CardCounter& discard = m_gameState->GetDiscard();

	LOG("");
	LOG("starting player turn action...");
	LOG("Hand Contents:");
	LOG(LogUtils::GetCollectionContents(hand, false, false));

	// query player - player code is passed a copy of the gamestate to prevent cheating
	ETurnAction choice = ETurnAction::PlayCard;
	std::size_t choiceIndex = 0;
	PublicGameState gameStateCopy = *m_gameState;
	m_player.ResolveTurnAction(gameStateCopy, choice, choiceIndex);

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

					m_player.OnLabrynthModified(m_gameState->GetLabrynth());
				}
			}
		}

		else if (choice == ETurnAction::DiscardCard)
		{
			// discard from hand
			if (hand.RemoveFromIndex(choiceIndex, handCard))
			{
				LOG("discarded card: " + LogUtils::GetCardName(handCard));
				discard.Add(handCard);

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
	CardCounter& discard = m_gameState->GetDiscard();
	DoorProgress& doorProgress = m_gameState->GetDoorProgress();

	LOG("");
	LOG("resolving nightmare card...");

	// query player - player code is passed a copy of the gamestate to prevent cheating
	EResolveNightmareAction choice = EResolveNightmareAction::DiscardDeck;
	EColor choiceColor = EColor::None;
	PublicGameState gameStateCopy = *m_gameState;
	m_player.ResolveNightmareCard(gameStateCopy, choice, choiceColor);

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
					discard.Add(deckCard);
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
				discard.Add(handCard);
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
				discard.Add(handCard);
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
			m_player.OnDoorModified(choiceColor, EDoorModification::Removed);

			doorProgress.RemoveDoor(choiceColor);
			limbo.AddToTop(Card(choiceColor, ECardType::Door));
		}
		else
		{
			assert(!"there was no to destroy");
		}
	}

	// discard the nightmare card
	discard.Add(card);
}

// -------------------------------------------------------------------------------------------------

void GameLogic::ResolveDoorCard(const Card& card)
{
	CardCollection& hand = m_gameState->GetHand();
	CardCollection& limbo = m_gameState->GetLimbo();
	CardCounter& discard = m_gameState->GetDiscard();
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

		// query player - player code is passed a copy of the gamestate to prevent cheating
		EResolveDoorAction choice = EResolveDoorAction::DiscardKeyCard;
		PublicGameState gameStateCopy = *m_gameState;
		m_player.ResolveDoorCard(gameStateCopy, card, choice);

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
					discard.Add(handCard);

					LOG("\ngained a " + LogUtils::GetColor(card.Color()) + " door!\n");
					m_player.OnDoorModified(card.Color(), EDoorModification::Added);
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
	CardCounter& discard = m_gameState->GetDiscard();

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

	// query player - player code is passed a copy of the gamestate to prevent cheating
	std::size_t choiceIndex = 0;
	std::vector<Card> reordered = deckCards;
	PublicGameState gameStateCopy = *m_gameState;
	m_player.ResolvePremonition(gameStateCopy, reordered);

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
	discard.Add(reordered[reordered.size() - 1]);
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
	const std::vector<Card>& labrynth = m_gameState->GetLabrynth().GetCards();
	DoorProgress& doorProgress = m_gameState->GetDoorProgress();

	// find how many cards of the same color are currently at the bottom of the labrynth (in a row)
	EColor color = EColor::None;
	int count = 0;
	for (auto it = labrynth.rbegin(); it != labrynth.rend(); ++it)
	{
		if (color == EColor::None)
		{
			color = it->Color();
		}
		else if (it->Color() != color)
		{
			break;
		}
		count++;
	}

	// if there is a multiple of three, we unlock a door, remove it from the deck and shuffle the deck
	if (color != EColor::None && count > 0 && (count % 3) == 0)
	{
		if (doorProgress.HasAllDoors(color) == false)
		{
			m_player.OnDoorModified(color, EDoorModification::Added);
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
