/*
	olc::NES - Mapper 000 - NROM
*/

#include "Mapper\Mapper_000.h"

bool Mapper_000::CpuMapRead(uint16_t address, uint32_t& mappedAddress, uint8_t& data) {
	// if PRGROM is 16KB
	//     CPU Address Bus          PRG ROM
	//     0x8000 -> 0xBFFF: Map    0x0000 -> 0x3FFF
	//     0xC000 -> 0xFFFF: Mirror 0x0000 -> 0x3FFF
	// if PRGROM is 32KB
	//     CPU Address Bus          PRG ROM
	//     0x8000 -> 0xFFFF: Map    0x0000 -> 0x7FFF
	if (address >= 0x8000 && address <= 0xFFFF) {
		mappedAddress = address & (programBanks > 1 ? 0x7FFF : 0x3FFF);
		return true;
	}

	return false;
}

bool Mapper_000::CpuMapWrite(uint16_t address, uint32_t& mappedAddress, uint8_t data) {
	if (address >= 0x8000 && address <= 0xFFFF) {
		mappedAddress = address & (programBanks > 1 ? 0x7FFF : 0x3FFF);
		return true;
	}

	return false;
}

bool Mapper_000::PpuMapRead(uint16_t address, uint32_t& mappedAddress) {
	// There is no mapping required for PPU
	// PPU Address Bus          CHR ROM
	// 0x0000 -> 0x1FFF: Map    0x0000 -> 0x1FFF
	if (address >= 0x0000 && address <= 0x1FFF) {
		mappedAddress = address;
		return true;
	}

	return false;
}

bool Mapper_000::PpuMapWrite(uint16_t address, uint32_t& mappedAddress) {
	if (address >= 0x0000 && address <= 0x1FFF && characterBanks == 0) {
		// Treat as RAM
		mappedAddress = address;
		return true;
	}

	return false;
}