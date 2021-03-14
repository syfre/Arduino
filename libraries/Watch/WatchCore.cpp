#include "WatchCore.h"

struct BPH_PARAMETERS bph_parameters[BPH_COUNT] = {			

{7200.0,  500.0000, 2.0, 0.006283185, 22.62}, 
{9000.0,  400.0000, 2.5, 0.007853982, 18.10}, 
{10800.0, 333.3333, 3.0, 0.009424778, 15.08}, 
{12600.0, 285.7143, 3.5, 0.010995574, 12.93}, 
{14400.0, 250.0000, 4.0, 0.012566371, 11.31}, 
{16200.0, 222.2222, 4.5, 0.014137167, 10.05}, 
{18000.0, 200.0000, 5.0, 0.015707963, 9.05}, 
{19800.0, 181.8181, 5.5, 0.01727876,  8.23}, 
{21600.0, 166.6666, 6.0, 0.018849556, 7.54}, 
{23400.0, 153.8461, 6.5, 0.020420352, 6.96}, 
{25200.0, 142.8571, 7.0, 0.021991149, 6.46}, 
{27000.0, 133.3333, 7.5, 0.023561945, 6.03}, 
{28800.0, 125.0000, 8.0, 0.025132741, 5.65}, 
{30600.0, 117.6470, 8.5, 0.026703538, 5.32}, 
{32400.0, 111.1111, 9.0, 0.028274334, 5.03}, 
{34200.0, 105.2631, 9.5, 0.02984513,  4.76}, 
{36000.0, 100.0000, 10.0, 0.031415927, 4.52}
};		

void WatchCore::begin(void) {
	setBphRef(bph);
}

void WatchCore::reset(void) {
	sample = 0;
}

WatchCore& WatchCore::setLiftAngle(double _liftAngle) {
	liftAngle = _liftAngle;
	return *this;
}

WatchCore& WatchCore::setBphRef(Bph _bph) {

	bph = _bph;
	result.bph = _bph;

	double p = bph_parameters[bph].period;
	double d = bph_parameters[bph].duration;

	// first beat
	beatDetector.beats[0].treshold = beatDetector.ToSample(0.25); // 25%
	beatDetector.beats[0].delta = beatDetector.ToSample(0.075); //0.075
	beatDetector.beats[0].delay = beatDetector.ToTime(d * 0.9); // min duration 360°
	beatDetector.beats[0].window = beatDetector.ToTime( p * 0.1);
	// second beat
	beatDetector.beats[1].treshold = beatDetector.ToSample(0.50); // 50%
	beatDetector.beats[1].delta = beatDetector.ToSample(0.10); // 0.60
	beatDetector.beats[1].delay = beatDetector.ToTime( p * 0.5 );
	beatDetector.beats[1].window = beatDetector.ToTime( p * 0.5);

	reset();
	
	#ifdef DEBUG_COMMAND
	char buffer[40];
	sprintf(buffer,"new bph:%u",bph);
	Serial.println(buffer);
	#endif
	
	if (onBphChange) onBphChange(bph);
	return *this;
}

uint8_t WatchCore::process(uint16_t data) {

	uint8_t rslt;
	sample = redressor.process(data);
	
	rslt = beatDetector.addSample(sample);
	if (rslt) {
		accumulator.addBeat(beatDetector.beat);
	}
	
	if (bphDetector.addSample(sample)) {
	}

    #ifdef DEBUG_BEAT_STREAM
	if (rslt) {
		char buffer [128];
		beatDetector.beat.ToString(buffer);
		Serial.println(buffer);
	}
    #endif
	return rslt;
}

void WatchCore::updateResult(uint16_t buflost) {

	double beatPerHour = 1.0e3 / ToMilliSeconds(accumulator.measure()) * 3600.0;
	double amplitude = ToMilliSeconds(accumulator.amplitude());
	double bphRef = bph_parameters[bph].beatPerHour;

	result.liftAngle = liftAngle;
	result.datas[cBeatPerHour] = beatPerHour;
	result.datas[cSecondsDay] = (beatPerHour-bphRef) / bphRef * 86400.0;
	result.datas[cBeatError] = ToMilliSeconds(accumulator.beatError());
	result.datas[cAmplitudeMS] = amplitude;
	result.datas[cAmplitudeDG] = result.liftAngle / sin( bph_parameters[bph].amplitudeK * amplitude);

	result.bufferLost(buflost);
}

