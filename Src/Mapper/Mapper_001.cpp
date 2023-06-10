/*
	Olc::NES - Mapper 1
*/

#include "Mapper\Mapper_001.h"

Mapper_001::Mapper_001(uint8_t programBanks, uint8_t characterBanks) : Mapper(programBanks, characterBanks) {
	vRamStatic.resize(32 * 1024);
}

bool Mapper_001::CpuMapRead(uint16_t address, uint32_t& mappedAddress, uint8_t& data) {
	if (address >= 0x6000 && address <= 0x7FFF) {
		mappedAddress = 0xFFFFFFFF;				// Read is from static ram on cartridge
		data = vRamStatic[address & 0x1FFF];	// Read data from RAM
		return true;							// Signal mapper has handled the request
	}
	else if (address >= 0x8000) {
		if (controlRegister & 0b01000) {
			// 16K Mode
			if (address >= 0x8000 && address <= 0xBFFF) {
				mappedAddress = programBankSelect16Low * 0x4000 + (address & 0x3FFF);
				return true;
			}
			else if (address >= 0xC000 && address <= 0xFFFF) {
				mappedAddress = programBankSelect16High * 0x4000 + (address & 0x3FFF);
				return true;
			}
		}
		else {
			// 32K Mode
			mappedAddress = programBankSelect32 * 0x8000 + (address & 0x7FFF);
			return true;
		}
	}

	return false;
}

bool Mapper_001::CpuMapWrite(uint16_t address, uint32_t& mappedAddress, uint8_t data) {
	if (address >= 0x6000 && address <= 0x7FFF) {
		mappedAddress = 0xFFFFFFFF;				// Write is to static ram on cartridge
		vRamStatic[address & 0x1FFF] = data;	// Write data to RAM
		return true;							// Signal mapper has handled request
	}
	else if (address >= 0x8000) {
		if (data & 0x80) {
			// MSB is set, so reset serial loading
			loadRegister = 0x00;
			loadRegisterCount = 0;
			controlRegister = controlRegister | 0x0C;
		}
		else {
			// Load data in serially into load register. It arrives LSB first, so implant this at
			// bit 5. After 5 writes, the register is ready
			loadRegister >>= 1;
			loadRegister |= (data & 0x01) << 4;
			loadRegisterCount++;

			if (loadRegisterCount == 5) {
				// Get mapper target register, by examining bits 13 & 14 of the address
				uint8_t targetRegister = (address >> 13) & 0x03;

				switch (targetRegister) {
				case 0: // 0x8000 - 0x9FFF
					// Set control register
					controlRegister = loadRegister & 0x1F;

					switch (controlRegister & 0x03) {
					case 0: mirrorMode = ONESCREEN_LOW;	 break;
					case 1: mirrorMode = ONESCREEN_HIGH; break;
					case 2: mirrorMode = VERTICAL;		 break;
					case 3: mirrorMode = HORIZONTAL;	 break;
					}
					break;

				case 1: // 0xA000 - 0xBFFF
					// Set character bank low
					if (controlRegister & 0b10000) {
						characterBankSelect4Low = loadRegister & 0x1F;	// 4K character bank at PPU 0x0000
					}
					else {
						characterBankSelect8 = loadRegister & 0x1E;		// 8K character bank at PPU 0x0000
					}
					break;

				case 2: // 0xC000 - 0xDFFF
					// Set character bank high
					if (controlRegister & 0b10000) {
						characterBankSelect4High = loadRegister & 0x1F;	// 4K character bank at PPU 0x1000
					}
					break;

				case 3: // 0xE000 - 0xFFFF
					// Configure program banks
					uint8_t programMode = (controlRegister >> 2) & 0x03;

					switch (programMode) {
					case 0:
					case 1:
						programBankSelect32 = (loadRegister & 0x0E) >> 1;	// Set 32K program bank at CPU 0x8000
						break;
					case 2:
						programBankSelect16Low = 0;							// Fix 16K program bank at CPU 0x8000 to first bank
						programBankSelect16High = loadRegister & 0x0F;		// Set 16K program bank at CPU 0xC000
						break;
					case 3:
						programBankSelect16Low = loadRegister & 0x0F;		// Set 16K program bank at CPU 0x8000
						programBankSelect16High = programBanks - 1;			// Fix 16K program bank at CPU 0xC000 to last bank
						break;
					}
					break;
				}

				// 5 bits were written, and decoded, so reset load register
				loadRegister = 0x00;
				loadRegisterCount = 0;
			}
		}
	}

	// Mapper has handled write, but do not update ROMs
	return false;
}

bool Mapper_001::PpuMapRead(uint16_t address, uint32_t& mappedAddress) {
	if (address < 0x2000) {
		if (characterBanks == 0) {
			mappedAddress = address;
			return true;
		}
		else if (controlRegister & 0b10000) {
			// 4K character bank mode
			if (address >= 0x0000 && address <= 0x0FFF) {
				mappedAddress = characterBankSelect4Low * 0x1000 + (address & 0x0FFF);
				return true;
			}
			else if (address >= 0x1000 && address <= 0x1FFF) {
				mappedAddress = characterBankSelect4High * 0x1000 + (address & 0x0FFF);
				return true;
			}
		}
		else {
			// 8K character bank mode
			mappedAddress = characterBankSelect8 * 0x2000 + (address & 0x1FFF);
			return true;
		}
	}

	return false;
}

bool Mapper_001::PpuMapWrite(uint16_t address, uint32_t& mappedAddress) {
	bool bValidAddress = address < 0x2000;
	if (bValidAddress && characterBanks == 0) { mappedAddress = address; }
	return bValidAddress;
}

void Mapper_001::Reset() {
	controlRegister = 0x1C;
	loadRegister = 0x00;
	loadRegisterCount = 0x00;

	characterBankSelect4Low = 0;
	characterBankSelect4High = 0;
	characterBankSelect8 = 0;

	programBankSelect32 = 0;
	programBankSelect16Low = 0;
	programBankSelect16High = programBanks - 1;
}