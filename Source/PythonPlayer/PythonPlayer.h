#pragma once
#include "PlayerInterface.h"
#include "MappedFile.h"
#include <sstream>

// -------------------------------------------------------------------------------------------------

class PythonPlayer : public PlayerInterface
{
public:
	PythonPlayer();
	virtual ~PythonPlayer();

	virtual void OnGameStarted(const PublicGameState& state);
	virtual void OnGameEnded(const PublicGameState& state, EGameResult result);

	virtual void OnCardDrawn(const Card& card);
	virtual void OnLabrynthModified(const CardCollection& labrynth);
	virtual void OnDoorModified(EColor color, EDoorModification modification);

	virtual void ResolveTurnAction(const PublicGameState& state, ETurnAction& choice, std::size_t& handIndex);
	virtual void ResolveNightmareCard(const PublicGameState& state, EResolveNightmareAction& choice, EColor& color);
	virtual void ResolveDoorCard(const PublicGameState& state, const Card& doorCard, EResolveDoorAction& choice);
	virtual void ResolvePremonition(const PublicGameState& state, std::vector<Card>& reorderedCards);

private:
	void SerializeGameState(std::stringstream& sstream, const PublicGameState& state);
	void ExchangeData(std::stringstream& cppData, std::vector<std::uint8_t>& pythonData);

private:
	MappedFile m_dataFile;
	EGameResult m_result;
};

// -------------------------------------------------------------------------------------------------

extern "C" __declspec(dllexport) PlayerInterface * CreatePlayerInstance()
{
	return new PythonPlayer();
}

// -------------------------------------------------------------------------------------------------

extern "C" __declspec(dllexport) void DestroyPlayerInstance(PlayerInterface * player)
{
	delete player;
}

// -------------------------------------------------------------------------------------------------

extern "C" __declspec(dllexport) void WaitForCppData();
extern "C" __declspec(dllexport) void SignalPythonData();

// -------------------------------------------------------------------------------------------------

