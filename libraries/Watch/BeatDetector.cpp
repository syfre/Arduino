#include "BeatDetector.h"


////////////////////////////////////////////////////
// beat detector
//                 3
//                --
//           1    | |
//         0|-| 2 | | 0
// ---------- ----- -----------------
//on.st1    |---| (7.5ms)
//on.st3          |-------| (20ms)
//expect.st2    |---------| (20ms)
//expect.st0              |------------------|(180 ms)
////////////////////////////////////////////////////

void BeatDetector::reset(void) {
  state = bsWait0;
  oddEven = 0;
  waitCount = beats[cMaxBeat-1].window;
  sampleCount = 0;
  previous = 0;
  beatCount = 0;
  lastTrigger = 0;
  lastMeasures[0] = 0;
  lastMeasures[1] = 0;
  beat.faultCount0 = 0;
  beat.faultCount2 = 0;
}

bool BeatDetector::addSample(Sample value) {

	#ifdef DEBUG_BEAT_DETECTOR
	char buffer[128];
	#endif
	
	if (!sampleCount) {
		sampleCount++;
		previous = value;
		return 0;
	}
	
	sampleCount++;
	Sample delta = value-previous;
	previous = value;
	
	switch (state) {
		case bsWait0:
			if ((value>beats[1].treshold) || (delta > beats[1].delta)) {
				state = bsOn2;
				waitCount = beats[1].delay;
				#ifdef DEBUG_BEAT_DETECTOR
				sprintf(buffer,"(BD) Start S:%u V:%u D:%u",sampleCount,value,delta);
				Serial.println(buffer);
				#endif
			}
		break;
	
		case bsWait1:
			if ((value>beats[0].treshold) || (delta > beats[0].delta)) {
				waitReminder = waitCount;
				waitCount = beats[0].delay;
				state = bsOn1;
				beat.trigger = sampleCount;
				
				#ifdef DEBUG_BEAT_DETECTOR
				sprintf(buffer,"(BD) B1 S:%u DS:%u V:%u D:%u T:%u",sampleCount,sampleCount-lastTrigger,value,delta,beats[0].delta);
				Serial.println(buffer);
				#endif
			} else {
				waitCount--;
				if (! waitCount) {
					// beat expected
					// beat lost
					beat.trigger = sampleCount;
					beat.fault = 1;
					beat.amplitude = 0;
					beat.oddEven = oddEven;
					beat.measure = lastMeasures[oddEven];
					//
					beat.faultCount0++;
					waitCount = beats[1].window;
					
					#ifdef DEBUG_BEAT_DETECTOR
					sprintf(buffer,"(BD) F0 S:%u NWC:%u OE:%u",sampleCount,waitCount,oddEven);
					Serial.println(buffer);
					#endif
					
					if (oddEven) oddEven=0; else oddEven=1;
					lastTrigger = 0;
					state = bsWait1;
					return 1;
				}
			}
		break;
		case bsOn1:
			waitCount--;
			if (! waitCount) {
			   waitCount = beats[0].window;
			   state = bsWait2;
			}
		 break;
		case bsWait2:
			if ((value>beats[1].treshold) || (delta > beats[1].delta)) {
				state = bsOn2;
				waitCount = beats[1].delay;
				//
				// Beat detected
				beat.amplitude = sampleCount-beat.trigger;
				beat.oddEven = oddEven;
				beat.fault = 0;
				if (lastTrigger) { 
					beat.measure = beat.trigger - lastTrigger;
					lastMeasures[oddEven] = beat.measure;
				} else {
					beat.fault = 1; // first measure not valid
					beat.measure = lastMeasures[oddEven];
				}
				lastTrigger = beat.trigger;
				//
   				
				#ifdef DEBUG_BEAT_DETECTOR
				sprintf(buffer,"(BD) B2 S:%u V:%u D:%u T:%u",sampleCount,value,delta,beats[1].delta);
				Serial.println(buffer);
				#endif
				#ifdef DEBUG_BEAT_DETECTOR
				sprintf(buffer,"(BD) OK S:%u BF:%u BM:%u WC:%u OE:%u BC:%u",sampleCount,beat.fault,beat.measure,waitCount,oddEven,beatCount);
				Serial.println(buffer);
				#endif

 			    if (oddEven) oddEven=0; else oddEven=1;
				beatCount++;
				//
				return 1; 
			} else {
				waitCount--;
				if (!waitCount) {
				   // beat expected
				   beat.faultCount2++;
				   waitCount = waitReminder;

   				   #ifdef DEBUG_BEAT_DETECTOR
				   sprintf(buffer,"(BD) F2 S:%u DS:%u WC:%u OE:%u",sampleCount, sampleCount-beat.trigger, waitCount,oddEven);
				   Serial.println(buffer);
				   #endif

				   state = bsWait1;
				}
			}
		break;
		case bsOn2:
			waitCount--;
			if (!waitCount) {
			   waitCount = beats[1].window;
			   state = bsWait1;
			 }
		break;
	}
	return 0;
}

