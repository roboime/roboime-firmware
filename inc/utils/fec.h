#pragma once

#include <stddef.h>
#include <inttypes.h>
#define NOF(X) (sizeof(X) / sizeof(X[0]))

class BCH{
public:
	static uint64_t Encode_64_16_parity(uint16_t word){
		static const uint16_t matriz[]={
				0x8000, 0x4000, 0x2000, 0x1000, 0x0800, 0x0400, 0x0200, 0x0100, 0x0080, 0x0040, 0x0020, 0x0010, 0x0008, 0x0004, 0x0002, 0x0001,
				0xEC47, 0x9A64, 0x4D32, 0x2699, 0xFF0B, 0x93C2, 0x49E1, 0xC8B7, 0x881C, 0x440E, 0x2207, 0xFD44, 0x7EA2, 0x3F51, 0xF3EF, 0x95B0,
				0x4AD8, 0x256C, 0x12B6, 0x095B, 0xE8EA, 0x7475, 0xD67D, 0x8779, 0xAFFB, 0xBBBA, 0x5DDD, 0xC2A9, 0x8D13, 0xAACE, 0x5567, 0xC6F4,
				0x637A, 0x31BD, 0xF499, 0x960B, 0xA742, 0x53A1, 0xC597, 0x8E8C, 0x4746, 0x23A3, 0xFD96, 0x7ECB, 0xD322, 0x6991, 0xD88F, 0x0003,
		};
		uint64_t response=0;
		uint16_t i;
		uint16_t size=sizeof(matriz)/sizeof(matriz[0]);
		for(i=0;i<size;i++){
			response|=(countparity(word & matriz[i])<<((uint64_t)63-i));
		}
		return response;
	}

	static uint64_t
	bch_63_16_encode(uint16_t value)
	{
	   static const uint64_t encoding_matrix[] = {
	      0x8000cd930bdd3b2aLL,
	      0x4000ab5a8e33a6beLL,
	      0x2000983e4cc4e874LL,
	      0x10004c1f2662743aLL,
	      0x0800eb9c98ec0136LL,
	      0x0400b85d47ab3bb0LL,
	      0x02005c2ea3d59dd8LL,
	      0x01002e1751eaceecLL,
	      0x0080170ba8f56776LL,
	      0x0040c616dfa78890LL,
	      0x0020630b6fd3c448LL,
	      0x00103185b7e9e224LL,
	      0x000818c2dbf4f112LL,
	      0x0004c1f2662743a2LL,
	      0x0002ad6a38ce9afbLL,
	      0x00019b2617ba7657LL
	   };

	   uint64_t cw = 0LL;
	   for(uint8_t i=0; i < 16; ++i) {
	      if(value & (1 << (15 - i))) {
	         cw ^= encoding_matrix[i];
	      }
	   }
	   return cw;
	}


