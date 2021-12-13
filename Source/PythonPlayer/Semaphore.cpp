#include "Semaphore.h"
#include <cassert>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// -------------------------------------------------------------------------------------------------

Semaphore::Semaphore(const std::string& name)
	: m_name(name)
	, m_handle(NULL)
{
	m_handle = CreateSemaphoreA(NULL, 0, 1, name.c_str());
	assert(m_handle != NULL);
}

// -------------------------------------------------------------------------------------------------

Semaphore::~Semaphore()
{
	bool result = CloseHandle(m_handle);
	assert(result);
}

// -------------------------------------------------------------------------------------------------

void Semaphore::Wait()
{
	WaitForSingleObject(m_handle, INFINITE);
}

// -------------------------------------------------------------------------------------------------

void Semaphore::Release()
{
	ReleaseSemaphore(m_handle, 1, NULL);
}

// -------------------------------------------------------------------------------------------------
