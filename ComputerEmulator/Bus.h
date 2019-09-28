#pragma once
#include "Types.h"
#include "Error.h"

//prototypes

template<typename Z> Z* convertAddress(DWORD);

#include "Cpu.h"

#define RAM_SIZE 2000

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

	template<typename Z>
	inline Z* convertAddress(DWORD address) {
		if (address >= 0 && address < RAM_SIZE)
			return (Z*)(m_ram + address);
		else
			return (Z*)nullptr;
	}

	BYTE read(DWORD address);
	ReturnCodes write(DWORD address, BYTE b);
};
