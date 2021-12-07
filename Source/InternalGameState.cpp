#include "InternalGameState.h"

// -------------------------------------------------------------------------------------------------

InternalGameState::InternalGameState()
{
}

// -------------------------------------------------------------------------------------------------

InternalGameState::~InternalGameState()
{
}

// -------------------------------------------------------------------------------------------------

void InternalGameState::UpdateDeckSize()
{
	m_deckRemaining = m_deck.Size();

	// count remaining nightmare cards in the deck
	m_nightmaresRemaining = 0;
	for (std::size_t i = 0; i < m_deckRemaining; ++i)
	{
		if (m_deck.GetCards()[i].Type() == ECardType::Nightmare)
		{
			m_nightmaresRemaining++;
		}
	}
}

// -------------------------------------------------------------------------------------------------
