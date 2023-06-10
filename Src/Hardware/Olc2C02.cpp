/*
	olc::NES - Picture Processing Unit (PPU) 2C02
*/

#include "Hardware\Olc2C02.h"

#include "Hardware\Cartridge.h"

Olc2C02::Olc2C02() {
	palScreen[0x00] = olc::Pixel(84, 84, 84);
	palScreen[0x01] = olc::Pixel(0, 30, 116);
	palScreen[0x02] = olc::Pixel(8, 16, 144);
	palScreen[0x03] = olc::Pixel(48, 0, 136);
	palScreen[0x04] = olc::Pixel(68, 0, 100);
	palScreen[0x05] = olc::Pixel(92, 0, 48);
	palScreen[0x06] = olc::Pixel(84, 4, 0);
	palScreen[0x07] = olc::Pixel(60, 24, 0);
	palScreen[0x08] = olc::Pixel(32, 42, 0);
	palScreen[0x09] = olc::Pixel(8, 58, 0);
	palScreen[0x0A] = olc::Pixel(0, 64, 0);
	palScreen[0x0B] = olc::Pixel(0, 60, 0);
	palScreen[0x0C] = olc::Pixel(0, 50, 60);
	palScreen[0x0D] = olc::Pixel(0, 0, 0);
	palScreen[0x0E] = olc::Pixel(0, 0, 0);
	palScreen[0x0F] = olc::Pixel(0, 0, 0);

	palScreen[0x10] = olc::Pixel(152, 150, 152);
	palScreen[0x11] = olc::Pixel(8, 76, 196);
	palScreen[0x12] = olc::Pixel(48, 50, 236);
	palScreen[0x13] = olc::Pixel(92, 30, 228);
	palScreen[0x14] = olc::Pixel(136, 20, 176);
	palScreen[0x15] = olc::Pixel(160, 20, 100);
	palScreen[0x16] = olc::Pixel(152, 34, 32);
	palScreen[0x17] = olc::Pixel(120, 60, 0);
	palScreen[0x18] = olc::Pixel(84, 90, 0);
	palScreen[0x19] = olc::Pixel(40, 114, 0);
	palScreen[0x1A] = olc::Pixel(8, 124, 0);
	palScreen[0x1B] = olc::Pixel(0, 118, 40);
	palScreen[0x1C] = olc::Pixel(0, 102, 120);
	palScreen[0x1D] = olc::Pixel(0, 0, 0);
	palScreen[0x1E] = olc::Pixel(0, 0, 0);
	palScreen[0x1F] = olc::Pixel(0, 0, 0);

	palScreen[0x20] = olc::Pixel(236, 238, 236);
	palScreen[0x21] = olc::Pixel(76, 154, 236);
	palScreen[0x22] = olc::Pixel(120, 124, 236);
	palScreen[0x23] = olc::Pixel(176, 98, 236);
	palScreen[0x24] = olc::Pixel(228, 84, 236);
	palScreen[0x25] = olc::Pixel(236, 88, 180);
	palScreen[0x26] = olc::Pixel(236, 106, 100);
	palScreen[0x27] = olc::Pixel(212, 136, 32);
	palScreen[0x28] = olc::Pixel(160, 170, 0);
	palScreen[0x29] = olc::Pixel(116, 196, 0);
	palScreen[0x2A] = olc::Pixel(76, 208, 32);
	palScreen[0x2B] = olc::Pixel(56, 204, 108);
	palScreen[0x2C] = olc::Pixel(56, 180, 204);
	palScreen[0x2D] = olc::Pixel(60, 60, 60);
	palScreen[0x2E] = olc::Pixel(0, 0, 0);
	palScreen[0x2F] = olc::Pixel(0, 0, 0);

	palScreen[0x30] = olc::Pixel(236, 238, 236);
	palScreen[0x31] = olc::Pixel(168, 204, 236);
	palScreen[0x32] = olc::Pixel(188, 188, 236);
	palScreen[0x33] = olc::Pixel(212, 178, 236);
	palScreen[0x34] = olc::Pixel(236, 174, 236);
	palScreen[0x35] = olc::Pixel(236, 174, 212);
	palScreen[0x36] = olc::Pixel(236, 180, 176);
	palScreen[0x37] = olc::Pixel(228, 196, 144);
	palScreen[0x38] = olc::Pixel(204, 210, 120);
	palScreen[0x39] = olc::Pixel(180, 222, 120);
	palScreen[0x3A] = olc::Pixel(168, 226, 144);
	palScreen[0x3B] = olc::Pixel(152, 226, 180);
	palScreen[0x3C] = olc::Pixel(160, 214, 228);
	palScreen[0x3D] = olc::Pixel(160, 162, 160);
	palScreen[0x3E] = olc::Pixel(0, 0, 0);
	palScreen[0x3F] = olc::Pixel(0, 0, 0);
}

Olc2C02::~Olc2C02() { }

olc::Sprite& Olc2C02::GetScreen() {
	// Simply returns the current sprite holding the rendered screen
	return spriteScreen;
}

olc::Sprite& Olc2C02::GetNameTable(uint8_t index) {
	// As of now unused, but a placeholder for nametable visualisation in teh future
	return spriteNameTable[index];
}

