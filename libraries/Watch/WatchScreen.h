/*
	Watch screen classes
	
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
#ifndef WatchScreen_h
#define WatchScreen_h

#include <Canvas.h>
#include "WatchCore.h"

#define cCharBufferSize	10

class WatchScreen {
private:
	Canvas& 	canvas;
	Rect		displayRect;
	WidgetList	widgets;
	char		formats[cWatchResultCount][5];
	char 		buffers[cWatchResultCount][cCharBufferSize];
	Rect		rects[2];
	Text		texts[cWatchResultCount];
	bool		shows[cWatchResultCount];

public:
	WatchScreen(Canvas& _canvas) : canvas(_canvas) {} ;
	void begin(void);
	void refresh(WatchResult& wdata);
};
#endif
