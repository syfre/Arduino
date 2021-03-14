#include <WatchMenu.h>

#define DEBUG

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

char mainText[6][7] = {"Vibro","Scope","","","","Params"};
char mparamsText[6][7] = {"BPH","Lift","","","","Back"};
char mvalueText[6][8] = {"<","",">","","","Back"};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
  

void WatchMenu::begin(void) {
	//
	displayRect = canvas.clientRect;
	displayRect.top += canvas.textHeight("A");
	displayRect.bottom -= canvas.textHeight("A");

	// Main menu
	main.rect(displayRect).horzCount(3).vertCount(2);
	main.colors(canvas.colorWhite(),canvas.colorGreen(),canvas.colorBlack(),canvas.colorGreen(),canvas.colorBlack());
	for (int i=0; i<main.itemCount(); i++) main.textItem(i, mainText[i] );
	main.firstValue = BASE_MAIN;
	
	// Parameters menu
	mparams.rect(displayRect).horzCount(3).vertCount(2);
	mparams.colors(canvas.colorWhite(),canvas.colorGreen(),canvas.colorBlack(),canvas.colorGreen(),canvas.colorBlack());
	for (int i=0; i<mparams.itemCount(); i++) mparams.textItem(i, mparamsText[i] );
	mparams.firstValue = BASE_PARAMS;

	// Value menu
	mvalue.rect(displayRect).horzCount(3).vertCount(2);
	mvalue.colors(canvas.colorWhite(),canvas.colorGreen(),canvas.colorBlack(),canvas.colorGreen(),canvas.colorBlack());
	for (int i=0; i<mvalue.itemCount(); i++) mvalue.textItem(i, mvalueText[i] );
	mvalue.firstValue = BASE_VALUE;

	// Active menu
	current = &main;
	_watchMode = wmMenu;
	bph = BPH_18000;
}

void WatchMenu::paint(void) {
	current->draw(canvas);
}

void WatchMenu::watchMode(WatchMode value) {

	if (_watchMode!=(WatchMode)value) {
		if (onWatchModeChange) onWatchModeChange(_watchMode,(WatchMode)value);
		_watchMode = (WatchMode)value;
	}
}


int WatchMenu::selected(COORD x, COORD y) {

	if (watchMode()!=wmMenu) {
		watchMode(wmMenu);
		return -1;
	}
	
	int sel = current->selected(canvas,x,y);
	
	#ifdef DEBUG
	char buffer[40];
	sprintf(buffer,"(MN) selected:%u",sel);
	Serial.println(buffer);
	#endif
	
	switch(sel) {
	// Main menu
	case MAIN_VIBRO :  
	case MAIN_SCOPE : 
		watchMode((WatchMode)sel);
		break;
	case MAIN_PARAMS:
		current = &mparams;
		paint();
		break;
		
	// Parameters menu
	case PARAM_BPH:
		mvalue.firstValue = BASE_BPH;
		sprintf(mvalueText[1],"%.0f",bph_parameters[bph].beatPerHour);
		current = &mvalue;
		paint();
		break;
	case PARAM_LIFT:
		mvalue.firstValue = BASE_LIFT;
		sprintf(mvalueText[1],"%.0f",liftAngle);
		current = &mvalue;
		paint();
		break;
		
	// Bph value change
	case BPH_MINUS:
		if (bph>0) bph--; else bph = BPH_COUNT-1;
		sprintf(mvalueText[1],"%.0f",bph_parameters[(Bph)bph].beatPerHour);
		mvalue.selected(canvas,mvalue.firstValue+1);
		if (onWatchParamChange) onWatchParamChange();
		break;

	case BPH_PLUS:
		if (bph<BPH_COUNT-1) bph++; else bph = 0;
		sprintf(mvalueText[1],"%.0f",bph_parameters[(Bph)bph].beatPerHour);
		mvalue.selected(canvas,mvalue.firstValue+1);
		if (onWatchParamChange) onWatchParamChange();
		break;
	
	// Lift angle value change
	case LIFT_MINUS:
		liftAngle -= 1.0;
		sprintf(mvalueText[1],"%.0f",liftAngle);
		mvalue.selected(canvas,mvalue.firstValue+1);
		if (onWatchParamChange) onWatchParamChange();
		break;

	case LIFT_PLUS:
		liftAngle += 1.0;
		sprintf(mvalueText[1],"%.0f",liftAngle);
		mvalue.selected(canvas,mvalue.firstValue+1);
		if (onWatchParamChange) onWatchParamChange();
		break;


	case PARAM_BACK:
	case LIFT_BACK:
	case BPH_BACK:
		current = &main;
		paint();
		break;
	}
	
	return sel;
}