olc::Sprite& Olc2C02::GetPatternTable(uint8_t index, uint8_t palette) {
	// This function draw the CHR ROM for a given pattern table into
	// an olc::Sprite, using a specified palette. Pattern tables consist
	// of 16x16 "tiles or characters". It is independent of the running
	// emulation and using it does not change the systems state, though
	// it gets all the data it needs from the live system. Consequently,
	// if the game has not yet established palettes or mapped to relevant
	// CHR ROM banks, the sprite may look empty. This approach permits a 
	// "live" extraction of the pattern table exactly how the NES, and 
	// ultimately the player would see it.

	// A tile consists of 8x8 pixels. On the NES, pixels are 2 bits, which
	// gives an index into 4 different colours of a specific palette. There
	// are 8 palettes to choose from. Colour "0" in each palette is effectively
	// considered transparent, as those locations in memory "mirror" the global
	// background colour being used. This mechanics of this are shown in 
	// detail in PpuRead() & PpuWrite()

	// Characters on NES
	// ~~~~~~~~~~~~~~~~~
	// The NES stores characters using 2-bit pixels. These are not stored sequentially
	// but in singular bit planes. For example:
	//
	// 2-Bit Pixels       LSB Bit Plane     MSB Bit Plane
	// 0 0 0 0 0 0 0 0	  0 0 0 0 0 0 0 0   0 0 0 0 0 0 0 0
	// 0 1 1 0 0 1 1 0	  0 1 1 0 0 1 1 0   0 0 0 0 0 0 0 0
	// 0 1 2 0 0 2 1 0	  0 1 1 0 0 1 1 0   0 0 1 0 0 1 0 0
	// 0 0 0 0 0 0 0 0 =  0 0 0 0 0 0 0 0 + 0 0 0 0 0 0 0 0
	// 0 1 1 0 0 1 1 0	  0 1 1 0 0 1 1 0   0 0 0 0 0 0 0 0
	// 0 0 1 1 1 1 0 0	  0 0 1 1 1 1 0 0   0 0 0 0 0 0 0 0
	// 0 0 0 2 2 0 0 0	  0 0 0 1 1 0 0 0   0 0 0 1 1 0 0 0
	// 0 0 0 0 0 0 0 0	  0 0 0 0 0 0 0 0   0 0 0 0 0 0 0 0
	//
	// The planes are stored as 8 bytes of LSB, followed by 8 bytes of MSB

	// Loop through all 16x16 tiles
	for (uint16_t tileY = 0; tileY < 16; tileY++) {
		for (uint16_t tileX = 0; tileX < 16; tileX++) {
			// Convert the 2D tile coordinate into a 1D offset into the pattern
			// table memory.
			uint16_t offset = tileY * 256 + tileX * 16;

			// Now loop through 8 rows of 8 pixels
			for (uint16_t row = 0; row < 8; row++) {
				// For each row, we need to read both bit planes of the character
				// in order to extract the least significant and most significant 
				// bits of the 2 bit pixel value. in the CHR ROM, each character
				// is stored as 64 bits of lsb, followed by 64 bits of msb. This
				// conveniently means that two corresponding rows are always 8
				// bytes apart in memory.
				uint8_t tileLeastSignificantBit = PpuRead(index * 0x1000 + offset + row + 0x0000);
				uint8_t tileMostSignificantBit = PpuRead(index * 0x1000 + offset + row + 0x0008);

				// Now we have a single row of the two bit planes for the character
				// we need to iterate through the 8-bit words, combining them to give
				// us the final pixel index
				for (uint16_t col = 0; col < 8; col++) {
					// We can get the index value by simply adding the bits together
					// but we're only interested in the lsb of the row words because...
					uint8_t pixel = (tileMostSignificantBit & 0x01) << 1 | (tileLeastSignificantBit & 0x01);

					// ...we will shift the row words 1 bit right for each column of
					// the character.
					tileLeastSignificantBit >>= 1;
					tileMostSignificantBit >>= 1;

					// Now we know the location and NES pixel value for a specific location
					// in the pattern table, we can translate that to a screen colour, and an
					// (x,y) location in the sprite
					spritePatternTable[index].SetPixel(
						tileX * 8 + (7 - col), // Because we are using the lsb of the row word first
						tileY * 8 + row, // we are effectively reading the row from right
						GetColorFromPaletteRam(palette, pixel)); 	// to left, so we need to draw the row "backwards"
				}
			}
		}
	}

	// Finally return the updated sprite representing the pattern table
	return spritePatternTable[index];
}

olc::Pixel& Olc2C02::GetColorFromPaletteRam(uint8_t palette, uint8_t pixel) {
	// This is a convenience function that takes a specified palette and pixel
	// index and returns the appropriate screen colour.
	// "0x3F00"       - Offset into PPU addressable range where palettes are stored
	// "palette << 2" - Each palette is 4 bytes in size
	// "pixel"        - Each pixel index is either 0, 1, 2 or 3
	// "& 0x3F"       - Stops us reading beyond the bounds of the palScreen array
	return palScreen[PpuRead(0x3F00 + (palette << 2) + pixel) & 0x3F];

	// Note: We dont access paletteTable directly here, instead we know that PpuRead()
	// will map the address onto the seperate small RAM attached to the PPU bus.
}

uint8_t Olc2C02::CpuRead(uint16_t address, bool bReadOnly) {
	uint8_t data = 0x00;

	if (bReadOnly) {
		// Reading from PPU registers can affect their contents
		// so this read only option is used for examining the
		// state of the PPU without changing its state. This is
		// really only used in debug mode.
		switch (address) {
		case 0x0000: data = control.reg; break; // Control
		case 0x0001: data = mask.reg; break; // Mask
		case 0x0002: data = status.reg; break; // Status
		case 0x0003: break; // OAM Address
		case 0x0004: break; // OAM Data
		case 0x0005: break; // Scroll
		case 0x0006: break; // PPU Address
		case 0x0007: break; // PPU Data
		}
	}
	else {
		// These are the live PPU registers that repsond
		// to being read from in various ways. Note that not
		// all the registers are capable of being read from
		// so they just return 0x00
		switch (address) {
			// Control - Not readable
		case 0x0000: break;

			// Mask - Not Readable
		case 0x0001: break;

			// Status
		case 0x0002:
			// Reading from the status register has the effect of resetting
			// different parts of the circuit. Only the top three bits
			// contain status information, however it is possible that
			// some "noise" gets picked up on the bottom 5 bits which 
			// represent the last PPU bus transaction. Some games "may"
			// use this noise as valid data (even though they probably
			// shouldn't)
			data = (status.reg & 0xE0) | (ppuDataBuffer & 0x1F);

			// Clear the vertical blanking flag
			status.verticalBlank = 0;

			// Reset Loopy's Address latch flag
			addressLatch = 0;
			break;

			// OAM Address - Not Readable
		case 0x0003: break;

			// OAM Data
		case 0x0004: data = pOam[oamAddress]; break;

			// Scroll - Not Readable
		case 0x0005: break;

			// PPU Address - Not Readable
		case 0x0006: break;

			// PPU Data
		case 0x0007:
			// Reads from the NameTable ram get delayed one cycle, 
			// so output buffer which contains the data from the 
			// previous read request
			data = ppuDataBuffer;

			// then update the buffer for next time
			ppuDataBuffer = PpuRead(vRamAddress.reg);

			// However, if the address was in the palette range, the
			// data is not delayed, so it returns immediately
			if (vRamAddress.reg >= 0x3F00) { data = ppuDataBuffer; }

			// All reads from PPU data automatically increment the nametable
			// address depending upon the mode set in the control register.
			// If set to vertical mode, the increment is 32, so it skips
			// one whole nametable row; in horizontal mode it just increments
			// by 1, moving to the next column
			vRamAddress.reg += (control.incrementMode ? 32 : 1);
			break;
		}
	}

	return data;
}

