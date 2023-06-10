/*
	olc::NES - Mapper 2
*/

#include "Mapper\Mapper_002.h"

bool Mapper_002::CpuMapRead(uint16_t address, uint32_t& mappedAddress, uint8_t& data) {
	if (address >= 0x8000 && address <= 0xBFFF) {
		mappedAddress = programBankSelectLow * 0x4000 + (address & 0x3FFF);
		return true;
	}
	else if (address >= 0xC000 && address <= 0xFFFF) {
		mappedAddress = programBankSelectHigh * 0x4000 + (address & 0x3FFF);
		return true;
	}

	return false;
}

bool Mapper_002::CpuMapWrite(uint16_t address, uint32_t& mappedAddress, uint8_t data) {
	if (address >= 0x8000 && address <= 0xFFFF) {
		programBankSelectLow = data & 0x0F;
	}

	// Mapper has handled write, but do not update ROMs
	return false;
}

bool Mapper_002::PpuMapRead(uint16_t address, uint32_t& mappedAddress) {
	bool bRead = address < 0x2000;
	if (bRead) { mappedAddress = address; }
	return bRead;
}

bool Mapper_002::PpuMapWrite(uint16_t address, uint32_t& mappedAddress) {
	bool bWritten = address < 0x2000 && characterBanks == 0; // Treating as RAM
	if (bWritten) { mappedAddress = address; }
	return bWritten;
}

void Mapper_002::Reset() {
	programBankSelectLow = 0;
	programBankSelectHigh = programBanks - 1;
}