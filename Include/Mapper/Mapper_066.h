/*
	olc::NES - Mapper 66
*/

#pragma once

#include "Mapper\Mapper.h"

class Mapper_066 : public Mapper {
public:
	Mapper_066(uint8_t programBanks, uint8_t characterBanks) : Mapper(programBanks, characterBanks) { };
	~Mapper_066() { };

	bool CpuMapRead(uint16_t address, uint32_t& mappedAddress, uint8_t& data) override;
	bool CpuMapWrite(uint16_t address, uint32_t& mappedAddress, uint8_t data = 0) override;

	bool PpuMapRead(uint16_t address, uint32_t& mappedAddress) override;
	bool PpuMapWrite(uint16_t address, uint32_t& mappedAddress) override;

	void Reset() override;

private:
	uint8_t characterBankSelect = 0x00;
	uint8_t programBankSelect = 0x00;
};

