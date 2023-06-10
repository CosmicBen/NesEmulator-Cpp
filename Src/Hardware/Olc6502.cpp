/*
    olc6502 - An emulation of the 6502/2A03 processor
*/

#include "Hardware\Olc6502.h"
#include "Hardware\Bus.h"

// Constructor
Olc6502::Olc6502() {
    // Assembles the translation table. It's big, it's ugly, but it yields a convenient way
    // to emulate the 6502. I'm certain there are some "code-golf" strategies to reduce this
    // but I've deliberately kept it verbose for study and alteration

    // It is 16x16 entries. This gives 256 instructions. It is arranged to that the bottom
    // 4 bits of the instruction choose the column, and the top 4 bits choose the row.

    // For convenience to get function pointers to members of this class, I'm using this
    // or else it will be much much larger :D

    // The table is one big initialiser list of initialiser lists...
    using a = Olc6502;
    lookup = {
        /*         0                               1                               2                               3                               4                               5                               6                               7                               8                               9                               A                               B                               C                               D                               E                               F                         */
        /* 0 */ { "BRK", &a::BRK, &a::IMP, 7 }, { "ORA", &a::ORA, &a::IZX, 6 }, { "???", &a::XXX, &a::IMP, 2 }, { "???", &a::XXX, &a::IMP, 8 }, { "???", &a::NOP, &a::IMP, 3 }, { "ORA", &a::ORA, &a::ZP0, 3 }, { "ASL", &a::ASL, &a::ZP0, 5 }, { "???", &a::XXX, &a::IMP, 5 }, { "PHP", &a::PHP, &a::IMP, 3 }, { "ORA", &a::ORA, &a::IMM, 2 }, { "ASL", &a::ASL, &a::IMP, 2 }, { "???", &a::XXX, &a::IMP, 2 }, { "???", &a::NOP, &a::IMP, 4 }, { "ORA", &a::ORA, &a::ABS, 4 },  { "ASL", &a::ASL, &a::ABS, 6 }, { "???", &a::XXX, &a::IMP, 6 },
        /* 1 */ { "BPL", &a::BPL, &a::REL, 2 }, { "ORA", &a::ORA, &a::IZY, 5 }, { "???", &a::XXX, &a::IMP, 2 }, { "???", &a::XXX, &a::IMP, 8 }, { "???", &a::NOP, &a::IMP, 4 }, { "ORA", &a::ORA, &a::ZPX, 4 }, { "ASL", &a::ASL, &a::ZPX, 6 }, { "???", &a::XXX, &a::IMP, 6 }, { "CLC", &a::CLC, &a::IMP, 2 }, { "ORA", &a::ORA, &a::ABY, 4 }, { "???", &a::NOP, &a::IMP, 2 }, { "???", &a::XXX, &a::IMP, 7 }, { "???", &a::NOP, &a::IMP, 4 }, { "ORA", &a::ORA, &a::ABX, 4 }, { "ASL", &a::ASL, &a::ABX, 7 }, { "???", &a::XXX, &a::IMP, 7 },
        /* 2 */ { "JSR", &a::JSR, &a::ABS, 6 }, { "AND", &a::AND, &a::IZX, 6 }, { "???", &a::XXX, &a::IMP, 2 }, { "???", &a::XXX, &a::IMP, 8 }, { "BIT", &a::BIT, &a::ZP0, 3 }, { "AND", &a::AND, &a::ZP0, 3 }, { "ROL", &a::ROL, &a::ZP0, 5 }, { "???", &a::XXX, &a::IMP, 5 }, { "PLP", &a::PLP, &a::IMP, 4 }, { "AND", &a::AND, &a::IMM, 2 }, { "ROL", &a::ROL, &a::IMP, 2 }, { "???", &a::XXX, &a::IMP, 2 }, { "BIT", &a::BIT, &a::ABS, 4 }, { "AND", &a::AND, &a::ABS, 4 }, { "ROL", &a::ROL, &a::ABS, 6 }, { "???", &a::XXX, &a::IMP, 6 },
        /* 3 */ { "BMI", &a::BMI, &a::REL, 2 }, { "AND", &a::AND, &a::IZY, 5 }, { "???", &a::XXX, &a::IMP, 2 }, { "???", &a::XXX, &a::IMP, 8 }, { "???", &a::NOP, &a::IMP, 4 }, { "AND", &a::AND, &a::ZPX, 4 }, { "ROL", &a::ROL, &a::ZPX, 6 }, { "???", &a::XXX, &a::IMP, 6 }, { "SEC", &a::SEC, &a::IMP, 2 }, { "AND", &a::AND, &a::ABY, 4 }, { "???", &a::NOP, &a::IMP, 2 }, { "???", &a::XXX, &a::IMP, 7 }, { "???", &a::NOP, &a::IMP, 4 }, { "AND", &a::AND, &a::ABX, 4 }, { "ROL", &a::ROL, &a::ABX, 7 }, { "???", &a::XXX, &a::IMP, 7 },
        /* 4 */ { "RTI", &a::RTI, &a::IMP, 6 }, { "EOR", &a::EOR, &a::IZX, 6 }, { "???", &a::XXX, &a::IMP, 2 }, { "???", &a::XXX, &a::IMP, 8 }, { "???", &a::NOP, &a::IMP, 3 }, { "EOR", &a::EOR, &a::ZP0, 3 }, { "LSR", &a::LSR, &a::ZP0, 5 }, { "???", &a::XXX, &a::IMP, 5 }, { "PHA", &a::PHA, &a::IMP, 3 }, { "EOR", &a::EOR, &a::IMM, 2 }, { "LSR", &a::LSR, &a::IMP, 2 }, { "???", &a::XXX, &a::IMP, 2 }, { "JMP", &a::JMP, &a::ABS, 3 }, { "EOR", &a::EOR, &a::ABS, 4 }, { "LSR", &a::LSR, &a::ABS, 6 }, { "???", &a::XXX, &a::IMP, 6 },
        /* 5 */ { "BVC", &a::BVC, &a::REL, 2 }, { "EOR", &a::EOR, &a::IZY, 5 }, { "???", &a::XXX, &a::IMP, 2 }, { "???", &a::XXX, &a::IMP, 8 }, { "???", &a::NOP, &a::IMP, 4 }, { "EOR", &a::EOR, &a::ZPX, 4 }, { "LSR", &a::LSR, &a::ZPX, 6 }, { "???", &a::XXX, &a::IMP, 6 }, { "CLI", &a::CLI, &a::IMP, 2 }, { "EOR", &a::EOR, &a::ABY, 4 }, { "???", &a::NOP, &a::IMP, 2 }, { "???", &a::XXX, &a::IMP, 7 }, { "???", &a::NOP, &a::IMP, 4 }, { "EOR", &a::EOR, &a::ABX, 4 }, { "LSR", &a::LSR, &a::ABX, 7 }, { "???", &a::XXX, &a::IMP, 7 },
        /* 6 */ { "RTS", &a::RTS, &a::IMP, 6 }, { "ADC", &a::ADC, &a::IZX, 6 }, { "???", &a::XXX, &a::IMP, 2 }, { "???", &a::XXX, &a::IMP, 8 }, { "???", &a::NOP, &a::IMP, 3 }, { "ADC", &a::ADC, &a::ZP0, 3 }, { "ROR", &a::ROR, &a::ZP0, 5 }, { "???", &a::XXX, &a::IMP, 5 }, { "PLA", &a::PLA, &a::IMP, 4 }, { "ADC", &a::ADC, &a::IMM, 2 }, { "ROR", &a::ROR, &a::IMP, 2 }, { "???", &a::XXX, &a::IMP, 2 }, { "JMP", &a::JMP, &a::IND, 5 }, { "ADC", &a::ADC, &a::ABS, 4 }, { "ROR", &a::ROR, &a::ABS, 6 }, { "???", &a::XXX, &a::IMP, 6 },
        /* 7 */ { "BVS", &a::BVS, &a::REL, 2 }, { "ADC", &a::ADC, &a::IZY, 5 }, { "???", &a::XXX, &a::IMP, 2 }, { "???", &a::XXX, &a::IMP, 8 }, { "???", &a::NOP, &a::IMP, 4 }, { "ADC", &a::ADC, &a::ZPX, 4 }, { "ROR", &a::ROR, &a::ZPX, 6 }, { "???", &a::XXX, &a::IMP, 6 }, { "SEI", &a::SEI, &a::IMP, 2 }, { "ADC", &a::ADC, &a::ABY, 4 }, { "???", &a::NOP, &a::IMP, 2 }, { "???", &a::XXX, &a::IMP, 7 }, { "???", &a::NOP, &a::IMP, 4 }, { "ADC", &a::ADC, &a::ABX, 4 }, { "ROR", &a::ROR, &a::ABX, 7 }, { "???", &a::XXX, &a::IMP, 7 },
        /* 8 */ { "???", &a::NOP, &a::IMP, 2 }, { "STA", &a::STA, &a::IZX, 6 }, { "???", &a::NOP, &a::IMP, 2 }, { "???", &a::XXX, &a::IMP, 6 }, { "STY", &a::STY, &a::ZP0, 3 }, { "STA", &a::STA, &a::ZP0, 3 }, { "STX", &a::STX, &a::ZP0, 3 }, { "???", &a::XXX, &a::IMP, 3 }, { "DEY", &a::DEY, &a::IMP, 2 }, { "???", &a::NOP, &a::IMP, 2 }, { "TXA", &a::TXA, &a::IMP, 2 }, { "???", &a::XXX, &a::IMP, 2 }, { "STY", &a::STY, &a::ABS, 4 }, { "STA", &a::STA, &a::ABS, 4 }, { "STX", &a::STX, &a::ABS, 4 }, { "???", &a::XXX, &a::IMP, 4 },
        /* 9 */ { "BCC", &a::BCC, &a::REL, 2 }, { "STA", &a::STA, &a::IZY, 6 }, { "???", &a::XXX, &a::IMP, 2 }, { "???", &a::XXX, &a::IMP, 6 }, { "STY", &a::STY, &a::ZPX, 4 }, { "STA", &a::STA, &a::ZPX, 4 }, { "STX", &a::STX, &a::ZPY, 4 }, { "???", &a::XXX, &a::IMP, 4 }, { "TYA", &a::TYA, &a::IMP, 2 }, { "STA", &a::STA, &a::ABY, 5 }, { "TXS", &a::TXS, &a::IMP, 2 }, { "???", &a::XXX, &a::IMP, 5 }, { "???", &a::NOP, &a::IMP, 5 }, { "STA", &a::STA, &a::ABX, 5 }, { "???", &a::XXX, &a::IMP, 5 }, { "???", &a::XXX, &a::IMP, 5 },
        /* A */ { "LDY", &a::LDY, &a::IMM, 2 }, { "LDA", &a::LDA, &a::IZX, 6 }, { "LDX", &a::LDX, &a::IMM, 2 }, { "???", &a::XXX, &a::IMP, 6 }, { "LDY", &a::LDY, &a::ZP0, 3 }, { "LDA", &a::LDA, &a::ZP0, 3 }, { "LDX", &a::LDX, &a::ZP0, 3 }, { "???", &a::XXX, &a::IMP, 3 }, { "TAY", &a::TAY, &a::IMP, 2 }, { "LDA", &a::LDA, &a::IMM, 2 }, { "TAX", &a::TAX, &a::IMP, 2 }, { "???", &a::XXX, &a::IMP, 2 }, { "LDY", &a::LDY, &a::ABS, 4 }, { "LDA", &a::LDA, &a::ABS, 4 }, { "LDX", &a::LDX, &a::ABS, 4 }, { "???", &a::XXX, &a::IMP, 4 },
        /* B */ { "BCS", &a::BCS, &a::REL, 2 }, { "LDA", &a::LDA, &a::IZY, 5 }, { "???", &a::XXX, &a::IMP, 2 }, { "???", &a::XXX, &a::IMP, 5 }, { "LDY", &a::LDY, &a::ZPX, 4 }, { "LDA", &a::LDA, &a::ZPX, 4 }, { "LDX", &a::LDX, &a::ZPY, 4 }, { "???", &a::XXX, &a::IMP, 4 }, { "CLV", &a::CLV, &a::IMP, 2 }, { "LDA", &a::LDA, &a::ABY, 4 }, { "TSX", &a::TSX, &a::IMP, 2 }, { "???", &a::XXX, &a::IMP, 4 }, { "LDY", &a::LDY, &a::ABX, 4 }, { "LDA", &a::LDA, &a::ABX, 4 }, { "LDX", &a::LDX, &a::ABY, 4 }, { "???", &a::XXX, &a::IMP, 4 },
        /* C */ { "CPY", &a::CPY, &a::IMM, 2 }, { "CMP", &a::CMP, &a::IZX, 6 }, { "???", &a::NOP, &a::IMP, 2 }, { "???", &a::XXX, &a::IMP, 8 }, { "CPY", &a::CPY, &a::ZP0, 3 }, { "CMP", &a::CMP, &a::ZP0, 3 }, { "DEC", &a::DEC, &a::ZP0, 5 }, { "???", &a::XXX, &a::IMP, 5 }, { "INY", &a::INY, &a::IMP, 2 }, { "CMP", &a::CMP, &a::IMM, 2 }, { "DEX", &a::DEX, &a::IMP, 2 }, { "???", &a::XXX, &a::IMP, 2 }, { "CPY", &a::CPY, &a::ABS, 4 }, { "CMP", &a::CMP, &a::ABS, 4 }, { "DEC", &a::DEC, &a::ABS, 6 }, { "???", &a::XXX, &a::IMP, 6 },
        /* D */ { "BNE", &a::BNE, &a::REL, 2 }, { "CMP", &a::CMP, &a::IZY, 5 }, { "???", &a::XXX, &a::IMP, 2 }, { "???", &a::XXX, &a::IMP, 8 }, { "???", &a::NOP, &a::IMP, 4 }, { "CMP", &a::CMP, &a::ZPX, 4 }, { "DEC", &a::DEC, &a::ZPX, 6 }, { "???", &a::XXX, &a::IMP, 6 }, { "CLD", &a::CLD, &a::IMP, 2 }, { "CMP", &a::CMP, &a::ABY, 4 }, { "???", &a::NOP, &a::IMP, 2 }, { "???", &a::XXX, &a::IMP, 7 }, { "???", &a::NOP, &a::IMP, 4 }, { "CMP", &a::CMP, &a::ABX, 4 }, { "DEC", &a::DEC, &a::ABX, 7 }, { "???", &a::XXX, &a::IMP, 7 },
        /* E */ { "CPX", &a::CPX, &a::IMM, 2 }, { "SBC", &a::SBC, &a::IZX, 6 }, { "???", &a::NOP, &a::IMP, 2 }, { "???", &a::XXX, &a::IMP, 8 }, { "CPX", &a::CPX, &a::ZP0, 3 }, { "SBC", &a::SBC, &a::ZP0, 3 }, { "INC", &a::INC, &a::ZP0, 5 }, { "???", &a::XXX, &a::IMP, 5 }, { "INX", &a::INX, &a::IMP, 2 }, { "SBC", &a::SBC, &a::IMM, 2 }, { "NOP", &a::NOP, &a::IMP, 2 }, { "???", &a::SBC, &a::IMP, 2 }, { "CPX", &a::CPX, &a::ABS, 4 }, { "SBC", &a::SBC, &a::ABS, 4 }, { "INC", &a::INC, &a::ABS, 6 }, { "???", &a::XXX, &a::IMP, 6 },
        /* F */ { "BEQ", &a::BEQ, &a::REL, 2 }, { "SBC", &a::SBC, &a::IZY, 5 }, { "???", &a::XXX, &a::IMP, 2 }, { "???", &a::XXX, &a::IMP, 8 }, { "???", &a::NOP, &a::IMP, 4 }, { "SBC", &a::SBC, &a::ZPX, 4 }, { "INC", &a::INC, &a::ZPX, 6 }, { "???", &a::XXX, &a::IMP, 6 }, { "SED", &a::SED, &a::IMP, 2 }, { "SBC", &a::SBC, &a::ABY, 4 }, { "???", &a::NOP, &a::IMP, 2 }, { "???", &a::XXX, &a::IMP, 7 }, { "???", &a::NOP, &a::IMP, 4 }, { "SBC", &a::SBC, &a::ABX, 4 }, { "INC", &a::INC, &a::ABX, 7 }, { "???", &a::XXX, &a::IMP, 7 },
    };
}

