/*
    olc::NES - System Bus
*/

#include "Hardware\Bus.h"

Bus::Bus() : ram() {
    // Clear Ram contents, just in case
    for (auto& i : ram) { i = 0x00; }

    // Connect CPU to communication bus
    cpu.ConnectBus(this);
}

bool Bus::Clock() {
    // Clocking. The heart and soul of an emulator. The running
    // frequency is controlled by whatever calls this function.
    // So here we "divide" the clock as necessary and call
    // the peripheral devices clock() function at the correct
    // times.

    // The fastest clock frequency the digital system cares
    // about is equivalent to the PPU clock. So the PPU is clocked
    // each time this function is called.
    ppu.Clock();

    // ...also clock the APU
    apu.Clock();

    // The CPU runs 3 times slower than the PPU so we only call its
    // clock() function every 3 times this function is called. We
    // have a global counter to keep track of this.
    if (systemClockCounter % 3 == 0) {
        // Is the system performing a DMA transfer form CPU memory to 
        // OAM memory on PPU?...
        if (bDmaTransfer) {
            // ...Yes! We need to wait until the next even CPU clock cycle
            // before it starts...
            if (bDmaDummy) {
                // ...So hang around in here each clock until 1 or 2 cycles
                // have elapsed...
                if (systemClockCounter % 2 == 1) {
                    // ...and finally allow DMA to start
                    bDmaDummy = false;
                }
            }
            // DMA can take place!
            else if (systemClockCounter % 2 == 0) {
                
                // On even clock cycles, read from CPU bus
                dmaData = CpuRead(dmaPage << 8 | dmaAddress);
            }
            else {
                // On odd clock cycles, write to PPU OAM
                ppu.pOam[dmaAddress++] = dmaData; // Increment the lo byte of the address

                // If this wraps around, we know that 256
                // bytes have been written, so end the DMA
                // transfer, and proceed as normal
                if (dmaAddress == 0x00) {
                    bDmaTransfer = false;
                    bDmaDummy = true;
                }
            }
        }
        else {
            // No DMA happening, the CPU is in control of its
            // own destiny. Go forth my friend and calculate
            // awesomeness for many generations to come...
            cpu.Clock();
        }
    }

    // Synchronising with Audio
    bool bAudioSampleReady = false;
    audioTime += audioTimePerNesClock;
    if (audioTime >= audioTimePerSystemSample) {
        audioTime -= audioTimePerSystemSample;
        audioSample = apu.GetOutputSample();
        bAudioSampleReady = true;
    }

    // The PPU is capable of emitting an interrupt to indicate the
    // vertical blanking period has been entered. If it has, we need
    // to send that irq to the CPU.
    if (ppu.bNonmaskableInterrupt) {
        ppu.bNonmaskableInterrupt = false;
        cpu.NonmaskableInterrupt();
    }

    // Check if cartridge is requesting IRQ
    if (cartridge->GetMapper()->IrqState()) {
        cartridge->GetMapper()->IrqClear();
        cpu.InterruptRequest();
    }

    systemClockCounter++;

    return bAudioSampleReady;
}

uint8_t Bus::CpuRead(uint16_t address, bool bReadOnly) {
    uint8_t data = 0x00;

    if (cartridge->CpuRead(address, data)) {
        // Cartridge Address Range
    }
    else if (address >= 0x0000 && address <= 0x1FFF) {
        // System RAM Address Range, mirrored every 2048
        data = ram[address & 0x07FF];
    }
    else if (address >= 0x2000 && address <= 0x3FFF) {
        // PPU Address range, mirrored every 8
        data = ppu.CpuRead(address & 0x0007, bReadOnly);
    }
    else if (address == 0x4015) {
        // APU Read Status
        data = apu.CpuRead(address);
    }
    else if (address >= 0x4016 && address <= 0x4017) {
        // Read out the MSB of the controller status word
        data = (controllerState[address & 0x0001] & 0x80) > 0;
        controllerState[address & 0x0001] <<= 1;
    }

    return data;
}

void Bus::CpuWrite(uint16_t address, uint8_t data) {
    if (cartridge->CpuWrite(address, data)) {
        // The cartridge "sees all" and has the facility to veto
        // the propagation of the bus transaction if it requires.
        // This allows the cartridge to map any address to some
        // other data, including the facility to divert transactions
        // with other physical devices. The NES does not do this
        // but I figured it might be quite a flexible way of adding
        // "custom" hardware to the NES in the future!
    }
    else if (address >= 0x0000 && address <= 0x1FFF) {
        // System RAM Address Range. The range covers 8KB, though
        // there is only 2KB available. That 2KB is "mirrored"
        // through this address range. Using bitwise AND to mask
        // the bottom 11 bits is the same as addr % 2048.
        ram[address & 0x07FF] = data;
    }
    else if (address >= 0x2000 && address <= 0x3FFF) {
        // PPU Address range. The PPU only has 8 primary registers
        // and these are repeated throughout this range. We can
        // use bitwise AND operation to mask the bottom 3 bits, 
        // which is the equivalent of addr % 8.
        ppu.CpuWrite(address & 0x0007, data);
    }
    else if ((address >= 0x4000 && address <= 0x4013) || address == 0x4015 || address == 0x4017) { // NES APU
        apu.CpuWrite(address, data);
    }
    else if (address == 0x4014) {
        // A write to this address initiates a DMA transfer
        dmaPage = data;
        dmaAddress = 0x00;
        bDmaTransfer = true;
    }
    else if (address >= 0x4016 && address <= 0x4017) {
        // "Lock In" controller state at this time
        controllerState[address & 0x0001] = controller[address & 0x0001];
    }
}

void Bus::InsertCartridge(const std::shared_ptr<Cartridge>& cartridge) {
    // Connects cartridge to both Main Bus and CPU Bus
    this->cartridge = cartridge;
    ppu.ConnectCartridge(cartridge);
}

void Bus::Reset() {
    cartridge->Reset();
    cpu.Reset();
    ppu.Reset();

    systemClockCounter = 0;

    dmaPage = 0x00;
    dmaAddress = 0x00;
    dmaData = 0x00;
    bDmaDummy = true;
    bDmaTransfer = false;
}

void Bus::SetSampleFrequency(uint32_t sampleRate) {
    audioTimePerSystemSample = 1.0 / (double)sampleRate;
    audioTimePerNesClock = 1.0 / 5369318.0; // PPU Clock Frequency
}