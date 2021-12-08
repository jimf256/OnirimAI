#pragma once
#include "Enums.h"
#include <unordered_map>

class Card;

// -------------------------------------------------------------------------------------------------

struct DiscardCounts
{
	int sun;
	int moon;
	int key;
};

// -------------------------------------------------------------------------------------------------

class DiscardTracker
{
public:
	DiscardTracker();

	int GetNightmareCount() const { return m_nightmareCount; }
	int GetColorCardCount(EColor color, ECardType type) const;
	bool DoorWasDiscarded() const { return m_doorDiscarded; }

	void DiscardCard(const Card& card);

private:
	int m_nightmareCount;
	bool m_doorDiscarded;
	std::unordered_map<EColor, DiscardCounts> m_cardCounts;
};

// -------------------------------------------------------------------------------------------------
