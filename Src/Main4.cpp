//#include <iostream>
//#include <cstdint>
//#include <memory>
//#include "Hardware\Bus.h"
//
//#define OLC_PGE_APPLICATION
//#include "Engine\olcPixelGameEngine.h"
//
//class Demo_6502 : public olc::PixelGameEngine
//{
//public:
//	Demo_6502() { sAppName = "6502 Demonstration"; }
//
//private:
//	// The NES
//	Bus nes;
//	std::shared_ptr<Cartridge> cartridge;
//	bool bEmulationRun = false;
//	float residualTime = 0.0f;
//
//	uint8_t selectedPalette = 0x00;
//
//private:
//	// Support Utilities
//	std::map<uint16_t, std::string> mapAsm;
//
//	std::string Hex(uint32_t n, uint8_t d) {
//		std::string s(d, '0');
//		for (int i = d - 1; i >= 0; i--, n >>= 4) {
//			s[i] = "0123456789ABCDEF"[n & 0xF];
//		}
//		return s;
//	};
//
//	void DrawRam(int x, int y, uint16_t nAddr, int nRows, int nColumns) {
//		int nRamX = x;
//		int nRamY = y;
//
//		for (int row = 0; row < nRows; row++) {
//			std::string sOffset = "$" + Hex(nAddr, 4) + ":";
//			for (int col = 0; col < nColumns; col++) {
//				sOffset += " " + Hex(nes.CpuRead(nAddr++, true), 2);
//			}
//			DrawString(nRamX, nRamY, sOffset);
//			nRamY += 10;
//		}
//	}
//
//	void DrawCpu(int x, int y) {
//		std::string status = "STATUS: ";
//		DrawString(x, y, "STATUS:", olc::WHITE);
//		DrawString(x + 64, y, "N", nes.cpu.status & (uint8_t)Olc6502::Flags6502::N ? olc::GREEN : olc::RED);
//		DrawString(x + 80, y, "V", nes.cpu.status & (uint8_t)Olc6502::Flags6502::V ? olc::GREEN : olc::RED);
//		DrawString(x + 96, y, "-", nes.cpu.status & (uint8_t)Olc6502::Flags6502::U ? olc::GREEN : olc::RED);
//		DrawString(x + 112, y, "B", nes.cpu.status & (uint8_t)Olc6502::Flags6502::B ? olc::GREEN : olc::RED);
//		DrawString(x + 128, y, "D", nes.cpu.status & (uint8_t)Olc6502::Flags6502::D ? olc::GREEN : olc::RED);
//		DrawString(x + 144, y, "I", nes.cpu.status & (uint8_t)Olc6502::Flags6502::I ? olc::GREEN : olc::RED);
//		DrawString(x + 160, y, "Z", nes.cpu.status & (uint8_t)Olc6502::Flags6502::Z ? olc::GREEN : olc::RED);
//		DrawString(x + 178, y, "C", nes.cpu.status & (uint8_t)Olc6502::Flags6502::C ? olc::GREEN : olc::RED);
//		DrawString(x, y + 10, "PC: $" + Hex(nes.cpu.pc, 4));
//		DrawString(x, y + 20, "A: $" + Hex(nes.cpu.a, 2) + "  [" + std::to_string(nes.cpu.a) + "]");
//		DrawString(x, y + 30, "X: $" + Hex(nes.cpu.x, 2) + "  [" + std::to_string(nes.cpu.x) + "]");
//		DrawString(x, y + 40, "Y: $" + Hex(nes.cpu.y, 2) + "  [" + std::to_string(nes.cpu.y) + "]");
//		DrawString(x, y + 50, "Stack P: $" + Hex(nes.cpu.stkp, 4));
//	}
//
//	void DrawCode(int x, int y, int nLines) {
//		auto it_a = mapAsm.find(nes.cpu.pc);
//		int nLineY = (nLines >> 1) * 10 + y;
//		if (it_a != mapAsm.end()) {
//			DrawString(x, nLineY, (*it_a).second, olc::CYAN);
//			while (nLineY < (nLines * 10) + y) {
//				nLineY += 10;
//				if (++it_a != mapAsm.end()) {
//					DrawString(x, nLineY, (*it_a).second);
//				}
//			}
//		}
//
//		it_a = mapAsm.find(nes.cpu.pc);
//		nLineY = (nLines >> 1) * 10 + y;
//
//		if (it_a != mapAsm.end()) {
//			while (nLineY > y) {
//				nLineY -= 10;
//				if (--it_a != mapAsm.end()) {
//					DrawString(x, nLineY, (*it_a).second);
//				}
//			}
//		}
//	}
//
//public:
//	bool OnUserCreate() {
//		// Load the cartridge
//		//cartridge = std::make_shared<Cartridge>("nestest.nes");
//		cartridge = std::make_shared<Cartridge>("DonkeyKong.nes");
//		//cartridge = std::make_shared<Cartridge>("DuckTales.nes");
//
//		if (!cartridge->ImageValid()) { return false; }
//
//		// Insert into NES
//		nes.InsertCartridge(cartridge);
//
//		// Extract dissassembly
//		mapAsm = nes.cpu.Disassemble(0x0000, 0xFFFF);
//
//		// Reset NES
//		nes.Reset();
//
//		return true;
//	}
//
//	bool OnUserUpdate(float fElapsedTime) override {
//		Clear(olc::DARK_BLUE);
//
//		// Sneaky peek of controller input in next video! ;P
//		nes.controller[0] = 0x00;
//		nes.controller[0] |= GetKey(olc::Key::X).bHeld ? 0x80 : 0x00;
//		nes.controller[0] |= GetKey(olc::Key::Z).bHeld ? 0x40 : 0x00;
//		nes.controller[0] |= GetKey(olc::Key::A).bHeld ? 0x20 : 0x00;
//		nes.controller[0] |= GetKey(olc::Key::S).bHeld ? 0x10 : 0x00;
//		nes.controller[0] |= GetKey(olc::Key::UP).bHeld ? 0x08 : 0x00;
//		nes.controller[0] |= GetKey(olc::Key::DOWN).bHeld ? 0x04 : 0x00;
//		nes.controller[0] |= GetKey(olc::Key::LEFT).bHeld ? 0x02 : 0x00;
//		nes.controller[0] |= GetKey(olc::Key::RIGHT).bHeld ? 0x01 : 0x00;
//
//		if (GetKey(olc::Key::SPACE).bPressed) { bEmulationRun = !bEmulationRun; }
//		if (GetKey(olc::Key::R).bPressed) { nes.Reset(); }
//		if (GetKey(olc::Key::P).bPressed) { (++selectedPalette) &= 0x07; }
//
//		if (bEmulationRun) {
//			if (residualTime > 0.0f) {
//				residualTime -= fElapsedTime;
//			}
//			else {
//				residualTime += (1.0f / 60.0f) - fElapsedTime;
//				do { nes.Clock(); } while (!nes.ppu.frameComplete);
//				nes.ppu.frameComplete = false;
//			}
//		}
//		else {
//			// Emulate code step-by-step
//			if (GetKey(olc::Key::C).bPressed) {
//				// Clock enough times to execute a whole CPU instruction
//				do { nes.Clock(); } while (!nes.cpu.Complete());
//				// CPU clock runs slower than system clock, so it may be
//				// complete for additional system clock cycles. Drain
//				// those out
//				do { nes.Clock(); } while (nes.cpu.Complete());
//			}
//
//			// Emulate one whole frame
//			if (GetKey(olc::Key::F).bPressed) {
//				// Clock enough times to draw a single frame
//				do { nes.Clock(); } while (!nes.ppu.frameComplete);
//				// Use residual clock cycles to complete current instruction
//				do { nes.Clock(); } while (!nes.cpu.Complete());
//				// Reset frame completion flag
//				nes.ppu.frameComplete = false;
//			}
//		}
//
//		DrawCpu(516, 2);
//		DrawCode(516, 72, 26);
//
//		// Draw Palettes & Pattern Tables ==============================================
//		const int swatchSize = 6;
//		for (int p = 0; p < 8; p++) { // For each palette
//			for (int s = 0; s < 4; s++) { // For each index
//				FillRect(516 + p * (swatchSize * 5) + s * swatchSize, 340,
//					swatchSize, swatchSize, nes.ppu.GetColorFromPaletteRam(p, s));
//			}
//		}
//
//		// Draw selection reticule around selected palette
//		DrawRect(516 + selectedPalette * (swatchSize * 5) - 1, 339, (swatchSize * 4), swatchSize, olc::WHITE);
//
//		// Generate Pattern Tables
//		DrawSprite(516, 348, &nes.ppu.GetPatternTable(0, selectedPalette));
//		DrawSprite(648, 348, &nes.ppu.GetPatternTable(1, selectedPalette));
//
//		// Draw rendered output ========================================================
//		DrawSprite(0, 0, &nes.ppu.GetScreen(), 2);
//
//		//olc::Sprite& patternTable = nes.ppu.GetPatternTable(0, selectedPalette);
//		//for (uint8_t y = 0; y < 30; y++) {
//		//	for (uint8_t x = 0; x < 32; x++) {
//		//		//DrawString(x * 16, y * 16, Hex((uint32_t)nes.ppu.nameTable[0][y * 32 + x], 2));
//		//		uint8_t id = (uint32_t)nes.ppu.nameTable[0][y * 32 + x];
//		//		DrawPartialSprite(x * 16, y * 16, &patternTable,
//		//			(id & 0x0F) << 3, ((id >> 4) & 0x0F) << 3, 8, 8, 2);
//		//	}
//		//}
//
//		return true;
//	}
//};
//
//
//int main()
//{
//	Demo_6502 demo;
//	if (demo.Construct(780, 480, 1, 1)) {
//		demo.Start();
//	}
//
//	return 0;
//}