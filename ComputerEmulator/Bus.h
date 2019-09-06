#pragma once

#include <array>
#include "Types.h"
#include "Error.h"
#include "Cpu.h"

class Cpu;

class Bus
{
private:

	//devices
	Cpu* m_cpu = nullptr;
	std::array<BYTE, 1024> m_ram;

public:
	Bus();
	~Bus();

	BYTE read(DWORD address);
	ReturnCodes write(DWORD address, BYTE b);
};
