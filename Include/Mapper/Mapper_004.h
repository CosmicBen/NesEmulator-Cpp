/*
    olc::NES - Mapper 4
*/

#pragma once

#include "Mapper\Mapper.h"
#include <vector>

class Mapper_004 : public Mapper {
public:
    Mapper_004(uint8_t programBanks, uint8_t characterBanks);
    ~Mapper_004() { }

    bool CpuMapRead(uint16_t address, uint32_t& mappedAddress, uint8_t& data) override;
    bool CpuMapWrite(uint16_t address, uint32_t& mappedAddress, uint8_t data = 0) override;

    void IrqClear() override { bIrqActive = false; }
    bool IrqState() override { return bIrqActive; }
    MIRROR Mirror() override { return mirrorMode; }

    bool PpuMapRead(uint16_t address, uint32_t& mappedAddress) override;
    bool PpuMapWrite(uint16_t address, uint32_t& mappedAddress) override;

    void Reset() override;
    void Scanline() override;

private:
    // Control variables
    uint8_t targetRegister = 0x00;
    bool bProgramBankMode = false;
    bool bCharacterInversion = false;
    MIRROR mirrorMode = MIRROR::HORIZONTAL;

    uint32_t pRegister[8];
    uint32_t characterBank[8];
    uint32_t programBank[4];

    bool bIrqActive = false;
    bool bIrqEnable = false;
    bool bIrqUpdate = false;
    uint16_t irqCounter = 0x0000;
    uint16_t irqReload = 0x0000;

    std::vector<uint8_t> vRamStatic;
};