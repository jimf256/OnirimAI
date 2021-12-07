#pragma once
#include "Card.h"
#include <vector>

// -------------------------------------------------------------------------------------------------

class CardCollection
{
public:
	CardCollection();

	const std::size_t Size() const { return m_cards.size(); }
	const std::vector<Card>& GetCards() const { return m_cards; }

	void AddAtIndex(std::size_t index, const Card& card);
	void AddToTop(const Card& card);
	void AddToBottom(const Card& card);

	bool GetAtIndex(std::size_t index, Card& card) const;
	bool GetTop(Card& card) const;
	bool GetBottom(Card& card) const;

	bool RemoveFromIndex(std::size_t index, Card& card);
	bool RemoveFromTop(Card& card);
	bool RemoveFromBottom(Card& card);

	void Shuffle();

private:
	std::vector<Card> m_cards;
};

// -------------------------------------------------------------------------------------------------

