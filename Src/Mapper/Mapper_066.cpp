/*
	olc::NES - Mapper 66
*/

#include "Mapper\Mapper_066.h"

bool Mapper_066::CpuMapRead(uint16_t address, uint32_t& mappedAddress, uint8_t& data) {
	bool bRead = address >= 0x8000 && address <= 0xFFFF;
	if (bRead) { mappedAddress = programBankSelect * 0x8000 + (address & 0x7FFF); }
	return bRead;
}

bool Mapper_066::CpuMapWrite(uint16_t address, uint32_t& mappedAddress, uint8_t data) {
	if (address >= 0x8000 && address <= 0xFFFF) {
		characterBankSelect = data & 0x03;
		programBankSelect = (data & 0x30) >> 4;
	}

	// Mapper has handled write, but do not update ROMs
	return false;
}

bool Mapper_066::PpuMapRead(uint16_t address, uint32_t& mappedAddress) {
	bool bRead = address < 0x2000;
	if (bRead) { mappedAddress = characterBankSelect * 0x2000 + address; }
	return bRead;
}

bool Mapper_066::PpuMapWrite(uint16_t address, uint32_t& mappedAddress) {
	return false;
}

void Mapper_066::Reset() {
	characterBankSelect = 0;
	programBankSelect = 0;
}