#include "CardCounter.h"
#include "Card.h"
#include <cassert>

// -------------------------------------------------------------------------------------------------

CardCounter::CardCounter()
{
}

// -------------------------------------------------------------------------------------------------

CardCounter::~CardCounter()
{
}

// -------------------------------------------------------------------------------------------------

void CardCounter::Add(const Card& card)
{
	Add(card.Color(), card.Type());
}

// -------------------------------------------------------------------------------------------------

void CardCounter::Add(EColor color, ECardType type)
{
	assert(color != EColor::None || type == ECardType::Nightmare);
	if (color != EColor::None || type == ECardType::Nightmare)
	{
		m_byColor[color] += 1;
		m_byType[type] += 1;
		m_byColorAndType[color][type] += 1;
	}
}

// -------------------------------------------------------------------------------------------------

void CardCounter::Remove(const Card& card)
{
	Remove(card.Color(), card.Type());
}

// -------------------------------------------------------------------------------------------------

void CardCounter::Remove(EColor color, ECardType type)
{
	assert(color != EColor::None || type == ECardType::Nightmare);
	if (color != EColor::None || type == ECardType::Nightmare)
	{
		m_byColor[color] -= 1;
		m_byType[type] -= 1;
		m_byColorAndType[color][type] -= 1;
	}
}

// -------------------------------------------------------------------------------------------------

std::size_t CardCounter::Get(EColor color) const
{
	auto it = m_byColor.find(color);
	if (it != m_byColor.end())
	{
		return it->second;
	}

	return 0;
}

// -------------------------------------------------------------------------------------------------

std::size_t CardCounter::Get(ECardType type) const
{
	auto it = m_byType.find(type);
	if (it != m_byType.end())
	{
		return it->second;
	}

	return 0;
}

// -------------------------------------------------------------------------------------------------

std::size_t CardCounter::Get(EColor color, ECardType type) const
{
	auto it = m_byColorAndType.find(color);
	if (it != m_byColorAndType.end())
	{
		auto it2 = it->second.find(type);
		if (it2 != it->second.end())
		{
			return it2->second;
		}
	}

	return 0;
}

// -------------------------------------------------------------------------------------------------
