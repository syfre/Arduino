#include "Watch.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Watch*	_watch;

void _bphChange(Bph bph) {
	_watch->OnBphChange(bph);
}

void _OnWatchModeChange(WatchMode _old, WatchMode _new) {
	_watch->OnWatchModeChange(_old,_new);
}

void _OnWatchParamChange(void) {
	_watch->OnWatchParamChange();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Watch::setup() {
	_watch = this;
	watch.begin();
	wscreen.begin();
    #ifdef VIBRO
	wvibro.begin();
	#endif
    #ifdef SCOPE
	wscope.begin();
	#endif
	watch.attachBphChange(_bphChange);
	watch.setBphRef(BPH_21600);
	//
	wmenu.begin();
	wmenu.onWatchModeChange = _OnWatchModeChange;
	wmenu.onWatchParamChange = _OnWatchParamChange;
	//
	wmenu.enable(1);
	wmenu.bph = watch.bph;
	wmenu.liftAngle = watch.liftAngle;
	
	lastTouchX = 0;
	lastTouchY = 0;
}

void Watch::OnBphChange(Bph bph) {
    #ifdef VIBRO
	wvibro.setBeatSamples(watch.beatSamples());
	#endif
    #ifdef SCOPE 
	wscope.setDelayMilliSeconds(watch.beatPeriod()*0.9).setTimeMilliSeconds(watch.beatDuration()*4.0);
	#endif
}

void Watch::OnWatchModeChange(WatchMode _old, WatchMode _new) {
  switch (_old) {
    case wmVibro : 
     #ifdef VIBRO 
     wvibro.enable(0); 
     #endif 
     break;
    case wmScope : 
     #ifdef SCOPE 
     wscope.enable(0); 
     #endif 
     break;
    case wmMenu  : wmenu.enable(0); break;
  }
  switch (_new) {
    case wmVibro : 
     #ifdef VIBRO 
     wvibro.enable(1); 
     #endif 
     break;
    case wmScope : 
     #ifdef SCOPE 
     wscope.enable(1); 
     #endif 
     break;
    case wmMenu  : wmenu.enable(1); break;
  }
}
void Watch::OnWatchParamChange(void) {
  watch.setBphRef((Bph)wmenu.bph);
  watch.setLiftAngle(wmenu.liftAngle);
}

void Watch::processBuffer(uint16_t *pbuf, uint16_t bufferLength) {

    for (int i=0; i<bufferLength; i++) {
      // 
      if (watch.process(*pbuf++)) {
        #ifdef VIBRO
        wvibro.addBeat(watch.beatDetector.beat);
        #endif
        #ifdef SCOPE
        wscope.trigger();
        #endif
      }

      #ifdef SCOPE
      wscope.addSample(watch.sample,watch.beatDetector.beatOn());
      #endif
    }
}

void Watch::irqMilliSeconds(uint16_t milliSeconds) {

	if ((lastTouchX==0) && (lastTouchY==0)) {

		if (touch.touched(milliSeconds)) { 
			lastTouchX = touch.x;
			lastTouchY = touch.y;
		}
	}
}

void Watch::loopMilliSeconds(uint16_t milliSeconds) {

	if ((lastTouchX) || (lastTouchY)) {

	  int sel = wmenu.selected(touch.x, touch.y);
	  lastTouchX = 0;
	  lastTouchY = 0;
	}
}

void Watch::loopSeconds(uint16_t bufLost) {
  
	// every seconds
	watch.updateResult(bufLost);
	wscreen.refresh(watch.result);
        
 }
