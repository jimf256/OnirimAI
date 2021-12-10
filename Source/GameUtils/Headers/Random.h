#pragma once
#include <random>

// -------------------------------------------------------------------------------------------------

namespace Random
{
	void SetSeed(int seed);
	void SetNonDeterministicSeed();

	std::mt19937& GetRandomNumberEngine();
}

// -------------------------------------------------------------------------------------------------
