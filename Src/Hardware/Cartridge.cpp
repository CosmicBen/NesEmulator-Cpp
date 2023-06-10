/*
	olc::NES - Cartridge
*/

#include "Hardware\Cartridge.h"

#include "Mapper\Mapper_000.h"
#include "Mapper\Mapper_001.h"
#include "Mapper\Mapper_002.h"
#include "Mapper\Mapper_003.h"
#include "Mapper\Mapper_004.h"
#include "Mapper\Mapper_066.h"

Cartridge::Cartridge(const std::string& fileName) {
	// iNES Format Header
	struct Header {
		char name[4];
		uint8_t programRomChunks;
		uint8_t characterRomChunks;
		uint8_t mapper1;
		uint8_t mapper2;
		uint8_t programRamSize;
		uint8_t tvSystem1;
		uint8_t tvSystem2;
		char unused[5];
	} header;

	bImageValid = false;

	std::ifstream ifs;
	ifs.open(fileName, std::ifstream::binary);
	if (ifs.is_open()) {
		// Read file header
		ifs.read((char*)&header, sizeof(Header));

		// If a "trainer" exists we just need to read past
		// it before we get to the good stuff
		if (header.mapper1 & 0x04) {
			ifs.seekg(512, std::ios_base::cur);
		}

		// Determine Mapper ID
		mapperId = ((header.mapper2 >> 4) << 4) | (header.mapper1 >> 4);
		hardwareMirror = (header.mapper1 & 0x01) ? VERTICAL : HORIZONTAL;

		// "Discover" File Format
		uint8_t fileType = (header.mapper2 & 0x0C) == 0x08 ? 2 : 1;
		
		uint8_t programBlockCount = 0;
		uint8_t characterBlockCount = 0;
		bool bRead = false;

		switch (fileType)
		{
		case 0: break;
		case 1:
			programBanks = header.programRomChunks;
			programBlockCount = programBanks;

			characterBanks = header.characterRomChunks;
			characterBlockCount = characterBanks == 0 ? 1 : characterBanks; // Create character RAM and ROM

			bRead = true;
			break;
		case 2:
			programBanks = ((header.programRamSize & 0x07) << 8) | header.programRomChunks;
			programBlockCount = programBanks;

			characterBanks = ((header.programRamSize & 0x38) << 8) | header.characterRomChunks;
			characterBlockCount = characterBanks;

			bRead = true;
			break;
		}

		if (bRead) {
			programMemory.resize(programBlockCount * 16 * 1024);
			ifs.read((char*)programMemory.data(), programMemory.size());

			characterMemory.resize(characterBlockCount * 8 * 1024);
			ifs.read((char*)characterMemory.data(), characterMemory.size());
		}

		// Load appropriate mapper
		switch (mapperId) {
		case 0:  mapper = std::make_shared<Mapper_000>(programBanks, characterBanks); break;
		case 1:  mapper = std::make_shared<Mapper_001>(programBanks, characterBanks); break;
		case 2:  mapper = std::make_shared<Mapper_002>(programBanks, characterBanks); break;
		case 3:  mapper = std::make_shared<Mapper_003>(programBanks, characterBanks); break;
		case 4:  mapper = std::make_shared<Mapper_004>(programBanks, characterBanks); break;
		case 66: mapper = std::make_shared<Mapper_066>(programBanks, characterBanks); break;
		}

		bImageValid = true;
		ifs.close();
	}
}

bool Cartridge::CpuRead(uint16_t address, uint8_t& data) {
	uint32_t mappedAddress = 0;
	bool bRead = mapper->CpuMapRead(address, mappedAddress, data);

	// Check if Mapper has actually set the data value, for example cartridge based RAM
	if (bRead && mappedAddress != 0xFFFFFFFF) {
		// Mapper has produced an offset into cartridge bank memory
		data = programMemory[mappedAddress];
	}

	return bRead;
}

bool Cartridge::CpuWrite(uint16_t address, uint8_t data) {
	uint32_t mappedAddress = 0;
	bool bWritten = mapper->CpuMapWrite(address, mappedAddress, data);

	// Check if Mapper has actually set the data value, for example cartridge based RAM
	if (bWritten && mappedAddress != 0xFFFFFFFF) {
		// Mapper has produced an offset into cartridge bank memory
		programMemory[mappedAddress] = data;
	}

	return bWritten;
}

MIRROR Cartridge::Mirror() {
	// Check if Mirror configuration was defined in hardware via soldering
	// or if Mirror configuration can be dynamically set via mapper
	return mapper->Mirror() == MIRROR::HARDWARE ? hardwareMirror : mirror;
}

bool Cartridge::PpuRead(uint16_t address, uint8_t& data) {
	uint32_t mappedAddress = 0;

	bool bRead = mapper->PpuMapRead(address, mappedAddress);
	if (bRead) { data = characterMemory[mappedAddress]; }

	return bRead;
}

bool Cartridge::PpuWrite(uint16_t address, uint8_t data) {
	uint32_t mappedAddress = 0;

	bool bWritten = mapper->PpuMapWrite(address, mappedAddress);
	if (bWritten) { characterMemory[mappedAddress] = data; }

	return bWritten;
}

void Cartridge::Reset() {
	// Note: This does not reset the ROM contents,
	// but does reset the mapper.
	if (mapper != nullptr) { mapper->Reset(); }
}