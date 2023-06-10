/*
	olc::NES - Mapper Base Class (Abstract)
*/

#include "Mapper\Mapper.h"

Mapper::Mapper(uint8_t programBanks, uint8_t characterBanks) {
	this->programBanks = programBanks;
	this->characterBanks = characterBanks;

	Reset();
}