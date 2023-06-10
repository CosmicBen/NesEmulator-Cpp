/*
	olc::NES - APU
*/

/*
IMPORTANT !!!!
THIS CLASS IS VERY UNFINISHED
*/
#pragma once

#include <cstdint>
#include <functional>

class Olc2A03
{
public:
	Olc2A03();
	~Olc2A03();

public:
	void CpuWrite(uint16_t address, uint8_t data);
	uint8_t CpuRead(uint16_t address);

	void Clock();
	void Reset();

	double GetOutputSample();

private:
	uint32_t frameClockCounter = 0;
	uint32_t clockCounter = 0;

	bool bUseRawMode = false;
	static uint8_t lengthTable[];

	// Sequencer Module
	// ~~~~~~~~~~~~~~~~
	// The purpose of the sequencer is to output a '1' after a given 
	// interval. It does this by counting down from a start value,
	// when that value is < 0, it gets reset, and an internal "rotary"
	// buffer is shifted. The nature of ths shifted pattern is different
	// depending upon the channel, or module that requires sequencing.
	// For example, the square wave channels simply rotate the preset
	// sequence, but the noise channel needs to generate pseudo-random
	// outputs originating from the preset sequence.
	//
	// Consider a square wave channel. A preset sequence of 01010101
	// will output a 1 more freqently than 00010001, assuming we 
	// always output the LSB. The speed of this output is also
	// governed by the timer counting down. The frequency is higher
	// for small timer values, and lower for larger. Increasing
	// the frequency of the output potentially increases the
	// audible frequency. In fact, this is how the pulse channels
	// fundamentally work. A "duty cycle" shape is loaded into the
	// sequencer and the timer is used to vary the pitch, yielding 
	// notes.

	struct Sequencer {
		uint32_t sequence = 0x00000000;
		uint32_t newSequence = 0x00000000;
		uint16_t timer = 0x0000;
		uint16_t reload = 0x0000;
		uint8_t output = 0x00;

		// Pass in a lambda function to manipulate the sequence as required
		// by the owner of this sequencer module
		uint8_t Clock(bool bEnable, std::function<void(uint32_t& s)> functionManip) {
			if (bEnable) {
				timer--;
				if (timer == 0xFFFF) {
					timer = reload;
					functionManip(sequence);
					output = sequence & 0x00000001;
				}
			}

			return output;
		}
	};

	struct LengthCounter {
		uint8_t counter = 0x00;

		uint8_t Clock(bool bEnable, bool bHalt) {
			if (!bEnable) {
				counter = 0;
			}
			else if (counter > 0 && !bHalt) {
				counter--;
			}

			return counter;
		}
	};

	struct Envelope {
		bool bStart = false;
		bool bDisable = false;
		uint16_t dividerCount = 0;
		uint16_t volume = 0;
		uint16_t output = 0;
		uint16_t decayCount = 0;

		void Clock(bool bLoop) {
			if (!bStart) {
				if (dividerCount == 0) {
					dividerCount = volume;

					if (decayCount == 0) {
						if (bLoop) {
							decayCount = 15;
						}
					}
					else {
						decayCount--;
					}
				}
				else {
					dividerCount--;
				}
			}
			else {
				bStart = false;
				decayCount = 15;
				dividerCount = volume;
			}

			if (bDisable) {
				output = volume;
			}
			else {
				output = decayCount;
			}
		}
	};

	struct OscellatorPulse {
		double frequency = 0.0;
		double dutyCycle = 0.0;
		double amplitude = 1.0;
		double pi = 3.14159;
		double harmonics = 20;

		double Sample(double t) {
			double a = 0.0;
			double b = 0.0;
			double p = dutyCycle * 2.0 * pi;

			auto approxsin = [](float t) {
				float j = t * 0.15915;
				j = j - (int)j;
				return 20.785 * j * (j - 0.5) * (j - 1.0f);
			};

			for (double n = 1; n < harmonics; ++n) {
				double c = n * frequency * 2.0 * pi * t;
				a += -approxsin(c) / n;
				b += -approxsin(c - p * n) / n;

				//a += -sin(c) / n;
				//b += -sin(c - p * n) / n;
			}

			return (2.0 * amplitude / pi) * (a - b);
		}
	};

	struct OscellatorTriangle {
		double frequency = 0;
		double amplitude = 1;
		double pi = 3.14159;
		// double harmonics = 20;

		double Sample(double t) {
			double a = 0;
			double b = 0;

			auto approxsin = [](float t) {
				float j = t * 0.15915;
				j = j - (int)j;
				return 20.785 * j * (j - 0.5) * (j - 1.0f);
			};

			a = approxsin(2 * pi * frequency * t);
			b = approxsin(2 * pi * frequency * t * 3) / 9 - approxsin(2 * pi * frequency * t * 5) / 25 + approxsin(2 * pi * frequency * t * 7) / 49;

			return (2.0 * amplitude / pi) * (a - b);
		}
	};

	struct Sweeper {
		bool bEnabled = false;
		bool bDown = false;
		bool bReload = false;
		uint8_t shift = 0x00;
		uint8_t timer = 0x00;
		uint8_t period = 0x00;
		uint16_t change = 0;
		bool bMute = false;

		void Track(uint16_t& target) {
			if (bEnabled) {
				change = target >> shift;
				bMute = (target < 8) || (target > 0x7FF);
			}
		}

		bool Clock(uint16_t& target, bool channel) {
			bool bChanged = false;

			if (timer == 0 && bEnabled && shift > 0 && !bMute) {
				if (target >= 8 && change < 0x07FF) {
					if (bDown) {
						target -= change - channel;
					}
					else {
						target += change;
					}

					bChanged = true;
				}
			}

			// if (bEnabled) {
				if (timer == 0 || bReload) {
					timer = period;
					bReload = false;
				}
				else {
					timer--;
				}

				bMute = (target < 8) || (target > 0x7FF);
			// }

			return bChanged;
		}
	};

	double globalTime = 0.0;

	// Square Wave Pulse Channel 1
	bool bPulse1Enable = false;
	bool bPulse1Halt = false;
	double pulse1Sample = 0.0;
	double pulse1Output = 0.0;
	Sequencer pulse1Sequencer;
	OscellatorPulse pulse1Oscellator;
	Envelope pulse1Envelope;
	LengthCounter pulse1LengthCounter;
	Sweeper pulse1Sweep;

	// Square Wave Pulse Channel 2
	bool bPulse2Enable = false;
	bool bPulse2Halt = false;
	double pulse2Sample = 0.0;
	double pulse2Output = 0.0;
	Sequencer pulse2Sequencer;
	OscellatorPulse pulse2Oscellator;
	Envelope pulse2Envelope;
	LengthCounter pulse2LengthCounter;
	Sweeper pulse2Sweep;

	// Noise Channel
	bool bNoiseEnable = false;
	bool bNoiseHalt = false;
	Envelope noiseEnvelope;
	LengthCounter noiseLengthCounter;
	Sequencer noiseSequencer;
	double noiseSample = 0;
	double noiseOutput = 0;

	// Triangle Wave Channel
	bool bTriangleEnable = false;
	bool bTriangleHalt = false;
	Sequencer triangleSequencer;
	OscellatorTriangle triangleOscellator;
	LengthCounter triangleLengthCounter;
	double triangleSample = 0.0;
	double triangleOutput = 0.0;

public:
	uint16_t pulse1Visual = 0;
	uint16_t pulse2Visual = 0;
	uint16_t noiseVisual = 0;
	uint16_t triangleVisual = 0;
};