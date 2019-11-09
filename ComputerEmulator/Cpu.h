#pragma once
#include "Utils.h"
#include <string>
#include "Bus.h"
#include <array>
#include <functional>
#include <cstring>
#include <iostream>

/*

- implement segment registers
- continue instruction table

http://www.c-jump.com/CIS77/CPU/x86/lecture.html

*/
/*
@TODO:
 - refactor throws with ASSERT

 - implement 8 bit instruction decoder
 - implement prefixes and 16 bit instruction decoder
 - implement segment registers
 - implement asynchronous events and interrupts

*/

/*
 ISSUES:

*/
class Bus;
class Cpu;

/*
 The purpose of this struct is to distinguish every instruction and package the corresponding function pointer,
 how many cycles it takes (to correctly emulate the machine) and a string_like name to disassemble it

*/

struct DisassembledInstruction {
	DisassembledInstruction(std::string n = "", std::function<void(Cpu*)>f = nullptr, BYTE c = 0)
		:name(n), instruction_function(f), cycles(c) {};

	std::string name;
	std::function<void(Cpu*)>instruction_function;
	BYTE cycles;
};

/*
 This instruction class encapsulates the first two bytes of the instruction (The opcode and the ModRM byte) and
 offers methods for quickly access the different parts of the two bytes

 The bytes are in the form:
 ------|-|-     --|---|---
 opcode|r_x|s   mod|reg|r_m

 opcode: identifies the instruction
 r_x: 1 = r_m --> reg
   0 = reg --> r_m
 s: 1 = 32bit
	0 = 8bit instruction

 mod: 00 = indirect register mode
	01 = 8bit displacement
	10 = 32 bit displacement
	11 = direct register mode

 reg: 000 = eax
   001 = ecx
   010 = edx
   011 = ebx
   100 = esp
   101 = ebp
   110 = esi
   111 = edi

 r_m: depends on the current mod

*/

struct Instruction {
	WORD inst;
	bool is_16bit_operand = false;
	bool is_16bit_addressing = false;
	Instruction(WORD word = 0)
		: inst(word) {}

	void setInstruction(WORD w) {
		inst = w;
	}

	BYTE getOpCode() const {
		return inst >> 0xA & 0x3F;
	}

	BYTE getR_X() const {
		return inst >> 0x9 & 0x1;
	}

	BYTE getS() const {
		return inst >> 0x8 & 0x1;
	}

	BYTE getMod() const {
		return inst >> 0x6 & 0x3;
	}
	BYTE getReg() const {
		return inst >> 0x3 & 0x7;
	}
	BYTE getR_M() const {
		return inst & 0x7;
	}

	BYTE getOpCodeByte() const {
		return inst >> 0x8 & 0xFF;
	}
	BYTE getModRMByte() const {
		return inst & 0xFF;
	}
};

/*
 This struct encapsulates the ScaleIndexBase byte that might follow the modRM byte

 Structure:
 --|---|---
 scale|index|base

 scale: 00 = index*1
	 01 = index*2
	 10 = index*4
	 11 = index*8

 index: 000 = eax
	 001 = ecx
	 010 = edx
	 011 = ebx
	 100 = illegal
	 101 = ebp
	 110 = esi
	 111 = edi

 base:  000 = eax
	 001 = ecx
	 010 = edx
	 011 = ebx
	 100 = esp
	 101 = if(mod == 00) displacement only else if(mod == 01 || mod == 10) ebp
	 110 = esi
	 111 = edi

*/

/*
 This struct encapsulates utility methods for the SIBByte that might follow some instructions

 It is in the form:
 --|---|---
 scale|index|base

 The final address will be computed as: index*2^scale + base

*/

struct SIBByte {
	BYTE  sib;

	SIBByte(BYTE b)
		:sib(b) {}

	BYTE getScale() const {
		return sib >> 0x6 & 0x3;
	}

	BYTE getIndex() const {
		return sib >> 0x3 & 0x7;
	}
	BYTE getBase() const {
		return sib & 0x7;
	}
};

class Cpu
{
private:

	//pointer to the bus that will hold all the devices inside the emulated computer.
	//it abstracts useful functions such as read and write to specific addresses on the bus
	Bus* m_bus = nullptr;

	//REGISTERS
	//Through the usage of unions and structs it is possible to create Addressable Registers so that it's possible
	//to address the lower WORD, and BYTES of each GP register

	//basically the structure of an addressable register is:
	// --------|--------|--------|--------|
	// the whole register is eax
	//                  |--------|--------|
	// the rightmost 2 bytes are ax
	//                  |--------|
	// the high byte of ax is ah (high)
	//                           |--------|
	// the low byte of ax is al (low);

