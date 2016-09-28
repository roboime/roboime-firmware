#pragma once
#include <inttypes.h>
#include <algorithm>
#include <initializer_list>

class IIR_Filter{
public:
	IIR_Filter(std::initializer_list<float> num, std::initializer_list<float> den):
		_ncoef(std::max(num.size(), den.size())),
		_num(new float[_ncoef]),
		_den(new float[_ncoef]),
		_mnum(new float[_ncoef]),
		_mden(new float[_ncoef])
	{
		std::fill(_mnum, _mnum+ _ncoef, 0.0f);
		std::fill(_mden, _mden+ _ncoef, 0.0f);
		std::fill(_num, _num+ _ncoef, 0.0f);
		std::fill(_den, _den+ _ncoef, 0.0f);
		std::copy(num.begin(), num.end(), _num);
		std::copy(den.begin(), den.end(), _den);
	}

	IIR_Filter(float *num, float *den, uint16_t ncoef):
		_ncoef(ncoef),
		_num(num),
		_den(den),
		_mnum(new float[ncoef]),
		_mden(new float[ncoef])
	{
		std::fill(_mnum, _mnum+ _ncoef, 0.0f);
		std::fill(_mden, _mden+ _ncoef, 0.0f);
	}
	~IIR_Filter(){
		delete _mnum;
		delete _mden;
	}

	float Filter(float input){
		register uint16_t i;
		register float saida;
		register float *mnum=_mnum;
		register float *mden=_mden;
		register float *num=_num;
		register float *den=_den;

		for(i=(_ncoef-1);i;i--){
			mnum[i]=mnum[i-1];
			mden[i]=mden[i-1];
		}
		mnum[0]=input;
		saida=mnum[0]*num[0];
		for(i=1;i<_ncoef;i++){
			saida+=mnum[i]*num[i];
			saida-=mden[i]*den[i];
		}
		saida/=den[0];
		mden[0]=saida;
		return saida;
	}
protected:
	uint16_t _ncoef;
	float *_num;
	float *_den;
	float *_mnum;
	float *_mden;
};
