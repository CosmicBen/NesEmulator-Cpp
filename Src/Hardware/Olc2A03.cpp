#include "Hardware\Olc2A03.h"

/*
	olc::NES - APU
*/

uint8_t Olc2A03::lengthTable[] = {  10, 254, 20,  2, 40,  4, 80,  6,
								   160,   8, 60, 10, 14, 12, 26, 14,
								    12,  16, 24, 18, 48, 20, 96, 22,
								   192,  24, 72, 26, 16, 28, 32, 30, };

Olc2A03::Olc2A03() {
	noiseSequencer.sequence = 0xDBDB;
}

Olc2A03::~Olc2A03() { }

void Olc2A03::CpuWrite(uint16_t address, uint8_t data) {
	switch (address) {
	case 0x4000:
		switch ((data & 0xC0) >> 6) {
		case 0x00: pulse1Sequencer.newSequence = 0b01000000; pulse1Oscellator.dutyCycle = 0.125; break;
		case 0x01: pulse1Sequencer.newSequence = 0b01100000; pulse1Oscellator.dutyCycle = 0.250; break;
		case 0x02: pulse1Sequencer.newSequence = 0b01111000; pulse1Oscellator.dutyCycle = 0.500; break;
		case 0x03: pulse1Sequencer.newSequence = 0b10011111; pulse1Oscellator.dutyCycle = 0.750; break;
		}

		pulse1Sequencer.sequence = pulse1Sequencer.newSequence;
		bPulse1Halt = (data & 0x20);
		pulse1Envelope.volume = (data & 0x0F);
		pulse1Envelope.bDisable = (data & 0x10);
		break;

	case 0x4001:
		pulse1Sweep.bEnabled = data & 0x80;
		pulse1Sweep.period = (data & 0x70) >> 4;
		pulse1Sweep.bDown = data & 0x08;
		pulse1Sweep.shift = data & 0x07;
		pulse1Sweep.bReload = true;
		break;

	case 0x4002:
		pulse1Sequencer.reload = (pulse1Sequencer.reload & 0xFF00) | data;
		break;

	case 0x4003:
		pulse1Sequencer.reload = (uint16_t)((data & 0x07)) << 8 | (pulse1Sequencer.reload & 0x00FF);
		pulse1Sequencer.timer = pulse1Sequencer.reload;
		pulse1Sequencer.sequence = pulse1Sequencer.newSequence;
		pulse1LengthCounter.counter = lengthTable[(data & 0xF8) >> 3];
		pulse1Envelope.bStart = true;
		break;

	case 0x4004:
		switch ((data & 0xC0) >> 6) {
		case 0x00: pulse2Sequencer.newSequence = 0b01000000; pulse2Oscellator.dutyCycle = 0.125; break;
		case 0x01: pulse2Sequencer.newSequence = 0b01100000; pulse2Oscellator.dutyCycle = 0.250; break;
		case 0x02: pulse2Sequencer.newSequence = 0b01111000; pulse2Oscellator.dutyCycle = 0.500; break;
		case 0x03: pulse2Sequencer.newSequence = 0b10011111; pulse2Oscellator.dutyCycle = 0.750; break;
		}

		pulse2Sequencer.sequence = pulse2Sequencer.newSequence;
		bPulse2Halt = (data & 0x20);
		pulse2Envelope.volume = (data & 0x0F);
		pulse2Envelope.bDisable = (data & 0x10);
		break;

	case 0x4005:
		pulse2Sweep.bEnabled = data & 0x80;
		pulse2Sweep.period = (data & 0x70) >> 4;
		pulse2Sweep.bDown = data & 0x08;
		pulse2Sweep.shift = data & 0x07;
		pulse2Sweep.bReload = true;
		break;

	case 0x4006:
		pulse2Sequencer.reload = (pulse2Sequencer.reload & 0xFF00) | data;
		break;

	case 0x4007:
		pulse2Sequencer.reload = (uint16_t)((data & 0x07)) << 8 | (pulse2Sequencer.reload & 0x00FF);
		pulse2Sequencer.timer = pulse2Sequencer.reload;
		pulse2Sequencer.sequence = pulse2Sequencer.newSequence;
		pulse2LengthCounter.counter = lengthTable[(data & 0xF8) >> 3];
		pulse2Envelope.bStart = true;
		break;

	case 0x4008:
		triangleLengthCounter.counter = (data & 0x7F);
		bTriangleHalt = (data & 0x80);
		break;

	case 0x400A:
		triangleSequencer.reload = (triangleSequencer.reload & 0xFF00) | data;
		break;

	case 0x400B:
		triangleSequencer.reload = (uint16_t)((data & 0x07)) << 8 | (triangleSequencer.reload & 0x00FF);
		triangleSequencer.timer = triangleSequencer.reload;
		// TODO: length counter + linear counter
		// triangleLengthCounter.additionalCounter = (data & 0xF8) >> 3;
		break;

	case 0x400C:
		noiseEnvelope.volume = (data & 0x0F);
		noiseEnvelope.bDisable = (data & 0x10);
		bNoiseHalt = (data & 0x20);
		break;

	case 0x400E:
		switch (data & 0x0F) {
		case 0x00: noiseSequencer.reload = 0; break;
		case 0x01: noiseSequencer.reload = 4; break;
		case 0x02: noiseSequencer.reload = 8; break;
		case 0x03: noiseSequencer.reload = 16; break;
		case 0x04: noiseSequencer.reload = 32; break;
		case 0x05: noiseSequencer.reload = 64; break;
		case 0x06: noiseSequencer.reload = 96; break;
		case 0x07: noiseSequencer.reload = 128; break;
		case 0x08: noiseSequencer.reload = 160; break;
		case 0x09: noiseSequencer.reload = 202; break;
		case 0x0A: noiseSequencer.reload = 254; break;
		case 0x0B: noiseSequencer.reload = 380; break;
		case 0x0C: noiseSequencer.reload = 508; break;
		case 0x0D: noiseSequencer.reload = 1016; break;
		case 0x0E: noiseSequencer.reload = 2034; break;
		case 0x0F: noiseSequencer.reload = 4068; break;
		}
		break;

	case 0x4015: // APU STATUS
		bPulse1Enable = data & 0x01;
		bPulse2Enable = data & 0x02;
		bNoiseEnable = data & 0x04;
		bTriangleEnable = data & 0x08;
		break;

	case 0x400F:
		pulse1Envelope.bStart = true;
		pulse2Envelope.bStart = true;
		noiseEnvelope.bStart = true;
		noiseLengthCounter.counter = lengthTable[(data & 0xF8) >> 3];
		break;
	}
}

