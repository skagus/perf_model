#include "templ.h"
#include "sim.h"
#include "sim_hw.h"
#include "rsc_model.h"

struct RscEvt
{
	RscId eId;
};

struct RscReqInfo
{
	CbAlloc cbDone;
	uint32 nTag;

	RscReqInfo() {} // default constructor needed for Queue.
	RscReqInfo(CbAlloc cbF, uint32 nTg)
	{
		cbDone = cbF;
		nTag = nTg;
	}
};

uint32 ganRscAvail[NUM_RSC];
bool gabRscEvt[NUM_RSC];		///< Event가 schedule되어 있는 상태..
Queue<RscReqInfo, SIZE_RSC_QUEUE> gaReq[NUM_RSC];

void rsc_NewEvt(RscId eId)
{
	if(false == gabRscEvt[eId])
	{
		RscEvt* pNew = (RscEvt*)SIM_NewEvt(HW_RSC, 0);
		pNew->eId = eId;
		gabRscEvt[eId] = true;
	}
}

void rsc_HandleEvt(void* pEvt)
{
	RscEvt* pstEvt = (RscEvt*)pEvt;
	RscId eId = pstEvt->eId;

	while (ganRscAvail[eId] > 0 && gaReq[eId].Count() > 0)
	{
		RscReqInfo stReq = gaReq[eId].PopHead();
		ganRscAvail[eId]--;
		stReq.cbDone(0, stReq.nTag);
	}	
	gabRscEvt[pstEvt->eId] = false;
}


void RSC_Allocate(RscId eId, CbAlloc cbDone, uint32 nTag)
{
	RscReqInfo stReq(cbDone, nTag);
	gaReq[eId].PushTail(stReq);
	rsc_NewEvt(eId);
}

void RSC_Release(RscId eId, uint32 nRscId)
{
	ganRscAvail[eId]++;
	rsc_NewEvt(eId);
}


void RSC_Init()
{
	for (uint32 nId = 0; nId < NUM_RSC; nId++)
	{
		ganRscAvail[nId] = anRscCnt[nId];
	}
	SIM_AddHW(HW_RSC, rsc_HandleEvt);
}
