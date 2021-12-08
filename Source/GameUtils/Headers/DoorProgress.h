#pragma once
#include "Enums.h"
#include <array>

// -------------------------------------------------------------------------------------------------

class DoorProgress
{
public:
	DoorProgress(int doorsPerColor = 2);

	int GetDoorsRequired() const { return m_doorsRequired; }

	bool HasAllDoors() const;
	bool HasAllDoors(EColor color) const;

	int CountDoors() const;
	int CountDoors(EColor color) const;
	
	void AddDoor(EColor color);
	void RemoveDoor(EColor color);

private:
	int m_doorsRequired;
	std::array<int, 4> m_doors;
};

// -------------------------------------------------------------------------------------------------
