#pragma once
#include <string>

typedef void* HANDLE;

// -------------------------------------------------------------------------------------------------

class Semaphore
{
public:
	Semaphore(const std::string& name);
	~Semaphore();

	void Wait();
	void Release();

private:
	std::string m_name;
	HANDLE m_handle;
};

// -------------------------------------------------------------------------------------------------
