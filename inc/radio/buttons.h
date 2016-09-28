#pragma once

#include <inttypes.h>
#include <queue>
#include <list>

class Buttons{
public:
	typedef enum {
		ButtonFunctionNothing=0,
		ButtonFunctionTurnOn,
		ButtonFunctionVolumeSpeakerUp,
		ButtonFunctionVolumeSpeakerDown,
		ButtonFunctionVolumeSpeakerCycleUp,
		ButtonFunctionVolumeSpeakerCycleDown,
		ButtonFunctionVolumeMicUp,
		ButtonFunctionVolumeMicDown,
		ButtonFunctionVolumeMicCycleUp,
		ButtonFunctionVolumeMicCycleDown,
		ButtonFunctionAudioReturnToggle,
		ButtonFunctionSaveState,

		ButtonFunctionPTTNet1,
		ButtonFunctionPTTNet2,
		ButtonFunctionPTTNet3,
		ButtonFunctionPTTNet4,
		ButtonFunctionPTTNet5,
		ButtonFunctionPTTNet6,
		ButtonFunctionPTTNet7,
		ButtonFunctionPTTNet8,
		ButtonFunctionPTTNet9,
		ButtonFunctionPTTNet10,
		ButtonFunctionPTTNet11,
		ButtonFunctionPTTNet12,
		ButtonFunctionPTTNet13,
		ButtonFunctionPTTNet14,
		ButtonFunctionPTTNet15,
		ButtonFunctionPTTNetSelected,
		ButtonFunctionPTTNetSelectedLowPower,
		ButtonFunctionPTTNetSelectedMediumPower,
		ButtonFunctionPTTNetSelectedHighPower,
		ButtonFunctionPTTNet1Release,
		ButtonFunctionPTTNet2Release,
		ButtonFunctionPTTNet3Release,
		ButtonFunctionPTTNet4Release,
		ButtonFunctionPTTNet5Release,
		ButtonFunctionPTTNet6Release,
		ButtonFunctionPTTNet7Release,
		ButtonFunctionPTTNet8Release,
		ButtonFunctionPTTNet9Release,
		ButtonFunctionPTTNet10Release,
		ButtonFunctionPTTNet11Release,
		ButtonFunctionPTTNet12Release,
		ButtonFunctionPTTNet13Release,
		ButtonFunctionPTTNet14Release,
		ButtonFunctionPTTNet15Release,
		ButtonFunctionPTTNetSelectedRelease,
		ButtonFunction1,
		ButtonFunction2,
		ButtonFunction3,
		ButtonFunction4,
		ButtonFunction5,
		ButtonFunction6,
		ButtonFunctionClick,
		ButtonFunctionNotifyMediumClick,
		ButtonFunctionLastMarker
	} ButtonFunction;

	enum {
		ButtonConfigLeft=0,
		ButtonConfigRight,
		ButtonPTT1,
		ButtonPTT2,
		ButtonExtPTT1,
		ButtonExtPTT2,

		ButtonLastMarker,
	};


	class ButtonSetup{
	public:
		std::list<ButtonFunction> _ButtonShortClick;
		std::list<ButtonFunction> _ButtonMediumClick;
		std::list<ButtonFunction> _ButtonLongClick;
		std::list<ButtonFunction> _ButtonRelease;
	};


	Buttons(uint8_t queuesize, void (*shortclicknotify)()=0, void (*mediumclicknotify)()=0, void (*longclicknotify)()=0);
	void Poll(uint32_t ButtonKeys, uint32_t TimeStampMs);
	Buttons::ButtonFunction  GetButtonFunctionFromQueue();
	void SetTestMode(uint8_t testmode);
protected:
	uint32_t ButtonPressTime[ButtonLastMarker];
	uint32_t ButtonPressStatus;
	uint32_t ButtonPressShortMask;
	uint32_t ButtonPressMediumMask;
	uint32_t ButtonPressLongMask;
	uint32_t ButtonPressTreatedMask;
	uint8_t QueueSize;
	uint16_t ButtonMediumClickTimeMs;
	uint16_t ButtonLongClickTimeMs;
	ButtonFunction *ButtonShortClick;
	ButtonFunction *ButtonMediumClick;
	ButtonFunction *ButtonLongClick;
	ButtonFunction *ButtonRelease;
	std::queue<uint32_t> Queue;
	static const ButtonFunction ButtonShortClickDefault[];
	static const ButtonFunction ButtonMediumClickDefault[];
	static const ButtonFunction ButtonLongClickDefault[];
	static const ButtonFunction ButtonReleaseDefault[];
	static const ButtonFunction ButtonShortClickTest[];
	static const ButtonFunction ButtonMediumClickTest[];
	static const ButtonFunction ButtonLongClickTest[];
	static const ButtonFunction ButtonReleaseTest[];

	void Enqueue(ButtonFunction function);

};
