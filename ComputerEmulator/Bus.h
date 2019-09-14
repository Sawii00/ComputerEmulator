#pragma once

#include "Types.h"
#include "Error.h"
#include "Cpu.h"

class Cpu;

class Bus
{
private:

	//devices
	Cpu* m_cpu = nullptr;
	BYTE* m_ram = nullptr;

public:
	Bus();
	~Bus();

	BYTE read(DWORD address);
	ReturnCodes write(DWORD address, BYTE b);
};
