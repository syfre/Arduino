#include "Redressor.h"

////////////////////////////////////////////////////
// Redressor
// Basically does :
//	if (value<0) value = -1*value
//	return value * Gain
////////////////////////////////////////////////////

void Redressor::reset(void) {
  peak = 0;
  offset = sampleRange; //2064
  floor = sampleRange;
  gainM = 1;
  gainD = 0;
  sampleCount = 0;
  sum = 0;
}

void Redressor::adjustGain(void) {

	average = (sum/sampleCount)-offset;

	#ifdef DEBUG_REDRESSOR
	char buffer [40];
	sprintf(buffer,"(RD) A:%d P:%u F:%u", average, peak,floor);
	Serial.println(buffer);
	#endif

	if (peak==0) {
		gainM = 1;
		gainD = 0;
	} else {
		gainM = sampleRange;
		gainD = peak;
	}
	sampleCount = 0;
	sum = 0;
	peak = 0;
	floor = sampleRange;
}

Sample Redressor::process(uint16_t data) {

	// 0..cSampleADC => 0..sampleRange
	sum += data;
	Sample value = (Sample) (data - offset);
	if (value < 0) value = (-1) * value;
	
	// peak detector
	if (value>peak) peak = value;
	if (value<floor) floor = value;

	// Adjust the gain every second
	sampleCount++;
	if (sampleCount==sampleFreq) {
		adjustGain();
	}
	// gain factor
	if (gainD) 	
			return value * gainM / gainD; 
	else 	return value * gainM;
}

