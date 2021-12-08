#include "LogUtils.h"
#include "Card.h"
#include "CardCollection.h"
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
