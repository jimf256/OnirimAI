#pragma once
#include "PublicGameState.h"

// -------------------------------------------------------------------------------------------------

class InternalGameState : public PublicGameState
{
public:
	InternalGameState();
	virtual ~InternalGameState();

	// return references for game logic use
	CardCollection& GetDeck() { return m_deck; }
	CardCollection& GetHand() { return m_hand; }
	CardCollection& GetLimbo() { return m_limbo; }
	CardCollection& GetLabrynth() { return m_labrynth; }
	CardCounter& GetDiscard() { return m_discard; }
	DoorProgress& GetDoorProgress() { return m_doorProgress; }

	void UpdateDeckSize();

private:
	CardCollection m_deck;
};

// -------------------------------------------------------------------------------------------------
