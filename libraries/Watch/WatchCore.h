/*
	Redressor
	
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
#ifndef WatchCore_h
#define WatchCore_h

#include <Arduino.h>
#include "DspCore.h"
#include "Redressor.h"
#include "BeatDetector.h"
#include "Accumulator.h"
#include "BphDetector.h"

#define cWatchResultCount	6
#define cBeatPerHour	0
#define	cAmplitudeMS	1
#define	cAmplitudeDG	2
#define cBeatError		3
#define cSecondsDay		4
#define cBufferLost		5

#define BPH_COUNT	17
enum Bph { 
 BPH_7200=0,
 BPH_9000=1,
 BPH_10800=2,
 BPH_12600=3,
 BPH_14400=4,
 BPH_16200=5,
 BPH_18000=6,
 BPH_19800=7,
 BPH_21600=8,
 BPH_23400=9,
 BPH_25200=10,
 BPH_27000=11,
 BPH_28800=12,
 BPH_30600=13,
 BPH_32400=14,
 BPH_34200=15,
 BPH_36000=16
};

struct BPH_PARAMETERS {
	double 		beatPerHour;	// number of beat per second
	double		period;			// duration in ms between two beat
	double		bps;			// beat per second
	double		amplitudeK;		// 2*pi*(bph/3600)/2000
	double		duration;		// 360 amplitude beat duration (min duration)
};

extern struct BPH_PARAMETERS bph_parameters[BPH_COUNT];

//// Amplitude = liftAngle / sin (2*pi*bph/3600 * amplitudeMs/2000)
class WatchResult {
public:
	Bph			bph;
	double		liftAngle;
	double		datas[cWatchResultCount];
	WatchResult(Bph _bph, double _liftAngle=52.0) : bph(_bph), liftAngle(_liftAngle) {};
	WatchResult& bufferLost(uint16_t value) { datas[cBufferLost] = (double) value; return *this;}
	inline double beatPerHour(void) { return bph_parameters[bph].beatPerHour; };
};


typedef void (*bphProc)(Bph bph);


class WatchCore: public DspCore {
private:
	bphProc			onBphChange;
	void reset(void);
public:
	Bph				bph;
	double			liftAngle;
	Redressor 		redressor;
	BeatDetector 	beatDetector; 
	BphDetector		bphDetector;
	Accumulator 	accumulator; 
	Sample 			sample;
    WatchResult		result;
	//
	WatchCore(Frequency iSampleFreq, Sample iSampleRange, Bph _bph=BPH_18000, double _liftAngle=52.0)
	:DspCore(iSampleFreq,iSampleRange)
	 ,redressor(iSampleFreq,iSampleRange)
	 ,beatDetector(iSampleFreq,iSampleRange)
	 ,bphDetector(iSampleFreq,iSampleRange)
	 ,accumulator(iSampleFreq,iSampleRange)
	 ,bph(_bph)
	 ,liftAngle(_liftAngle)
	 ,result(_bph)
	 ,onBphChange(NULL) { reset(); }

	void begin(void);
	uint8_t process(uint16_t data);
	void updateResult(uint16_t buflost=0);
	
	WatchCore& setLiftAngle(double _liftAngle);
	WatchCore& setBphRef(Bph _bph);
	WatchCore& attachBphChange(bphProc handler) { onBphChange = handler; return *this; };
	inline Time beatSamples(void) { return ToTime(bph_parameters[bph].period); };
	inline double beatPeriod(void) { return bph_parameters[bph].period; }
	inline double beatDuration(void) { return bph_parameters[bph].duration; }
};
#endif
