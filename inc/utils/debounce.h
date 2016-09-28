#pragma once

#include <inttypes.h>
#include <utils/time_functions.h>

template <typename t> class Debounce{
public:
	Debounce(t value, uint32_t timedelayms=100, uint32_t now=0):
		_timedelayms(timedelayms),
		_lasttime(now)
	{
	}
	t& GetValue(const t &value, const uint32_t now){
		if(value!=_value_old){
			_lasttime=now;
			_value_old=value;
		}
		if(value!=_value && (now-_lasttime)>_timedelayms){
			_value=value;
			_value_old=value;
		}
		return _value;
	}
	t& GetValue(const t &value){
		return GetValue(value, GetLocalTime());
	}

	void SetValue(t value){
		_value_old=value;
		_value=value;
	}
protected:
	uint32_t _timedelayms;
	uint32_t _lasttime;

	t _value;
	t _value_old;
};
