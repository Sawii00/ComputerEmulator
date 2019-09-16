#include "Bus.h"

Bus::Bus()
{
	m_cpu = new Cpu();
	m_cpu->attachBus(this);
	m_ram = new BYTE[0x400];
	memset(m_ram, 0, sizeof(BYTE) * 0x400);
	//m_cpu->test();
}

Bus::~Bus()
{
	delete[] m_ram;
}

BYTE Bus::read(DWORD address)
{
	if (address >= 0x00 && address < 0x400)
		return m_ram[address];
	else
		throw "Bad Memory Request";
}

ReturnCodes Bus::write(DWORD address, BYTE b)
{
	if (address >= 0x00 && address < 0x0400)
	{
		m_ram[address] = b;
		return ReturnCodes::SUCCESS;
	}
	else
		return ReturnCodes::BAD_MEMORY_REQUEST;
}