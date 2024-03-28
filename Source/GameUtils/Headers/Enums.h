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
	PlayCard    = 0,
	DiscardCard = 1,
};

// -------------------------------------------------------------------------------------------------

enum class EResolveNightmareAction
{
	RemoveDoor  = 0,
	DiscardKey  = 1,
	DiscardHand = 2,
	DiscardDeck = 3,
};

// -------------------------------------------------------------------------------------------------

enum class EResolveDoorAction
{
	DiscardKeyCard = 0,
	KeepKeyCard    = 1,
};

// -------------------------------------------------------------------------------------------------

enum class EGameResult
{
	Unknown            = 0,
	Win                = 1,
	Loss_OutOfCards    = 2,
	Loss_DiscardedDoor = 3,
	Error              = 4,
};

// -------------------------------------------------------------------------------------------------

enum class EDoorModification
{
	Added,
	Removed,
};

// -------------------------------------------------------------------------------------------------