void Olc2C02::CpuWrite(uint16_t address, uint8_t data) {
	switch (address) {
	case 0x0000: // Control
		control.reg = data;
		tRamAddress.nameTableX = control.nameTableX;
		tRamAddress.nameTableY = control.nameTableY;
		break;

	case 0x0001: mask.reg = data; break; // Mask
	case 0x0002: break; // Status
	case 0x0003: oamAddress = data; break; // OAM Address
	case 0x0004: pOam[oamAddress] = data; break; // OAM Data

	case 0x0005: // Scroll
		if (addressLatch == 0) {
			// First write to scroll register contains X offset in pixel space
			// which we split into coarse and fine x values
			fineX = data & 0x07;
			tRamAddress.coarseX = data >> 3;
			addressLatch = 1;
		}
		else {
			// First write to scroll register contains Y offset in pixel space
			// which we split into coarse and fine Y values
			tRamAddress.fineY = data & 0x07;
			tRamAddress.coarseY = data >> 3;
			addressLatch = 0;
		}
		break;

	case 0x0006: // PPU Address
		if (addressLatch == 0) {
			// PPU address bus can be accessed by CPU via the ADDR and DATA
			// registers. The fisrt write to this register latches the high byte
			// of the address, the second is the low byte. Note the writes
			// are stored in the tram register...
			tRamAddress.reg = (tRamAddress.reg & 0x00FF) | (uint16_t)((data & 0x3F) << 8);
			addressLatch = 1;
		}
		else {
			// ...when a whole address has been written, the internal vram address
			// buffer is updated. Writing to the PPU is unwise during rendering
			// as the PPU will maintam the vram address automatically whilst
			// rendering the scanline position.
			tRamAddress.reg = (tRamAddress.reg & 0xFF00) | data;
			vRamAddress = tRamAddress;
			addressLatch = 0;
		}
		break;

	case 0x0007: // PPU Data
		PpuWrite(vRamAddress.reg, data);
		// All writes from PPU data automatically increment the nametable
		// address depending upon the mode set in the control register.
		// If set to vertical mode, the increment is 32, so it skips
		// one whole nametable row; in horizontal mode it just increments
		// by 1, moving to the next column
		vRamAddress.reg += (control.incrementMode ? 32 : 1);
		break;
	}
}

uint8_t Olc2C02::PpuRead(uint16_t address, bool bReadOnly) {
	uint8_t data = 0x00;
	address &= 0x3FFF;

	if (cartridge->PpuRead(address, data)) {

	}
	else if (address >= 0x0000 && address <= 0x1FFF) {
		// If the cartridge cant map the address, have
		// a physical location ready here
		data = patternTable[(address & 0x1000) >> 12][address & 0x0FFF];
	}
	else if (address >= 0x2000 && address <= 0x3EFF) {
		address &= 0x0FFF;

		if (cartridge->Mirror() == MIRROR::VERTICAL) {
			// Vertical
			if (address >= 0x0000 && address <= 0x03FF) { data = nameTable[0][address & 0x03FF]; }
			if (address >= 0x0400 && address <= 0x07FF) { data = nameTable[1][address & 0x03FF]; }
			if (address >= 0x0800 && address <= 0x0BFF) { data = nameTable[0][address & 0x03FF]; }
			if (address >= 0x0C00 && address <= 0x0FFF) { data = nameTable[1][address & 0x03FF]; }
		}
		else if (cartridge->Mirror() == MIRROR::HORIZONTAL) {
			// Horizontal
			if (address >= 0x0000 && address <= 0x03FF) { data = nameTable[0][address & 0x03FF]; }
			if (address >= 0x0400 && address <= 0x07FF) { data = nameTable[0][address & 0x03FF]; }
			if (address >= 0x0800 && address <= 0x0BFF) { data = nameTable[1][address & 0x03FF]; }
			if (address >= 0x0C00 && address <= 0x0FFF) { data = nameTable[1][address & 0x03FF]; }
		}
	}
	else if (address >= 0x3F00 && address <= 0x3FFF) {
		address &= 0x001F;

		if (address == 0x0010) { address = 0x0000; }
		if (address == 0x0014) { address = 0x0004; }
		if (address == 0x0018) { address = 0x0008; }
		if (address == 0x001C) { address = 0x000C; }

		data = paletteTable[address] & (mask.grayscale ? 0x30 : 0x3F);
	}

	return data;
}

void Olc2C02::PpuWrite(uint16_t address, uint8_t data) {
	address &= 0x3FFF;

	if (cartridge->PpuWrite(address, data)) {

	}
	else if (address >= 0x0000 && address <= 0x1FFF) {
		patternTable[(address & 0x1000) >> 12][address & 0x0FFF] = data;
	}
	else if (address >= 0x2000 && address <= 0x3EFF) {
		address &= 0x0FFF;

		if (cartridge->Mirror() == MIRROR::VERTICAL) {
			// Vertical
			if (address >= 0x0000 && address <= 0x03FF) { nameTable[0][address & 0x03FF] = data; }
			if (address >= 0x0400 && address <= 0x07FF) { nameTable[1][address & 0x03FF] = data; }
			if (address >= 0x0800 && address <= 0x0BFF) { nameTable[0][address & 0x03FF] = data; }
			if (address >= 0x0C00 && address <= 0x0FFF) { nameTable[1][address & 0x03FF] = data; }
		}
		else if (cartridge->Mirror() == MIRROR::HORIZONTAL) {
			// Horizontal
			if (address >= 0x0000 && address <= 0x03FF) { nameTable[0][address & 0x03FF] = data; }
			if (address >= 0x0400 && address <= 0x07FF) { nameTable[0][address & 0x03FF] = data; }
			if (address >= 0x0800 && address <= 0x0BFF) { nameTable[1][address & 0x03FF] = data; }
			if (address >= 0x0C00 && address <= 0x0FFF) { nameTable[1][address & 0x03FF] = data; }
		}
	}
	else if (address >= 0x3F00 && address <= 0x3FFF) {
		address &= 0x001F;

		if (address == 0x0010) { address = 0x0000; }
		if (address == 0x0014) { address = 0x0004; }
		if (address == 0x0018) { address = 0x0008; }
		if (address == 0x001C) { address = 0x000C; }

		paletteTable[address] = data;
	}
}

