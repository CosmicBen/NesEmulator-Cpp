/*
	olc::NES - Mapper 3
*/

#include "Mapper\Mapper_003.h"

Mapper_003::Mapper_003(uint8_t programBanks, uint8_t characterBanks) : Mapper(programBanks, characterBanks) { }
Mapper_003::~Mapper_003() { }

bool Mapper_003::CpuMapRead(uint16_t address, uint32_t& mappedAddress, uint8_t& data) {
	bool bRead = address >= 0x8000 && address <= 0xFFFF;

	if (bRead) {
		switch (programBanks) {
		case 1: mappedAddress = address & 0x3FFF; break; // 16K ROM
		case 2: mappedAddress = address & 0x7FFF; break; // 32K ROM
		}
	}

	return bRead;
}

bool Mapper_003::CpuMapWrite(uint16_t address, uint32_t& mappedAddress, uint8_t data) {
	if (address >= 0x8000 && address <= 0xFFFF) {
		characterBankSelect = data & 0x03;
		mappedAddress = address;
	}

	// Mapper has handled write, but do not update ROMs
	return false;
}

bool Mapper_003::PpuMapRead(uint16_t address, uint32_t& mappedAddress) {
	bool bRead = address < 0x2000;
	if (bRead) { mappedAddress = characterBankSelect * 0x2000 + address; }
	return bRead;
}

bool Mapper_003::PpuMapWrite(uint16_t address, uint32_t& mappedAddress) {
	return false;
}

void Mapper_003::Reset() {
	characterBankSelect = 0x00;
}