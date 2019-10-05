#include "Bus.h"

Bus::Bus()
{
	m_cpu = new Cpu();
	m_cpu->attachBus(this);
	m_ram = new BYTE[RAM_SIZE];
	memset(m_ram, 0, sizeof(BYTE) * RAM_SIZE);
	m_cpu->test();
}

Bus::~Bus()
{
	delete[] m_ram;
}

BYTE Bus::read(DWORD address)
{
	if (address >= 0x00 && address < RAM_SIZE)
		return m_ram[address];
	else
		throw "Bad Memory Request";
}

ReturnCodes Bus::write(DWORD address, BYTE b)
{
	if (address >= 0x00 && address < RAM_SIZE)
	{
		m_ram[address] = b;
		return ReturnCodes::SUCCESS;
	}
	else
		return ReturnCodes::BAD_MEMORY_REQUEST;
}