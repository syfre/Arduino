/*
	Base clss for dsp class

	Copyright (C) 2017 sylvain Frere
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef DspCore_h
#define DspCore_h

#include <Arduino.h>

typedef int32_t Sample;			// Sample value
typedef	uint32_t Time;			// Time (sample count)
typedef uint32_t Frequency;		// Frequency


class DspCore {
public:
	Frequency	sampleFreq;
	Sample		sampleRange; 	// ex 2048 for a 12 bits ADC
	
	DspCore(Frequency iSampleFreq, Sample iSampleRange) 
		:sampleFreq(iSampleFreq),sampleRange(iSampleRange) {};
	
	inline Sample ToSample(double normalizedValue) { return (Sample) (normalizedValue * sampleRange); }
	inline Sample ToSample(uint16_t value) { return (Sample) (value - sampleRange); }
	inline Sample ToSample(uint32_t value) { return (Sample) (value - sampleRange); }
	inline Time	ToTime(double milliSeconds) { return (Time) (milliSeconds * 1.0e-3 * sampleFreq); }
	inline double ToMilliSeconds(Time value) { return (double) value * 1.0e3 / sampleFreq; }
	inline double ToMilliSeconds(double value) { return value * 1.0e3 / sampleFreq; }
};
#endif
