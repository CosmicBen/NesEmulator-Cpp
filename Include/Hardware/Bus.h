/*
    olc::NES - System Bus
*/

#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include "Hardware\Olc6502.h"
#include "Hardware\Olc2C02.h"
#include "Hardware\Olc2A03.h"
#include "Hardware\Cartridge.h"

class Bus
{
public:
    Bus();
    ~Bus() { }

public:
    // Devices on Main Bus
    Olc6502 cpu;    // The 6502 derived processor
    Olc2C02 ppu;    // The 2C02 Picture Processing Unit
    Olc2A03 apu;    // The "2A03" Audio Processing Unit
    
    // The Cartridge or "GamePak"
    std::shared_ptr<Cartridge> cartridge;
    uint8_t controller[2];  // Controllers
        
    std::array<uint8_t, 2 * 1024> ram;   // 2KB of RAM
    double audioSample = 0.0;
    
    // Clocks the system - a single whole system tick
    bool Clock();

    // Main Bus Read & Write
    uint8_t CpuRead(uint16_t address, bool bReadOnly = false);
    void    CpuWrite(uint16_t address, uint8_t data);

    // Connects a cartridge object to the internal buses
    void InsertCartridge(const std::shared_ptr<Cartridge>& cartridge);

    // Resets the system
    void Reset();

    // Synchronisation with system Audio
    void SetSampleFrequency(uint32_t sampleRate);

private:
    double audioTime = 0.0;
    double audioGlobalTime = 0.0;
    double audioTimePerNesClock = 0.0;
    double audioTimePerSystemSample = 0.0;

    // A simple form of Direct Memory Access is used to swiftly
    // transfer data from CPU bus memory into the OAM memory. It would
    // take too long to sensibly do this manually using a CPU loop, so
    // the program prepares a page of memory with the sprite info required
    // for the next frame and initiates a DMA transfer. This suspends the
    // CPU momentarily while the PPU gets sent data at PPU clock speeds.
    // Note here, that dma_page and dma_addr form a 16-bit address in 
    // the CPU bus address space
    uint8_t dmaPage = 0x00;
    uint8_t dmaAddress = 0x00;
    uint8_t dmaData = 0x00;

    // DMA transfers need to be timed accurately. In principle it takes
    // 512 cycles to read and write the 256 bytes of the OAM memory, a
    // read followed by a write. However, the CPU needs to be on an "even"
    // clock cycle, so a dummy cycle of idleness may be required
    bool bDmaDummy = true;

    // Finally a flag to indicate that a DMA transfer is happening
    bool bDmaTransfer = false;
        
    uint32_t systemClockCounter = 0;    // A count of how many clocks have passed
    uint8_t controllerState[2];         // Internal cache of controller state
};