// Destructor - has nothing to do
Olc6502::~Olc6502() { }

///////////////////////////////////////////////////////////////////////////////
// BUS CONNECTIVITY
///////////////////////////////////////////////////////////////////////////////

// Reads an 8-bit byte from the bus, located at the specified 16-bit address
uint8_t Olc6502::Read(uint16_t address) {
    // In normal operation "read only" is set to false. This may seem odd. Some
    // devices on the bus may change state when they are read from, and this 
    // is intentional under normal circumstances. However the disassembler will
    // want to read the data at an address without changing the state of the
    // devices on the bus
    return bus->CpuRead(address, false);
}

// Writes a byte to the bus at the specified address
void Olc6502::Write(uint16_t address, uint8_t data) {
    bus->CpuWrite(address, data);
}

///////////////////////////////////////////////////////////////////////////////
// EXTERNAL INPUTS
///////////////////////////////////////////////////////////////////////////////

// Forces the 6502 into a known state. This is hard-wired inside the CPU. The
// registers are set to 0x00, the status register is cleared except for unused
// bit which remains at 1. An absolute address is read from location 0xFFFC
// which contains a second address that the program counter is set to. This 
// allows the programmer to jump to a known and programmable location in the
// memory to start executing from. Typically the programmer would set the value
// at location 0xFFFC at compile time.
void Olc6502::Reset() {
    // Get address to set program counter to
    addressAbsolute = 0xFFFC;

    // Set it
    pc = (uint16_t)Read(addressAbsolute + 0);
    pc |= (uint16_t)Read(addressAbsolute + 1) << 8;

    // Reset internal registers
    a = 0;
    x = 0;
    y = 0;
    stkp = 0xFD;
    status = 0x00 | (uint8_t)Flags6502::U;

    // Clear internal helper variables
    addressRelative = 0x0000;
    addressAbsolute = 0x0000;
    fetched = 0x00;

    // Reset takes time
    cycles = 8;
}

