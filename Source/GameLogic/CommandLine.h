#pragma once
#include <string>

// -------------------------------------------------------------------------------------------------

namespace CommandLine
{
	void Set(int count, char* arguments[]);

	const std::string& GetFullCommandLine();
	std::size_t GetNumArgs();
	std::string GetArg(std::size_t index);

	bool FindValue(const std::string& key, int& outValue);
	bool FindValue(const std::string& key, float& outValue);
	bool FindValue(const std::string& key, std::string& outValue);
};

// -------------------------------------------------------------------------------------------------
