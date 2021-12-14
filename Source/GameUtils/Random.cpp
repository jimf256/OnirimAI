#include "Random.h"
#include <chrono>

// -------------------------------------------------------------------------------------------------

namespace Random
{
	// by default is initialized with a non-deterministic seed
	static std::mt19937 g_generator;
	static int g_seed = 0;
}

// -------------------------------------------------------------------------------------------------

int Random::GetSeed()
{
	return g_seed;
}

// -------------------------------------------------------------------------------------------------

void Random::SetSeed(int seed)
{
	g_seed = seed;
	g_generator.seed(seed);
}

// -------------------------------------------------------------------------------------------------

void Random::SetRandomSeed()
{
	g_seed = static_cast<int>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
	g_generator.seed(g_seed);
}

// -------------------------------------------------------------------------------------------------

std::mt19937& Random::GetRandomNumberEngine()
{
	return g_generator;
}

// -------------------------------------------------------------------------------------------------