// Interrupt requests are a complex operation and only happen if the
// "disable interrupt" flag is 0. IRQs can happen at any time, but
// you dont want them to be destructive to the operation of the running 
// program. Therefore the current instruction is allowed to finish
// (which I facilitate by doing the whole thing when cycles == 0) and 
// then the current program counter is stored on the stack. Then the
// current status register is stored on the stack. When the routine
// that services the interrupt has finished, the status register
// and program counter can be restored to how they where before it 
// occurred. This is impemented by the "RTI" instruction. Once the IRQ
// has happened, in a similar way to a reset, a programmable address
// is read form hard coded location 0xFFFE, which is subsequently
// set to the program counter.
void Olc6502::InterruptRequest() {
    // If interrupts are allowed
    if (!GetFlag(Flags6502::I)) {
        // Push the program counter to the stack. It's 16-bits dont
        // forget so that takes two pushes
        Write(0x0100 + stkp--, (pc >> 8) & 0x00FF);
        Write(0x0100 + stkp--, pc & 0x00FF);

        // Then Push the status register to the stack
        SetFlag(Flags6502::B, false);
        SetFlag(Flags6502::U, true);
        SetFlag(Flags6502::I, true);
        Write(0x0100 + stkp--, status);

        // Read new program counter location from fixed address
        addressAbsolute = 0xFFFE;
        pc = (uint16_t)Read(addressAbsolute + 0);
        pc |= (uint16_t)Read(addressAbsolute + 1) << 8;

        // IRQs take time
        cycles = 7;
    }
}

// A Non-Maskable Interrupt cannot be ignored. It behaves in exactly the
// same way as a regular IRQ, but reads the new program counter address
// form location 0xFFFA.
void Olc6502::NonmaskableInterrupt() {
    Write(0x0100 + stkp--, (pc >> 8) & 0x00FF);
    Write(0x0100 + stkp--, pc & 0x00FF);

    SetFlag(Flags6502::B, false);
    SetFlag(Flags6502::U, true);
    SetFlag(Flags6502::I, true);
    Write(0x0100 + stkp--, status);

    addressAbsolute = 0xFFFA;
    pc = (uint16_t)Read(addressAbsolute + 0);
    pc |= (uint16_t)Read(addressAbsolute + 1) << 8;

    cycles = 8;
}

// Perform one clock cycles worth of emulation
void Olc6502::Clock() {
    // Each instruction requires a variable number of clock cycles to execute.
    // In my emulation, I only care about the final result and so I perform
    // the entire computation in one hit. In hardware, each clock cycle would
    // perform "microcode" style transformations of the CPUs state.
    //
    // To remain compliant with connected devices, it's important that the 
    // emulation also takes "time" in order to execute instructions, so I
    // implement that delay by simply counting down the cycles required by 
    // the instruction. When it reaches 0, the instruction is complete, and
    // the next one is ready to be executed.
    if (cycles == 0) {
#ifdef LOGMODE
        uint16_t log_pc = pc;
#endif

        // Read next instruction byte. This 8-bit value is used to index
        // the translation table to get the relevant information about
        // how to implement the instruction
        opcode = Read(pc++);    // Increment program counter, we read the opcode byte

        // Always set the unused status flag bit to 1
        SetFlag(Flags6502::U, true);

        // Get Starting number of cycles
        cycles = lookup[opcode].cycles;

        // Perform fetch of intermmediate data using the
        // required addressing mode
        uint8_t additionalCycle1 = (this->*lookup[opcode].addressMode)();

        // Perform operation
        uint8_t additionalCycle2 = (this->*lookup[opcode].operate)();

        // The addressmode and opcode may have altered the number
        // of cycles this instruction requires before its completed
        cycles += (additionalCycle1 & additionalCycle2);

        // Always set the unused status flag bit to 1
        SetFlag(Flags6502::U, true);

#ifdef LOGMODE
        // This logger dumps every cycle the entire processor state for analysis.
        // This can be used for debugging the emulation, but has little utility
        // during emulation. Its also very slow, so only use if you have to.
        if (logfile == nullptr)	logfile = fopen("olc6502.txt", "wt");
        if (logfile != nullptr)
        {
            fprintf(logfile, "%10d:%02d PC:%04X %s A:%02X X:%02X Y:%02X %s%s%s%s%s%s%s%s STKP:%02X\n",
                clock_count, 0, log_pc, "XXX", a, x, y,
                GetFlag(Flags6502::N) ? "N" : ".", GetFlag(Flags6502::V) ? "V" : ".", GetFlag(Flags6502::U) ? "U" : ".",
                GetFlag(Flags6502::B) ? "B" : ".", GetFlag(Flags6502::D) ? "D" : ".", GetFlag(Flags6502::I) ? "I" : ".",
                GetFlag(Flags6502::Z) ? "Z" : ".", GetFlag(Flags6502::C) ? "C" : ".", stkp);
        }
#endif
    }

    // Increment global clock count - This is actually unused unless logging is enabled
    // but I've kept it in because its a handy watch variable for debugging
    clockCount++;

    // Decrement the number of cycles remaining for this instruction
    cycles--;
}

