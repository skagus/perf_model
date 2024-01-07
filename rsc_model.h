#pragma once
#include "types.h"

/**
* �� model�� memory�� ���� �Ҵ��� �ʿ��� ���� �𵨸��� ���̴�. 
* ������ �ټ��� resource�� ������, resource�� �Ҵ�, ������ �����ϴ�.
* Event driven�� �����ϵ��� queue�� ����ϵ��� �Ѵ�.
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
