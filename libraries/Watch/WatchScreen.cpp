#include "WatchScreen.h"

////////////////////////////////////////////////////
// TextLine
////////////////////////////////////////////////////

////////////////////////////////////////////////////
// WatchScreen
////////////////////////////////////////////////////

#define cTop			0
#define cBottom			1
#define	cTextSize		3

void WatchScreen::begin(void) {

	canvas.setFontSize(cTextSize);

	for (int i=0;i<cWatchResultCount;i++) { 
		strcpy(formats[i],"%.2f");
		buffers[i][0] = 0;
		texts[i].setText(buffers[i]);
		shows[i]=0;
	}
	strcpy(formats[cBufferLost],"%.0f");
	strcpy(formats[cAmplitudeDG],"%.0f");

	rects[cTop] = canvas.clientRect;
	rects[cTop].bottom = canvas.textHeight(NULL);

	rects[cBottom] = canvas.clientRect;
	rects[cBottom].top = rects[cBottom].bottom-canvas.textHeight(NULL);
	
	texts[cBeatPerHour].setRect(rects[cTop]).alignment(leftAlign);
	texts[cAmplitudeMS].setRect(rects[cTop]).alignment(rightAlign);
	texts[cAmplitudeDG].setRect(rects[cTop]).alignment(rightAlign);
	texts[cBeatError].setRect(rects[cBottom]).alignment(leftAlign);
	texts[cSecondsDay].setRect(rects[cBottom]).alignment(rightAlign);
	texts[cBufferLost].setRect(rects[cBottom]).alignment(centerAlign);

	for (int i=0;i<2;i++) widgets.add(rects[i]);
	//widgets.add(texts[cBeatPerHour]).add(texts[cAmplitudeMS]).add(texts[cBeatError]).add(texts[cSecondsDay]);
	widgets.add(texts[cBeatPerHour]).add(texts[cAmplitudeMS]).add(texts[cBeatError]).add(texts[cSecondsDay]).add(texts[cBufferLost]);
	//widgets.add(texts[cBufferLost]);
	
	shows[cBeatPerHour] = 1;
	shows[cAmplitudeMS] = 1;
	shows[cBeatError] = 1;
	shows[cSecondsDay] = 1;
	shows[cBufferLost] = 1;
	
}

void WatchScreen::refresh(WatchResult& wdata) {

	char previous[cCharBufferSize];
	for (int i=0; i<cWatchResultCount;i++) { 
		if (shows[i]) {
			strcpy(previous,buffers[i]);
			sprintf(buffers[i],formats[i],wdata.datas[i]);
			texts[i].textDiff(canvas, texts[i].align, buffers[i], previous, canvas.textColor, canvas.backColor);
		}
	}
	//widgets.paint(canvas);
}