///////////////////////////////////////////////////////////////////////////////
// FLAG FUNCTIONS
///////////////////////////////////////////////////////////////////////////////

// Returns the value of a specific bit of the status register
uint8_t Olc6502::GetFlag(Flags6502 flag) {
    return ((status & (uint8_t)flag) > 0) ? 1 : 0;
}

// Sets or clears a specific bit of the status register
void Olc6502::SetFlag(Flags6502 flag, bool value) {
    if (value) {
        status |= (uint8_t)flag;
    }
    else {
        status &= ~(uint8_t)flag;
    }
}

///////////////////////////////////////////////////////////////////////////////
// ADDRESSING MODES

// The 6502 can address between 0x0000 - 0xFFFF. The high byte is often referred
// to as the "page", and the low byte is the offset into that page. This implies
// there are 256 pages, each containing 256 bytes.
//
// Several addressing modes have the potential to require an additional clock
// cycle if they cross a page boundary. This is combined with several instructions
// that enable this additional clock cycle. So each addressing function returns
// a flag saying it has potential, as does each instruction. If both instruction
// and address function return 1, then an additional clock cycle is required.
///////////////////////////////////////////////////////////////////////////////

// Address Mode: Implied
// There is no additional data required for this instruction. The instruction
// does something very simple like like sets a status bit. However, we will
// target the accumulator, for instructions like PHA
uint8_t Olc6502::IMP() {
    fetched = a;
    return 0;
}

// Address Mode: Immediate
// The instruction expects the next byte to be used as a value, so we'll prep
// the read address to point to the next byte
uint8_t Olc6502::IMM() {
    addressAbsolute = pc++;
    return 0;
}

// Address Mode: Zero Page
// To save program bytes, zero page addressing allows you to absolutely address
// a location in first 0xFF bytes of address range. Clearly this only requires
// one byte instead of the usual two.
uint8_t Olc6502::ZP0() {
    addressAbsolute = Read(pc++) & 0x00FF;
    return 0;
}

// Address Mode: Zero Page with X Offset
// Fundamentally the same as Zero Page addressing, but the contents of the X Register
// is added to the supplied single byte address. This is useful for iterating through
// ranges within the first page.
uint8_t Olc6502::ZPX() {
    addressAbsolute = (Read(pc++) + x) & 0x00FF;
    return 0;
}

// Address Mode: Zero Page with Y Offset
// Same as above but uses Y Register for offset
uint8_t Olc6502::ZPY() {
    addressAbsolute = (Read(pc++) + y) & 0x00FF;
    return 0;
}

// Address Mode: Relative
// This address mode is exclusive to branch instructions. The address
// must reside within -128 to +127 of the branch instruction, i.e.
// you cant directly branch to any address in the addressable range.
uint8_t Olc6502::REL() {
    addressRelative = Read(pc++);

    if (addressRelative & 0x80) { // Check if negative and set all high bytes to 1 to keep negative
        addressRelative |= 0xFF00;
    }

    return 0;
}

// Address Mode: Absolute 
// A full 16-bit address is loaded and used
uint8_t Olc6502::ABS() {
    addressAbsolute = Read(pc++);
    addressAbsolute |= Read(pc++) << 8;
    return 0;
}

// Address Mode: Absolute with X Offset
// Fundamentally the same as absolute addressing, but the contents of the X Register
// is added to the supplied two byte address. If the resulting address changes
// the page, an additional clock cycle is required
uint8_t Olc6502::ABX() {
    uint16_t low = Read(pc++);
    uint16_t high = Read(pc++);

    addressAbsolute = ((high << 8) | low) + x;
    return (addressAbsolute & 0xFF00) != (high << 8) ? 1 : 0;
}

// Address Mode: Absolute with Y Offset
// Fundamentally the same as absolute addressing, but the contents of the Y Register
// is added to the supplied two byte address. If the resulting address changes
// the page, an additional clock cycle is required
uint8_t Olc6502::ABY() {
    uint16_t low = Read(pc++);
    uint16_t high = Read(pc++);

    addressAbsolute = ((high << 8) | low) + y;
    return (addressAbsolute & 0xFF00) != (high << 8) ? 1 : 0;
}

// Note: The next 3 address modes use indirection (aka Pointers!)

// Address Mode: Indirect
// The supplied 16-bit address is read to get the actual 16-bit address. This is
// instruction is unusual in that it has a bug in the hardware! To emulate its
// function accurately, we also need to emulate this bug. If the low byte of the
// supplied address is 0xFF, then to read the high byte of the actual address
// we need to cross a page boundary. This doesnt actually work on the chip as 
// designed, instead it wraps back around in the same page, yielding an 
// invalid actual address
uint8_t Olc6502::IND() {
    uint16_t ptrLow = Read(pc++);
    uint16_t ptrHigh = Read(pc++);
    uint16_t ptr = (ptrHigh << 8) | ptrLow;

    if (ptrLow == 0x00FF) { // Simulate page boundary hardware bug
        addressAbsolute = (Read(ptr & 0xFF00) << 8) | Read(ptr + 0);
    }
    else { // Behave normally
        addressAbsolute = (Read(ptr + 1) << 8) | Read(ptr + 0);
    }

    return 0;
}

// Address Mode: Indirect X
// The supplied 8-bit address is offset by X Register to index
// a location in page 0x00. The actual 16-bit address is read 
// from this location
uint8_t Olc6502::IZX() {
    uint16_t t = Read(pc++);
    uint16_t low = Read((uint16_t)(t + (uint16_t)x + 0) & 0x00FF);
    uint16_t high = Read((uint16_t)(t + (uint16_t)x + 1) & 0x00FF);

    addressAbsolute = (high << 8) | low;
    return 0;
}

// Address Mode: Indirect Y
// The supplied 8-bit address indexes a location in page 0x00. From 
// here the actual 16-bit address is read, and the contents of
// Y Register is added to it to offset it. If the offset causes a
// change in page then an additional clock cycle is required.
uint8_t Olc6502::IZY() {
    uint16_t t = Read(pc++);
    uint16_t low = Read((t + 0) & 0x00FF);
    uint16_t high = Read((t + 1) & 0x00FF);

    addressAbsolute = ((high << 8) | low) + y;

    return (addressAbsolute & 0xFF00) != (high << 8) ? 1 : 0;
}

// This function sources the data used by the instruction into 
// a convenient numeric variable. Some instructions dont have to 
// fetch data as the source is implied by the instruction. For example
// "INX" increments the X register. There is no additional data
// required. For all other addressing modes, the data resides at 
// the location held within addr_abs, so it is read from there. 
// Immediate adress mode exploits this slightly, as that has
// set addr_abs = pc + 1, so it fetches the data from the
// next byte for example "LDA $FF" just loads the accumulator with
// 256, i.e. no far reaching memory fetch is required. "fetched"
// is a variable global to the CPU, and is set by calling this 
// function. It also returns it for convenience.
uint8_t Olc6502::Fetch() {
    if (lookup[opcode].addressMode != &Olc6502::IMP) {
        fetched = Read(addressAbsolute);
    }

    return fetched;
}