	static uint16_t	bch_63_16_decode(uint64_t codeword, uint8_t *nof_errs=0)
	{
		const uint8_t N = 63;
		const uint8_t K = 16;
		const uint8_t T = 11;
		const uint8_t TWO_T = 2 * T;


		static const int8_t GF_EXP[64] = {
				1, 2, 4, 8, 16, 32, 3, 6, 12, 24, 48, 35, 5, 10, 20, 40,
				19, 38, 15, 30, 60, 59, 53, 41, 17, 34, 7, 14, 28, 56, 51, 37,
				9, 18, 36, 11, 22, 44, 27, 54, 47, 29, 58, 55, 45, 25, 50, 39,
				13, 26, 52, 43, 21, 42, 23, 46, 31, 62, 63, 61, 57, 49, 33, 0
		};

		static const int8_t GF_LOG[64] = {
				-1, 0, 1, 6, 2, 12, 7, 26, 3, 32, 13, 35, 8, 48, 27, 18,
				4, 24, 33, 16, 14, 52, 36, 54, 9, 45, 49, 38, 28, 41, 19, 56,
				5, 62, 25, 11, 34, 31, 17, 47, 15, 23, 53, 51, 37, 44, 55, 40,
				10, 61, 46, 30, 50, 22, 39, 43, 29, 60, 42, 21, 20, 59, 57, 58
		};

		uint8_t nerrs = 0;
		int8_t syndromes[TWO_T];
		for(size_t i = 0; i < NOF(syndromes); ++i) {
			syndromes[i] = 0;
			for(size_t j = 0; j < N; ++j) {
				if(codeword & (((uint64_t)1)<<(j+1))){
					syndromes[i] ^= GF_EXP[((i + 1) * j) % 63];
				}
			}
			if(syndromes[i]) {
				++nerrs;
			}
			syndromes[i] = GF_LOG[syndromes[i]];
		}

		if(nerrs) {

			int8_t elp[24][TWO_T];
			int8_t discrepancy[23], L[24], uLu[24];
//			int8_t root[K],
			int8_t locn[K];
			int i,j,U,q,count;

			// L[0]
			     L[0] = 0;
			uLu[0] = -1;
			discrepancy[0] = 0;
			elp[0][0] = 0;

			// L[1]
			     L[1] = 0;
			uLu[1] = 0;
			discrepancy[1] = syndromes[0];
			elp[1][0] = 1;

			// setup first two rows of elp matrix
			for(i = 1; i <= 21; i++) {
				elp[0][i] = -1;
				elp[1][i] = 0;
			}

			U = 0;
			do {
				U = U + 1;
				if(-1 == discrepancy[U]) {
					L[U + 1] = L[U];
					for(i = 0; i <= L[U]; i++) {
						elp[U + 1][i] = elp[U][i];
						elp[U][i] = GF_LOG[elp[U][i]];
					}
				} else {

					// search for words with greatest uLu(q) for which d(q)!=0
							q = U - 1;
							while((-1 == discrepancy[q]) && (q > 0)) {
								q = q - 1;
							}
							// have found first non-zero d(q)
							if(q > 0) {
								j = q;
								do {
									j = j - 1;
									if((discrepancy[j] != -1) && (uLu[q] < uLu[j])) {
										q = j;
									}
								} while(j > 0);
							}

							// store degree of new elp polynomial
							if(L[U] > L[q] + U - q) {
								L[U + 1] = L[U];
							} else {
								L[U + 1] = L[q] + U - q;
							}

							// form new elp(x)
							for(i = 0; i <= 21; i++) {
								elp[U + 1][i] = 0;
							}
							for(i = 0; i <= L[q]; i++) {
								if(elp[q][i] != -1) {
									elp[U + 1][i + U - q] = GF_EXP[(discrepancy[U] + 63 - discrepancy[q] + elp[q][i]) % 63];
								}
							}
							for(i = 0; i <= L[U]; i++) {
								elp[U + 1][i] = elp[U + 1][i] ^ elp[U][i];
								elp[U][i] = GF_LOG[elp[U][i]];
							}
				}
				uLu[U + 1] = U - L[U + 1];

				// form (u+1)th discrepancy
				if(U < (int)TWO_T) {
					// no discrepancy computed on last iteration
					if(syndromes[U] != -1) {
						discrepancy[U + 1] = GF_EXP[syndromes[U]];
					} else {
						discrepancy[U + 1] = 0;
					}
					for(i = 1; i <= L[U + 1]; i++) {
						if((syndromes[U - i] != -1) && (elp[U + 1][i] != 0)) {
							discrepancy[U + 1] ^= GF_EXP[(syndromes[U - i] + GF_LOG[elp[U + 1][i]]) % 63];
						}
					}
					// convert d(u+1) into index form */
					discrepancy[U + 1] = GF_LOG[discrepancy[U + 1]];
				}
			} while((U < 22) && (L[U + 1] <= 11));

			++U;
			if(L[U] <= 11) {
				// Can correct errors
				// put elp into index form
				for(i = 0; i <= L[U]; i++) {
					elp[U][i] = GF_LOG[elp[U][i]];
				}

				//Chien search: find roots of the error location polynomial
				int reg[12];
				for(i = 1; i <= L[U]; i++) {
					reg[i] = elp[U][i];
				}
				count = 0;
				for(i = 1; i <= 63; i++) {
					q = 1;
					for(j = 1; j <= L[U]; j++) {
						if( reg[j] != -1) {
							reg[j] =(reg[j] + j) % 63;
							q = q ^ GF_EXP[reg[j]];
						}
					}
					if(0 == q) {
						//store root and error location number indices
//						root[count] = i;
						locn[count] = 63 - i;
						++count;
					}
				}
				if(count == L[U]) {
					// no. roots = degree of elp hence <= t errors
					for(i = 0; i <= L[U] - 1; i++) {
						codeword ^= ((uint64_t)1)<<(locn[i]+1);
					}
					nerrs = count;
				} else {
					nerrs = 0xff;
				}
			} else {
				nerrs = 0xff;
			}
		}

		if(nof_errs) {
			*nof_errs = nerrs;
		}

		uint16_t val =codeword>>48 & 0xFFFF;

		return val;
	}