	union {
		DWORD eax;

		struct
		{
			WORD ax;
			WORD _unused;
		} word;

		struct
		{
			BYTE al;
			BYTE ah;
			WORD _unused;
		} bytes;
	} EAX;
	DWORD& eax = EAX.eax;
	WORD& ax = EAX.word.ax;
	BYTE& ah = EAX.bytes.ah;
	BYTE& al = EAX.bytes.al;

	union {
		DWORD ebx;

		struct
		{
			WORD bx;
			WORD _unused;
		} word;

		struct
		{
			BYTE bl;
			BYTE bh;
			WORD _unused;
		}bytes;
	} EBX;
	DWORD& ebx = EBX.ebx;
	WORD& bx = EBX.word.bx;
	BYTE& bh = EBX.bytes.bh;
	BYTE& bl = EBX.bytes.bl;
	union {
		DWORD ecx;

		struct
		{
			WORD cx;
			WORD _unused;
		} word;

		struct
		{
			BYTE cl;
			BYTE ch;
			WORD _unused;
		}bytes;
	}ECX;
	DWORD& ecx = ECX.ecx;
	WORD& cx = ECX.word.cx;
	BYTE& ch = ECX.bytes.ch;
	BYTE& cl = ECX.bytes.cl;
	union {
		DWORD edx;

		struct
		{
			WORD dx;
			WORD _unused;
		} word;

		struct
		{
			BYTE dl;
			BYTE dh;
			WORD _unused;
		}bytes;
	}EDX;
	DWORD& edx = EDX.edx;
	WORD& dx = EDX.word.dx;
	BYTE& dh = EDX.bytes.dh;
	BYTE& dl = EDX.bytes.dl;
	union {
		DWORD esi;

		struct
		{
			WORD si;
			WORD _unused;
		} word;
	}ESI;
	DWORD& esi = ESI.esi;
	WORD& si = ESI.word.si;
	union {
		DWORD edi;

		struct
		{
			WORD di;
			WORD _unused;
		} word;
	}EDI;
	DWORD& edi = EDI.edi;
	WORD& di = EDI.word.di;
	union {
		DWORD esp;

		struct
		{
			WORD sp;
			WORD _unused;
		} word;
	}ESP;
	DWORD& esp = ESP.esp;
	WORD& sp = ESP.word.sp;
	union {
		DWORD ebp;

		struct
		{
			WORD bp;
			WORD _unused;
		} word;
	}EBP;
	DWORD& ebp = EBP.ebp;
	WORD& bp = EBP.word.bp;

	//the other registers are not addressable
	DWORD pc = 0x00000000; //program counter.

	BYTE flags = 0x00;
	//segment registers
	WORD cs = 0x0000; //code
	WORD ds = 0x0000; //data
	WORD ss = 0x0000; //stack
	WORD es = 0x0000; //extra
	WORD fs = 0x0000;
	WORD gs = 0x0000;

	///////////////////END REGISTERS

	//////////////////////INTRUCTIONS VARIABLE

	//curr_instruction holds the current instruction that is fetched from memory
	//it enables every function to access parts of it
	Instruction curr_instruction;

	//@TODO(sawii): populate instruction list
	//this is going to hold all the possible opcodes and instructions that we want to support
	std::array<DisassembledInstruction, 256> m_instruction_list;

public:

	//The enum that is currently holding the flags is set up in such a way that is sufficient for us to directly specify it into any bitwise operation with the actual status register.
	// for example if I want to check whether a flag is set it is possible to directly: if(status & N)... this is because N is actually 1<<2 (the third bit in the status register)

	enum FLAGS {
		C = 1 << 0,
		Z = 1 << 1,
		N = 1 << 2,
		IE = 1 << 3,
		BR = 1 << 4,
		O = 1 << 5,
		UNUSED = 1 << 6,
		UNUSED_2 = 1 << 7
	};

	Cpu();
	~Cpu();
	//attaches the cpu to the bus
	void attachBus(Bus* bus);
	//Helper method for dealing with flags
	bool getFlag(FLAGS f);
	void setFlag(FLAGS f, bool v);
	void clearFlags();

	void clock();

	void fetch();
	void fetchModRM();

	void execute();

	void print_registers();

