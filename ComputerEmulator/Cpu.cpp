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
//have no clue if that works
ReturnCodes Cpu::writeWORD(DWORD address, WORD v)
{
	ReturnCodes stat = SUCCESS;
	for (register int i = 0; i < 2 && stat != BAD_MEMORY_REQUEST; i++) {
		stat = writeBYTE(address + i, v >> (8 * i) & 0xff);
	}
	return stat;
}
//@TODO: check if this works
ReturnCodes Cpu::writeDWORD(DWORD address, DWORD v)
{
	ReturnCodes stat = SUCCESS;
	for (register int i = 0; i < 4 && stat != BAD_MEMORY_REQUEST; i++) {
		stat = writeBYTE(address + i, v >> (8 * i) & 0xff);
	}
	return stat;
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
	curr_instruction = Instruction(readWORD(pc));
	pc += 2;
	/*BYTE op = curr_instruction.getOpCode();
	BYTE r = curr_instruction.getR_X();
	BYTE s = curr_instruction.getS();
	BYTE m = curr_instruction.getMod();
	BYTE reg = curr_instruction.getReg();
	BYTE r_ = curr_instruction.getR_M();*/
}

void Cpu::execute()
{
	(m_instruction_list[curr_instruction.getOpCode()].instruction_function)(this);
}
//instructions

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