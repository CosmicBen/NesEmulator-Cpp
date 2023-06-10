/*
	olc::NES - Mapper 3
*/

#pragma once

#include "Mapper\Mapper.h"

class Mapper_003 : public Mapper {
public:
	Mapper_003(uint8_t programBanks, uint8_t characterBanks);
	~Mapper_003();

	bool CpuMapRead(uint16_t address, uint32_t& mappedAddress, uint8_t& data) override;
	bool CpuMapWrite(uint16_t address, uint32_t& mappedAddress, uint8_t data = 0) override;

	bool PpuMapRead(uint16_t address, uint32_t& mappedAddress) override;
	bool PpuMapWrite(uint16_t address, uint32_t& mappedAddress) override;

	void Reset() override;

private:
	uint8_t characterBankSelect = 0x00;
};