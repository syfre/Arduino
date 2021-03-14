/*
	BeatDetector
	
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
#ifndef BeatDetector_h
#define BeatDetector_h

#include <Arduino.h>
#include "DspCore.h"
#include "Beat.h"

enum BeatState { bsWait0, bsWait1, bsOn1, bsWait2, bsOn2 };

class BeatDetector: public DspCore {
public:
	Time		sampleCount;
    BeatState	state;
    Time		waitCount;
	uint8_t		oddEven;			// odd/even flag
	uint32_t	beatCount;			// beat count
	Sample		lastTrigger;		//
	Sample		previous;
	Sample		lastMeasures[1];	// last good measures (odd/even)
	Time		waitReminder;		// waitCount reminder beat detected in state0

	BeatData		beats[cMaxBeat];
	Beat			beat;

	BeatDetector(Frequency iSampleFreq, Sample iSampleRange)
		:DspCore(iSampleFreq,iSampleRange) { reset(); }
		
	void reset(void);
	bool addSample(Sample value);
	bool beatOn(void) { if ((state==bsOn1)|(state==bsOn2)) return 1; else return 0; }
};
#endif
