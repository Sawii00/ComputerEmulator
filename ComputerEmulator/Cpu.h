#pragma once
#include <variant>
#include "Error.h"
#include "Types.h"
#include <string>
#include <functional>
#include "Bus.h"

class Bus;

enum ADDRESSING_MODE {
	IMM, IMP, ABS, ABS_EBI_OFFSET, PC_REL, REG_IND, REG
};

struct Instruction {
	std::string mnemonic;
	ADDRESSING_MODE addressing_mode;
	std::function<void()> instruction_function;
	BYTE cycles;
};

class Cpu
{
private:
	Bus* m_bus = nullptr;

	DWORD eax = 0x00000000; //accumulator 0
	DWORD ebx = 0x00000000; //base 1
	DWORD ecx = 0x00000000; //counter  2
	DWORD edx = 0x00000000; //data   3

	DWORD ebi = 0x00000000; //addresses

	DWORD pc = 0x00000000; //program counter
	DWORD sp = 0x00000000; //stack pointer
	DWORD bp = 0x00000000; //base pointer

	BYTE status = 0x00;

	DWORD fetched = 0x00000000;
	DWORD* curr_reg = nullptr;

	DWORD abs_address = 0x00000000;
	DWORD rel_address = 0x00000000;
	BYTE opcode = 0x00;
	BYTE cycles = 0x00;

	DWORD temp = 0x00000000;
	//@TODO
	//instruction list to be populated
	std::array<Instruction, 256> m_instructions;

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
	void execute();

	void interrupt();
	void non_maskable_interrupt();

	BYTE read(DWORD address);
	DWORD readDWORD(DWORD address);
	DWORD getRegisterValue(BYTE id);
	void setRegisterValue(BYTE id, DWORD val);
	DWORD* getRegisterPointer(BYTE id);
	ReturnCodes write(DWORD address, BYTE v);
	ReturnCodes writeDWORD(DWORD address, DWORD v);

	void handle_addressing_mode();

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
	//decide which conditional jumps to implement
	void MOVA();
	void MOVB();
	void MOVC();
	void MOVD();
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