	static uint64_t Decode_64_16_parity(uint64_t code)
	{
		code>>=1;
		uint8_t s[22];
		uint64_t codec=code;
		//entrada com 63 bits
		//se entrada com 64bits (63 + 1bit de paridade)
		//neste caso c=c/2; (elimina-se o bit de paridade)
		//até 11 erros colocados nas posições "x"

		//cada valor da matriz equivale a uma coluna de 63 bits
		//cada AND de um elemento de "H" com o código modulado resulta em um dos 132 bits do vetor de Sindrome "s"


		uint16_t size=sizeof(H)/sizeof(H[0]);
		for(uint16_t i=0;i<size;i++){
			if(i%6==0){
				s[i/6]=0;
			}
			s[i/6]|=countparity(code & H[i])<<(5-i%6);
		}


		//CALCULO DE LAMBDA (representado por c)
		//C=c0 + c1 x + c2 x^2 + ... + c22 x^22
		//em que
		//c0=1;
		//c1= X1 + X2 + X3 + X4 + ... + X11 (até 11 erros)
		//c2= X1X2 + X1X3+ ... + X1X10 + X1X11 + X2X3 + X2X4 + ...+ X2X11 + X3X4 + ... + X10X11
		//c2 é o somatório de todos XiXj com i<j;
		//e assim sucessivamente

		uint16_t L=0; //número de erros
		uint16_t d;        //6 bits
		uint16_t dm=000001;//d anterior
		uint16_t p[23]={1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};// grau 22
		uint16_t t[23];
		uint16_t k,soma;
		uint16_t c[23]={1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};// grau 22
		uint16_t l=1;
		uint16_t i,j,aux;

		for(k=1;k<=22;k=k+2){
			soma=0;
			if(L>0){
				for(i=1;i<=L;i++){
					soma=soma^produto(c[i],s[k-i-1]);
				}
			}

			d=s[k-1]^soma;
			if(d==0)
				l=l+1;
			else
			{
				if( (2*L) >= k )
				{
					aux=produto(d,inverso(dm));
					for(j=l;j<=22;j++)
						c[j]=c[j] ^ (produto(aux,p[j-l]));
					l=l+1;
				}
				else
				{
					for(j=0;j<=22;j++)
						t[j]=c[j];

					aux=produto(d,inverso(dm));
					for(j=l;j<=22;j++)
						c[j]=c[j] ^ (produto(aux,p[j-l]));
					L=k-L;
					for(j=0;j<=22;j++)
						p[j]=t[j];
					dm=d;
					l=1;
				}
			}
			l=l+1;
		}

		//cálculo das raízes de LAMBDA (c)
		//serão substituídos todos os valores de alfa no polinômio c
		//os que satisfizerem a equação c(alfa(i))=0 serão as raízes
		//cada alfa que satisfizer representa um erro e o seu índice "i" sua localização

		l=0;//anda no vetor de erros
		for(i=1;i<=63;i++)
		{
			aux=0;
			for(j=0;j<23;j++)
				aux=aux ^ produto( c[j] , potencia(alfa[i],j) );

			if(aux==0)
			{
				codec^=((uint64_t)1<<(63-i));
				l++;
			}
		}
		codec<<=1;
		//TODO Calculate Parity Bit
		return codec;
	}

protected:
	template <class T> static uint64_t countparity(T value){
		uint8_t count=0;
		while(value){
			if(value & 1){
				count++;
			}
			value>>=1;
		}
		return(count & 1);
	}

	static uint16_t produto(uint16_t num, uint16_t num1) //produto no GF
	{
		int i,aux=0,prod;

		for(i=0;i<6;i++)
		{
			if(num & (1<<i))
				aux=aux ^ (num1<<i);
		}
		prod=divisao(aux);
		return prod;
	}


	static uint16_t divisao(uint16_t num)	//divisão no GF
	{
		uint16_t tamden=7;
		uint16_t den=67;

		uint16_t i;
		uint16_t aux;
		aux= num >> (11-tamden);
		for(i=0;i<=11-tamden;i++)
		{
			if( (  aux & (((uint16_t)1)<<(tamden-1))  ) )
				aux= ((aux^den));
			if(i!=(11-tamden))
			{
				aux=aux<<1;
				if ( num & (((uint16_t)1)<<(10-tamden-i)) )
					aux= aux + 1 ;
			}
		}
		return aux;
	}


	static uint16_t inverso(uint16_t num) //inverso no GF
	{
		uint16_t cont=0;
		while(num!=alfa[cont])
			cont++;
		return alfa[63-cont];
	}


	static uint16_t potencia(uint16_t num, uint16_t indice) //potencia no GF
	{
		if(indice==0)
		{
			if(num)
				return 1;
			else
				return 0;
		}
		uint16_t pot=1;
		for (uint16_t i=0;i<indice;i++)
			pot=produto(pot,num);

		return pot;
	}
	static const uint8_t alfa[];
	static const uint64_t H[132];
};

