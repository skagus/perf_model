#pragma once
#include "types.h"

/**
* 이 model은 memory와 같이 할당이 필요한 것을 모델링한 것이다. 
* 동일한 다수의 resource가 있으며, resource를 할당, 해제가 가능하다.
* Event driven에 적합하도록 queue를 사용하도록 한다.
*/

#define SIZE_RSC_QUEUE		(100)

typedef enum _RscId
{
	RSC_MEM,
	RSC_SLOT,
	NUM_RSC,
} RscId;

const uint32 anRscCnt[NUM_RSC] =
{
	100, 10,
};

typedef void (*CbAlloc)(uint32 nRscId, uint32 nTag);

/**
* 
*/

void RSC_Allocate(RscId eId, CbAlloc cbDone, uint32 nTag);
void RSC_Release(RscId eId, uint32 nRscId);
void RSC_Init();
