#include "BphDetector.h"


////////////////////////////////////////////////////
// Count beat during 2 seconds
////////////////////////////////////////////////////

void BphDetector::reset(void) {
  state = bph_wait;
  beatPerSeconds = 0;
  beatCount = 0;
  triggerValue = sampleRange * 3 / 4;
  waitBeatOn = ToTime(50.0);
  waitSeconds = sampleFreq * 2; // N seconds
  waitCount = 0;
  measureCount = 0;
}

bool BphDetector::addSample(Sample value) {

	#ifdef DEBUG_BPH_DETECTOR
	char buffer[128];
	#endif

	if( measureCount) {
		measureCount--;
		if (!measureCount) {
			beatPerSeconds = beatCount;
			beatCount = 0;
			#ifdef DEBUG_BPH_DETECTOR
			sprintf(buffer,"(BPS) %u", beatPerSeconds);
			Serial.println(buffer);
			#endif
			return 1;
		}
	}

	switch (state) {
		case bph_wait:
			if (value > triggerValue) {
				beatCount++;
				state = bph_on;
				waitCount = waitBeatOn;
				if (!measureCount) {
					measureCount = waitSeconds;
				}
			}
		break;
		case bph_on:
			waitCount--;
			if (! waitCount) {
			   state = bph_wait;
			}
		break;
	}
	
	return 0;
}

