#pragma once
#include <string>
#include <functional>

// -------------------------------------------------------------------------------------------------

#define LOGGING_ENABLED (_DEBUG)

// -------------------------------------------------------------------------------------------------

#if LOGGING_ENABLED
#define LOG(str) Logging::Write(str)
#else
#define LOG(str) void(0)
#endif

// -------------------------------------------------------------------------------------------------

namespace Logging
{
	typedef std::function<void(const std::string&)> PlatformLogHook;

	void Initialize(const std::string& filename, PlatformLogHook platformHook = nullptr);
	void Write(const std::string& text);
}

// -------------------------------------------------------------------------------------------------
