#pragma once
#include <utils/iir_filter.h>

class CVSD{
protected:
	signed short cvsd_step_min;
	float bpinnum[3];
	float bpinden[3];
	IIR_Filter bpinfilter;
	float bpoutnum[3];
	float bpoutden[3];
	IIR_Filter bpoutfilter;
private:
	signed int cvsd_syl_decay;
	signed int cvsd_pcp_decay;
	signed int cvsd_dec_ref;
	signed int cvsd_dec_step;
	unsigned char cvsd_dec_companding;
	signed int cvsd_enc_ref;
	signed int cvsd_enc_step;
	signed char cvsd_enc_companding;
	signed short cvsd_step_max;
	unsigned char cvsd_companding_mask;
public:
	CVSD(uint16_t freq_sample, uint16_t step_min=1000, uint16_t step_max=30000, uint16_t companding_bits=3);
	void Code(const int16_t *Source, uint8_t *Dest, uint16_t NrSamples);
	void Decode(const uint8_t *Source, int16_t *Dest, uint16_t NrSamples);
};
