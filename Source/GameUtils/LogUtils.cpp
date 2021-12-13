#include "LogUtils.h"
#include "Card.h"
#include "CardCollection.h"
#include "DoorProgress.h"
#include "CardCounter.h"
#include <sstream>

// -------------------------------------------------------------------------------------------------

std::string LogUtils::GetCardName(const Card& card)
{
	if (card.Type() == ECardType::Nightmare)
	{
		return "<Nightmare>";
	}
	else
	{
		std::string name = "";
		switch (card.Color())
		{
			case EColor::Red:    name += "<Red "; break;
			case EColor::Blue:   name += "<Blue "; break;
			case EColor::Green:  name += "<Green "; break;
			case EColor::Yellow: name += "<Yellow "; break;
		}
		switch (card.Type())
		{
			case ECardType::Sun:  name += "Sun>"; break;
			case ECardType::Moon: name += "Moon>"; break;
			case ECardType::Key:  name += "Key>"; break;
			case ECardType::Door: name += "Door>"; break;
		}

		return name;
	}
}

// -------------------------------------------------------------------------------------------------

std::string LogUtils::GetCardAbbrev(const Card& card)
{
	if (card.Type() == ECardType::Nightmare)
	{
		return "<NM>";
	}
	else
	{
		std::string name = "";
		switch (card.Color())
		{
			case EColor::Red:    name += "<R"; break;
			case EColor::Blue:   name += "<B"; break;
			case EColor::Green:  name += "<G"; break;
			case EColor::Yellow: name += "<Y"; break;
		}
		switch (card.Type())
		{
			case ECardType::Sun:  name += "S>"; break;
			case ECardType::Moon: name += "M>"; break;
			case ECardType::Key:  name += "K>"; break;
			case ECardType::Door: name += "D>"; break;
		}

		return name;
	}
}

// -------------------------------------------------------------------------------------------------

std::string LogUtils::GetCollectionContents(const CardCollection& collection, bool oneLine, bool useAbbrev)
{
	static std::ostringstream output;
	output.str("");

	for (std::size_t i = 0; i < collection.Size(); ++i)
	{
		if (i > 0)
		{
			output << ((oneLine) ? ", " : "\n");
		}

		if (!oneLine)
		{
			output << (i + 1) << ". ";
		}

		const Card& card = collection.GetCards()[i];
		output << ((useAbbrev) ? GetCardAbbrev(card) : GetCardName(card));
	}

	output << "\n";
	return output.str();
}

// -------------------------------------------------------------------------------------------------

std::string LogUtils::GetColor(EColor color)
{
	switch (color)
	{
		case EColor::Red:    return "Red";
		case EColor::Blue:   return "Blue";
		case EColor::Green:  return "Green";
		case EColor::Yellow: return "Yellow";
	}

	return "None";
}

// -------------------------------------------------------------------------------------------------

char LogUtils::GetColorChar(EColor color)
{
	switch (color)
	{
		case EColor::Red:    return 'R';
		case EColor::Blue:   return 'B';
		case EColor::Green:  return 'G';
		case EColor::Yellow: return 'Y';
	}

	return '\0';
}

// -------------------------------------------------------------------------------------------------

std::string LogUtils::GetCardType(ECardType type)
{
	switch (type)
	{
		case ECardType::Sun:       return "Sun";
		case ECardType::Moon:      return "Moon";
		case ECardType::Key:       return "Key";
		case ECardType::Door:      return "Door";
		case ECardType::Nightmare: return "Nightmare";
	}

	return "None";
}

// -------------------------------------------------------------------------------------------------

char LogUtils::GetCardTypeChar(ECardType type)
{
	switch (type)
	{
	case ECardType::Sun:       return 'S';
	case ECardType::Moon:      return 'M';
	case ECardType::Key:       return 'K';
	case ECardType::Door:      return 'D';
	case ECardType::Nightmare: return 'N';
	}

	return '\0';
}

// -------------------------------------------------------------------------------------------------

std::string LogUtils::GetGameResult(EGameResult result)
{
	switch (result)
	{
		case EGameResult::Unknown:            return "Unknown";
		case EGameResult::Win:                return "Win";
		case EGameResult::Loss_DiscardedDoor: return "Lose (door discarded)";
		case EGameResult::Loss_OutOfCards:    return "Lose (out of cards)";
	}

	return "unknown";
}

// -------------------------------------------------------------------------------------------------

std::string LogUtils::GetDoorState(const DoorProgress& progress)
{
	static std::ostringstream output;
	output.str("");

	static const std::vector<EColor> colors { EColor::Red, EColor::Blue, EColor::Green, EColor::Yellow };
	for (auto it = colors.begin(); it != colors.end(); ++it)
	{
		output << " " << GetColor(*it) << " doors: " << progress.CountDoors(*it) << "/" << progress.GetDoorsRequired() << "\n";
	}

	return output.str();
}

// -------------------------------------------------------------------------------------------------

std::string LogUtils::GetCardCounterState(const CardCounter& counter)
{
	static std::ostringstream output;
	output.str("");

	static const std::vector<ECardType> cardTypes { ECardType::Sun, ECardType::Moon, ECardType::Key, ECardType::Door };
	static const std::vector<EColor> colors { EColor::Red, EColor::Blue, EColor::Green, EColor::Yellow };
	for (auto colIt = colors.begin(); colIt != colors.end(); ++colIt)
	{
		output << " " << GetColor(*colIt) << ": ";
		for (auto typeIt = cardTypes.begin(); typeIt != cardTypes.end(); ++typeIt)
		{
			output << GetCardType(*typeIt) << ": " << counter.Get(*colIt, *typeIt) << " ";
		}
		output << "\n";
	}
	output << " Nightmare: " << counter.Get(ECardType::Nightmare) << "\n";

	return output.str();
}

// -------------------------------------------------------------------------------------------------
