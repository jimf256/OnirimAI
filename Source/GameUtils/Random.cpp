#include "Random.h"

// -------------------------------------------------------------------------------------------------

namespace Random
{
	// by default is initialized with a non-deterministic seed
	static std::mt19937 g_generator;
}

// -------------------------------------------------------------------------------------------------

void Random::SetSeed(int seed)
{
	g_generator.seed(seed);
}

// -------------------------------------------------------------------------------------------------

void Random::SetNonDeterministicSeed()
{
	g_generator.seed(std::random_device()());
}

// -------------------------------------------------------------------------------------------------

std::mt19937& Random::GetRandomNumberEngine()
{
	return g_generator;
}

// -------------------------------------------------------------------------------------------------
