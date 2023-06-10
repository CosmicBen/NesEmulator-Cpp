/*
	olc::NES - Cartridge
*/

#pragma once

#include <cstdint>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

#include "Mapper\Mapper.h"

class Cartridge
{
public:
	Cartridge(const std::string& fileName);
	~Cartridge() { }
	
	// Communication with the Main Bus
	bool CpuRead(uint16_t address, uint8_t& data);
	bool CpuWrite(uint16_t address, uint8_t data);

	std::shared_ptr<Mapper> GetMapper() { return mapper; }
	bool ImageValid() {	return bImageValid;	}

	// Get Mirror configuration
	MIRROR Mirror();

	// Communication with the PPU Bus
	bool PpuRead(uint16_t address, uint8_t& data);
	bool PpuWrite(uint16_t address, uint8_t data);

	// Permits system reset of mapper to known state
	void Reset();

private:
	uint8_t mapperId = 0;
	uint8_t characterBanks = 0;
	uint8_t programBanks = 0;

	bool bImageValid = false;

	MIRROR hardwareMirror = MIRROR::HORIZONTAL;
	MIRROR mirror = MIRROR::HORIZONTAL;

	std::vector<uint8_t> characterMemory;
	std::vector<uint8_t> programMemory;

	std::shared_ptr<Mapper> mapper;
};