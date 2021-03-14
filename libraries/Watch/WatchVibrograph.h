/*
	Watch Vibrograph
	
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
#ifndef WatchVibrograph_h
#define WatchVibrograph_h

#include <Canvas.h>
#include "Beat.h"

//////////////////////////////////////////
// WatchVibrograph
//  display beats like on a timing machine
//////////////////////////////////////////

#define	PIXEL_BUFFER
#define PIXEL_BUFFER_CLEAR_AT_EOF
#define cMaxPixels	800

class WatchVibrograph: public Display {
private:
	Rect	displayRect;
	Time 	refBeatSamples;
	double	sensibility;
	double	sumErrors;
	COORD	currentX;
	COORD	offsetY;
	COORD	paperHeight;
	void reset(void);
public:
	COLOR	color;
	COORD	paperSpeed;
	#ifdef PIXEL_BUFFER
	COLOR 	backColor;
	COORD	pixels[cMaxPixels];
	#endif
	//
	WatchVibrograph(Canvas& _canvas, Time iRefBeatSamples, COORD iPaperSpeed=1, double iSensibility=1.0)
		:Display(_canvas), refBeatSamples(iRefBeatSamples), paperSpeed(iPaperSpeed), sensibility(iSensibility) {  }
	void begin(void);
	void paint(void);
	WatchVibrograph& setBeatSamples(Time value);
	void addBeat(Beat& beat);
	
};
#endif