///////////////////////////////////////////////////////////////////////////////
// INSTRUCTION IMPLEMENTATIONS
///////////////////////////////////////////////////////////////////////////////

// Note: Ive started with the two most complicated instructions to emulate, which
// ironically is addition and subtraction! Ive tried to include a detailed 
// explanation as to why they are so complex, yet so fundamental. Im also NOT
// going to do this through the explanation of 1 and 2's complement.

// Instruction: Add with Carry In
// Function:    A = A + M + C
// Flags Out:   C, V, N, Z
//
// Explanation:
// The purpose of this function is to add a value to the accumulator and a carry bit. If
// the result is > 255 there is an overflow setting the carry bit. Ths allows you to
// chain together ADC instructions to add numbers larger than 8-bits. This in itself is
// simple, however the 6502 supports the concepts of Negativity/Positivity and Signed Overflow.
//
// 10000100 = 128 + 4 = 132 in normal circumstances, we know this as unsigned and it allows
// us to represent numbers between 0 and 255 (given 8 bits). The 6502 can also interpret 
// this word as something else if we assume those 8 bits represent the range -128 to +127,
// i.e. it has become signed.
//
// Since 132 > 127, it effectively wraps around, through -128, to -124. This wraparound is
// called overflow, and this is a useful to know as it indicates that the calculation has
// gone outside the permissable range, and therefore no longer makes numeric sense.
//
// Note the implementation of ADD is the same in binary, this is just about how the numbers
// are represented, so the word 10000100 can be both -124 and 132 depending upon the 
// context the programming is using it in. We can prove this!
//
//  10000100 =  132  or  -124
// +00010001 = + 17      + 17
//  ========    ===       ===     See, both are valid additions, but our interpretation of
//  10010101 =  149  or  -107     the context changes the value, not the hardware!
//
// In principle under the -128 to 127 range:
// 10000000 = -128, 11111111 = -1, 00000000 = 0, 00000000 = +1, 01111111 = +127
// therefore negative numbers have the most significant set, positive numbers do not
//
// To assist us, the 6502 can set the overflow flag, if the result of the addition has
// wrapped around. V <- ~(A^M) & A^(A+M+C) :D lol, let's work out why!
//
// Let's suppose we have A = 30, M = 10 and C = 0
//          A = 30 = 00011110
//          M = 10 = 00001010+
//     RESULT = 40 = 00101000
//
// Here we have not gone out of range. The resulting significant bit has not changed.
// So let's make a truth table to understand when overflow has occurred. Here I take
// the MSB of each component, where R is RESULT.
//
// A  M  R | V | A^R | A^M |~(A^M) | 
// 0  0  0 | 0 |  0  |  0  |   1   |
// 0  0  1 | 1 |  1  |  0  |   1   |
// 0  1  0 | 0 |  0  |  1  |   0   |
// 0  1  1 | 0 |  1  |  1  |   0   |  so V = ~(A^M) & (A^R)
// 1  0  0 | 0 |  1  |  1  |   0   |
// 1  0  1 | 0 |  0  |  1  |   0   |
// 1  1  0 | 1 |  1  |  0  |   1   |
// 1  1  1 | 0 |  0  |  0  |   1   |
//
// We can see how the above equation calculates V, based on A, M and R. V was chosen
// based on the following hypothesis:
//       Positive Number + Positive Number = Negative Result -> Overflow
//       Negative Number + Negative Number = Positive Result -> Overflow
//       Positive Number + Negative Number = Either Result -> Cannot Overflow
//       Positive Number + Positive Number = Positive Result -> OK! No Overflow
//       Negative Number + Negative Number = Negative Result -> OK! NO Overflow

uint8_t Olc6502::ADC() {
    // Grab the data that we are adding to the accumulator
    Fetch();

    // Add is performed in 16-bit domain for emulation to capture any
    // carry bit, which will exist in bit 8 of the 16-bit word
    uint16_t a16 = (uint16_t)a;
    uint16_t f16 = (uint16_t)fetched;
    uint16_t temp = a16 + f16 + (uint16_t)GetFlag(Flags6502::C);

    SetFlag(Flags6502::C, temp > 255);                          // The carry flag out exists in the high byte bit 0
    SetFlag(Flags6502::Z, (temp & 0x00FF) == 0);                   // The Zero flag is set if the result is 0
    SetFlag(Flags6502::N, temp & 0x0080);                          // The negative flag is set to the most significant bit of the result
    SetFlag(Flags6502::V, (a16 ^ temp) & (f16 ^ temp) & 0x0080);   // The signed Overflow flag is set based on all that up there! :D

    // Load the result into the accumulator (it's 8-bit dont forget!)
    a = temp & 0x00FF;

    // This instruction has the potential to require an additional clock cycle
    return 1;
}

// Instruction: Subtraction with Borrow In
// Function:    A = A - M - (1 - C)
// Flags Out:   C, V, N, Z
//
// Explanation:
// Given the explanation for ADC above, we can reorganise our data
// to use the same computation for addition, for subtraction by multiplying
// the data by -1, i.e. make it negative
//
// A = A - M - (1 - C)  ->  A = A + -1 * (M - (1 - C))  ->  A = A + (-M + 1 + C)
//
// To make a signed positive number negative, we can invert the bits and add 1
// (OK, I lied, a little bit of 1 and 2s complement :P)
//
//  5 = 00000101
// -5 = 11111010 + 00000001 = 11111011 (or 251 in our 0 to 255 range)
//
// The range is actually unimportant, because if I take the value 15, and add 251
// to it, given we wrap around at 256, the result is 10, so it has effectively 
// subtracted 5, which was the original intention. (15 + 251) % 256 = 10
//
// Note that the equation above used (1-C), but this got converted to + 1 + C.
// This means we already have the +1, so all we need to do is invert the bits
// of M, the data(!) therfore we can simply add, exactly the same way we did 
// before.

uint8_t Olc6502::SBC() {
    Fetch();

    // Operating in 16-bit domain to capture carry out
    // We can invert the bottom 8 bits with bitwise xor
    uint16_t a16 = (uint16_t)a;
    uint16_t f16 = (uint16_t)fetched;
    uint16_t value = f16 ^ 0x00FF;
    uint16_t temp = a16 + value + (uint16_t)GetFlag(Flags6502::C);

    // Notice this is exactly the same as addition from here!
    SetFlag(Flags6502::C, temp & 0xFF00);
    SetFlag(Flags6502::Z, (temp & 0x00FF) == 0);
    SetFlag(Flags6502::N, temp & 0x0080);
    SetFlag(Flags6502::V, (a16 ^ temp) & (value ^ temp) & 0x0080);

    a = temp & 0x00FF;

    return 1;
}

// OK! Complicated operations are done! the following are much simpler
// and conventional. The typical order of events is:
// 1) Fetch the data you are working with
// 2) Perform calculation
// 3) Store the result in desired place
// 4) Set Flags of the status register
// 5) Return if instruction has potential to require additional 
//    clock cycle


// Instruction: Bitwise Logic AND
// Function:    A = A & M
// Flags Out:   N, Z
uint8_t Olc6502::AND() {
    Fetch();
    a = a & fetched;
    SetFlag(Flags6502::Z, a == 0x00);
    SetFlag(Flags6502::N, a & 0x80);
    return 1;
}

