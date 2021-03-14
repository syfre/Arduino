/*
	Watch
	
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
#ifndef Watch_h
#define Watch_h

#define	VIBRO
#define	SCOPE

#include <Canvas.h>
#include <WatchCore.h>
#include <WatchScreen.h>
#ifdef	VIBRO
#include <WatchVibrograph.h>
#endif
#ifdef	SCOPE
#include <Scope.h>
#endif
#include <WatchMenu.h>

class Watch {
protected:
	Canvas&			canvas;
	XPT2046& 		touch;

	WatchCore 		watch;
	WatchScreen 	wscreen;
	WatchMenu 		wmenu;
	#ifdef	VIBRO
	WatchVibrograph wvibro;
	#endif
	#ifdef	SCOPE
	Scope 			wscope;
	#endif
	
	COORD			lastTouchX;
	COORD			lastTouchY;
public:
	Watch(Canvas& _canvas, XPT2046& _touch, Frequency iSampleFreq, Sample iSampleRange) 
		: canvas(_canvas), touch(_touch)
		, watch(iSampleFreq, iSampleRange)
		, wscreen(canvas)
		#ifdef	VIBRO
		, wvibro(canvas, watch.ToTime(200.0),1,10.0)
		#endif
		#ifdef	SCOPE
		, wscope(canvas, iSampleFreq, iSampleRange)
		#endif
		, wmenu(canvas)
		{ };

	void OnBphChange(Bph bph);
	void OnWatchModeChange(WatchMode _old, WatchMode _new);
	void OnWatchParamChange(void);

	void setup();
	void processBuffer(uint16_t *pbuf, uint16_t bufferLength);
	void irqMilliSeconds(uint16_t milliSeconds);
	void loopMilliSeconds(uint16_t milliSeconds);
	void loopSeconds(uint16_t bufLost);
};

#endif