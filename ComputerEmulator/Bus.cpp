#include "Bus.h"

Bus::Bus()
{
	m_cpu->attachBus(this);
}

Bus::~Bus()
{
}

BYTE Bus::read(DWORD address)
{
	if (address >= 0x00 && address <= 0x400)
		return m_ram[address];
	else
		return 0x00;
}

ReturnCodes Bus::write(DWORD address, BYTE b)
{
	if (address >= 0x00 && address <= 0x0400)
	{
		m_ram[address] = b;
		return ReturnCodes::SUCCESS;
	}
	else
		return ReturnCodes::BAD_MEMORY_REQUEST;
}