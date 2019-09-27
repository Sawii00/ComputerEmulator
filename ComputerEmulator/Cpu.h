#pragma once
#include "Error.h"
#include "Types.h"
#include <string>
#include "Bus.h"
#include <array>
#include <functional>
#include <cstring>
#include <iostream>

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

	BYTE getMod() const {
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

	void print_registers();

	void test() {
		cl = 2;
		al = 3;
		WORD inst = 0xC100;
		writeWORD(0xFF, inst);
		pc = 0xFF;

		print_registers();

		fetch();
		execute();
		print_registers();
	}
	void interrupt();
	void non_maskable_interrupt();

	BYTE read(DWORD address);
	DWORD readDWORD(DWORD address);
	WORD readWORD(DWORD address);

	ReturnCodes write(DWORD address, BYTE v);
	ReturnCodes writeWORD(DWORD address, WORD v);
	ReturnCodes writeDWORD(DWORD address, DWORD v);


    template<class T>void handleModRM(T*& first, T*& second)
    {
    
        if(sizeof(T) == 1){
            //8 bit instructions
             

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

        
        }
        else if(sizeof(T) == 2){
            //16bit instruction
        
        
        }
        else if(sizeof(T) == 4){
            //32bit instruction
        

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
            
        }
        else{
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