	void test() {
		eax = 0;
		ebx = 1;
		ecx = 2;
		edx = 3;

		//writing machine code to memory
		ebp = 1024;
		constexpr int size = 38;
		BYTE arr[size] = { 0x03, 0x01, 0x03, 0x04, 0x4A, 0x03, 0x05, 0x3F, 0x00,
			0x00, 0x00, 0x03, 0x43, 0x0A, 0x03, 0x44, 0x59, 0x0B, 0x03, 0x83,
			0x00, 0x01, 0x00, 0x00, 0x03, 0x84, 0x5a, 0x00, 0x01, 0x00, 0x00, 0x01, 0xD8,
		0x05, 0xaa, 0x00,0x00,0x00 };
		for (int i = 0; i < size; i++) {
			writeBYTE(ebp++, arr[i]);
		}

		pc = 1024;
		writeBYTE(0x02, 2);
		writeBYTE(0x07, 3);
		writeBYTE(0x3F, 5);
		writeBYTE(0xB, 8);
		writeBYTE(0xF, 7);
		writeBYTE(0x101, 17);
		writeBYTE(0x105, 15);
		for (int i = 0; i < 9; i++) {
			fetch();
			execute();
			print_registers();
			std::cout << "\n\n";
		}
	}
	void interrupt();
	void non_maskable_interrupt();

	BYTE readBYTE(DWORD address);
	DWORD readDWORD(DWORD address);
	WORD readWORD(DWORD address);

	//this method returns the Data read using the PC as address (it is used for instructions and subsequent bytes and TAKES CARE OF INCREASING THE PROGRAM COUNTER
	//it can return BYTE, WORD and DWORD (32 bit)

	template <typename TYPE>
	TYPE readFromPC() {
		TYPE res = 0;
		if (sizeof(TYPE) == 1) {
			res = readBYTE(pc++);
		}
		else if (sizeof(TYPE) == 2) {
			res = readWORD(pc);
			pc += 2;
		}
		else if (sizeof(TYPE) == 4) {
			res = readDWORD(pc);
			pc += 4;
		}
		else {
			throw "Invalid size";
		}
		return res;
	}

	ReturnCodes writeBYTE(DWORD address, BYTE v);
	ReturnCodes writeWORD(DWORD address, WORD v);

	ReturnCodes writeDWORD(DWORD address, DWORD v);

	void handlePrefixes();

	DWORD handleSIBInstruction();

