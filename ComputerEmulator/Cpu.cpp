#include "Cpu.h"

Cpu::Cpu()
{
	//register initialization
	eax = 0x00;
	ebx = 0x00;
	ecx = 0x00;
	edx = 0x00;
	esi = 0x00;
	edi = 0x00;
	esp = 0x00;
	ebp = 0x00;

	m_instruction_list[0] = DisassembledInstruction(std::string("ADD"), &Cpu::ADD, (BYTE)1);
}

Cpu::~Cpu()
{
}

void Cpu::attachBus(Bus * bus)
{
	m_bus = bus;
}

bool Cpu::getFlag(FLAGS f)
{
	return (flags & f) > 0 ? true : false;
}

void Cpu::setFlag(FLAGS f, bool v)
{
	if (v) {
		flags |= f;
	}
	else {
		flags &= ~f;
	}
}

void Cpu::clearFlags()
{
	flags = 0x00;
}

void Cpu::clock()
{
}
BYTE Cpu::readBYTE(DWORD address)
{
	return m_bus->read(address);
}

WORD Cpu::readWORD(DWORD address)
{
	return(readBYTE(address + 1) << 8 | readBYTE(address));
}

DWORD Cpu::readDWORD(DWORD address)
{
	return (readBYTE(address + 3) << 24 | readBYTE(address + 2) << 16 | readBYTE(address + 1) << 8 | readBYTE(address));
}

ReturnCodes Cpu::writeBYTE(DWORD address, BYTE v)
{
	return m_bus->write(address, v);
}
ReturnCodes Cpu::writeWORD(DWORD address, WORD v)
{
	ReturnCodes stat = SUCCESS;
	for (int i = 0; i < 2 && stat != BAD_MEMORY_REQUEST; i++) {
		stat = writeBYTE(address + i, v >> (8 * i) & 0xff);
	}
	return stat;
}
//@TODO: check if this works
ReturnCodes Cpu::writeDWORD(DWORD address, DWORD v)
{
	ReturnCodes stat = SUCCESS;
	for (int i = 0; i < 4 && stat != BAD_MEMORY_REQUEST; i++) {
		stat = writeBYTE(address + i, v >> (8 * i) & 0xff);
	}
	return stat;
}

DWORD Cpu::handleSIBInstruction()
{
	{
		SIBByte sib(readFromPC<BYTE>());

		BYTE scale = sib.getScale();

		DWORD final_address = 0x00;

		BYTE index = sib.getIndex();
		switch (index)
		{
		case 0x00:
		{
			final_address = eax;
			break;
		}
		case 0x01:
		{
			final_address = ecx;

			break;
		}
		case 0x02:
		{
			final_address = edx;

			break;
		}
		case 0x03:
		{
			final_address = ebx;

			break;
		}
		case 0x04:
		{
			break;
		}
		case 0x05:
		{
			final_address = ebp;

			break;
		}
		case 0x06:
		{
			final_address = esi;

			break;
		}
		case 0x07:
		{
			final_address = edi;

			break;
		}
		default:
			throw "Invalid Index";
		}

		final_address = final_address << scale;

		BYTE base = sib.getBase();
		switch (base)
		{
		case 0x00:
		{
			final_address += eax;
			break;
		}
		case 0x01:
		{
			final_address += ecx;

			break;
		}
		case 0x02:
		{
			final_address += edx;

			break;
		}
		case 0x03:
		{
			final_address += ebx;

			break;
		}
		case 0x04:
		{
			final_address += esp;
		}
		case 0x05:
		{
			//TODO(important): fix this according to the correct intel guidelines
			final_address += ebp;

			break;
		}
		case 0x06:
		{
			final_address += esi;

			break;
		}
		case 0x07:
		{
			final_address += edi;

			break;
		}
		default:
			throw "Invalid Index";
		}

		return final_address;
	}
}

void Cpu::print_registers() {
	std::cout << "eax: " << eax << '\n';
	std::cout << "ebx: " << ebx << '\n';
	std::cout << "ecx: " << ecx << '\n';
	std::cout << "edx: " << edx << '\n';
	std::cout << "esi: " << esi << '\n';
	std::cout << "edi: " << edi << '\n';
	std::cout << "esp: " << esp << '\n';
	std::cout << "ebp: " << ebp << '\n';
}

void Cpu::fetch()
{
	//decode next instruction
	//read next instruction from memory at program counter
	handlePrefixes();
	BYTE opcode = readFromPC<BYTE>();
	BYTE mod_rm = readFromPC<BYTE>();
	curr_instruction.setInstruction(opcode << 8 | mod_rm);
}

void Cpu::execute()
{
	(m_instruction_list[curr_instruction.getOpCode()].instruction_function)(this);
}
//instructions

void Cpu::handlePrefixes()
{
	BYTE pref = readBYTE(pc);
	switch (pref)
	{
		//the first prefixes are skipped.

	case 0x2E:
	{
		//CS segment override
		pc++;
		break;
	}
	case 0x36:
	{
		//SS segment override
		pc++;
		break;
	}
	case 0x3E:
	{
		//DS segment override
		pc++;
		break;
	}
	case 0x26:
	{
		//ES segment override
		pc++;
		break;
	}
	case 0x64:
	{
		//FS segment override
		pc++;
		break;
	}
	case 0x65:
	{
		//GS segment override
		pc++;
		break;
	}
	case 0x66:
	{
		//operand size override
		curr_instruction.is_16bit_operand = true;
		pc++;
		break;
	}
	case 0x67:
	{
		//address size override
		curr_instruction.is_16bit_addressing = true;
		pc++;
		break;
	}
	}
}

void Cpu::ADD()
{
	if (curr_instruction.getS()) {
		//32-16 bit
		//@TODO(sawii) check if its 16 via prefixes

		DWORD* first = nullptr;
		DWORD* second = nullptr;

		handleModRM<DWORD>(first, second);

		if (!curr_instruction.getR_X()) {
			*second += *first;
		}
		else {
			*first += *second;
		}
	}
	else {
		//8 bit

		BYTE* first = nullptr;
		BYTE* second = nullptr;

		handleModRM<BYTE>(first, second);
		if (!curr_instruction.getR_X()) {
			*second += *first;
		}
		else {
			*first += *second;
		}
	}
}