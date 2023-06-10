/*
	olc::NES - Mapper 2
*/

#pragma once

#include "Mapper\Mapper.h"

class Mapper_002 : public Mapper {
public:
	Mapper_002(uint8_t programBanks, uint8_t characterBanks) : Mapper(programBanks, characterBanks) { }
	~Mapper_002() { }

	bool CpuMapRead(uint16_t address, uint32_t& mappedAddress, uint8_t& data) override;
	bool CpuMapWrite(uint16_t address, uint32_t& mappedAddress, uint8_t data = 0) override;

	bool PpuMapRead(uint16_t address, uint32_t& mappedAddress) override;
	bool PpuMapWrite(uint16_t address, uint32_t& mappedAddress) override;

	void Reset() override;

private:
	uint8_t programBankSelectLow = 0x00;
	uint8_t programBankSelectHigh = 0x00;
};