void Olc2C02::ConnectCartridge(const std::shared_ptr<Cartridge>& cartridge) {
	this->cartridge = cartridge;
}

void Olc2C02::Clock() {
	// As we progress through scanlines and cycles, the PPU is effectively
	// a state machine going through the motions of fetching background 
	// information and sprite information, compositing them into a pixel
	// to be output.

	// The lambda functions (functions inside functions) contain the various
	// actions to be performed depending upon the output of the state machine
	// for a given scanline/cycle combination

	// ==============================================================================
	// Increment the background tile "pointer" one tile/column horizontally
	auto IncrementScrollX = [&]() {
		// Note: pixel perfect scrolling horizontally is handled by the 
		// data shifters. Here we are operating in the spatial domain of 
		// tiles, 8x8 pixel blocks.

		// Ony if rendering is enabled
		if (mask.renderBackground || mask.renderSprites) {
			// A single name table is 32x30 tiles. As we increment horizontally
			// we may cross into a neighbouring nametable, or wrap around to
			// a neighbouring nametable
			if (vRamAddress.coarseX == 31) {
				// Leaving nametable so wrap address round
				vRamAddress.coarseX = 0;
				// Flip target nametable bit
				vRamAddress.nameTableX = ~vRamAddress.nameTableX;
			}
			else {
				// Staying in current nametable, so just increment
				vRamAddress.coarseX++;
			}
		}
	};

	// ==============================================================================
	// Increment the background tile "pointer" one scanline vertically
	auto IncrementScrollY = [&]() {
		// Incrementing vertically is more complicated. The visible nametable
		// is 32x30 tiles, but in memory there is enough room for 32x32 tiles.
		// The bottom two rows of tiles are in fact not tiles at all, they
		// contain the "attribute" information for the entire table. This is
		// information that describes which palettes are used for different 
		// regions of the nametable.

		// In addition, the NES doesnt scroll vertically in chunks of 8 pixels
		// i.e. the height of a tile, it can perform fine scrolling by using
		// the fineY component of the register. This means an increment in Y
		// first adjusts the fine offset, but may need to adjust the whole
		// row offset, since fineY is a value 0 to 7, and a row is 8 pixels high

		// Ony if rendering is enabled
		if (mask.renderBackground || mask.renderSprites) {
			// If possible, just increment the fine y offset
			if (vRamAddress.fineY < 7) {
				vRamAddress.fineY++;
			}
			else {
				// If we have gone beyond the height of a row, we need to
				// increment the row, potentially wrapping into neighbouring
				// vertical nametables. Dont forget however, the bottom two rows
				// do not contain tile information. The coarse y offset is used
				// to identify which row of the nametable we want, and the fine
				// y offset is the specific "scanline"

				// Reset fine y offset
				vRamAddress.fineY = 0;

				// Check if we need to swap vertical nametable targets
				if (vRamAddress.coarseY == 29) {
					// We do, so reset coarse y offset
					vRamAddress.coarseY = 0;
					// And flip the target nametable bit
					vRamAddress.nameTableY = ~vRamAddress.nameTableY;
				}
				else if (vRamAddress.coarseY == 31) {
					// In case the pointer is in the attribute memory, we
					// just wrap around the current nametable
					vRamAddress.coarseY = 0;
				}
				else {
					// None of the above boundary/wrapping conditions apply
					// so just increment the coarse y offset
					vRamAddress.coarseY++;
				}
			}
		}
	};

	// ==============================================================================
	// Transfer the temporarily stored horizontal nametable access information
	// into the "pointer". Note that fine x scrolling is not part of the "pointer"
	// addressing mechanism
	auto TransferAddressX = [&]() {
		// Only if rendering is enabled
		if (mask.renderBackground || mask.renderSprites) {
			vRamAddress.nameTableX = tRamAddress.nameTableX;
			vRamAddress.coarseX = tRamAddress.coarseX;
		}
	};

	// ==============================================================================
	// Transfer the temporarily stored vertical nametable access information
	// into the "pointer". Note that fine y scrolling is part of the "pointer"
	// addressing mechanism
	auto TransferAddressY = [&]() {
		// Only if rendering is enabled
		if (mask.renderBackground || mask.renderSprites) {
			vRamAddress.fineY = tRamAddress.fineY;
			vRamAddress.nameTableY = tRamAddress.nameTableY;
			vRamAddress.coarseY = tRamAddress.coarseY;
		}
	};

	// ==============================================================================
	// Prime the "in-effect" background tile shifters ready for outputting next
	// 8 pixels in scanline.
	auto LoadBackgroundShifters = [&]() {
		// Each PPU update we calculate one pixel. These shifters shift 1 bit along
		// feeding the pixel compositor with the binary information it needs. Its
		// 16 bits wide, because the top 8 bits are the current 8 pixels being drawn
		// and the bottom 8 bits are the next 8 pixels to be drawn. Naturally this means
		// the required bit is always the MSB of the shifter. However, "fine x" scrolling
		// plays a part in this too, whcih is seen later, so in fact we can choose
		// any one of the top 8 bits.
		bgShifterPatternLow = (bgShifterPatternLow & 0xFF00) | bgNextTileLeastSignificantBit;
		bgShifterPatternHigh = (bgShifterPatternHigh & 0xFF00) | bgNextTileMostSignificantBit;

		// Attribute bits do not change per pixel, rather they change every 8 pixels
		// but are synchronised with the pattern shifters for convenience, so here
		// we take the bottom 2 bits of the attribute word which represent which 
		// palette is being used for the current 8 pixels and the next 8 pixels, and 
		// "inflate" them to 8 bit words.
		bgShifterAttributeLow = (bgShifterAttributeLow & 0xFF00) | ((bgNextTileAttribute & 0b01) ? 0xFF : 0x00);
		bgShifterAttributeHigh = (bgShifterAttributeHigh & 0xFF00) | ((bgNextTileAttribute & 0b10) ? 0xFF : 0x00);
	};

	// ==============================================================================
	// Every cycle the shifters storing pattern and attribute information shift
	// their contents by 1 bit. This is because every cycle, the output progresses
	// by 1 pixel. This means relatively, the state of the shifter is in sync
	// with the pixels being drawn for that 8 pixel section of the scanline.
	auto UpdateShifters = [&]() {
		if (mask.renderBackground) {
			// Shifting background tile pattern row
			bgShifterPatternLow <<= 1;
			bgShifterPatternHigh <<= 1;

			// Shifting palette attributes by 1
			bgShifterAttributeLow <<= 1;
			bgShifterAttributeHigh <<= 1;
		}

		if (mask.renderSprites && cycle >= 1 && cycle < 258) {
			for (int i = 0; i < spriteCount; ++i) {
				if (spriteScanline[i].x > 0) {
					spriteScanline[i].x--;
				}
				else {
					spriteShifterPatternLow[i] <<= 1;
					spriteShifterPatternHigh[i] <<= 1;
				}
			}
		}
	};

	// All but 1 of the secanlines is visible to the user. The pre-render scanline
	// at -1, is used to configure the "shifters" for the first visible scanline, 0.
	if (scanline >= -1 && scanline < 240) {
		// Background Rendering ======================================================

		if (scanline == 0 && cycle == 0 && bOddFrame && (mask.renderBackground || mask.renderSprites)) {
			// "Odd Frame" cycle skip
			cycle = 1;
		}

		if (scanline == -1 && cycle == 1) {
			status.verticalBlank = 0; // Effectively start of new frame, so clear vertical blank flag
			status.spriteOverflow = 0; // Clear sprite overflow flag
			status.spriteZeroHit = 0; // Clear the sprite zero hit flag

			// Clear Shifters
			for (int i = 0; i < 8; ++i) {
				spriteShifterPatternLow[i] = 0;
				spriteShifterPatternHigh[i] = 0;
			}
		}

		if ((cycle >= 2 && cycle < 258) || (cycle >= 321 && cycle < 338)) {
			UpdateShifters();

			// In these cycles we are collecting and working with visible data
			// The "shifters" have been preloaded by the end of the previous
			// scanline with the data for the start of this scanline. Once we
			// leave the visible region, we go dormant until the shifters are
			// preloaded for the next scanline.

			// Fortunately, for background rendering, we go through a fairly
			// repeatable sequence of events, every 2 clock cycles.
			switch ((cycle - 1) % 8) {
			case 0:
				// Load the current background tile pattern and attributes into the "shifter"
				LoadBackgroundShifters();

				// Fetch the next background tile ID
				// "(vRamAddress.reg & 0x0FFF)" : Mask to 12 bits that are relevant
				// "| 0x2000"                 : Offset into nametable space on PPU address bus
				bgNextTileId = PpuRead(0x2000 | (vRamAddress.reg & 0x0FFF));

				// Explanation:
				// The bottom 12 bits of the loopy register provide an index into
				// the 4 nametables, regardless of nametable mirroring configuration.
				// nameTableY(1) nameTableX(1) coarseY(5) coarseX(5)
				//
				// Consider a single nametable is a 32x32 array, and we have four of them
				//   0                1
				// 0 +----------------+----------------+
				//   |                |                |
				//   |                |                |
				//   |    (32x32)     |    (32x32)     |
				//   |                |                |
				//   |                |                |
				// 1 +----------------+----------------+
				//   |                |                |
				//   |                |                |
				//   |    (32x32)     |    (32x32)     |
				//   |                |                |
				//   |                |                |
				//   +----------------+----------------+
				//
				// This means there are 4096 potential locations in this array, which 
				// just so happens to be 2^12!
				break;

			case 2:
				// Fetch the next background tile attribute. OK, so this one is a bit
				// more involved :P

				// Recall that each nametable has two rows of cells that are not tile 
				// information, instead they represent the attribute information that
				// indicates which palettes are applied to which area on the screen.
				// Importantly (and frustratingly) there is not a 1 to 1 correspondance
				// between background tile and palette. Two rows of tile data holds
				// 64 attributes. Therfore we can assume that the attributes affect
				// 8x8 zones on the screen for that nametable. Given a working resolution
				// of 256x240, we can further assume that each zone is 32x32 pixels
				// in screen space, or 4x4 tiles. Four system palettes are allocated
				// to background rendering, so a palette can be specified using just
				// 2 bits. The attribute byte therefore can specify 4 distinct palettes.
				// Therefore we can even further assume that a single palette is
				// applied to a 2x2 tile combination of the 4x4 tile zone. The very fact
				// that background tiles "share" a palette locally is the reason why
				// in some games you see distortion in the colours at screen edges.

				// As before when choosing the tile ID, we can use the bottom 12 bits of
				// the loopy register, but we need to make the implementation "coarser"
				// because instead of a specific tile, we want the attribute byte for a 
				// group of 4x4 tiles, or in other words, we divide our 32x32 address
				// by 4 to give us an equivalent 8x8 address, and we offset this address
				// into the attribute section of the target nametable.

				// Reconstruct the 12 bit loopy address into an offset into the
				// attribute memory

				// "(vRamAddress.coarseX >> 2)"        : integer divide coarse x by 4, 
				//                                      from 5 bits to 3 bits
				// "((vRamAddress.coarseY >> 2) << 3)" : integer divide coarse y by 4, 
				//                                      from 5 bits to 3 bits,
				//                                      shift to make room for coarse x

				// Result so far: YX00 00yy yxxx

				// All attribute memory begins at 0x03C0 within a nametable, so OR with
				// result to select target nametable, and attribute byte offset. Finally
				// OR with 0x2000 to offset into nametable address space on PPU bus.				
				bgNextTileAttribute = PpuRead(0x23C0 | (vRamAddress.nameTableY << 11)
					| (vRamAddress.nameTableX << 10)
					| ((vRamAddress.coarseY >> 2) << 3)
					| (vRamAddress.coarseX >> 2));

				// Right we've read the correct attribute byte for a specified address,
				// but the byte itself is broken down further into the 2x2 tile groups
				// in the 4x4 attribute zone.

				// The attribute byte is assembled thus: BR(76) BL(54) TR(32) TL(10)
				//
				// +----+----+			    +----+----+
				// | TL | TR |			    | ID | ID |
				// +----+----+ where TL =   +----+----+
				// | BL | BR |			    | ID | ID |
				// +----+----+			    +----+----+
				//
				// Since we know we can access a tile directly from the 12 bit address, we
				// can analyse the bottom bits of the coarse coordinates to provide us with
				// the correct offset into the 8-bit word, to yield the 2 bits we are
				// actually interested in which specifies the palette for the 2x2 group of
				// tiles. We know if "coarse y % 4" < 2 we are in the top half else bottom half.
				// Likewise if "coarse x % 4" < 2 we are in the left half else right half.
				// Ultimately we want the bottom two bits of our attribute word to be the
				// palette selected. So shift as required...				
				if (vRamAddress.coarseY & 0x02) bgNextTileAttribute >>= 4;
				if (vRamAddress.coarseX & 0x02) bgNextTileAttribute >>= 2;
				bgNextTileAttribute &= 0x03;
				break;

				// Compared to the last two, the next two are the easy ones... :P
			case 4:
				// Fetch the next background tile LSB bit plane from the pattern memory
				// The Tile ID has been read from the nametable. We will use this id to 
				// index into the pattern memory to find the correct sprite (assuming
				// the sprites lie on 8x8 pixel boundaries in that memory, which they do
				// even though 8x16 sprites exist, as background tiles are always 8x8).
				//
				// Since the sprites are effectively 1 bit deep, but 8 pixels wide, we 
				// can represent a whole sprite row as a single byte, so offsetting
				// into the pattern memory is easy. In total there is 8KB so we need a 
				// 13 bit address.

				// "(control.patternBackground << 12)"  : the pattern memory selector 
				//                                         from control register, either 0K
				//                                         or 4K offset
				// "((uint16_t)bgNextTileId << 4)"    : the tile id multiplied by 16, as
				//                                         2 lots of 8 rows of 8 bit pixels
				// "(vRamAddress.fineY)"                  : Offset into which row based on
				//                                         vertical scroll offset
				// "+ 0"                                 : Mental clarity for plane offset
				// Note: No PPU address bus offset required as it starts at 0x0000
				bgNextTileLeastSignificantBit = PpuRead((control.patternBackground << 12)
					+ ((uint16_t)bgNextTileId << 4)
					+ (vRamAddress.fineY) + 0);
				break;

			case 6:
				// Fetch the next background tile MSB bit plane from the pattern memory
				// This is the same as above, but has a +8 offset to select the next bit plane
				bgNextTileMostSignificantBit = PpuRead((control.patternBackground << 12)
					+ ((uint16_t)bgNextTileId << 4)
					+ (vRamAddress.fineY) + 8);
				break;

			case 7:
				// Increment the background tile "pointer" to the next tile horizontally
				// in the nametable memory. Note this may cross nametable boundaries which
				// is a little complex, but essential to implement scrolling
				IncrementScrollX();
				break;
			}
		}

		// End of a visible scanline, so increment downwards...
		if (cycle == 256) {
			IncrementScrollY();
		}

		//...and reset the x position
		if (cycle == 257) {
			LoadBackgroundShifters();
			TransferAddressX();
		}

		// Superfluous reads of tile id at end of scanline
		if (cycle == 338 || cycle == 340) {
			bgNextTileId = PpuRead(0x2000 | (vRamAddress.reg & 0x0FFF));
		}

		if (scanline == -1 && cycle >= 280 && cycle < 305) {
			// End of vertical blank period so reset the Y address ready for rendering
			TransferAddressY();
		}

		// Foreground Rendering ========================================================
		// I'm gonna cheat a bit here, which may reduce compatibility, but greatly
		// simplifies delivering an intuitive understanding of what exactly is going
		// on. The PPU loads sprite information successively during the region that
		// background tiles are not being drawn. Instead, I'm going to perform
		// all sprite evaluation in one hit. THE NES DOES NOT DO IT LIKE THIS! This makes
		// it easier to see the process of sprite evaluation.
		if (cycle == 257 && scanline >= 0) {
			// We've reached the end of a visible scanline. It is now time to determine
			// which sprites are visible on the next scanline, and preload this info
			// into buffers that we can work with while the scanline scans the row.

			// Firstly, clear out the sprite memory. This memory is used to store the
			// sprites to be rendered. It is not the oam.
			std::memset(spriteScanline, 0xFF, 8 * sizeof(ObjectAttributeEntry));

			// The NES supports a maximum number of sprites per scanline. Nominally
			// this is 8 or fewer sprites. This is why in some games you see sprites
			// flicker or disappear when the scene gets busy.
			spriteCount = 0;

			// Secondly, clear out any residual information in sprite pattern shifters
			for (uint8_t i = 0; i < 8; i++) {
				spriteShifterPatternLow[i] = 0;
				spriteShifterPatternHigh[i] = 0;
			}

			// Thirdly, Evaluate which sprites are visible in the next scanline. We need
			// to iterate through the oam until we have found 8 sprites that have Y-positions
			// and heights that are within vertical range of the next scanline. Once we have
			// found 8 or exhausted the oam we stop. Now, notice I count to 9 sprites. This
			// is so I can set the sprite overflow flag in the event of there being > 8 sprites.
			uint8_t oamEntry = 0;

			// New set of sprites. Sprite zero may not exist in the new set, so clear this
			// flag.
			bSpriteZeroHitPossible = false;

			while (oamEntry < 64 && spriteCount < 9) {
				// Note the conversion to signed numbers here
				int16_t difference = ((int16_t)scanline - (int16_t)oam[oamEntry].y);

				// If the difference is positive then the scanline is at least at the
				// same height as the sprite, so check if it resides in the sprite vertically
				// depending on the current "sprite height mode"
				// FLAGGED
				if (difference >= 0 && difference < (control.spriteSize ? 16 : 8) && spriteCount < 8) {
					// Sprite is visible, so copy the attribute entry over to our
					// scanline sprite cache. Ive added < 8 here to guard the array
					// being written to.
					if (spriteCount < 8) {
						// Is this sprite sprite zero?
						if (oamEntry == 0) {
							// It is, so its possible it may trigger a
							// sprite zero hit when drawn
							bSpriteZeroHitPossible = true;
						}

						memcpy(&spriteScanline[spriteCount], &oam[oamEntry], sizeof(ObjectAttributeEntry));
					}

                                        spriteCount++;
				}

				oamEntry++;
			} // End of sprite evaluation for next scanline

			// Set sprite overflow flag
			status.spriteOverflow = (spriteCount >= 8);

			// Now we have an array of the 8 visible sprites for the next scanline. By 
			// the nature of this search, they are also ranked in priority, because
			// those lower down in the oam have the higher priority.

			// We also guarantee that "Sprite Zero" will exist in spriteScanline[0] if
			// it is evaluated to be visible. 
		}

		if (cycle == 340) {
			// Now we're at the very end of the scanline, I'm going to prepare the 
			// sprite shifters with the 8 or less selected sprites.
			for (uint8_t i = 0; i < spriteCount; ++i) {
				// We need to extract the 8-bit row patterns of the sprite with the
				// correct vertical offset. The "Sprite Mode" also affects this as
				// the sprites may be 8 or 16 rows high. Additionally, the sprite
				// can be flipped both vertically and horizontally. So there's a lot
				// going on here :P
				uint8_t spritePatternByteLow;
				uint8_t spritePatternByteHigh;

				uint16_t spritePatternAddressLow;
				uint16_t spritePatternAddressHigh;

				// Determine the memory addresses that contain the byte of pattern data. We
				// only need the lo pattern address, because the hi pattern address is always
				// offset by 8 from the lo address.
				if (!control.spriteSize) {
					// 8x8 Sprite Mode - The control register determines the pattern table.
					if (!(spriteScanline[i].attribute & 0x80)) {
						// Sprite is not flipped vertically, i.e. normal
						spritePatternAddressLow =
							(control.patternSprite << 12)  // Which Pattern Table? 0KB or 4KB offset
							| (spriteScanline[i].id << 4)  // Which Cell? Tile ID * 16 (16 bytes per tile)
							| (scanline - spriteScanline[i].y); // Which Row in cell? (0->7)
					}
					else {
						// Sprite is flipped vertically, i.e. upside down
						spritePatternAddressLow =
							(control.patternSprite << 12)  // Which Pattern Table? 0KB or 4KB offset
							| (spriteScanline[i].id << 4)  // Which Cell? Tile ID * 16 (16 bytes per tile)
							| (7 - (scanline - spriteScanline[i].y)); // Which Row in cell? (7->0)
					}
				}
				else {
					// 8x16 Sprite Mode - The sprite attribute determines the pattern table
					if (!(spriteScanline[i].attribute & 0x80)) {
						// Sprite is not flipped vertically, i.e. normal
						if (scanline - spriteScanline[i].y < 8) {
							// Reading Top Half Tile
							spritePatternAddressLow =
								((spriteScanline[i].id & 0x01) << 12)  // Which Pattern Table? 0KB or 4KB offset
								| ((spriteScanline[i].id & 0xFE) << 4)  // Which Cell? Tile ID * 16 (16 bytes per tile)
								| ((scanline - spriteScanline[i].y) & 0x07); // Which Row in cell? (0->7)
						}
						else {
							// Reading Bottom Half Tile
							spritePatternAddressLow =
								((spriteScanline[i].id & 0x01) << 12)  // Which Pattern Table? 0KB or 4KB offset
								| (((spriteScanline[i].id & 0xFE) + 1) << 4)  // Which Cell? Tile ID * 16 (16 bytes per tile)
								| ((scanline - spriteScanline[i].y) & 0x07); // Which Row in cell? (0->7)
						}
					}
					else {
						// Sprite is flipped vertically, i.e. upside down
						if (scanline - spriteScanline[i].y < 8) {
							// Reading Top Half Tile
							spritePatternAddressLow =
								((spriteScanline[i].id & 0x01) << 12)    // Which Pattern Table? 0KB or 4KB offset
								| (((spriteScanline[i].id & 0xFE) + 1) << 4)    // Which Cell? Tile ID * 16 (16 bytes per tile)
								| (7 - (scanline - spriteScanline[i].y) & 0x07); // Which Row in cell? (0->7)
						}
						else {
							// Reading Bottom Half Tile
							spritePatternAddressLow =
								((spriteScanline[i].id & 0x01) << 12)    // Which Pattern Table? 0KB or 4KB offset
								| ((spriteScanline[i].id & 0xFE) << 4)    // Which Cell? Tile ID * 16 (16 bytes per tile)
								| (7 - (scanline - spriteScanline[i].y) & 0x07); // Which Row in cell? (0->7)
						}
					}
				}

				// Phew... XD I'm absolutely certain you can use some fantastic bit 
				// manipulation to reduce all of that to a few one liners, but in this
				// form it's easy to see the processes required for the different
				// sizes and vertical orientations

				// Hi bit plane equivalent is always offset by 8 bytes from lo bit plane
				spritePatternAddressHigh = spritePatternAddressLow + 8;

				// Now we have the address of the sprite patterns, we can read them
				spritePatternByteLow = PpuRead(spritePatternAddressLow);
				spritePatternByteHigh = PpuRead(spritePatternAddressHigh);

				// If the sprite is flipped horizontally, we need to flip the 
				// pattern bytes. 
				if (spriteScanline[i].attribute & 0x40) {
					// This little lambda function "flips" a byte
					// so 0b11100000 becomes 0b00000111. It's very
					// clever, and stolen completely from here:
					// https://stackoverflow.com/a/2602885
					auto flipByte = [](uint8_t b) {
						b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
						b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
						b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
						return b;
					};

					// Flip Patterns Horizontally
					spritePatternByteLow = flipByte(spritePatternByteLow);
					spritePatternByteHigh = flipByte(spritePatternByteHigh);
				}

				// Finally! We can load the pattern into our sprite shift registers
				// ready for rendering on the next scanline
				spriteShifterPatternLow[i] = spritePatternByteLow;
				spriteShifterPatternHigh[i] = spritePatternByteHigh;
			}
		}
	}

	if (scanline == 240) {
		// Post render scanline - Do nothing.
	}

	if (scanline >= 241 && scanline < 261) {
		if (scanline == 241 && cycle == 1) {
			// Effectively end of frame, so set vertical blank flag
			status.verticalBlank = 1;

			// If the control register tells us to emit a NMI when
			// entering vertical blanking period, do it! The CPU
			// will be informed that rendering is complete so it can
			// perform operations with the PPU knowing it wont
			// produce visible artefacts
			if (control.enableNonmaskableInterrupt) {
				bNonmaskableInterrupt = true;
			}
		}
	}

	// Composition - We now have background & foreground pixel information for this cycle

	// Background =============================================================
	uint8_t backgroundPixel = 0x00;   // The 2-bit pixel to be rendered
	uint8_t backgroundPalette = 0x00; // The 3-bit index of the palette the pixel indexes

	// We only render backgrounds if the PPU is enabled to do so. Note if 
	// background rendering is disabled, the pixel and palette combine
	// to form 0x00. This will fall through the colour tables to yield
	// the current background colour in effect
	if (mask.renderBackground) {
		if (mask.renderBackgroundLeft || (cycle >= 9)) {
			// Handle Pixel Selection by selecting the relevant bit
			// depending upon fine x scolling. This has the effect of
			// offsetting ALL background rendering by a set number
			// of pixels, permitting smooth scrolling
			uint16_t bitMultiplexer = 0x8000 >> fineX;

			// Select Plane pixels by extracting from the shifter 
			// at the required location. 
			uint8_t pixelPlane0 = (bgShifterPatternLow & bitMultiplexer) > 0;
			uint8_t pixelPlane1 = (bgShifterPatternHigh & bitMultiplexer) > 0;

			// Combine to form pixel index
			backgroundPixel = (pixelPlane1 << 1) | pixelPlane0;

			// Get palette
			uint8_t pixelPalette0 = (bgShifterAttributeLow & bitMultiplexer) > 0;
			uint8_t pixelPalette1 = (bgShifterAttributeHigh & bitMultiplexer) > 0;
			backgroundPalette = (pixelPalette1 << 1) | pixelPalette0;
		}
	}

	// Foreground =============================================================
	uint8_t foregroundPixel = 0x00;    // The 2-bit pixel to be rendered
	uint8_t foregroundPalette = 0x00;  // The 3-bit index of the palette the pixel indexes
	uint8_t foregroundPriority = 0x00; // A bit of the sprite attribute indicates if its more important than the background

	if (mask.renderSprites) {
		// Iterate through all sprites for this scanline. This is to maintain
		// sprite priority. As soon as we find a non transparent pixel of
		// a sprite we can abort
		if (mask.renderSpritesLeft || (cycle >= 9)) {
			bSpriteZeroBeingRendered = false;

			for (uint8_t i = 0; i < spriteCount; i++) {
				// Scanline cycle has "collided" with sprite, shifters taking over
				if (spriteScanline[i].x == 0) {
					// Note Fine X scrolling does not apply to sprites, the game
					// should maintain their relationship with the background. So
					// we'll just use the MSB of the shifter

					// Determine the pixel value...
					uint8_t foregroundPixelLow = (spriteShifterPatternLow[i] & 0x80) > 0;
					uint8_t foregroundPixelHigh = (spriteShifterPatternHigh[i] & 0x80) > 0;
					foregroundPixel = (foregroundPixelHigh << 1) | foregroundPixelLow;

					// Extract the palette from the bottom two bits. Recall
					// that foreground palettes are the latter 4 in the 
					// palette memory.
					foregroundPalette = (spriteScanline[i].attribute & 0x03) + 0x04;
					foregroundPriority = (spriteScanline[i].attribute & 0x20) == 0;

					// If pixel is not transparent, we render it, and dont
					// bother checking the rest because the earlier sprites
					// in the list are higher priority
					if (foregroundPixel != 0) {
						if (i == 0) { // Is this sprite zero?
							bSpriteZeroBeingRendered = true;
						}

						break;
					}
				}
			}
		}
	}

	// Now we have a background pixel and a foreground pixel. They need
	// to be combined. It is possible for sprites to go behind background
	// tiles that are not "transparent", yet another neat trick of the PPU
	// that adds complexity for us poor emulator developers...

	uint8_t pixel = 0x00;   // The Final Pixel...
	uint8_t palette = 0x00; // The Final Palette...

	if (backgroundPixel == 0 && foregroundPixel == 0) {
		// The background pixel is transparent
		// The foreground pixel is transparent
		// No winner, draw "background" color
		pixel = 0x00;
		palette = 0x00;
	}
	else if (backgroundPixel == 0 && foregroundPixel > 0) {
		// The background pixel is transparent
		// The foreground pixel is visible
		// Foreground wins.
		pixel = foregroundPixel;
		palette = foregroundPalette;
	}
	else if (backgroundPixel > 0 && foregroundPixel == 0) {
		// The background pixel is visible
		// The foreground pixel is transparent
		// Background wins!
		pixel = backgroundPixel;
		palette = backgroundPalette;
	}
	else if (backgroundPixel > 0 && foregroundPixel > 0) {
		// The background pixel is visible
		// The foreground pixel is visible
		// Hmmm...
		if (foregroundPriority) {
			// Foreground cheats its way to victory.
			pixel = foregroundPixel;
			palette = foregroundPalette;
		}
		else {
			// Background is considered more important.
			pixel = backgroundPixel;
			palette = backgroundPalette;
		}

		// Sprite Zero Hit detection
		if (bSpriteZeroHitPossible && bSpriteZeroBeingRendered) {
			// Sprite zero is a collision between foreground and background
			// so they must both be enabled
			if (mask.renderBackground & mask.renderSprites) {
				// The left edge of the screen has specific switches to control
				// its appearance. This is used to smooth inconsistencies when
				// scrolling (since sprites x coord must be >= 0)
				if (!(mask.renderBackgroundLeft | mask.renderSpritesLeft)) {
					if (cycle >= 9 && cycle < 258) {
						status.spriteZeroHit = 1;
					}
				}
				else {
					if (cycle >= 1 && cycle < 258) {
						status.spriteZeroHit = 1;
					}
				}
			}
		}
	}

	// Now we have a final pixel colour, and a palette for this cycle
	// of the current scanline. Let's at long last, draw that ^&%*er :P
	spriteScreen.SetPixel(cycle - 1, scanline, GetColorFromPaletteRam(palette, pixel));

	// Advance renderer - it never stops, it's relentless
	cycle++;
        if (mask.renderBackground || mask.renderSprites) {
            if (cycle == 260 && scanline < 240) {
                cartridge->GetMapper()->Scanline();
            }
        }

	if (cycle >= 341) {
		cycle = 0;
		scanline++;

		if (scanline >= 261) {
			scanline = -1;
			frameComplete = true;
                        bOddFrame = !bOddFrame;
		}
	}
}

void Olc2C02::Reset() {
	fineX = 0x00;
	addressLatch = 0x00;
	ppuDataBuffer = 0x00;
	scanline = 0;
	cycle = 0;
	bgNextTileId = 0x00;
	bgNextTileAttribute = 0x00;
	bgNextTileLeastSignificantBit = 0x00;
	bgNextTileMostSignificantBit = 0x00;
	bgShifterPatternLow = 0x0000;
	bgShifterPatternHigh = 0x0000;
	bgShifterAttributeLow = 0x0000;
	bgShifterAttributeHigh = 0x0000;
	status.reg = 0x00;
	mask.reg = 0x00;
	control.reg = 0x00;
	vRamAddress.reg = 0x0000;
	tRamAddress.reg = 0x0000;
    bScanlineTrigger = false;
    bOddFrame = false;
}