uint8_t Olc2A03::CpuRead(uint16_t address) {
	uint8_t data = 0x00;

	if (address == 0x4015) {
		// data |= (pulse1LengthCounter.counter > 0) ? 0x01 : 0x00;
		// data |= (pulse2LengthCounter.counter > 0) ? 0x02 : 0x00;
		// data |= (noiseLengthCounter.counter > 0) ? 0x04 : 0x00;
	}

	return data;
}

void Olc2A03::Clock() {
	// Depending on the frame count, we set a flag to tell 
	// us where we are in the sequence. Essentially, changes
	// to notes only occur at these intervals, meaning, in a
	// way, this is responsible for ensuring musical time is
	// maintained.
	bool bQuarterFrameClock = false;
	bool bHalfFrameClock = false;

	globalTime += (0.3333333333 / 1789773);

	if (clockCounter % 6 == 0) {
		frameClockCounter++;

		// 4-Step Sequence Mode
		if (frameClockCounter == 3729) {
			bQuarterFrameClock = true;
		}

		if (frameClockCounter == 7457) {
			bQuarterFrameClock = true;
			bHalfFrameClock = true;
		}

		if (frameClockCounter == 11186) {
			bQuarterFrameClock = true;
		}

		if (frameClockCounter == 14916) {
			bQuarterFrameClock = true;
			bHalfFrameClock = true;
			frameClockCounter = 0;
		}

		// Update functional units

		// Quarter frame "beats" adjust the volume envelope
		if (bQuarterFrameClock) {
			pulse1Envelope.Clock(bPulse1Halt);
			pulse2Envelope.Clock(bPulse2Halt);
			noiseEnvelope.Clock(bNoiseHalt);
		}

		// Half frame "beats" adjust the note length and
		// frequency sweepers
		if (bHalfFrameClock) {
			pulse1LengthCounter.Clock(bPulse1Enable, bPulse1Halt);
			pulse2LengthCounter.Clock(bPulse2Enable, bPulse2Halt);
			noiseLengthCounter.Clock(bNoiseEnable, bNoiseHalt);
			triangleLengthCounter.Clock(bTriangleEnable, bTriangleHalt);
			pulse1Sweep.Clock(pulse1Sequencer.reload, 0);
			pulse2Sweep.Clock(pulse2Sequencer.reload, 1);
		}

		// if (bUseRawMode) {
			// Update Pulse1 Channel  ================================
			pulse1Sequencer.Clock(bPulse1Enable, [](uint32_t& s) {
				// Shift right by 1 bit, wrapping around
				s = ((s & 0x0001) << 7) | ((s & 0x00FE) >> 1);
				});

			// pulse1Sample = (double)pulse1Sequencer.output;
		// }
		// else {
			pulse1Oscellator.frequency = 1789773.0 / (16.0 * (double)(pulse1Sequencer.reload + 1));
			pulse1Oscellator.amplitude = (double)(pulse1Envelope.output - 1) / 16.0;
			pulse1Sample = pulse1Oscellator.Sample(globalTime);

			if (pulse1LengthCounter.counter > 0 && pulse1Sequencer.timer >= 8 && !pulse1Sweep.bMute && pulse1Envelope.output > 2) {
				pulse1Output += (pulse1Sample - pulse1Output) * 0.5;
			}
			else {
				pulse1Output = 0;
			}
		// }

		// if (bUseRawMode) {
			// Update Pulse2 Channel  ================================
			pulse2Sequencer.Clock(bPulse2Enable, [](uint32_t& s) {
				// Shift right by 1 bit, wrapping around
				s = ((s & 0x0001) << 7) | ((s & 0x00FE) >> 1);
				});

			// pulse2Sample = (double)pulse2Sequencer.output;
		// }
		// else {
			pulse2Oscellator.frequency = 1789773.0 / (16.0 * (double)(pulse2Sequencer.reload + 1));
			pulse2Oscellator.amplitude = (double)(pulse2Envelope.output - 1) / 16.0;
			pulse2Sample = pulse2Oscellator.Sample(globalTime);

			if (pulse2LengthCounter.counter > 0 && pulse2Sequencer.timer >= 8 && !pulse2Sweep.bMute && pulse2Envelope.output > 2) {
				pulse2Output += (pulse2Sample - pulse2Output) * 0.5;
			}
			else {
				pulse2Output = 0;
			}
			// }

			noiseSequencer.Clock(bNoiseEnable, [](uint32_t& s) {
				s = (((s & 0x0001) ^ ((s & 0x0002) >> 1)) << 14) | ((s & 0x7FFF) >> 1);
				});

			if (noiseLengthCounter.counter > 0 && noiseSequencer.timer >= 8) {
				noiseOutput = (double)noiseSequencer.output * ((double)(noiseEnvelope.output - 1) / 16.0);
			}

			// if (bUseRawMode) {
				// Update Triangle Channel  ================================
				triangleSequencer.Clock(bTriangleEnable, [](uint32_t& s) {
					// Shift right by 1 bit, wrapping around
					s = ((s & 0x0001) << 7) | ((s & 0x00FE) >> 1);
					});
			// }
			// else {
				triangleOscellator.frequency = 1789773.0 / (32.0 * (double)(triangleSequencer.reload + 1));
				triangleOscellator.amplitude = (double)1.0;
				triangleSample = triangleOscellator.Sample(globalTime);

				if (triangleLengthCounter.counter > 0 && triangleSequencer.timer >= 8) {
					triangleOutput += (triangleSample - triangleOutput) * 0.5;
				}
				else {
					triangleOutput = 0;
				}
			// }

			if (!bPulse1Enable) { pulse1Output = 0; }
			if (!bPulse2Enable) { pulse2Output = 0; }
			if (!bNoiseEnable) { noiseOutput = 0; }
			if (!bTriangleEnable) { triangleOutput = 0; }
	}

	// Frequency sweepers change at high frequency
	pulse1Sweep.Track(pulse1Sequencer.reload);
	pulse2Sweep.Track(pulse2Sequencer.reload);

	pulse1Visual   = (bPulse1Enable && pulse1Envelope.output > 1 && !pulse1Sweep.bMute) ? pulse1Sequencer.reload   : 2047;
	pulse2Visual   = (bPulse2Enable && pulse2Envelope.output > 1 && !pulse2Sweep.bMute) ? pulse2Sequencer.reload   : 2047;
	noiseVisual    = (bNoiseEnable  && noiseEnvelope.output  > 1)                       ? noiseSequencer.reload    : 2047;
	triangleVisual = (bTriangleEnable)                                                  ? triangleSequencer.reload : 2047;

	clockCounter++;
}

void Olc2A03::Reset() { }

double Olc2A03::GetOutputSample() {
	if (bUseRawMode) {
		return (pulse1Sample - 0.5) * 0.5 + (pulse2Sample - 0.5) * 0.5;
	}
	else {
		//return ((1.0 * triangleOutput) - 0.5) * 0.2;
		return ((1.0 * pulse1Output) - 0.8) * 0.1 +
		       ((1.0 * pulse2Output) - 0.8) * 0.1 +
		       ((2.0 * (noiseOutput - 0.5))) * 0.1 +
		       ((1.0 * triangleOutput) - 0.8) * 0.1;
	}
}