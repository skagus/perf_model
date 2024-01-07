#pragma once
#include "types.h"
/**
* Use model�� ������ ������, processing time�� �ʿ��� HW�� modeling�� ���̴�. 
* ���� ���, DMA engine�� copy �翡 ���� ������ �ð��� �ʿ��ϸ�, 
* request�� queueing�Ǿ� �ִٰ�, DMA channel�� ��� �Ǹ�, request�� �ϳ��� ó���� �� �ִ�. 
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
