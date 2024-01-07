#pragma once
#include "types.h"
/**
* Use model은 개수가 정해진, processing time이 필요한 HW를 modeling한 것이다. 
* 예를 들어, DMA engine은 copy 양에 따라서 정해진 시간이 필요하며, 
* request는 queueing되어 있다가, DMA channel이 비게 되면, request를 하나씩 처리할 수 있다. 
*/

#define SIZE_HWA_QUEUE		(100)

typedef enum _HwaId
{
	HWA_NCH,
	HWA_COMP,
	NUM_HWA,
} HwaId;

const uint32 anHwaCnt[NUM_HWA] =
{
	1,1,
};

/**

*/
void HWA_Request(HwaId eId, uint32 nDuration, Cbf cbDone, uint32 nTag);
void HWA_Init();
