#include "Scope.h"

void ScopeTrace::reset(Rect& _rect, COLOR _color) {

	rect = _rect;
	offsetY = rect.height() / 2;
	height = offsetY;
	current.x = 0;
	current.y = offsetY;
	color = _color;
}

void ScopeTrace::paint(void) {
	canvas.fillRectangle(rect);
	canvas.drawRectangle(rect,color);
}

bool ScopeTrace::add(Sample value, Sample valueRange) {

	Point pt;
	
	pt.x = current.x+1;
	pt.y = offsetY - height * value / valueRange;
	
	canvas.lineTo(current, pt, color );
	current = pt;
	//
	if (current.x >= canvas.width() ) {
		current.x = 0;
		return 1;
	}
	return 0;
}

void ScopeTrace::clear(void) {
	canvas.fillRectangle(rect);
}

////////////////////////////////////////////////////
// Scope
////////////////////////////////////////////////////

void Scope::begin(void) {

	rect = canvas.clientRect;
	rect.inflatePercent(0.0,-10.0);
	
	Rect rc = rect;
	//rc.bottom = rc.bottom - rect.height()/2;
	trace1.reset(rc, canvas.rgb(0,255,0) );

	rc = rect;
	//rc.top = rc.top + rect.height()/2;
	trace2.reset(rc, canvas.rgb(255,0,0) );

	state = iddle;
	setDelayMilliSeconds(180.0);
	setTimeMilliSeconds(40.0);
}

void Scope::paint(void) {
	state = iddle;
	trace1.paint();
	trace2.paint();
}

Scope& Scope::setTriggerMode(TriggerMode mode) {
	triggerMode = mode;
}

Scope& Scope::setDelayMilliSeconds(double delay) {

	waitSamples = ToTime(delay);
	//
	#ifdef DEBUG_SCOPE
	char buffer[40];
	sprintf(buffer,"SetDelay:%.2f WS:%u DS:%u SS:%u",delay,waitSamples,displaySamples,skipSamples);
	Serial.println(buffer);
	#endif
	return *this;
}

Scope& Scope::setTimeMilliSeconds(double time) {

	displaySamples = ToTime(time);
	if (displaySamples > rect.width()) {
		skipSamples = displaySamples / rect.width() ;
	} else skipSamples = 0;
	//

	#ifdef DEBUG_SCOPE
	char buffer[40];
	sprintf(buffer,"SetTime:%.2f  WS:%u DS:%u SS:%u",time,waitSamples,displaySamples,skipSamples);
	Serial.println(buffer);
	#endif
	return *this;
}

Scope& Scope::trigger(void) {

	if (!enable()) return *this; 

	switch (state) {
		case iddle : { state = wait; waitCount = waitSamples; }
	}
	return *this;
}

Sample Scope::addSample(Sample sample, bool beatState) {

	if (!enable()) return sample; 

	switch (state) {
		case iddle : return sample;
		case wait  : 
			waitCount--; 
			if (!waitCount) { 
				state = display; 
				skipCount = skipSamples;
				canvas.fillRectangle(rect);
			}
			return sample; 
	}
	
	if (skipCount) {
		skipCount--;
		if (skipCount) return sample;
	}
	//
	trace2.add(beatState,2);
	if (trace1.add(sample,sampleRange)) {
		state = iddle; 
	}

	skipCount = skipSamples;
	return sample;
}

