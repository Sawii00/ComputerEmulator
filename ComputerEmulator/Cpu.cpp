#include "Cpu.h"

Cpu::Cpu()
{
	//register initialization
	eax = 0x00;
	ebx = 0x00;
	ecx = 0x00;
	edx = 0x00;
	esi = 0x00;
	ebi = 0x00;
	esp = 0x00;
	ebp = 0x00;
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
BYTE Cpu::read(DWORD address)
{
	return m_bus->read(address);
}

WORD Cpu::readWORD(DWORD address)
{
	return(read(address++) << 8 | read(address));
}

DWORD Cpu::readDWORD(DWORD address)
{
	return (read(address++) << 24 | read(address++) << 16 | read(address++) << 8 | read(address));
}

ReturnCodes Cpu::write(DWORD address, BYTE v)
{
	return m_bus->write(address, v);
}
//have no clue if that works
ReturnCodes Cpu::writeWORD(DWORD address, WORD v)
{
	ReturnCodes stat = SUCCESS;
	for (register int i = 0; i < 2 && stat != BAD_MEMORY_REQUEST; i++) {
		stat = write(address, v >> (8 * i) & 0xff);
	}
	return stat;
}
//@TODO: check if this works
ReturnCodes Cpu::writeDWORD(DWORD address, DWORD v)
{
	ReturnCodes stat = SUCCESS;
	for (register int i = 0; i < 4 && stat != BAD_MEMORY_REQUEST; i++) {
		stat = write(address, v >> (8 * i) & 0xff);
	}
	return stat;
}

void Cpu::decode() {
	if (curr_instruction.getS()) {
		//32 bit

		BYTE reg = curr_instruction.getReg();
		switch (reg)
		{
		case 0x0:
		{
			operand_register = eax;
			break;
		}
		case 0x1:
		{
			operand_register = ecx;
			break;
		}
		case 0x2:
		{
			operand_register = edx;
			break;
		}
		case 0x3:
		{
			operand_register = ebx;
			break;
		}
		case 0x4:
		{
			operand_register = esp;
			break;
		}
		case 0x5:
		{
			operand_register = ebp;
			break;
		}
		case 0x6:
		{
			operand_register = esi;
			break;
		}
		case 0x7:
		{
			operand_register = edi;
			break;
		}
		default:
			throw "Invalid register";
		}
	}
	else {
		//8 bit
		BYTE reg = curr_instruction.getReg();
		switch (reg)
		{
		case 0x0:
		{
			operand_register = al;
			break;
		}
		case 0x1:
		{
			operand_register = cl;
			break;
		}
		case 0x2:
		{
			operand_register = dl;
			break;
		}
		case 0x3:
		{
			operand_register = bl;
			break;
		}
		case 0x4:
		{
			operand_register = ah;
			break;
		}
		case 0x5:
		{
			operand_register = ch;
			break;
		}
		case 0x6:
		{
			operand_register = dh;
			break;
		}
		case 0x7:
		{
			operand_register = bh;
			break;
		}
		default:
			throw "Invalid register";
		}
	}
}

//the address to be read from is set by the handle_addressing_mode into abs_address o rel_address
void Cpu::fetch()
{
	//decode next instruction
	//read next instruction from memory at program counter
	curr_instruction = Instruction(readWORD(pc++));
	/*BYTE op = curr_instruction.getOpCode();
	BYTE r = curr_instruction.getR_X();
	BYTE s = curr_instruction.getS();
	BYTE m = curr_instruction.getMod();
	BYTE reg = curr_instruction.getReg();
	BYTE r_ = curr_instruction.getR_M();*/

	decode();
}

void Cpu::execute()
{
}

void Cpu::AND() {
}

void Cpu::OR() {}

void Cpu::XOR() {}

void Cpu::NOT() {
}

void Cpu::CLC() {
	setFlag(C, 0);
}

void Cpu::CLI() {
	setFlag(IE, 0);
}
void Cpu::STC() {
	setFlag(C, 1);
}
void Cpu::STI() {
	setFlag(IE, 1);
}

void Cpu::NEG() {
}

void Cpu::INC() {
}
void Cpu::DEC() {
}

void Cpu::MOV() {}

void Cpu::PUSH() {
}

void Cpu::POP() {
}

void Cpu::JMP() {
}
void Cpu::JE() {
}
void Cpu::JNE() {
}
void Cpu::JA() {
}
void Cpu::JAE() {
}
void Cpu::JB() {
}
void Cpu::JBE() {
}

void Cpu::CALL() {
}

void Cpu::RET() {
}