#pragma once
#include <cstddef>

// -------------------------------------------------------------------------------------------------

enum class EColor
{
	Red    = 0,
	Blue   = 1,
	Green  = 2,
	Yellow = 3,

	None,
};

// -------------------------------------------------------------------------------------------------

enum class ECardType
{
	Sun,
	Moon,
	Key,
	Door,
	Nightmare,
};

// -------------------------------------------------------------------------------------------------

enum class ETurnAction
{
	PlayCard,
	DiscardCard,
};

// -------------------------------------------------------------------------------------------------

enum class EResolveNightmareAction
{
	RemoveDoor,
	DiscardKey,
	DiscardHand,
	DiscardDeck,
};

// -------------------------------------------------------------------------------------------------

enum class EResolveDoorAction
{
	DiscardKeyCard,
	KeepKeyCard,
};

// -------------------------------------------------------------------------------------------------

enum class EGameResult
{
	Unknown            = 0,
	Win                = 1,
	Loss_OutOfCards    = 2,
	Loss_DiscardedDoor = 3,
};

// -------------------------------------------------------------------------------------------------
