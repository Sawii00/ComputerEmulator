#pragma once

#include "Types.h"
#include "Error.h"
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

    template<typename T>
    T* convertAddress(DWORD address){

        if(address >= 0 && address < RAM_SIZE) 
            return (T*)(m_ram + address);
        else
            return (T*)nullptr;
    }

	BYTE read(DWORD address);
	ReturnCodes write(DWORD address, BYTE b);
};
