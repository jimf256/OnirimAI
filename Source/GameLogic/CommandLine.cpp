#include "CommandLine.h"
#include <vector>
#include <unordered_map>
#include <cstdlib>
#include <cassert>

// -------------------------------------------------------------------------------------------------

namespace CommandLine
{
	static std::string g_fullCommandLine;
	static std::vector<std::string> g_args;
	static std::unordered_map<std::string, int> g_intValues;
	static std::unordered_map<std::string, float> g_floatValues;
	static std::unordered_map<std::string, std::string> g_stringValues;
}

// -------------------------------------------------------------------------------------------------

void CommandLine::Set(int count, char* arguments[])
{
	// cache the args and concatenate them to build the full commandline
	g_fullCommandLine = "";
	g_args.clear();
	for (int i = 0; i < count; ++i)
	{
		g_args.push_back(arguments[i]);
		g_fullCommandLine += arguments[i];
		if (i != count - 1)
		{
			g_fullCommandLine += " ";
		}
	}

	// find args with an '=' and sort them into those with int, float or string values
	for (auto it = g_args.begin(); it != g_args.end(); ++it)
	{
		auto found = (*it).find('=');
		if (found != it->npos)
		{
			std::string key = it->substr(0, found);
			std::string value = it->substr(found + 1);

			// first attempt to convert to integer
			char* end = 0;
			int i = strtol(value.c_str(), &end, 10);
			if (end == value.c_str())
			{
				// next attempt to convert to float
				float f = strtof(value.c_str(), &end);
				if (end == value.c_str())
				{
					// we found a string
					assert(g_stringValues.find(key) == g_stringValues.end());
					g_stringValues[key] = value;
				}
				else
				{
					// we found a float
					assert(g_floatValues.find(key) == g_floatValues.end());
					g_floatValues[key] = f;
				}
			}
			else
			{
				// we found an integer
				assert(g_intValues.find(key) == g_intValues.end());
				g_intValues[key] = i;
			}
		}
	}
}

// -------------------------------------------------------------------------------------------------

const std::string& CommandLine::GetFullCommandLine()
{
	return g_fullCommandLine;
}

// -------------------------------------------------------------------------------------------------

std::size_t CommandLine::GetNumArgs()
{
	return g_args.size();
}

// -------------------------------------------------------------------------------------------------

std::string CommandLine::GetArg(std::size_t index)
{
	assert(index < g_args.size());
	if (index < g_args.size())
	{
		return g_args[index];
	}

	return "";
}

// -------------------------------------------------------------------------------------------------

bool CommandLine::FindValue(const std::string& key, int& outValue)
{
	auto found = g_intValues.find(key);
	if (found != g_intValues.end())
	{
		outValue = found->second;
		return true;
	}

	return false;
}

// -------------------------------------------------------------------------------------------------

bool CommandLine::FindValue(const std::string& key, float& outValue)
{
	auto found = g_floatValues.find(key);
	if (found != g_floatValues.end())
	{
		outValue = found->second;
		return true;
	}

	return false;
}

// -------------------------------------------------------------------------------------------------

bool CommandLine::FindValue(const std::string& key, std::string& outValue)
{
	auto found = g_stringValues.find(key);
	if (found != g_stringValues.end())
	{
		outValue = found->second;
		return true;
	}

	return false;
}

// -------------------------------------------------------------------------------------------------
