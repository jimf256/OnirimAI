#include "ConsolePlayer.h"
#include "PublicGameState.h"
#include "Logging.h"
#include "LogUtils.h"
#include <iostream>
#include <unordered_map>

// -------------------------------------------------------------------------------------------------

ConsolePlayer::ConsolePlayer()
{
}

// -------------------------------------------------------------------------------------------------

void ConsolePlayer::OnGameOver(EGameResult result)
{
	std::cout << "\n::: GAME OVER :::\n";
	std::cout << "You " << LogUtils::GetGameResult(result) << "\n\n";
	system("pause");
}

// -------------------------------------------------------------------------------------------------

void ConsolePlayer::OnCardDrawn(const Card& card)
{
	std::cout << "draw card: " << LogUtils::GetCardName(card) << "\n";
}

// -------------------------------------------------------------------------------------------------

void ConsolePlayer::OnLabrynthModified(const CardCollection& labrynth)
{
}

// -------------------------------------------------------------------------------------------------

void ConsolePlayer::OnDoorModified(EColor color, EDoorModification modification)
{
	std::cout << "door (" << LogUtils::GetColor(color) << ") was " 
		<< (modification == EDoorModification::Added ? "ADDED!" : "LOST!") << "\n";
}

// -------------------------------------------------------------------------------------------------

void ConsolePlayer::ResolveTurnAction(const PublicGameState& state, ETurnAction& choice, std::size_t& handIndex)
{
	std::cout << "\n------------------------------\nstarting turn\n";

	// print door progress, labrynth and hand contents
	const DoorProgress& doors = state.GetDoorProgress();
	std::cout << "doors: red: " << doors.CountDoors(EColor::Red)
		<< ", blue: " << doors.CountDoors(EColor::Blue)
		<< ", green: " << doors.CountDoors(EColor::Green)
		<< ", yellow: " << doors.CountDoors(EColor::Yellow) << "\n";
	std::cout << "labrynth: " << LogUtils::GetCollectionContents(state.GetLabrynth(), true, true);
	std::cout << "deck: " << state.GetDeckRemaining() << " cards remaining\n";
	std::cout << "nightmares: " << state.GetNightmaresRemaining() << " cards remaining\n";
	std::cout << "hand:\n" << LogUtils::GetCollectionContents(state.GetHand(), false, false) << "\n";

	while (true)
	{
		// pick a card to play or discard
		std::string input = "";
		std::cout << "pick a card, or 'd' to view discard: ";
		std::getline(std::cin, input, '\n');

		if (input.size() != 1 || (input[0] != 'd' && (input[0] < '1' || input[0] > '5')))
		{
			std::cout << "try again\n";
			continue;
		}

		if (input[0] == 'd')
		{
			// print discard pile
			std::cout << "discard pile:\n";
			std::cout << LogUtils::GetDiscardState(state.GetDiscard()) << "\n";
		}
		else
		{
			std::size_t cardNumber = static_cast<std::size_t>(input[0]) - '1' + 1;

			// pick an action for the chosen card
			std::cout << "pick an action:\n 1. play\n 2. discard\n\nchoice: ";
			std::getline(std::cin, input, '\n');
			if (input.size() != 1 || (input[0] != '1' && input[0] != '2'))
			{
				std::cout << "\ntry again\n";
				continue;
			}
			else if (input[0] == '1')
			{
				// check the chosen card can be played to the labrynth
				Card handCard;
				if (state.GetHand().GetAtIndex(cardNumber - 1, handCard))
				{
					Card labCard;
					if (state.GetLabrynth().Size() == 0 ||
						(state.GetLabrynth().GetBottom(labCard) && labCard.Type() != handCard.Type()))
					{
						choice = ETurnAction::PlayCard;
						handIndex = cardNumber - 1;

						std::cout << "\nplaying " << LogUtils::GetCardName(handCard) << "\n";
						break;
					}
					else
					{
						std::cout << "\nyou can't play that\n";
						continue;
					}
				}
				else
				{
					std::cout << "\ninvalid card\n";
					continue;
				}
			}
			else if (input[0] == '2')
			{
				// card chosen to be discarded
				Card handCard;
				if (state.GetHand().GetAtIndex(cardNumber - 1, handCard))
				{
					choice = ETurnAction::DiscardCard;
					handIndex = cardNumber - 1;

					std::cout << "\ndiscarding " << LogUtils::GetCardName(handCard) << "\n";
					break;
				}
			}
		}
	}
}

// -------------------------------------------------------------------------------------------------

