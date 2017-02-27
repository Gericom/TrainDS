#ifndef __DecodeARMv5Stride256VXS2_H__
#define __DecodeARMv5Stride256VXS2_H__

typedef struct
{
	uint8_t* Data;
	uint32_t Width;
	uint32_t Height;
	uint8_t* Y[6];
	uint8_t* UV[6];
	void* Table0;
	void* Table1;
	uint8_t* MinMaxTable;
	uint32_t Quantizer;
	uint32_t YuvFormat;
	uint32_t Internal[392];
} AsmData;

extern uint8_t DecodeARMv5Stride256VXS2[26048];

static inline int DecodeARMv5Stride256VXS2_Func(AsmData* Data)
{
	return ((int(*)(AsmData*)) &DecodeARMv5Stride256VXS2[0])(Data);
}

#endif