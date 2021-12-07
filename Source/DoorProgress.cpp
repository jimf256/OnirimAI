#include "DoorProgress.h"
#include <cassert>

// -------------------------------------------------------------------------------------------------

DoorProgress::DoorProgress(int keysPerColor)
	: m_doorsRequired(keysPerColor)
{
}

// -------------------------------------------------------------------------------------------------

bool DoorProgress::HasAllDoors() const
{
	for (std::size_t i = 0; i < m_doors.size(); ++i)
	{
		if (m_doors[i] != m_doorsRequired)
		{
			return false;
		}
	}

	return true;
}

// -------------------------------------------------------------------------------------------------

bool DoorProgress::HasAllDoors(EColor color) const
{
	assert(color != EColor::None);
	if (color != EColor::None)
	{
		return (m_doors[static_cast<int>(color)] == m_doorsRequired);
	}

	return false;
}

// -------------------------------------------------------------------------------------------------

int DoorProgress::CountDoors() const
{
	int count = 0;
	for (std::size_t i = 0; i < m_doors.size(); ++i)
	{
		count += m_doors[i];
	}

	return count;
}

// -------------------------------------------------------------------------------------------------

int DoorProgress::CountDoors(EColor color) const
{
	assert(color != EColor::None);
	if (color != EColor::None)
	{
		return m_doors[static_cast<int>(color)];
	}

	return 0;
}

// -------------------------------------------------------------------------------------------------

void DoorProgress::AddDoor(EColor color)
{
	assert(color != EColor::None);
	if (color != EColor::None)
	{
		int index = static_cast<int>(color);
		if (m_doors[index] < m_doorsRequired)
		{
			m_doors[index]++;
		}
	}
}

// -------------------------------------------------------------------------------------------------

void DoorProgress::RemoveDoor(EColor color)
{
	assert(color != EColor::None);
	if (color != EColor::None)
	{
		int index = static_cast<int>(color);
		if (m_doors[index] > 0)
		{
			m_doors[index]--;
		}
	}
}

// -------------------------------------------------------------------------------------------------
