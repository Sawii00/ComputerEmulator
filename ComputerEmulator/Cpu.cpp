#include "Cpu.h"

Cpu::Cpu()
{
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
	return (status & f) > 0 ? true : false;
}

void Cpu::setFlag(FLAGS f, bool v)
{
	if (v) {
		status |= f;
	}
	else {
		status &= ~f;
	}
}

void Cpu::clearFlags()
{
	status = 0x00;
}

void Cpu::clock()
{
}
BYTE Cpu::read(DWORD address)
{
	return m_bus->read(address);
}

DWORD Cpu::readDWORD(DWORD address)
{
	return (read(address++) << 24 | read(address++) << 16 | read(address++) << 8 | read(address++));
}

DWORD Cpu::getRegisterValue(BYTE id)
{
	switch (id) {
	case 0:
		return eax;

	case 1:
		return ebx;

	case 2:
		return ecx;

	case 3:
		return edx;

	default:
		throw "Invalid Register";
	}
}

void Cpu::setRegisterValue(BYTE id, DWORD val)
{
	switch (id) {
	case 0:
		eax = val;

	case 1:
		ebx = val;

	case 2:
		ecx = val;

	case 3:
		edx = val;

	default:
		throw "Invalid Register";
	}
}

DWORD * Cpu::getRegisterPointer(BYTE id)
{
	switch (id) {
	case 0:
		return &eax;

	case 1:
		return &ebx;

	case 2:
		return &ecx;

	case 3:
		return &edx;

	default:
		throw "Invalid Register";
	}
}

ReturnCodes Cpu::write(DWORD address, BYTE v)
{
	return m_bus->write(address, v);
}
//@TODO: check if this works
ReturnCodes Cpu::writeDWORD(DWORD address, DWORD v)
{
	ReturnCodes stat = SUCCESS;
	for (register int i = 0; i < 3 && stat != BAD_MEMORY_REQUEST; i++) {
		stat = write(address, v >> (8 * i) & 0xff);
	}
	return stat;
}

void Cpu::handle_addressing_mode()
{
	ADDRESSING_MODE mode = m_instructions[opcode].addressing_mode;
	switch (mode)
	{
		//immediate addressing mode expects the next byte as the value
	case ADDRESSING_MODE::IMM:
	{
		//it should autoincrement it
		fetched = readDWORD(pc);
		break;
	}
	case ADDRESSING_MODE::IMP:
	{//often if implied we use the accumulator register
		fetched = eax;
		break;
		//full 32 bit address is read from ram 1 byte at the time
	}
	case ADDRESSING_MODE::ABS:
	{/*BYTE first = read(pc);
		pc++;
		BYTE second = read(pc);
		pc++;
		BYTE third = read(pc);
		pc++;
		BYTE forth = read(pc);
		pc++;

		abs_address = first << 24 | second << 16 | third << 8 | forth;
		*/
		//it should increment it
		abs_address = readDWORD(pc);
		fetched = readDWORD(abs_address);
		break;
	}
	case ADDRESSING_MODE::ABS_EBI_OFFSET:
	{
		abs_address = readDWORD(pc);

		abs_address += ebi;
		fetched = readDWORD(abs_address);

		break;
		//@TO_CHECK
	}
	case ADDRESSING_MODE::PC_REL:
	{//usually relative offsets are within -+127 addresses
		rel_address = read(pc++);
		fetched = readDWORD(pc + rel_address);

		break;

		//indirect mode is like a pointer: the address supplied the next DWORD is instead a pointer
			//to the address where we can find the actual address
	}
	case ADDRESSING_MODE::REG_IND:
	{
		BYTE reg_id = read(pc++);
		abs_address = readDWORD(getRegisterValue(reg_id));
		fetched = readDWORD(abs_address);
		break;
	}

	case ADDRESSING_MODE::REG:
	{//register, register mode.
		BYTE reg_id = read(pc++);
		fetched = getRegisterValue(reg_id);
		curr_reg = getRegisterPointer(reg_id);
		break;
	}

	default:
		throw"Invalid Addressing mode";
	}
}

//the address to be read from is set by the handle_addressing_mode into abs_address o rel_address
void Cpu::fetch()
{
	opcode = read(pc++);

	handle_addressing_mode();
}

void Cpu::execute()
{
	m_instructions[opcode].instruction_function();
}

void Cpu::AND() {
	//bitwise AND between accumulator and fetched value.
	//REG, MEM
	//MEM, REG
	//REG, REG
	//MEM, IMMEDIATE
	//REG, IMMEDIATE

	eax &= fetched;
	setFlag(Z, eax == 0x00);
	setFlag(N, eax & 0x80000000);
}

void Cpu::OR() {
	//bitwise OR between accumulator and fetched value
	//REG, MEM
	//MEM, REG
	//REG, REG
	//MEM, IMMEDIATE
	//REG, IMMEDIATE
	eax |= fetched;
	setFlag(Z, eax == 0x00);
	setFlag(N, eax & 0x80000000);
}

void Cpu::XOR() {
	//bitwise XOR between accumulator and fetched val
	//REG, MEM
	//MEM, REG
	//REG, REG
	//MEM, IMMEDIATE
	//REG, IMMEDIATE
	eax ^= fetched;
	setFlag(Z, eax == 0x00);
	setFlag(N, eax & 0x80000000);
}

void Cpu::NOT() {
	//REG
	//MEM
	fetched = ~fetched;
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
	//REG
	(*curr_reg) *= -1;
	setFlag(N, (*curr_reg) & 0x80000000);
}

void Cpu::INC() {
	//REG
	setFlag(O, (*curr_reg) == 0x7fffffff);
	(*curr_reg) += 1;
	setFlag(N, (*curr_reg) & 0x80000000);
	setFlag(Z, *(curr_reg) == 0x0);
}
void Cpu::DEC() {
	//REG
	setFlag(O, (*curr_reg) == 0xffffffff);
	(*curr_reg) -= 1;
	setFlag(N, (*curr_reg) & 0x80000000);
	setFlag(Z, *(curr_reg) == 0x00);
}

void Cpu::MOVA() {
	eax = fetched;
}
void Cpu::MOVB() {
	ebx = fetched;
}
void Cpu::MOVC() {
	ecx = fetched;
}
void Cpu::MOVD() {
	edx = fetched;
}

void Cpu::PUSH() {
	writeDWORD(sp++, fetched);
}

void Cpu::POP() {
	setRegisterValue(fetched, readDWORD(sp--));
}

void Cpu::CMP() {
	DWORD op2 = readDWORD(pc++);

	DWORD res = fetched - op2;
	setFlag(Z, res == 0x00);
	setFlag(N, res & 0x80000000);
}

void Cpu::JMP() {
	pc = fetched;
}
void Cpu::JE() {
	if (getFlag(Z))
		pc = fetched;
}
void Cpu::JNE() {
	if (!getFlag(Z))
		pc = fetched;
}
void Cpu::JA() {
	if (!getFlag(N) && !getFlag(Z))
		pc = fetched;
}
void Cpu::JAE() {
	if (!getFlag(N) || getFlag(Z))
		pc = fetched;
}
void Cpu::JB() {
	if (getFlag(N) && !getFlag(Z))
		pc = fetched;
}
void Cpu::JBE() {
	if (getFlag(N) || getFlag(Z))
		pc = fetched;
}

//TO FINISH
void Cpu::CALL() {
	writeDWORD(sp++, ++pc);
}

void Cpu::RET() {
}