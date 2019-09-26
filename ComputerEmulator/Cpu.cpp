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
				first = &al;
				break;
			}
			case 0x1:
			{
				first = &cl;

				break;
			}
			case 0x2:
			{
				first = &dl;

				break;
			}
			case 0x3:
			{
				first = &bl;

				break;
			}
			case 0x4:
			{
				first = &ah;

				break;
			}
			case 0x5:
			{
				first = &ch;

				break;
			}
			case 0x6:
			{
				first = &dh;

				break;
			}
			case 0x7:
			{
				first = &bh;

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
				second = &al;
				break;
			}
			case 0x1:
			{
				second = &cl;

				break;
			}
			case 0x2:
			{
				second = &dl;

				break;
			}
			case 0x3:
			{
				second = &bl;

				break;
			}
			case 0x4:
			{
				second = &ah;

				break;
			}
			case 0x5:
			{
				second = &ch;

				break;
			}
			case 0x6:
			{
				second = &dh;

				break;
			}
			case 0x7:
			{
				second = &bh;

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
		if (!curr_instruction.getR_X()) {
			*second += *first;
		}
		else {
			*first += *second;
		}
	}
}