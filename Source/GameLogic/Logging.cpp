#include "Logging.h"
#include <fstream>
#include <cassert>

// -------------------------------------------------------------------------------------------------

namespace Logging
{
	static std::ofstream g_logfile;
	static PlatformLogHook g_platformHook = nullptr;
}

// -------------------------------------------------------------------------------------------------

void Logging::Initialize(const std::string& filename, PlatformLogHook platformHook)
{
#if LOGGING_ENABLED

	g_platformHook = platformHook;

	if (g_logfile.is_open())
	{
		assert(!"Logging::Initialize() encountered an already-open file handle");
		g_logfile.flush();
		g_logfile.close();
	}

	g_logfile.open(filename, std::ios_base::out);
	assert(g_logfile.is_open());

#endif
}

// -------------------------------------------------------------------------------------------------

void Logging::ShutDown()
{
#if LOGGING_ENABLED

	if (g_logfile.is_open())
	{
		g_logfile.flush();
		g_logfile.close();
	}

#endif
}

// -------------------------------------------------------------------------------------------------

void Logging::Write(const std::string& text)
{
#if LOGGING_ENABLED

	// write the string to the log file
	assert(g_logfile.is_open());
	if (g_logfile.is_open())
	{
		g_logfile << text << "\n";
		g_logfile.flush();
	}

	// send the string to the platform log
	if (g_platformHook != nullptr)
	{
		g_platformHook(text + "\n");
	}

#endif
}

// -------------------------------------------------------------------------------------------------
