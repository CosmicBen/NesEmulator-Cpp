/*
	olc::NES - Mapper 4
*/

#include "Mapper\Mapper_004.h"

Mapper_004::Mapper_004(uint8_t programBanks, uint8_t characterBanks) : Mapper(programBanks, characterBanks) {
	vRamStatic.resize(32 * 1024);
}

bool Mapper_004::CpuMapRead(uint16_t address, uint32_t& mappedAddress, uint8_t& data) {
	if (address >= 0x6000 && address <= 0x7FFF) {
		mappedAddress = 0xFFFFFFFF;				// Write is to static ram on the cartridge
		data = vRamStatic[address & 0x1FFF];	// Write data to RAM
	}
	else if (address >= 0x8000 && address <= 0x9FFF) { mappedAddress = programBank[0] + (address & 0x1FFF); }
	else if (address >= 0xA000 && address <= 0xBFFF) { mappedAddress = programBank[1] + (address & 0x1FFF); }
	else if (address >= 0xC000 && address <= 0xDFFF) { mappedAddress = programBank[2] + (address & 0x1FFF); }
	else if (address >= 0xE000 && address <= 0xFFFF) { mappedAddress = programBank[3] + (address & 0x1FFF); }
	else {
		return false;
	}

	return true; // Signal mapper has handled request
}

bool Mapper_004::CpuMapWrite(uint16_t address, uint32_t& mappedAddress, uint8_t data) {
	if (address >= 0x6000 && address <= 0x7FFF) {
		mappedAddress = 0xFFFFFFFF;				// Write is to static ram on the cartridge
		vRamStatic[address & 0x1FFF] = data;	// Write data to RAM
		return true;							// Signal mapper has handled request
	}

	if (address >= 0x8000 && address <= 0x9FFF) {
		// Bank Select
		if (!(address & 0x0001)) {
			targetRegister = data & 0x07;
			bProgramBankMode = (data & 0x40);
			bCharacterInversion = (data & 0x80);
		}
		else {
			// Update target register
			pRegister[targetRegister] = data;

			// Update Pointer Table
			if (bCharacterInversion) {
				characterBank[0] = pRegister[2] * 0x0400;
				characterBank[1] = pRegister[3] * 0x0400;
				characterBank[2] = pRegister[4] * 0x0400;
				characterBank[3] = pRegister[5] * 0x0400;
				characterBank[4] = (pRegister[0] & 0xFE) * 0x0400;
				characterBank[5] = pRegister[0] * 0x0400 + 0x0400;
				characterBank[6] = (pRegister[1] & 0xFE) * 0x0400;
				characterBank[7] = pRegister[1] * 0x0400 + 0x0400;
			}
			else {
				characterBank[0] = (pRegister[0] & 0xFE) * 0x0400;
				characterBank[1] = pRegister[0] * 0x0400 + 0x0400;
				characterBank[2] = (pRegister[1] & 0xFE) * 0x0400;
				characterBank[3] = pRegister[1] * 0x0400 + 0x0400;
				characterBank[4] = pRegister[2] * 0x0400;
				characterBank[5] = pRegister[3] * 0x0400;
				characterBank[6] = pRegister[4] * 0x0400;
				characterBank[7] = pRegister[5] * 0x0400;
			}

			if (bProgramBankMode) {
				programBank[2] = (pRegister[6] & 0x3F) * 0x2000;
				programBank[0] = (programBanks * 2 - 2) * 0x2000;
			}
			else {
				programBank[0] = (pRegister[6] & 0x3F) * 0x2000;
				programBank[2] = (programBanks * 2 - 2) * 0x2000;
			}

			programBank[1] = (pRegister[7] & 0x3F) * 0x2000;
			programBank[3] = (programBanks * 2 - 1) * 0x2000;
		}
	}
	else if (address >= 0xA000 && address <= 0xBFFF) {
		if (!(address & 0x0001)) {
			// Mirroring
			mirrorMode = data & 0x01 ? MIRROR::HORIZONTAL : MIRROR::VERTICAL;
		}
		else {
			// Program RAM Protect
			// TODO:
		}
	}
	else if (address >= 0xC000 && address <= 0xDFFF) {
		if (!(address & 0x0001)) {
			irqReload = data;
		}
		else {
			irqCounter = 0x0000;
		}
	}
	else if (address >= 0xE000 && address <= 0xFFFF) {
		if (!(address & 0x0001)) {
			bIrqEnable = false;
			bIrqActive = false;
		}
		else {
			bIrqEnable = true;
		}
	}

	return false;
}

bool Mapper_004::PpuMapRead(uint16_t address, uint32_t& mappedAddress) {
	if      (address >= 0x0000 && address <= 0x03FF) { mappedAddress = characterBank[0] + (address & 0x03FF); }
	else if (address >= 0x0400 && address <= 0x07FF) { mappedAddress = characterBank[1] + (address & 0x03FF); }
	else if (address >= 0x0800 && address <= 0x0BFF) { mappedAddress = characterBank[2] + (address & 0x03FF); }
	else if (address >= 0x0C00 && address <= 0x0FFF) { mappedAddress = characterBank[3] + (address & 0x03FF); }
	else if (address >= 0x1000 && address <= 0x13FF) { mappedAddress = characterBank[4] + (address & 0x03FF); }
	else if (address >= 0x1400 && address <= 0x17FF) { mappedAddress = characterBank[5] + (address & 0x03FF); }
	else if (address >= 0x1800 && address <= 0x1BFF) { mappedAddress = characterBank[6] + (address & 0x03FF); }
	else if (address >= 0x1C00 && address <= 0x1FFF) { mappedAddress = characterBank[7] + (address & 0x03FF); }
	else {
		return false;
	}

	return true;
}

bool Mapper_004::PpuMapWrite(uint16_t address, uint32_t& mappedAddress) {
	return false;
}

void Mapper_004::Reset() {
	targetRegister = 0x00;
	bProgramBankMode = false;
	bCharacterInversion = false;
	mirrorMode = MIRROR::HORIZONTAL;

	bIrqActive = false;
	bIrqEnable = false;
	bIrqUpdate = false;
	irqCounter = 0x0000;
	irqReload = 0x0000;

	for (int i = 0; i < 4; i++) { programBank[i] = 0; }
	for (int i = 0; i < 8; i++) { characterBank[i] = 0; pRegister[i] = 0; }

	programBank[0] = 0 * 0x2000;
	programBank[1] = 1 * 0x2000;
	programBank[2] = (programBanks * 2 - 2) * 0x2000;
	programBank[3] = (programBanks * 2 - 1) * 0x2000;
}

void Mapper_004::Scanline() {
	if (irqCounter == 0) {
		irqCounter = irqReload;
	}
	else {
		irqCounter--;
	}

	if (irqCounter == 0 && bIrqEnable) {
		bIrqActive = true;
	}
}