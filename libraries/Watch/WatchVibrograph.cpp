#include "WatchVibrograph.h"

////////////////////////////////////////////////////
// WatchVibrograph
////////////////////////////////////////////////////

void WatchVibrograph::begin(void) {
	reset();
}

WatchVibrograph& WatchVibrograph::setBeatSamples(Time value) {
	refBeatSamples = value;
	reset();
	return *this;
}

void WatchVibrograph::reset(void) {
	sumErrors = 0;
	currentX = 0;
	offsetY = canvas.height() / 2;
	paperHeight = offsetY;
	//
	color = canvas.rgb(0,255,0);
	displayRect = canvas.clientRect;
	displayRect.top += canvas.textHeight("A");
	displayRect.bottom -= canvas.textHeight("A");
	
	#ifdef PIXEL_BUFFER
	backColor = canvas.rgb(0,0,0);
	for (int idx=0; idx<cMaxPixels; idx++) pixels[idx] = backColor;
	#endif
}

void WatchVibrograph::paint(void) {
	sumErrors = 0;
	currentX = 0;
	canvas.fillRectangle(displayRect);
}

void WatchVibrograph::addBeat(Beat& beat) {

double	tempM;
double	error;

	if (!enable()) return;

	tempM = (double) beat.measure;
	error = sensibility * (tempM-refBeatSamples)/refBeatSamples;
	//
	sumErrors += error;
	if (sumErrors>1.0) sumErrors = 0;
	if (sumErrors<-1.0) sumErrors = 0;
	//
	#ifdef PIXEL_BUFFER
	#ifndef PIXEL_BUFFER_CLEAR_AT_EOF
	canvas.setPixel(currentX, pixels[currentX], 2, backColor );
	#endif
	pixels[currentX] = offsetY + paperHeight*sumErrors;
	canvas.setPixel(currentX, pixels[currentX], 2, color );
	#else
	COORD Y = offsetY + paperHeight*sumErrors;
	canvas.setPixel(currentX, Y, 2, color );
	#endif

	currentX += paperSpeed;
	currentX += paperSpeed;
	//
	if (currentX >= canvas.width() ) {
		currentX = 0;
		sumErrors = 0;
		#ifdef PIXEL_BUFFER
		#ifdef PIXEL_BUFFER_CLEAR_AT_EOF
		for (int idx=0; idx<canvas.width(); idx++) {
		  canvas.setPixel(idx, pixels[idx], 2, backColor );
		}
		#endif
		#else
		canvas.fillRectangle(displayRect);
		#endif
	}
}

