/*
	Watch Scope
	
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
#ifndef Scope_h
#define Scope_h

#include "DspCore.h"
#include "Canvas.h"

#define DEBUG_SCOPE


//////////////////////////////////////////
// Scope
//  display as scope
//////////////////////////////////////////

enum ScopeState { iddle, wait, display };

class ScopeTrace {
public:
	Canvas&	canvas;
	Rect	rect;
	COORD	offsetY;
	COORD	height;
	Point	current;
	COLOR	color;
	ScopeTrace(Canvas& _canvas) : canvas(_canvas) { };
	void reset(Rect& _rect, COLOR _color);
	bool add(Sample value, Sample valueRange);
	void clear(void);
	void paint(void);
};

enum TriggerMode {
	tmFreeRun,
	tmTrigger
};

class Scope:public DspCore, public Display  {
private:
	Rect	rect;
	ScopeTrace	trace1;
	ScopeTrace	trace2;
	ScopeState	state;
	TriggerMode	triggerMode;
	
	Time	displaySamples;
	Time	waitSamples;
	Time	waitCount;
	Time	skipSamples;
	Time	skipCount;

public:
	Scope(Canvas& _canvas, Frequency iSampleFreq, Sample iSampleRange, TriggerMode _trigger=tmTrigger)
		:Display(_canvas), trace1(_canvas), trace2(_canvas), triggerMode(_trigger), DspCore(iSampleFreq,iSampleRange) { }
	void begin(void);
	Scope& setTriggerMode(TriggerMode mode);
	Scope& setDelayMilliSeconds(double delay);
	Scope& setTimeMilliSeconds(double time);
	Scope& trigger(void);
	Sample addSample(Sample sample, bool beatState);
	void paint(void);
};
#endif
