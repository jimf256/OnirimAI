#pragma once
#include "DoorProgress.h"
#include "CardCounter.h"
#include "CardCollection.h"

// -------------------------------------------------------------------------------------------------

class PublicGameState
{
public:
	PublicGameState();
	virtual ~PublicGameState() = 0;

	// return copies
	const CardCollection& GetHand() const { return m_hand; }
	const CardCollection& GetLimbo() const { return m_limbo; }
	const CardCollection& GetLabrynth() const { return m_labrynth; }
	const CardCounter& GetDiscard() const { return m_discard; }
	const DoorProgress& GetDoorProgress() const { return m_doorProgress; }

	std::size_t GetDeckRemaining() const { return m_deckRemaining; }
	std::size_t GetNightmaresRemaining() const { return m_nightmaresRemaining; }

protected:
	CardCollection m_hand;
	CardCollection m_limbo;
	CardCollection m_labrynth;
	CardCounter m_discard;
	DoorProgress m_doorProgress;

	std::size_t m_deckRemaining;
	std::size_t m_nightmaresRemaining;
};

// -------------------------------------------------------------------------------------------------
