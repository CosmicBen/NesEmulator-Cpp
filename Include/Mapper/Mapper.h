/*
	olc::NES - Mapper Base Class (Abstract)
*/

#pragma once

#include <cstdint>

enum MIRROR {
	HARDWARE,
	HORIZONTAL,
	VERTICAL,
	ONESCREEN_LOW,
	ONESCREEN_HIGH,
};

class Mapper
{
public:
	Mapper(uint8_t programBanks, uint8_t characterBanks);
	~Mapper() { }

public:
	// Transform CPU bus address into program ROM offset
	virtual bool CpuMapRead(uint16_t address, uint32_t& mappedAddress, uint8_t& data) = 0;
	virtual bool CpuMapWrite(uint16_t address, uint32_t& mappedAddress, uint8_t data = 0x00) = 0;

	// IRQ Interface
	virtual bool IrqState() { return false; }
	virtual void IrqClear() { }

	// Get Mirror mode if mapper is in control
	virtual MIRROR Mirror() { return MIRROR::HARDWARE; }

	// Transform PPU bus address into character ROM offset
	virtual bool PpuMapRead(uint16_t address, uint32_t& mappedAddress) = 0;
	virtual bool PpuMapWrite(uint16_t address, uint32_t& mappedAddress) = 0;

	// Reset mapper to a known state
	virtual void Reset() { };

	// Scanline Counting
	virtual void Scanline() { }

protected:
	// These are stored locally as many of the mappers require this information
	uint8_t characterBanks = 0;
	uint8_t programBanks = 0;
};