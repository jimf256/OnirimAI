#pragma once
#include "Enums.h"

// -------------------------------------------------------------------------------------------------

class Card
{
public:
	Card();
	Card(EColor color, ECardType type);

	EColor Color() const { return m_color; }
	ECardType Type() const { return m_type; }

private:
	EColor m_color;
	ECardType m_type;
};

// -------------------------------------------------------------------------------------------------

bool operator==(const Card& left, const Card& right);

// -------------------------------------------------------------------------------------------------
