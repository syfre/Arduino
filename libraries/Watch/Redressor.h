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
#ifndef Redressor_h
#define Redressor_h

#include <Arduino.h>
#include "DspCore.h"
#include "Beat.h"

class Redressor:public DspCore {
public:
	Time		sampleCount;
	uint32_t	sum;
	uint16_t	offset;
	Sample		peak;
	Sample		floor;
	Sample		gainM;
	Sample		gainD;
	Sample		average;
	void adjustGain(void);
	
	Redressor(Frequency iSampleFreq, Sample iSampleRange)
		:DspCore(iSampleFreq,iSampleRange) { reset(); }
		
	void reset(void);
	Sample process(uint16_t data);
};
#endif
