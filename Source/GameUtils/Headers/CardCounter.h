#pragma once
#include "Enums.h"
#include <unordered_map>

class Card;

// -------------------------------------------------------------------------------------------------

class CardCounter
{
public:
	CardCounter();
	~CardCounter();

	void Add(const Card& card);
	void Add(EColor color, ECardType type);
	void Remove(const Card& card);
	void Remove(EColor color, ECardType type);

	std::size_t Get(EColor color) const;
	std::size_t Get(ECardType type) const;
	std::size_t Get(EColor color, ECardType type) const;

private:
	std::unordered_map<EColor, std::size_t> m_byColor;
	std::unordered_map<ECardType, std::size_t> m_byType;
	std::unordered_map<EColor, std::unordered_map<ECardType, std::size_t>> m_byColorAndType;
};

// -------------------------------------------------------------------------------------------------
