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
		stat = write(address + i, v >> (8 * i) & 0xff);
	}
	return stat;
}
//@TODO: check if this works
ReturnCodes Cpu::writeDWORD(DWORD address, DWORD v)
{
	ReturnCodes stat = SUCCESS;
	for (register int i = 0; i < 4 && stat != BAD_MEMORY_REQUEST; i++) {
		stat = write(address + i, v >> (8 * i) & 0xff);
	}
	return stat;
}

/*//COMPUTE SECOND OPERAND

void Cpu::computeSecondOperandMod0() {
	BYTE r_m = curr_instruction.getR_M();

	switch (r_m)
	{
	case 0x00:
	{
		if (curr_instruction.getS()) {
			second_operand = &eax;
		}
		else {
			second_operand = &a
		}
	}
	case 0x01:
	{}
	case 0x02:
	{}
	case 0x03:
	{}
	case 0x04:
	{}
	case 0x05:
	{}
	case 0x06:
	{}
	case 0x07:
	{}

	default:
		throw "Invalid r_m val";
	}
}
void Cpu::computeSecondOperandMod1() {
}
void Cpu::computeSecondOperandMod2() {
}
void Cpu::computeSecondOperandMod3() {
}

void Cpu::computeFirstOperand() {
	if (curr_instruction.getS()) {
		//32 bit

		BYTE reg = curr_instruction.getReg();
		switch (reg)
		{
		case 0x0:
		{
			operand_register = &eax;
			break;
		}
		case 0x1:
		{
			operand_register = &ecx;
			break;
		}
		case 0x2:
		{
			operand_register = &edx;
			break;
		}
		case 0x3:
		{
			operand_register = &ebx;
			break;
		}
		case 0x4:
		{
			operand_register = &esp;
			break;
		}
		case 0x5:
		{
			operand_register = &ebp;
			break;
		}
		case 0x6:
		{
			operand_register = &esi;
			break;
		}
		case 0x7:
		{
			operand_register = &edi;
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
			operand_register = &al;
			break;
		}
		case 0x1:
		{
			operand_register = &cl;
			break;
		}
		case 0x2:
		{
			operand_register = &dl;
			break;
		}
		case 0x3:
		{
			operand_register = &bl;
			break;
		}
		case 0x4:
		{
			operand_register = &ah;
			break;
		}
		case 0x5:
		{
			operand_register = &ch;
			break;
		}
		case 0x6:
		{
			operand_register = &dh;
			break;
		}
		case 0x7:
		{
			operand_register = &bh;
			break;
		}
		default:
			throw "Invalid register";
		}
	}
}

void Cpu::decode() {
	computeFirstOperand();

	BYTE mod = curr_instruction.getMod();

	switch (mod)
	{
	case 0x00:
	{
		//register indirect
		computeSecondOperandMod0();

		break;
	}

	case 0x01:
	{
		//8 bit displacement
		computeSecondOperandMod1();

		break;
	}

	case 0x02:
	{
		//32 bit displacement
		computeSecondOperandMod2();

		break;
	}

	case 0x03:
	{
		//register direct

		computeSecondOperandMod3();

		break;
	}

	default:
		throw "Invalid Mode";
	}
}
*/

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

		BYTE _mod = curr_instruction.getMod();
		switch (_mod)
		{
		case 0x0:
		{
			break;
		}
		case 0x1:
		{
			break;
		}
		case 0x2:
		{
			break;
		}
		case 0x3:
		{
			//r_m is register
			BYTE _reg = curr_instruction.getReg();

			switch (_reg)
			{
			case 0x0:
			{
				first = &eax;
				break;
			}
			case 0x1:
			{
				first = &ecx;

				break;
			}
			case 0x2:
			{
				first = &edx;

				break;
			}
			case 0x3:
			{
				first = &ebx;

				break;
			}
			case 0x4:
			{
				first = &esp;

				break;
			}
			case 0x5:
			{
				first = &ebp;

				break;
			}
			case 0x6:
			{
				first = &esi;

				break;
			}
			case 0x7:
			{
				first = &edi;

				break;
			}

			default:
				throw "Invalid Reg";
			}
			BYTE _r_m = curr_instruction.getR_M();

			switch (_r_m)
			{
			case 0x0:
			{
				second = &eax;
				break;
			}
			case 0x1:
			{
				second = &ecx;

				break;
			}
			case 0x2:
			{
				second = &edx;

				break;
			}
			case 0x3:
			{
				second = &ebx;

				break;
			}
			case 0x4:
			{
				second = &esp;

				break;
			}
			case 0x5:
			{
				second = &ebp;

				break;
			}
			case 0x6:
			{
				second = &esi;

				break;
			}
			case 0x7:
			{
				second = &edi;

				break;
			}

			default:
				throw "Invalid r_m";
			}
			break;
		}

		default:
			throw "Invalid Mode";
		}
		if (curr_instruction.getR_X()) {
			*second += *first;
		}
		else {
			*first += *second;
		}
	}
	else {
		//8 bit
	}
}