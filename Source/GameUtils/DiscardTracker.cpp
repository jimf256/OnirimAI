#include "DiscardTracker.h"
#include "Card.h"
#include <cassert>

// -------------------------------------------------------------------------------------------------

DiscardTracker::DiscardTracker()
	: m_nightmareCount(0)
	, m_doorDiscarded(false)
{
}

// -------------------------------------------------------------------------------------------------

int DiscardTracker::GetColorCardCount(EColor color, ECardType type) const
{
	if (color != EColor::None)
	{
		switch (type)
		{
			case ECardType::Sun:  return m_cardCounts.at(color).sun;
			case ECardType::Moon: return m_cardCounts.at(color).moon;
			case ECardType::Key:  return m_cardCounts.at(color).key;
			case ECardType::Door: return m_cardCounts.at(color).door;

			default:
				return 0;
		}
	}

	return 0;
}

// -------------------------------------------------------------------------------------------------

void DiscardTracker::DiscardCard(const Card& card)
{
	if (card.Type() == ECardType::Nightmare)
	{
		m_nightmareCount++;
	}
	else
	{
		assert(card.Color() != EColor::None);
		if (card.Color() != EColor::None)
		{
			switch (card.Type())
			{
				case ECardType::Sun:  m_cardCounts[card.Color()].sun++; break;
				case ECardType::Moon: m_cardCounts[card.Color()].moon++; break;
				case ECardType::Key:  m_cardCounts[card.Color()].key++; break;

				case ECardType::Door:
				{
					m_cardCounts[card.Color()].door++;
					m_doorDiscarded = true;
					break;
				}
			}
		}
	}
}

// -------------------------------------------------------------------------------------------------