// Instruction: Arithmetic Shift Left
// Function:    A = C <- (A << 1) <- 0
// Flags Out:   N, Z, C
uint8_t Olc6502::ASL() {
    Fetch();
    uint16_t temp = (uint16_t)fetched << 1;
    SetFlag(Flags6502::C, (temp & 0xFF00) > 0);
    SetFlag(Flags6502::Z, (temp & 0x00FF) == 0x00);
    SetFlag(Flags6502::N, temp & 0x0080);

    if (lookup[opcode].addressMode == &Olc6502::IMP) {
        a = temp & 0x00FF;
    }
    else {
        Write(addressAbsolute, temp & 0x00FF);
    }

    return 0;
}

// Instruction: Branch if Carry Clear
// Function:    if(C == 0) pc = address 
uint8_t Olc6502::BCC() {
    if (!GetFlag(Flags6502::C)) {
        cycles++;
        addressAbsolute = pc + addressRelative;

        if ((addressAbsolute & 0xFF00) != (pc & 0xFF00)) {
            cycles++;
        }

        pc = addressAbsolute;
    }

    return 0;
}

// Instruction: Branch if Carry Set
// Function:    if(C == 1) pc = address
uint8_t Olc6502::BCS() {
    if (GetFlag(Flags6502::C)) {
        cycles++;
        addressAbsolute = pc + addressRelative;

        if ((addressAbsolute & 0xFF00) != (pc & 0xFF00)) {
            cycles++;
        }

        pc = addressAbsolute;
    }

    return 0;
}

// Instruction: Branch if Equal
// Function:    if(Z == 1) pc = address
uint8_t Olc6502::BEQ() {
    if (GetFlag(Flags6502::Z)) {
        cycles++;
        addressAbsolute = pc + addressRelative;

        if ((addressAbsolute & 0xFF00) != (pc & 0xFF00)) {
            cycles++;
        }

        pc = addressAbsolute;
    }

    return 0;
}

// Instruction: Test Bits in Memory with Accumulator
uint8_t Olc6502::BIT() {
    Fetch();
    uint16_t temp = a & fetched;
    SetFlag(Flags6502::Z, (temp & 0x00FF) == 0x00);
    SetFlag(Flags6502::N, fetched & (1 << 7));
    SetFlag(Flags6502::V, fetched & (1 << 6));
    return 0;
}

// Instruction: Branch if Negative
// Function:    if(N == 1) pc = address
uint8_t Olc6502::BMI() {
    if (GetFlag(Flags6502::N)) {
        cycles++;
        addressAbsolute = pc + addressRelative;

        if ((addressAbsolute & 0xFF00) != (pc & 0xFF00)) {
            cycles++;
        }

        pc = addressAbsolute;
    }

    return 0;
}

// Instruction: Branch if Not Equal
// Function:    if(Z == 0) pc = address
uint8_t Olc6502::BNE() {
    if (!GetFlag(Flags6502::Z)) {
        cycles++;
        addressAbsolute = pc + addressRelative;

        if ((addressAbsolute & 0xFF00) != (pc & 0xFF00)) {
            cycles++;
        }

        pc = addressAbsolute;
    }

    return 0;
}

// Instruction: Branch if Positive
// Function:    if(N == 0) pc = address
uint8_t Olc6502::BPL() {
    if (!GetFlag(Flags6502::N)) {
        cycles++;
        addressAbsolute = pc + addressRelative;

        if ((addressAbsolute & 0xFF00) != (pc & 0xFF00)) {
            cycles++;
        }

        pc = addressAbsolute;
    }

    return 0;
}

// Instruction: Break
// Function:    Program Sourced Interrupt
uint8_t Olc6502::BRK() {
    pc++;

    SetFlag(Flags6502::I, true);
    Write(0x0100 + stkp--, (pc >> 8) & 0x00FF);
    Write(0x0100 + stkp--, pc & 0x00FF);

    SetFlag(Flags6502::B, true);
    Write(0x0100 + stkp--, status);
    SetFlag(Flags6502::B, false);

    pc = (uint16_t)Read(0xFFFE) | ((uint16_t)Read(0xFFFF) << 8);
    return 0;
}

// Instruction: Branch if Overflow Clear
// Function:    if(V == 0) pc = address
uint8_t Olc6502::BVC() {
    if (!GetFlag(Flags6502::V)) {
        cycles++;
        addressAbsolute = pc + addressRelative;

        if ((addressAbsolute & 0xFF00) != (pc & 0xFF00)) {
            cycles++;
        }

        pc = addressAbsolute;
    }

    return 0;
}

// Instruction: Branch if Overflow Set
// Function:    if(V == 1) pc = address
uint8_t Olc6502::BVS() {
    if (GetFlag(Flags6502::V)) {
        cycles++;
        addressAbsolute = pc + addressRelative;

        if ((addressAbsolute & 0xFF00) != (pc & 0xFF00)) {
            cycles++;
        }

        pc = addressAbsolute;
    }

    return 0;
}

// Instruction: Clear Carry Flag
// Function:    C = 0
uint8_t Olc6502::CLC() {
    SetFlag(Flags6502::C, false);
    return 0;
}

// Instruction: Clear Decimal Flag
// Function:    D = 0
uint8_t Olc6502::CLD() {
    SetFlag(Flags6502::D, false);
    return 0;
}

// Instruction: Disable Interrupts / Clear Interrupt Flag
// Function:    I = 0
uint8_t Olc6502::CLI() {
    SetFlag(Flags6502::I, false);
    return 0;
}

// Instruction: Clear Overflow Flag
// Function:    V = 0
uint8_t Olc6502::CLV() {
    SetFlag(Flags6502::V, false);
    return 0;
}

// Instruction: Compare Accumulator
// Function:    C <- A >= M      Z <- (A - M) == 0
// Flags Out:   N, C, Z
uint8_t Olc6502::CMP() {
    Fetch();
    uint16_t temp = (uint16_t)a - (uint16_t)fetched;
    SetFlag(Flags6502::C, a >= fetched);
    SetFlag(Flags6502::Z, (temp & 0x00FF) == 0x0000);
    SetFlag(Flags6502::N, temp & 0x0080);
    return 1;
}

// Instruction: Compare X Register
// Function:    C <- X >= M      Z <- (X - M) == 0
// Flags Out:   N, C, Z
uint8_t Olc6502::CPX() {
    Fetch();
    uint16_t temp = (uint16_t)x - (uint16_t)fetched;
    SetFlag(Flags6502::C, x >= fetched);
    SetFlag(Flags6502::Z, (temp & 0x00FF) == 0x0000);
    SetFlag(Flags6502::N, temp & 0x0080);
    return 0;
}

// Instruction: Compare Y Register
// Function:    C <- Y >= M      Z <- (Y - M) == 0
// Flags Out:   N, C, Z
uint8_t Olc6502::CPY() {
    Fetch();
    uint16_t temp = (uint16_t)y - (uint16_t)fetched;
    SetFlag(Flags6502::C, y >= fetched);
    SetFlag(Flags6502::Z, (temp & 0x00FF) == 0x0000);
    SetFlag(Flags6502::N, temp & 0x0080);
    return 0;
}

// Instruction: Decrement Value at Memory Location
// Function:    M = M - 1
// Flags Out:   N, Z
uint8_t Olc6502::DEC() {
    Fetch();
    uint16_t temp = fetched - 1;
    Write(addressAbsolute, temp & 0x00FF);
    SetFlag(Flags6502::Z, (temp & 0x00FF) == 0x0000);
    SetFlag(Flags6502::N, temp & 0x0080);
    return 0;
}

// Instruction: Decrement X Register
// Function:    X = X - 1
// Flags Out:   N, Z
uint8_t Olc6502::DEX() {
    x--;
    SetFlag(Flags6502::Z, x == 0x00);
    SetFlag(Flags6502::N, x & 0x80);
    return 0;
}

