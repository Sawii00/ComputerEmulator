#pragma once
#include <variant>
#include "Error.h"
#include "Types.h"
#include <string>
#include <functional>
#include "Bus.h"
#include <array>

/*
- INTEL XED

- single implementation of the instruction which acts based upon the current instruction form
... in terms of operands

- implement segment registers
- continue instruction table

http://www.c-jump.com/CIS77/CPU/x86/lecture.html

*/

class Bus;

struct DisassembledInstruction {
	std::string name;
	std::function<void()> instruction_function;
	BYTE cycles;
};

struct Instruction {
	WORD inst;

	Instruction(WORD word = 0)
		: inst(word) {}

	BYTE getOpCode() const {
		return inst >> 0x2 & 0x3F;
	}

	BYTE getR_X() const {
		return inst >> 0x1 & 0x1;
	}

	BYTE getS() const {
		return inst & 0x1;
	}

	BYTE getMod()const {
		return inst >> 0xE & 0x3;
	}
	BYTE getReg() const {
		return inst >> 0xB & 0x7;
	}
	BYTE getR_M() const {
		return inst >> 0x8 & 0x7;
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
	};
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
	};
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
	};
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
	};
	union {
		DWORD esi;

		struct
		{
			WORD _unused;
			WORD si;
		};
	};
	union {
		DWORD edi;

		struct
		{
			WORD _unused;
			WORD di;
		};
	};
	union {
		DWORD esp;

		struct
		{
			WORD _unused;
			WORD sp;
		};
	};
	union {
		DWORD ebp;

		struct
		{
			WORD _unused;
			WORD bp;
		};
	};

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

	std::variant<BYTE*, WORD*, DWORD*> operand_register;

	std::variant<BYTE*, WORD*, DWORD*> second_operand;

	bool sib_enabled = false;

	//@TODO
	//instruction list to be populated
	std::array<DisassembledInstruction, 256> m_instruction_list;

	void computeSecondOperandMod0();
	void computeSecondOperandMod1();
	void computeSecondOperandMod2();
	void computeSecondOperandMod3();

	void computeFirstOperand();

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

	/*void test() {
		WORD inst = 0xC100;
		writeWORD(0xFF, inst);
		pc = 0xFF;

		fetch();
	}*/

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
	//add with carry
	void ADC();
	void AND();
	void CALL();
	//clear carry bit
	void CLC();
	//clear interrupt enable bit
	void CLI();
	//compare
	void CMP();

	void DEC();
	void DIV();
	void IDIV();
	void HLT();
	void MUL();
	void IMUL();
	void INC();

	void JMP();
	void JE();
	void JNE();
	void JA();
	void JB();
	void JAE();
	void JBE();

	void MOV();

	void NEG();
	void NOT();
	void OR();
	void POP();
	void PUSH();
	//rotations
	void RET();
	//set carry flag
	void STC();
	//set interrupt enable flag
	void STI();
	void SUB();
	void XOR();
};