void ConsolePlayer::ResolveNightmareCard(const PublicGameState& state, EResolveNightmareAction& choice, EColor& color)
{
	while (true)
	{
		std::cout << "\noops, you drew a nightmare card, you must do one of the following:\n"
			<< " 1. discard a key card\n 2. discard your hand\n 3. discard top five cards of the deck\n 4. destroy a door"
			<< "\n\n choice: ";

		std::string input = "";
		std::getline(std::cin, input, '\n');
		if (input.size() != 1 || (input[0] < '1' || input[0] > '4'))
		{
			std::cout << "try again\n";
			continue;
		}
		else if (input[0] == '1')
		{
			// discard a key card
			// find which colours of keys we have in hand
			std::unordered_map<EColor, bool> hasKey;
			int keyCount = 0;
			const std::vector<Card>& hand = state.GetHand().GetCards();
			for (std::size_t i = 0; i < hand.size(); ++i)
			{
				if (hand[i].Type() == ECardType::Key)
				{
					hasKey[hand[i].Color()] = true;
					keyCount++;
				}
			}
			
			// prompt for a color key to discard, if any
			std::unordered_map<int, EColor> colorNumbers;
			if (keyCount > 0)
			{
				std::cout << "\nchoose a key to discard:\n";
				int number = 1;
				for (auto it = hasKey.begin(); it != hasKey.end(); ++it)
				{
					if (it->second)
					{
						colorNumbers[number] = it->first;
						std::cout << " " << number << ". " << LogUtils::GetColor(it->first) << " key\n";
						number++;
					}
				}
				std::cout << "\nchoice: ";

				std::getline(std::cin, input, '\n');
				int colorChoice = (input.size() == 1) ? (static_cast<int>(input[0]) - '1' + 1) : 0;
				if (colorChoice < 1 || colorChoice > static_cast<int>(hasKey.size()))
				{
					std::cout << "invalid key selection\n";
					continue;
				}
				else
				{
					color = colorNumbers[colorChoice];
					choice = EResolveNightmareAction::DiscardKey;
					break;
				}
			}
		}
		else if (input[0] == '2')
		{
			// discard entire hand
			choice = EResolveNightmareAction::DiscardHand;
			break;
		}
		else if (input[0] == '3')
		{
			// discard top five cards of the deck
			choice = EResolveNightmareAction::DiscardDeck;
			break;
		}
		else if (input[0] == '4')
		{
			// destroy a door
			// check there are actually doors to destroy
			const DoorProgress& doors = state.GetDoorProgress();
			if (doors.CountDoors() == 0)
			{
				std::cout << "\nno doors to destroy\n";
				continue;
			}

			// find which doors can be destroyed and list them
			std::cout << "\nchoose a door to destroy:\n";
			std::vector<EColor> colors {EColor::Red, EColor::Blue, EColor::Green, EColor::Yellow};
			std::unordered_map<int, EColor> colorNumbers;
			int number = 1;
			for (auto it = colors.begin(); it != colors.end(); ++it)
			{
				if (doors.CountDoors(*it) > 0)
				{
					colorNumbers[number] = *it;
					std::cout << " " << number << ". " << LogUtils::GetColor(*it) << " door\n";
					number++;
				}
			}

			std::cout << "\nchoice: ";
			std::getline(std::cin, input, '\n');
			int colorChoice = (input.size() == 1) ? (static_cast<int>(input[0]) - '1' + 1) : 0;
			if (colorChoice < 1 || colorChoice > static_cast<int>(colorNumbers.size()))
			{
				std::cout << "invalid door selection\n";
				continue;
			}
			else
			{
				color = colorNumbers[colorChoice];
				choice = EResolveNightmareAction::RemoveDoor;
				break;
			}
		}
	}
}

// -------------------------------------------------------------------------------------------------

void ConsolePlayer::ResolveDoorCard(const PublicGameState& state, const Card& doorCard, EResolveDoorAction& choice)
{
	std::cout << "\ndiscard a " << LogUtils::GetColor(doorCard.Color()) << " key to gain a "
		      << LogUtils::GetColor(doorCard.Color()) << " door (y/n)? ";

	std::string input = "";
	std::getline(std::cin, input, '\n');
	if (input.size() == 1 && input[0] == 'y')
	{
		choice = EResolveDoorAction::DiscardKeyCard;
	}
	else
	{
		choice = EResolveDoorAction::KeepKeyCard;
	}
}

// -------------------------------------------------------------------------------------------------

void ConsolePlayer::ResolvePremonition(const PublicGameState& state, std::vector<Card>& reorderedCards)
{
	while (true)
	{
		// print top five cards from the deck
		std::vector<Card> cards = reorderedCards;
		std::cout << "\ntop five cards in the deck are:\n";
		int number = 1;
		for (auto it = cards.begin(); it != cards.end(); ++it)
		{
			std::cout << " " << number++ << ". " << LogUtils::GetCardName(*it) << "\n";
		}

		// prompt for a new order
		std::cout << "\nreorder cards, using digits 1-5, eg: 54321 to reverse the order\n";
		std::cout << "the card furthest on the right will be discarded (tip: don't discard doors)\n";
		std::cout << "reordered cards: ";
		std::string input = "";
		std::getline(std::cin, input, '\n');

		if (input.size() != cards.size())
		{
			std::cout << "try again\n";
			continue;
		}

		// check we didn't use any incorrect digits, and didn't repeat any digits
		bool invalid = false;
		std::unordered_map<int, bool> alreadyUsed;
		std::vector<int> newOrder;
		for (std::size_t i = 0; i < input.size(); ++i)
		{
			if (input[i] < '1' || input[i] > ('1' + static_cast<int>(input.size()) - 1))
			{
				invalid = true;
				break;
			}

			int digit = input[i] - '1' + 1;
			if (alreadyUsed[digit])
			{
				invalid = true;
				break;
			}

			alreadyUsed[digit] = true;
			newOrder.push_back(digit - 1);
		}
		if (invalid)
		{
			std::cout << "try again\n";
			continue;
		}

		// reorder the cards
		reorderedCards.clear();
		for (auto it = newOrder.begin(); it != newOrder.end(); ++it)
		{
			reorderedCards.push_back(cards[*it]);
		}
		break;
	}
}

// -------------------------------------------------------------------------------------------------