	//Generic method that decodes the ModRM Byte depending on the instruction size
	template<typename T>
	void handleModRM(T*& first, T*& second)
	{
		//if desperate for efficiency, we can try to use the array trick for the other switch cases by using a lambda for each
		//	case and create an array of std::function that can hold them... NOT SURE IT IMPROVES
		//it works but somehow in release mod it breaks... probably some hard optimizations

		if (sizeof(T) == 1) {
			//8 bit instructions
			BYTE _mod = curr_instruction.getMod();

			//the first operand is always the Register BYTE
			BYTE* regs[8] = { &al, &cl, &dl, &bl, &ah, &ch, &dh, &bh };
			BYTE _reg = curr_instruction.getReg();

			if (!(_reg >= 0 && _reg < 8)) {
				throw "Invalid Register BYTE";
			}
			first = (T*)regs[_reg];

			//@TODO(sawii): finish 8 bit Mod cases
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

				BYTE _r_m = curr_instruction.getR_M();

				if (!(_r_m >= 0 && _r_m < 8)) {
					throw "Invalid _r_m BYTE";
				}

				second = (T*)regs[_r_m];

				break;
			}

			default:
				throw "Invalid Mode";
			}
		}
		else if (sizeof(T) == 2) {
			//16bit instruction
			//@TODO(sawii): implement 16 bit instruction MODRM handler
		}
		else if (sizeof(T) == 4) {
			//32bit instruction

			//the first operand is always the reg byte

			BYTE _reg = curr_instruction.getReg();
			DWORD* regs[8] = { &eax, &ecx, &edx, &ebx, &esp, &ebp, &esi, &edi };

			if (!(_reg >= 0 && _reg < 8)) {
				throw "Invalid _reg BYTE";
			}
			first = (T*)regs[_reg];

			BYTE _mod = curr_instruction.getMod();
			switch (_mod)
			{
			case 0x0:
			{
				BYTE _r_m = curr_instruction.getR_M();

				//this gets optimized away somehow
				/*std::function<void()> r_m_arr[8] =
	{
	 [&]() { second = (T*)(m_bus->convertAddress<DWORD>(eax)); },
	 [&]() { second = (T*)(m_bus->convertAddress<DWORD>(ecx)); },
	 [&]() { second = (T*)(m_bus->convertAddress<DWORD>(edx)); },
	 [&]() { second = (T*)(m_bus->convertAddress<DWORD>(ebx)); },
	 [&]() { second = (T*)m_bus->convertAddress<DWORD>(handleSIBInstruction()); },
	 [&]() { second = (T*)(m_bus->convertAddress<DWORD>(readFromPC<DWORD>())); },
	 [&]() { second = (T*)(m_bus->convertAddress<DWORD>(esi)); },
	 [&]() { second = (T*)(m_bus->convertAddress<DWORD>(edi)); }
	};

		(r_m_arr[_r_m])();
		break;*/

				switch (_r_m)
				{
				case 0x0:
				{
					second = (T*)(m_bus->convertAddress<DWORD>(eax));
					break;
				}
				case 0x1:
				{
					second = (T*)(m_bus->convertAddress<DWORD>(ecx));
					break;
				}
				case 0x2:
				{
					second = (T*)(m_bus->convertAddress<DWORD>(edx));
					break;
				}
				case 0x3:
				{
					second = (T*)m_bus->convertAddress<DWORD>(ebx);
					break;
				}
				case 0x4:
				{
					//sib no displacement

					second = (T*)m_bus->convertAddress<DWORD>(handleSIBInstruction());

					break;
				}
				case 0x5:
				{
					//displacement only

					second = (T*)m_bus->convertAddress<DWORD>(readFromPC<DWORD>());

					break;
				}
				case 0x6:
				{
					second = (T*)m_bus->convertAddress<DWORD>(esi);

					break;
				}
				case 0x7:
				{
					second = (T*)m_bus->convertAddress<DWORD>(edi);

					break;
				}

				default:
					throw "Invalid r_m";
				}
				break;
			}
			case 0x1:
			{
				//@TODO(sawii): implement this
				//register address + 8bit displacement
				BYTE _r_m = curr_instruction.getR_M();
				/*std::function<void()> r_m_arr[8] =
	{
	 [&]() {second = (T*)m_bus->convertAddress<DWORD>(eax + readFromPC<BYTE>()); },
	 [&]() { second = (T*)m_bus->convertAddress<DWORD>(ecx + readFromPC<BYTE>()); },
	 [&]() { second = (T*)m_bus->convertAddress<DWORD>(edx + readFromPC<BYTE>()); },
	 [&]() { second = (T*)m_bus->convertAddress<DWORD>(ebx + readFromPC<BYTE>()); },
	 [&]() { second = (T*)m_bus->convertAddress<DWORD>(handleSIBInstruction() + readFromPC<BYTE>()); },
	 [&]() { second = (T*)m_bus->convertAddress<DWORD>(ebp + readFromPC<BYTE>()); },
	 [&]() { second = (T*)m_bus->convertAddress<DWORD>(esi + readFromPC<BYTE>()); },
	 [&]() {second = (T*)m_bus->convertAddress<DWORD>(edi + readFromPC<BYTE>()); }
	};

	(r_m_arr[_r_m])();
	break;*/

				switch (_r_m)
				{
				case 0x0:
				{
					BYTE disp = readFromPC<BYTE>();

					second = (T*)m_bus->convertAddress<DWORD>(eax + disp);

					break;
				}
				case 0x1:
				{
					BYTE disp = readFromPC<BYTE>();

					second = (T*)m_bus->convertAddress<DWORD>(ecx + disp);
					break;
				}
				case 0x2:
				{
					BYTE disp = readFromPC<BYTE>();

					second = (T*)m_bus->convertAddress<DWORD>(edx + disp);
					break;
				}
				case 0x3:
				{
					BYTE disp = readFromPC<BYTE>();

					second = (T*)m_bus->convertAddress<DWORD>(ebx + disp);
					break;
				}
				case 0x4:
				{
					//sib byte + disp8
					DWORD sib_address = handleSIBInstruction();
					BYTE disp = readFromPC<BYTE>();
					second = (T*)m_bus->convertAddress<DWORD>(sib_address + disp);
					break;
				}
				case 0x5:
				{
					BYTE disp = readFromPC<BYTE>();

					second = (T*)m_bus->convertAddress<DWORD>(ebp + disp);
					break;
				}
				case 0x6:
				{
					BYTE disp = readFromPC<BYTE>();

					second = (T*)m_bus->convertAddress<DWORD>(esi + disp);
					break;
				}
				case 0x7:
				{
					BYTE disp = readFromPC<BYTE>();

					second = (T*)m_bus->convertAddress<DWORD>(edi + disp);
					break;
				}

				default:
					throw "Invalid r_m";
				}
				break;
			}
			case 0x2:
			{
				//@TODO(sawii): implement this

				//register address + 32bit displacement

				BYTE _r_m = curr_instruction.getR_M();
				/*std::function<void()> r_m_arr[8] =
	{
	 [&]() {second = (T*)m_bus->convertAddress<DWORD>(eax + readFromPC<DWORD>()); },
	 [&]() { second = (T*)m_bus->convertAddress<DWORD>(ecx + readFromPC<DWORD>()); },
	 [&]() { second = (T*)m_bus->convertAddress<DWORD>(edx + readFromPC<DWORD>()); },
	 [&]() { second = (T*)m_bus->convertAddress<DWORD>(ebx + readFromPC<DWORD>()); },
	 [&]() { second = (T*)m_bus->convertAddress<DWORD>(handleSIBInstruction() + readFromPC<DWORD>()); },
	 [&]() { second = (T*)m_bus->convertAddress<DWORD>(ebp + readFromPC<DWORD>()); },
	 [&]() { second = (T*)m_bus->convertAddress<DWORD>(esi + readFromPC<DWORD>()); },
	 [&]() {second = (T*)m_bus->convertAddress<DWORD>(edi + readFromPC<DWORD>()); }
	};

	(r_m_arr[_r_m])();
	break;*/

				switch (_r_m)
				{
				case 0x0:
				{
					DWORD disp = readFromPC<DWORD>();

					second = (T*)m_bus->convertAddress<DWORD>(eax + disp);

					break;
				}
				case 0x1:
				{
					DWORD disp = readFromPC<DWORD>();

					second = (T*)m_bus->convertAddress<DWORD>(ecx + disp);
					break;
				}
				case 0x2:
				{
					DWORD disp = readFromPC<DWORD>();

					second = (T*)m_bus->convertAddress<DWORD>(edx + disp);
					break;
				}
				case 0x3:
				{
					DWORD disp = readFromPC<DWORD>();

					second = (T*)m_bus->convertAddress<DWORD>(ebx + disp);
					break;
				}
				case 0x4:
				{
					//sib byte + disp32
					DWORD sib_address = handleSIBInstruction();
					DWORD disp = readFromPC<DWORD>();

					second = (T*)m_bus->convertAddress<DWORD>(sib_address + disp);
					break;
				}
				case 0x5:
				{
					DWORD disp = readFromPC<DWORD>();

					second = (T*)m_bus->convertAddress<DWORD>(ebp + disp);
					break;
				}
				case 0x6:
				{
					DWORD disp = readFromPC<DWORD>();

					second = (T*)m_bus->convertAddress<DWORD>(esi + disp);
					break;
				}
				case 0x7:
				{
					DWORD disp = readFromPC<DWORD>();

					second = (T*)m_bus->convertAddress<DWORD>(edi + disp);
					break;
				}

				default:
					throw "Invalid r_m";
				}

				break;
			}
			case 0x3:
			{
				BYTE _r_m = curr_instruction.getR_M();
				if (!(_r_m >= 0 && _r_m < 8)) {
					throw "Invalid _r_m BYTE";
				}
				second = (T*)regs[_r_m];

				break;
			}

			default:
				throw "Invalid Mode";
			}
		}
		else {
			throw "Invalid Instruction Size";
		}
	}

	//instruction functions

	void ADD();
	void ADD_RAX();
	void PUSH_ES();
	void POP_ES();
	void OR();
	void OR_RAX();
	void PUSH_CS();
	void ADC();
	void ADC_RAX();
	void PUSH_SS();
	void POP_SS();
	void SBB();
	void SBB_RAX();
	void PUSH_DS();
	void POP_DS();
	void AND();
	void AND_RAX();
	void DAA();
	void SUB();
	void SUB_RAX();
	void DAS();
	void XOR();
	void XOR_RAX();
	void AAA();
	void CMP();
	void CMP_RAX();
	void AAS();
	void INC_EAX();
	void INC_ECX();
	void INC_EDX();
	void INC_EBX();
	void INC_ESP();
	void INC_EBP();
	void INC_ESI();
	void INC_EDI();
	void DEC_EAX();
	void DEC_ECX();
	void DEC_EDX();
	void DEC_EBX();
	void DEC_ESP();
	void DEC_EBP();
	void DEC_ESI();
	void DEC_EDI();
	void PUSH_EAX();
	void PUSH_ECX();
	void PUSH_EDX();
	void PUSH_EBX();
	void PUSH_ESP();
	void PUSH_EBP();
	void PUSH_ESI();
	void PUSH_EDI();
	void POP_EAX();
	void POP_ECX();
	void POP_EDX();
	void POP_EBX();
	void POP_ESP();
	void POP_EBP();
	void POP_ESI();
	void POP_EDI();
	void PUSHA();
	void POPA();
	void BOUND();
	void PUSH_IMM();
	void IMUL_THREE_OPS_16_32();
	void IMUL_THREE_OPS_8();
};
