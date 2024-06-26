#pragma once
#include "Enums.h"
#include <string>

class Card;
class CardCollection;
class DoorProgress;
class CardCounter;

// -------------------------------------------------------------------------------------------------

namespace LogUtils
{
	std::string GetCardName(const Card& card);
	std::string GetCardAbbrev(const Card& card);

	std::string GetCollectionContents(const CardCollection& collection, bool oneLine, bool useAbbrev);

	std::string GetColor(EColor color);
	char GetColorChar(EColor color);

	std::string GetCardType(ECardType type);
	char GetCardTypeChar(ECardType type);

	std::string GetGameResult(EGameResult result);
	std::string GetDoorState(const DoorProgress& progress);
	std::string GetCardCounterState(const CardCounter& counter);
}

// -------------------------------------------------------------------------------------------------