// Instruction: Decrement Y Register
// Function:    Y = Y - 1
// Flags Out:   N, Z
uint8_t Olc6502::DEY() {
    y--;
    SetFlag(Flags6502::Z, y == 0x00);
    SetFlag(Flags6502::N, y & 0x80);
    return 0;
}

// Instruction: Bitwise Logic XOR
// Function:    A = A xor M
// Flags Out:   N, Z
uint8_t Olc6502::EOR() {
    Fetch();
    a = a ^ fetched;
    SetFlag(Flags6502::Z, a == 0x00);
    SetFlag(Flags6502::N, a & 0x80);
    return 1;
}

// Instruction: Increment Value at Memory Location
// Function:    M = M + 1
// Flags Out:   N, Z
uint8_t Olc6502::INC() {
    Fetch();
    uint16_t temp = fetched + 1;
    Write(addressAbsolute, temp & 0x00FF);
    SetFlag(Flags6502::Z, (temp & 0x00FF) == 0x0000);
    SetFlag(Flags6502::N, temp & 0x0080);
    return 0;
}

// Instruction: Increment X Register
// Function:    X = X + 1
// Flags Out:   N, Z
uint8_t Olc6502::INX() {
    x++;
    SetFlag(Flags6502::Z, x == 0x00);
    SetFlag(Flags6502::N, x & 0x80);
    return 0;
}

// Instruction: Increment Y Register
// Function:    Y = Y + 1
// Flags Out:   N, Z
uint8_t Olc6502::INY() {
    y++;
    SetFlag(Flags6502::Z, y == 0x00);
    SetFlag(Flags6502::N, y & 0x80);
    return 0;
}

// Instruction: Jump To Location
// Function:    pc = address
uint8_t Olc6502::JMP() {
    pc = addressAbsolute;
    return 0;
}

// Instruction: Jump To Sub-Routine
// Function:    Push current pc to stack, pc = address
uint8_t Olc6502::JSR() {
    pc--;
    Write(0x0100 + stkp--, (pc >> 8) & 0x00FF);
    Write(0x0100 + stkp--, pc & 0x00FF);

    pc = addressAbsolute;
    return 0;
}

// Instruction: Load The Accumulator
// Function:    A = M
// Flags Out:   N, Z
uint8_t Olc6502::LDA() {
    Fetch();
    a = fetched;
    SetFlag(Flags6502::Z, a == 0x00);
    SetFlag(Flags6502::N, a & 0x80);
    return 1;
}

// Instruction: Load The X Register
// Function:    X = M
// Flags Out:   N, Z
uint8_t Olc6502::LDX() {
    Fetch();
    x = fetched;
    SetFlag(Flags6502::Z, x == 0x00);
    SetFlag(Flags6502::N, x & 0x80);
    return 1;
}

// Instruction: Load The Y Register
// Function:    Y = M
// Flags Out:   N, Z
uint8_t Olc6502::LDY() {
    Fetch();
    y = fetched;
    SetFlag(Flags6502::Z, y == 0x00);
    SetFlag(Flags6502::N, y & 0x80);
    return 1;
}

// Instruction: Shift One Bit Right (Memory or Accumulator)
uint8_t Olc6502::LSR() {
    Fetch();
    SetFlag(Flags6502::C, fetched & 0x0001);
    uint16_t temp = fetched >> 1;
    SetFlag(Flags6502::Z, (temp & 0x00FF) == 0x0000);
    SetFlag(Flags6502::N, temp & 0x0080);

    if (lookup[opcode].addressMode == &Olc6502::IMP) {
        a = temp & 0x00FF;
    }
    else {
        Write(addressAbsolute, temp & 0x00FF);
    }

    return 0;
}

// Instruction: No Operation
uint8_t Olc6502::NOP() {
    // Sadly not all NOPs are equal, Ive added a few here
    // based on https://wiki.nesdev.com/w/index.php/CPU_unofficial_opcodes
    // and will add more based on game compatibility, and ultimately
    // I'd like to cover all illegal opcodes too
    switch (opcode) {
    case 0x1C:
    case 0x3C:
    case 0x5C:
    case 0x7C:
    case 0xDC:
    case 0xFC:
        return 1;
        break;
    }
    return 0;
}

// Instruction: Bitwise Logic OR
// Function:    A = A | M
// Flags Out:   N, Z
uint8_t Olc6502::ORA() {
    Fetch();
    a = a | fetched;
    SetFlag(Flags6502::Z, a == 0x00);
    SetFlag(Flags6502::N, a & 0x80);
    return 1;
}

// Instruction: Push Accumulator to Stack
// Function:    A -> stack
uint8_t Olc6502::PHA() {
    Write(0x0100 + stkp--, a);
    return 0;
}

// Instruction: Push Status Register to Stack
// Function:    status -> stack
// Note:        Break flag is set to 1 before push
uint8_t Olc6502::PHP() {
    Write(0x0100 + stkp--, status | (uint8_t)Flags6502::B | (uint8_t)Flags6502::U);
    SetFlag(Flags6502::B, false);
    SetFlag(Flags6502::U, false);
    return 0;
}

// Instruction: Pop Accumulator off Stack
// Function:    A <- stack
// Flags Out:   N, Z
uint8_t Olc6502::PLA() {
    a = Read(0x0100 + ++stkp);
    SetFlag(Flags6502::Z, a == 0x00);
    SetFlag(Flags6502::N, a & 0x80);
    return 0;
}

// Instruction: Pop Status Register off Stack
// Function:    Status <- stack
uint8_t Olc6502::PLP() {
    status = Read(0x0100 + ++stkp);
    SetFlag(Flags6502::U, true);
    return 0;
}

// Instruction: Rotate One Bit Left (Memory or Accumulator)
uint8_t Olc6502::ROL() {
    Fetch();

    uint16_t temp = (uint16_t)(fetched << 1) | (uint16_t)GetFlag(Flags6502::C);
    SetFlag(Flags6502::C, temp & 0xFF00);
    SetFlag(Flags6502::Z, (temp & 0x00FF) == 0x0000);
    SetFlag(Flags6502::N, temp & 0x0080);

    if (lookup[opcode].addressMode == &Olc6502::IMP) {
        a = temp & 0x00FF;
    }
    else {
        Write(addressAbsolute, temp & 0x00FF);
    }

    return 0;
}

// Instruction: Rotate One Bit Right (Memory or Accumulator)
uint8_t Olc6502::ROR() {
    Fetch();

    uint16_t temp = (uint16_t)(GetFlag(Flags6502::C) << 7) | (fetched >> 1);
    SetFlag(Flags6502::C, fetched & 0x01);
    SetFlag(Flags6502::Z, (temp & 0x00FF) == 0x0000);
    SetFlag(Flags6502::N, temp & 0x0080);

    if (lookup[opcode].addressMode == &Olc6502::IMP) {
        a = temp & 0x00FF;
    }
    else {
        Write(addressAbsolute, temp & 0x00FF);
    }

    return 0;
}

// Instruction: Return from Interrupt
uint8_t Olc6502::RTI() {
    status = Read(0x0100 + ++stkp);
    status &= ~(uint8_t)Flags6502::B;
    status &= ~(uint8_t)Flags6502::U;

    pc = (uint16_t)Read(0x0100 + ++stkp);
    pc |= (uint16_t)Read(0x0100 + ++stkp) << 8;

    return 0;
}

