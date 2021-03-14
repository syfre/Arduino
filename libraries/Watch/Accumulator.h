/*
	Accumulator
	
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
#ifndef Accumulator_h
#define Accumulator_h

#include <Arduino.h>
#include "DspCore.h"
#include "Beat.h"

class Sum {
	public:
	Time sum;
	int	count;
	inline void clear() { sum=0; count=0; };
	inline void add(Time val, int sign) { if (val) {sum+=sign*val; count+=sign;} }
	double average(void) { if (count) return (double) sum / count; else return 0; };
};

#define cMaxAccumulate	10

class BeatBuffer {
public:
	void reset(void);
	void sumBeat(Beat& beat, int sign);
	void addBeat(Beat& beat);

	int		next;
	Beat	beats[cMaxAccumulate];
	Sum		sums[2];
	inline double measure(void) { return sums[0].average(); };
	inline double amplitude(void) { return sums[1].average(); };
};

class Accumulator:public DspCore {
public:
	BeatBuffer	beatBuffers[2];
	
	Accumulator(Frequency iSampleFreq, Sample iSampleRange)
		:DspCore(iSampleFreq,iSampleRange) { reset(); }
		
	void reset(void);
	void addBeat(Beat& beat);
	//
	double beatError(void);
	inline double beatErrorMilliSeconds(void) { return ToMilliSeconds(beatError()); }
	//
	double measure(void);
	inline double measureMilliSeconds(void) {return ToMilliSeconds(measure()); } 
	//
	double amplitude(void);
	inline double amplitudeMilliSeconds(void) {return ToMilliSeconds(amplitude()); }

};
#endif
