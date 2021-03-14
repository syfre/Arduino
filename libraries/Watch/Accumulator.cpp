#include "Accumulator.h"

void BeatBuffer::reset(void) {

	int	idx;
	
	next = 0;
	//
	for (idx=0; idx<2; idx++) { 
		sums[idx].clear(); 
	}
	for (idx; idx<cMaxAccumulate; idx++) {
		beats[idx].clear();
	}
}

void BeatBuffer::sumBeat(Beat& beat, int sign) {

	sums[0].add(beat.measure,sign);
	sums[1].add(beat.amplitude,sign);
}

void BeatBuffer::addBeat(Beat& beat) {

	sumBeat(beats[next],-1);
	beats[next] = beat;
	sumBeat(beats[next],+1);
	//
	next++;
	if (next==cMaxAccumulate) next=0;
}

////////////////////////////////////////////////////
// Accumulator
////////////////////////////////////////////////////

void Accumulator::reset(void) {
	beatBuffers[0].reset();
	beatBuffers[1].reset();
}

void Accumulator::addBeat(Beat& beat) {
	if (beat.fault) return;
	beatBuffers[beat.oddEven].addBeat(beat);
}

double Accumulator::beatError(void) {
	double t1,t2;
	t1 = beatBuffers[0].measure();
	t2 = beatBuffers[1].measure();
	if (t1>t2) return (t1-t2)/2; else return (t2-t1)/2;
}
double Accumulator::measure(void) {

	return (beatBuffers[0].measure()+beatBuffers[1].measure())/2 ;
}
double Accumulator::amplitude(void) {
	return (double) ( beatBuffers[0].amplitude() + beatBuffers[1].amplitude() ) /2 ;
}



