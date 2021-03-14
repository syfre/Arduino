#ifndef _WATCHMENU_H_
#define _WATCHMENU_H_

#include <Canvas.h>
#include "watchCore.h"

#define BASE_MAIN   0
#define MAIN_VIBRO	BASE_MAIN+0
#define MAIN_SCOPE	BASE_MAIN+1
#define	MAIN_PARAMS	BASE_MAIN+5

#define BASE_PARAMS	10
#define PARAM_BPH	BASE_PARAMS+0
#define	PARAM_LIFT	BASE_PARAMS+1
#define PARAM_BACK	BASE_PARAMS+5

#define BASE_VALUE	20
#define VALUE_MINUS	BASE_VALUE+0
#define VALUE_PLUS	BASE_VALUE+2
#define VALUE_BACK BASE_VALUE+5

#define BASE_BPH	30
#define BPH_MINUS	BASE_BPH+0
#define BPH_PLUS	BASE_BPH+2
#define BPH_BACK 	BASE_BPH+5

#define BASE_LIFT	40
#define LIFT_MINUS	BASE_LIFT+0
#define LIFT_PLUS	BASE_LIFT+2
#define LIFT_BACK BASE_LIFT+5

enum WatchMode { wmVibro, wmScope, wmMenu };
typedef void (*WatchModeEvent)(WatchMode _old, WatchMode _new);
typedef void (*WatchParamEvent)(void);


class WatchMenu: public Display {
private:
	Rect	displayRect;
	Menu	main;
	Menu	mparams;
	Menu 	mvalue;
	Menu*	current;
	WatchMode	_watchMode;
public:
	WatchModeEvent 	onWatchModeChange;
	WatchParamEvent	onWatchParamChange;
	// 
	int		bph;
	double	liftAngle;
	//
	inline WatchMode watchMode(void) { return _watchMode; }
	void watchMode(WatchMode value);

	WatchMenu(Canvas& _canvas) : Display(_canvas), current(NULL) { _watchMode=wmMenu; onWatchModeChange=NULL; onWatchParamChange=NULL; };

	void begin(void);
	void paint(void) ;
	int selected(COORD x, COORD y);
};

#endif