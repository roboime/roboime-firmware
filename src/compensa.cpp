#include <stdint.h>

const float D[4][4]={
		{ -0.5, 0.8660254037844, 1.0 },
		{ -0.5, -0.8660254037844, 1.0 },
		{ 0.7071067811865, -0.7071067811865, 1.0 },
		{ 0.7071067811865, 0.7071067811865, 1.0 }
};

void multiplica1(float vet[3], float *result){
	result[0]=D[0][0]*vet[0]+D[0][1]*vet[1]+D[0][2]*vet[2];
	result[1]=D[1][0]*vet[0]+D[1][1]*vet[1]+D[1][2]*vet[2];
	result[2]=D[2][0]*vet[0]+D[2][1]*vet[1]+D[2][2]*vet[2];
	result[3]=D[3][0]*vet[0]+D[3][1]*vet[1]+D[3][2]*vet[2];
}

#define sin_phi 0.50
#define cos_phi 0.866
#define sin_theta 0.707
#define cos_theta 0.707

void multiplica(float vet[3], float *result){
	const float R = 0.075; //Raio do robo = 7.5cm
	result[0] = vet[1]*cos_phi - vet[0]*sin_phi + vet[2]*R;
	result[2] = -vet[1]*cos_phi - vet[0]*sin_phi + vet[2]*R;
	result[3] = -vet[1]*cos_theta + vet[0]*sin_theta + vet[2]*R;
	result[1] = vet[1]*cos_theta + vet[0]*sin_theta + vet[2]*R;
}

float abs(float a){
	if(a<0){
		return -a;
	} else {
		return a;
	}
}

uint8_t compensa(const float ml[4], const float v[3], float md[4], uint8_t gt[4]){
	uint8_t i=0;
	float vd[3];
	uint8_t gtnew[4];
	uint8_t result=1;
	float val=1;
	for(val=1.0;val>0.0;val-=0.01){
		for(i=0;i<3;i++){
			vd[i]=v[i]*val;
		}
		multiplica(vd, md);
		result=1;
		for(i=0;i<4;i++){
			if((md[i]*ml[i])<0.0 || abs(md[i])<abs(ml[i])){
				gtnew[i]=1;
			} else {
				gtnew[i]=0;
			}
			result*=gtnew[i];
		}
		if(result){
			return 1;
			break;
		} else {
			for(i=0;i<4;i++){
				gt[i]=gtnew[i];
			}
		}
	}
//	for(i=0;i<4;i++){
//		if(gtold[i]){
//			//sobrescrever o valor do setpoint por md[i]
//		}
//	}
	return 0;
}
