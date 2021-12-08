#include "CardCollection.h"
#include <algorithm>
#include <random>
#include <cassert>

// -------------------------------------------------------------------------------------------------

CardCollection::CardCollection()
{
}

// -------------------------------------------------------------------------------------------------

void CardCollection::AddAtIndex(std::size_t index, const Card& card)
{
	assert(index <= m_cards.size());
	if (index <= m_cards.size())
	{
		m_cards.insert(m_cards.begin() + index, card);
	}
}

// -------------------------------------------------------------------------------------------------

void CardCollection::AddToTop(const Card& card)
{
	m_cards.insert(m_cards.begin(), card);
}

// -------------------------------------------------------------------------------------------------

void CardCollection::AddToBottom(const Card& card)
{
	m_cards.push_back(card);
}

// -------------------------------------------------------------------------------------------------

bool CardCollection::GetAtIndex(std::size_t index, Card& card) const
{
	if (index < m_cards.size())
	{
		card = m_cards[index];
		return true;
	}

	return false;
}

// -------------------------------------------------------------------------------------------------

bool CardCollection::GetTop(Card& card) const
{
	return GetAtIndex(0, card);
}

// -------------------------------------------------------------------------------------------------

bool CardCollection::GetBottom(Card& card) const
{
	return GetAtIndex(m_cards.size() - 1, card);
}

// -------------------------------------------------------------------------------------------------

bool CardCollection::RemoveFromIndex(std::size_t index, Card& card)
{
	if (index < m_cards.size())
	{
		card = m_cards[index];
		m_cards.erase(m_cards.begin() + index);
		return true;
	}

	return false;
}

// -------------------------------------------------------------------------------------------------

bool CardCollection::RemoveFromTop(Card& card)
{
	return RemoveFromIndex(0, card);
}

// -------------------------------------------------------------------------------------------------

bool CardCollection::RemoveFromBottom(Card& card)
{
	return RemoveFromIndex(m_cards.size() - 1, card);
}

// -------------------------------------------------------------------------------------------------

void CardCollection::Shuffle()
{
	std::shuffle(m_cards.begin(), m_cards.end(), std::random_device());
}

// -------------------------------------------------------------------------------------------------
