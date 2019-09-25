#pragma once
#include "Error.h"
#include "Types.h"
#include <string>
#include "Bus.h"
#include <array>
#include <functional>

/*
- INTEL XED

- single implementation of the instruction which acts based upon the current instruction form
... in terms of operands

- implement segment registers
- continue instruction table

http://www.c-jump.com/CIS77/CPU/x86/lecture.html

*/

class Bus;
class Cpu;

struct DisassembledInstruction {
	DisassembledInstruction(std::string n = "", std::function<void(Cpu*)>f = nullptr, BYTE c = 0)
		:name(n), instruction_function(f), cycles(c) {};

	std::string name;
	std::function<void(Cpu*)>instruction_function;
	BYTE cycles;
};

struct Instruction {
	WORD inst;

	Instruction(WORD word = 0)
		: inst(word) {}

	BYTE getOpCode() const {
		return inst >> 0xA & 0x3F;
		//return inst >> 0x2 & 0x3F;
	}

	BYTE getR_X() const {
		return inst >> 0x9 & 0x1;
		//return inst >> 0x1 & 0x1;
	}

	BYTE getS() const {
		return inst >> 0x8 & 0x1;
		//return inst & 0x1;
	}

	BYTE getMod()const {
		return inst >> 0x6 & 0x3;
		//return inst >> 0xE & 0x3;
	}
	BYTE getReg() const {
		return inst >> 0x3 & 0x7;
		//return inst >> 0xB & 0x7;
	}
	BYTE getR_M() const {
		return inst & 0x7;
		//return inst >> 0x8 & 0x7;
	}

	BYTE getOpCodeByte() const {
		return inst >> 0x8 & 0xFF;
	}
	BYTE getModRMByte() const {
		return inst & 0xFF;
	}
};

class Cpu
{
private:
	Bus* m_bus = nullptr;

	//REGISTERS

	union {
		DWORD eax;

		struct
		{
			WORD _unused;
			WORD ax;
		};

		struct
		{
			WORD _unused;
			BYTE ah;
			BYTE al;
		};
	} EAX;
	DWORD eax = EAX.eax;
	WORD ax = EAX.ax;
	BYTE ah = EAX.ah;
	BYTE al = EAX.al;

	union {
		DWORD ebx;

		struct
		{
			WORD _unused;
			WORD bx;
		};

		struct
		{
			WORD _unused;
			BYTE bh;
			BYTE bl;
		};
	} EBX;
	DWORD ebx = EBX.ebx;
	WORD bx = EBX.bx;
	BYTE bh = EBX.bh;
	BYTE bl = EBX.bl;
	union {
		DWORD ecx;

		struct
		{
			WORD _unused;
			WORD cx;
		};

		struct
		{
			WORD _unused;
			BYTE ch;
			BYTE cl;
		};
	}ECX;
	DWORD ecx = ECX.ecx;
	WORD cx = ECX.cx;
	BYTE ch = ECX.ch;
	BYTE cl = ECX.cl;
	union {
		DWORD edx;

		struct
		{
			WORD _unused;
			WORD dx;
		};

		struct
		{
			WORD _unused;
			BYTE dh;
			BYTE dl;
		};
	}EDX;
	DWORD edx = EDX.edx;
	WORD dx = EDX.dx;
	BYTE dh = EDX.dh;
	BYTE dl = EDX.dl;
	union {
		DWORD esi;

		struct
		{
			WORD _unused;
			WORD si;
		};
	}ESI;
	DWORD esi = ESI.esi;
	WORD si = ESI.si;
	union {
		DWORD edi;

		struct
		{
			WORD _unused;
			WORD di;
		};
	}EDI;
	DWORD edi = EDI.edi;
	WORD di = EDI.di;
	union {
		DWORD esp;

		struct
		{
			WORD _unused;
			WORD sp;
		};
	}ESP;
	DWORD esp = ESP.esp;
	WORD sp = ESP.sp;
	union {
		DWORD ebp;

		struct
		{
			WORD _unused;
			WORD bp;
		};
	}EBP;
	DWORD ebp = EBP.ebp;
	WORD bp = EBP.bp;

	DWORD pc = 0x00000000; //program counter.

	BYTE flags = 0x00;

	WORD cs = 0x0000; //code
	WORD ds = 0x0000; //data
	WORD ss = 0x0000; //stack
	WORD es = 0x0000; //extra
	WORD fs = 0x0000;
	WORD gs = 0x0000;

	///////////////////END REGISTERS

	//////////////////////INTRUCTIONS VARIABLES
	Instruction curr_instruction;

	bool sib_enabled = false;

	//@TODO
	//instruction list to be populated
	std::array<DisassembledInstruction, 256> m_instruction_list;

public:
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

	void attachBus(Bus* bus);

	bool getFlag(FLAGS f);
	void setFlag(FLAGS f, bool v);
	void clearFlags();

	void clock();

	void fetch();
	void decode();
	void execute();

	void test() {
		cl = 2;
		al = 3;
		WORD inst = 0xC101;
		writeWORD(0xFF, inst);
		pc = 0xFF;

		fetch();
		execute();
	}

	void interrupt();
	void non_maskable_interrupt();

	BYTE read(DWORD address);
	DWORD readDWORD(DWORD address);
	WORD readWORD(DWORD address);

	ReturnCodes write(DWORD address, BYTE v);
	ReturnCodes writeWORD(DWORD address, WORD v);
	ReturnCodes writeDWORD(DWORD address, DWORD v);

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
