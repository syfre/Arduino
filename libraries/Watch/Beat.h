/*
	Beat definition
	
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
#ifndef Beat_h
#define Beat_h

#include <Arduino.h>
#include "DspCore.h"

///////////////////////////////////////
// DEBUG
///////////////////////////////////////
#define _DEBUG_REDRESSOR
#define _DEBUG_BEAT_DETECTOR
#define _DEBUG_BEAT_ACCUMULATOR
#define _DEBUG_BEAT_STREAM
#define _DEBUG_COMMAND
#define _DEBUG_BPH_DETECTOR

#define cMaxBeat	2

#define ODD		0
#define EVEN	1

class BeatData {
	public:
	Sample		treshold;	// treshold value to trigger the beat
	Sample		delta;		// delta value to trigger this beat (slope)
	Time		delay;		// Time where the beat is "on" after the triggering
	Time		window;		// Time before we expect the next Beat to occure
};

class Beat {
	public:
	uint8_t		oddEven;			// Odd/even flag
	uint8_t		fault;				// This beat is fault
	Time		trigger;			// sample which trigger this beat
	Time		measure;			// measure : diff in sample between the previous beat
	Time		amplitude;			// amplitude : diff in sample between first and second beat
    uint32_t	faultCount0;		// global fault count state 0
    uint32_t	faultCount2;		// global fault count state 2
	inline void clear(void) {measure = 0; amplitude=0; };
	inline void ToString(char * buffer) { sprintf(buffer,"(BS) E:%u f:%u M:%u A:%u f0:%u f2:%u T:%u",oddEven,fault,measure,amplitude,faultCount0,faultCount2,trigger); };
};


#endif