// Instruction: Return from Subroutine
uint8_t Olc6502::RTS() {
    pc = (uint16_t)Read(0x0100 + ++stkp);
    pc |= (uint16_t)Read(0x0100 + ++stkp) << 8;
    pc++;
    return 0;
}

// Instruction: Set Carry Flag
// Function:    C = 1
uint8_t Olc6502::SEC() {
    SetFlag(Flags6502::C, true);
    return 0;
}

// Instruction: Set Decimal Flag
// Function:    D = 1
uint8_t Olc6502::SED() {
    SetFlag(Flags6502::D, true);
    return 0;
}

// Instruction: Set Interrupt Flag / Enable Interrupts
// Function:    I = 1
uint8_t Olc6502::SEI() {
    SetFlag(Flags6502::I, true);
    return 0;
}

// Instruction: Store Accumulator at Address
// Function:    M = A
uint8_t Olc6502::STA() {
    Write(addressAbsolute, a);
    return 0;
}

// Instruction: Store X Register at Address
// Function:    M = X
uint8_t Olc6502::STX() {
    Write(addressAbsolute, x);
    return 0;
}

// Instruction: Store Y Register at Address
// Function:    M = Y
uint8_t Olc6502::STY() {
    Write(addressAbsolute, y);
    return 0;
}

// Instruction: Transfer Accumulator to X Register
// Function:    X = A
// Flags Out:   N, Z
uint8_t Olc6502::TAX() {
    x = a;
    SetFlag(Flags6502::Z, x == 0x00);
    SetFlag(Flags6502::N, x & 0x80);
    return 0;
}

// Instruction: Transfer Accumulator to Y Register
// Function:    Y = A
// Flags Out:   N, Z
uint8_t Olc6502::TAY() {
    y = a;
    SetFlag(Flags6502::Z, y == 0x00);
    SetFlag(Flags6502::N, y & 0x80);
    return 0;
}

// Instruction: Transfer Stack Pointer to X Register
// Function:    X = stack pointer
// Flags Out:   N, Z
uint8_t Olc6502::TSX() {
    x = stkp;
    SetFlag(Flags6502::Z, x == 0x00);
    SetFlag(Flags6502::N, x & 0x80);
    return 0;
}

// Instruction: Transfer X Register to Accumulator
// Function:    A = X
// Flags Out:   N, Z
uint8_t Olc6502::TXA() {
    a = x;
    SetFlag(Flags6502::Z, a == 0x00);
    SetFlag(Flags6502::N, a & 0x80);
    return 0;
}

// Instruction: Transfer X Register to Stack Pointer
// Function:    stack pointer = X
uint8_t Olc6502::TXS() {
    stkp = x;
    return 0;
}

// Instruction: Transfer Y Register to Accumulator
// Function:    A = Y
// Flags Out:   N, Z
uint8_t Olc6502::TYA() {
    a = y;
    SetFlag(Flags6502::Z, a == 0x00);
    SetFlag(Flags6502::N, a & 0x80);
    return 0;
}

// This function captures illegal opcodes
uint8_t Olc6502::XXX() {
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS
///////////////////////////////////////////////////////////////////////////////
bool Olc6502::Complete() {
    return cycles == 0;
}

// This is the disassembly function. Its workings are not required for emulation.
// It is merely a convenience function to turn the binary instruction code into
// human readable form. Its included as part of the emulator because it can take
// advantage of many of the CPUs internal operations to do this.
std::map<uint16_t, std::string> Olc6502::Disassemble(uint16_t startAddress, uint16_t stopAddress) {
    uint32_t address = startAddress;
    uint8_t value = 0x00;
    uint8_t low = 0x00;
    uint8_t high = 0x00;
    std::map<uint16_t, std::string> mapLines;
    uint16_t lineAddress = 0x0000;

    // A convenient utility to convert variables into
    // hex strings because "modern C++"'s method with 
    // streams is atrocious
    auto hex = [](uint32_t n, uint8_t d) {
        std::string s(d, '0');
        for (int i = d - 1; i >= 0; i--, n >>= 4) {
            s[i] = "0123456789ABCDEF"[n & 0xF];
        }
        return s;
    };

    // Starting at the specified address we read an instruction
    // byte, which in turn yields information from the lookup table
    // as to how many additional bytes we need to read and what the
    // addressing mode is. I need this info to assemble human readable
    // syntax, which is different depending upon the addressing mode

    // As the instruction is decoded, a std::string is assembled
    // with the readable output
    while (address <= (uint32_t)stopAddress) {
        lineAddress = address;

        // Prefix line with instruction address
        std::string instruction = "$" + hex(address, 4) + ": ";

        // Read instruction, and get its readable name
        uint8_t opcode = bus->CpuRead(address++, true);
        instruction += lookup[opcode].name + " ";

        // Get oprands from desired locations, and form the
        // instruction based upon its addressing mode. These
        // routines mimmick the actual fetch routine of the
        // 6502 in order to get accurate data as part of the
        // instruction
        auto addressMode = lookup[opcode].addressMode;
        if (addressMode == &Olc6502::IMP) {
            instruction += " {IMP}";
        }
        else if (addressMode == &Olc6502::IMM) {
            value = bus->CpuRead(address++, true);
            instruction += "#$" + hex(value, 2) + " {IMM}";
        }
        else if (addressMode == &Olc6502::ZP0) {
            low = bus->CpuRead(address++, true);
            high = 0x00;
            instruction += "$" + hex(low, 2) + " {ZP0}";
        }
        else if (addressMode == &Olc6502::ZPX) {
            low = bus->CpuRead(address++, true);
            high = 0x00;
            instruction += "$" + hex(low, 2) + ", X {ZPX}";
        }
        else if (addressMode == &Olc6502::ZPY) {
            low = bus->CpuRead(address++, true);
            high = 0x00;
            instruction += "$" + hex(low, 2) + ", Y {ZPY}";
        }
        else if (addressMode == &Olc6502::IZX) {
            low = bus->CpuRead(address++, true);
            high = 0x00;
            instruction += "($" + hex(low, 2) + ", X) {IZX}";
        }
        else if (addressMode == &Olc6502::IZY) {
            low = bus->CpuRead(address++, true);
            high = 0x00;
            instruction += "($" + hex(low, 2) + ", Y) {IZY}";
        }
        else if (addressMode == &Olc6502::ABS) {
            low = bus->CpuRead(address++, true);
            high = bus->CpuRead(address++, true);
            instruction += "$" + hex((uint16_t)(high << 8) | low, 4) + " {ABS}";
        }
        else if (addressMode == &Olc6502::ABX) {
            low = bus->CpuRead(address++, true);
            high = bus->CpuRead(address++, true);
            instruction += "$" + hex((uint16_t)(high << 8) | low, 4) + ", X {ABX}";
        }
        else if (addressMode == &Olc6502::ABY) {
            low = bus->CpuRead(address++, true);
            high = bus->CpuRead(address++, true);
            instruction += "$" + hex((uint16_t)(high << 8) | low, 4) + ", Y {ABY}";
        }
        else if (addressMode == &Olc6502::IND) {
            low = bus->CpuRead(address++, true);
            high = bus->CpuRead(address++, true);
            instruction += "($" + hex((uint16_t)(high << 8) | low, 4) + ") {IND}";
        }
        else if (addressMode == &Olc6502::REL) {
            value = bus->CpuRead(address++, true);
            instruction += "$" + hex(value, 2) + " [$" + hex(address + (int8_t)value, 4) + "] {REL}";
        }

        // Add the formed string to a std::map, using the instruction's
        // address as the key. This makes it convenient to look for later
        // as the instructions are variable in length, so a straight up
        // incremental index is not sufficient.
        mapLines[lineAddress] = instruction;
    }

    return mapLines;
}