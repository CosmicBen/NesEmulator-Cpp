/*
	Olc::NES - Mapper 1
*/

#pragma once

#include "Mapper\Mapper.h"
#include <vector>

class Mapper_001 : public Mapper
{
public:
	Mapper_001(uint8_t programBanks, uint8_t characterBanks);
	~Mapper_001() { }

	bool CpuMapRead(uint16_t address, uint32_t& mappedAddress, uint8_t& data) override;
	bool CpuMapWrite(uint16_t address, uint32_t& mappedAddress, uint8_t data = 0) override;

	MIRROR Mirror() { return mirrorMode; }

	bool PpuMapRead(uint16_t address, uint32_t& mappedAddress) override;
	bool PpuMapWrite(uint16_t address, uint32_t& mappedAddress) override;

	void Reset() override;

private:
	uint8_t characterBankSelect4Low = 0x00;
	uint8_t characterBankSelect4High = 0x00;
	uint8_t characterBankSelect8 = 0x00;

	uint8_t programBankSelect16Low = 0x00;
	uint8_t programBankSelect16High = 0x00;
	uint8_t programBankSelect32 = 0x00;

	uint8_t controlRegister = 0x00;
	uint8_t loadRegister = 0x00;
	uint8_t loadRegisterCount = 0x00;

	MIRROR mirrorMode = MIRROR::HORIZONTAL;

	std::vector<uint8_t> vRamStatic;
};