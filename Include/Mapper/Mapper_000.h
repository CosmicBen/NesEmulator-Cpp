/*
	olc::NES - Mapper 000 - NROM
*/

#pragma once

#include "Mapper\Mapper.h"

class Mapper_000 : public Mapper
{
public:
	Mapper_000(uint8_t programBanks, uint8_t characterBanks) : Mapper(programBanks, characterBanks) { }
	~Mapper_000() { }

public:
	bool CpuMapRead(uint16_t address, uint32_t& mappedAddress, uint8_t& data) override;
	bool CpuMapWrite(uint16_t address, uint32_t& mappedAddress, uint8_t data = 0x00) override;

	bool PpuMapRead(uint16_t address, uint32_t& mappedAddress) override;
	bool PpuMapWrite(uint16_t address, uint32_t& mappedAddress) override;

	void Reset() override { }

	// No local equipment required
};