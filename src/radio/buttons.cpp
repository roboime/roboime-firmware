#include <radio/buttons.h>

const Buttons::ButtonFunction Buttons::ButtonShortClickDefault[]={ButtonFunctionVolumeSpeakerDown, ButtonFunctionVolumeSpeakerUp, ButtonFunctionPTTNetSelected, ButtonFunctionPTTNetSelectedLowPower, ButtonFunctionPTTNetSelected, ButtonFunctionPTTNetSelectedMediumPower};
const Buttons::ButtonFunction Buttons::ButtonMediumClickDefault[]={ButtonFunctionVolumeMicDown, ButtonFunctionVolumeMicUp, ButtonFunctionNothing, ButtonFunctionNothing, ButtonFunctionNothing, ButtonFunctionNothing};
const Buttons::ButtonFunction Buttons::ButtonLongClickDefault[]={ButtonFunctionAudioReturnToggle, ButtonFunctionSaveState, ButtonFunctionNothing, ButtonFunctionNothing, ButtonFunctionNothing, ButtonFunctionNothing};
const Buttons::ButtonFunction Buttons::ButtonReleaseDefault[]={ButtonFunctionNothing, ButtonFunctionNothing, ButtonFunctionPTTNetSelectedRelease, ButtonFunctionPTTNetSelectedRelease, ButtonFunctionPTTNetSelectedRelease, ButtonFunctionPTTNetSelectedRelease};

const Buttons::ButtonFunction Buttons::ButtonShortClickTest[]={ButtonFunction1, ButtonFunction2, ButtonFunction3, ButtonFunction4, ButtonFunction5, ButtonFunction6};
const Buttons::ButtonFunction Buttons::ButtonMediumClickTest[]={ButtonFunctionNothing, ButtonFunctionNothing, ButtonFunctionNothing, ButtonFunctionNothing, ButtonFunctionNothing, ButtonFunctionNothing};
const Buttons::ButtonFunction Buttons::ButtonLongClickTest[]={ButtonFunctionNothing, ButtonFunctionNothing, ButtonFunctionNothing, ButtonFunctionNothing, ButtonFunctionNothing, ButtonFunctionNothing};
const Buttons::ButtonFunction Buttons::ButtonReleaseTest[]={ButtonFunctionClick, ButtonFunctionClick, ButtonFunctionClick, ButtonFunctionClick, ButtonFunctionClick, ButtonFunctionClick};

Buttons::Buttons(uint8_t queuesize, void (*shortclicknotify)(), void (*mediumclicknotify)(), void (*longclicknotify)()):
				ButtonPressStatus(0),
				ButtonPressShortMask(0),
				ButtonPressMediumMask(0),
				ButtonPressLongMask(0),
				ButtonPressTreatedMask(0),
				QueueSize(queuesize),
				ButtonMediumClickTimeMs(1000),
				ButtonLongClickTimeMs(3000),
				ButtonShortClick((ButtonFunction*)ButtonShortClickDefault),
				ButtonMediumClick((ButtonFunction*)ButtonMediumClickDefault),
				ButtonLongClick((ButtonFunction*)ButtonLongClickDefault),
				ButtonRelease((ButtonFunction*)ButtonReleaseDefault)
{
}

void Buttons::SetTestMode(uint8_t testmode) {
	if(testmode){
		ButtonShortClick=(ButtonFunction*)ButtonShortClickTest;
		ButtonMediumClick=(ButtonFunction*)ButtonMediumClickTest;
		ButtonLongClick=(ButtonFunction*)ButtonLongClickTest;
		ButtonRelease=(ButtonFunction*)ButtonReleaseTest;
	} else {
		ButtonShortClick=(ButtonFunction*)ButtonShortClickDefault;
		ButtonMediumClick=(ButtonFunction*)ButtonMediumClickDefault;
		ButtonLongClick=(ButtonFunction*)ButtonLongClickDefault;
		ButtonRelease=(ButtonFunction*)ButtonReleaseDefault;
	}
}

void Buttons::Enqueue(ButtonFunction function){
	if(Queue.size()<QueueSize && function!=ButtonFunctionNothing && function!=ButtonFunctionLastMarker){
		Queue.push(function);
	}
}

Buttons::ButtonFunction Buttons::GetButtonFunctionFromQueue(){
	if(Queue.size()){
		ButtonFunction ret=(ButtonFunction)Queue.front();
		Queue.pop();
		return ret;
	} else {
		return ButtonFunctionNothing;
	}
}

void Buttons::Poll(uint32_t ButtonKeys, uint32_t TimeStampMs){
	uint8_t i=0;
	uint32_t current;
	uint32_t currenttime;

	for(i=0; i<ButtonLastMarker;i++){
		current=(1<<i);
		currenttime=TimeStampMs-ButtonPressTime[i];
		//still pressed
		if((ButtonPressStatus & current) && (ButtonKeys & current)){
			if((currenttime>ButtonLongClickTimeMs) && (ButtonLongClick[i]!=ButtonFunctionNothing)){
				if(!(ButtonPressLongMask & current)){
					ButtonPressLongMask|=current;
					if(!(ButtonPressTreatedMask & current)){
						ButtonPressTreatedMask|=current;
						Enqueue(ButtonLongClick[i]);
					}
				}
			} else if((currenttime>ButtonMediumClickTimeMs) && (ButtonMediumClick[i]!=ButtonFunctionNothing)){
				if(!(ButtonPressMediumMask & current)){
					ButtonPressMediumMask|=current;
					if(ButtonLongClick[i]!=ButtonFunctionNothing){
						Enqueue(ButtonFunctionNotifyMediumClick);
					}
					if(!(ButtonPressTreatedMask & current) && ButtonLongClick[i]==ButtonFunctionNothing){
						ButtonPressTreatedMask|=current;
						Enqueue(ButtonMediumClick[i]);
					}
				}
			} else if((ButtonShortClick[i]!=ButtonFunctionNothing) && !(ButtonPressShortMask & current)){
				ButtonPressShortMask|=current;
				if(!(ButtonPressTreatedMask & current) && ButtonMediumClick[i]==ButtonFunctionNothing && ButtonLongClick[i]==ButtonFunctionNothing){
					ButtonPressTreatedMask|=current;
					Enqueue(ButtonShortClick[i]);
				}
			}
		}

		//just pressed
		if(!(ButtonPressStatus & current) && (ButtonKeys & current)){
			ButtonPressTime[i]=TimeStampMs;
			ButtonPressStatus|=current;
		}

		//just released
		if((ButtonPressStatus & current) && !(ButtonKeys & current)){
			if(!(ButtonPressTreatedMask & current)){
				if(ButtonPressLongMask & current){
					Enqueue(ButtonLongClick[i]);
				} else if(ButtonPressMediumMask & current){
					Enqueue(ButtonMediumClick[i]);
				} else if(ButtonPressShortMask & current){
					Enqueue(ButtonShortClick[i]);
				}
			}
			if(ButtonRelease[i]!=ButtonFunctionNothing){
				Enqueue(ButtonRelease[i]);
			}
			ButtonPressShortMask&=~current;
			ButtonPressMediumMask&=~current;
			ButtonPressLongMask&=~current;
			ButtonPressTreatedMask&=~current;
			ButtonPressStatus&=~current;
		}
	}
}
