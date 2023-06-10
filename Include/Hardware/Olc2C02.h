/*
	olc::NES - Picture Processing Unit (PPU) 2C02
*/

#pragma once

#include <cstdint>
#include <memory>

#include "Engine\OlcPixelGameEngine.h"

class Cartridge;

class Olc2C02
{
public:
	Olc2C02();
	~Olc2C02();

private:
	uint8_t nameTable[2][1024];
	uint8_t	paletteTable[32];
	uint8_t patternTable[2][4 * 1024];

private:
	olc::Pixel palScreen[0x40];
	olc::Sprite spriteScreen{ 256, 240 };
	olc::Sprite spriteNameTable[2]{ { 256, 240 }, { 256, 240 } };
	olc::Sprite spritePatternTable[2]{ { 128, 128 }, { 128, 128 } };

public:
	// Debugging Utilities
	olc::Sprite& GetScreen();
	olc::Sprite& GetNameTable(uint8_t index);
	olc::Sprite& GetPatternTable(uint8_t index, uint8_t palette);
	olc::Pixel& GetColorFromPaletteRam(uint8_t palette, uint8_t pixel);
	bool frameComplete = false;

private:
	union PpuStatus {
		struct {
			uint8_t unused : 5;
			uint8_t spriteOverflow : 1;
			uint8_t spriteZeroHit : 1;
			uint8_t verticalBlank : 1;
		};

		uint8_t reg;
	} status;

	union PpuMask {
		struct {
			uint8_t grayscale : 1;
			uint8_t renderBackgroundLeft : 1;
			uint8_t renderSpritesLeft : 1;
			uint8_t renderBackground : 1;
			uint8_t renderSprites : 1;
			uint8_t enhanceRed : 1;
			uint8_t enhanceGreen : 1;
			uint8_t enhanceBlue : 1;
		};

		uint8_t reg;
	} mask;

	union PpuControl {
		struct {
			uint8_t nameTableX : 1;
			uint8_t nameTableY : 1;
			uint8_t incrementMode : 1;
			uint8_t patternSprite : 1;
			uint8_t patternBackground : 1;
			uint8_t spriteSize : 1;
			uint8_t slaveMode : 1; // unused
			uint8_t enableNonmaskableInterrupt : 1;
		};

		uint8_t reg;
	} control;

	union LoopyRegister {
		// Credit to Loopy for working this out
		struct {
			uint16_t coarseX : 5;
			uint16_t coarseY : 5;
			uint16_t nameTableX : 1;
			uint16_t nameTableY : 1;
			uint16_t fineY : 3;
			uint16_t unused : 1;
		};

		uint16_t reg = 0x0000;
	};

	LoopyRegister vRamAddress;	// Active "pointer" address into nametable to extract background tile info
	LoopyRegister tRamAddress;	// Temporary store of information to be "transferred" into "pointer" at various times

	// Pixel offset horizontally
	uint8_t fineX = 0x00;

	// Internal communications
	uint8_t addressLatch = 0x00;
	uint8_t ppuDataBuffer = 0x00;

	// Pixel "dot" position information
	int16_t scanline = 0;
	int16_t cycle = 0;
    bool bOddFrame = false;

	// Background rendering =========================================
	uint8_t bgNextTileId = 0x00;
	uint8_t bgNextTileAttribute = 0x00;
	uint8_t bgNextTileLeastSignificantBit = 0x00;
	uint8_t bgNextTileMostSignificantBit = 0x00;

	uint16_t bgShifterPatternLow = 0x0000;
	uint16_t bgShifterPatternHigh = 0x0000;
	uint16_t bgShifterAttributeLow = 0x0000;
	uint16_t bgShifterAttributeHigh = 0x0000;

	// Foreground "Sprite" rendering ================================
	// The OAM is an additional memory internal to the PPU. It is
	// not connected via the any bus. It stores the locations of
	// 64off 8x8 (or 8x16) tiles to be drawn on the next frame.
	struct ObjectAttributeEntry {
		uint8_t y;	// Y position of sprite
		uint8_t id;	// ID of tile from pattern memory
		uint8_t attribute;	// Flags define how sprite should be rendered
		uint8_t x;	// X position of sprite
	} oam[64];

	// A register to store the address when the CPU manually communicates
	// with OAM via PPU registers. This is not commonly used because it 
	// is very slow, and instead a 256-Byte DMA transfer is used. See
	// the Bus header for a description of this.
	uint8_t oamAddress = 0x00;

	ObjectAttributeEntry spriteScanline[8];
	uint8_t spriteCount;
	uint8_t spriteShifterPatternLow[8];
	uint8_t spriteShifterPatternHigh[8];

        // Sprite Zero Collision Flags
	bool bSpriteZeroHitPossible = false;
	bool bSpriteZeroBeingRendered = false;

	// The OAM is conveniently package above to work with, but the DMA
        // mechanism will need access to it for writing one byute at a time
public:
	uint8_t* pOam = (uint8_t*)oam;

public:
	// Communications with Main Bus
	uint8_t CpuRead(uint16_t address, bool bReadOnly = false);
	void CpuWrite(uint16_t address, uint8_t data);

	// Communications with PPU Bus
	uint8_t PpuRead(uint16_t address, bool bReadOnly = false);
	void PpuWrite(uint16_t address, uint8_t data);

private:
	// The Cartridge or "GamePak"
	std::shared_ptr<Cartridge> cartridge;

public:
	// Interface
	void ConnectCartridge(const std::shared_ptr<Cartridge>& cartridge);
	void Clock();
	void Reset();

	bool bNonmaskableInterrupt = false;
    bool bScanlineTrigger = false;
};