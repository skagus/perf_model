

#include "templ.h"
#include "sim_hw.h"
#include "hwa_model.h"

struct HwaReqInfo
{
	uint32 nDuration;
	Cbf cbDone;
	uint32 nTag;
	HwaReqInfo() {} // default constructor needed for Queue.
	HwaReqInfo(uint32 nDur, Cbf cbF, uint32 nT)
	{
		nDuration = nDur;
		cbDone = cbF;
		nTag = nT;
	}
};
struct HwaEvt
{
	HwaId eHwaId;
	HwaReqInfo stReq;
};
uint32 ganHwaAvail[NUM_HWA];
Queue<HwaReqInfo, SIZE_HWA_QUEUE> gaReq[NUM_HWA];

HwaEvt* hwa_StartNew(HwaId eId, HwaReqInfo* pstReq)
{
	HwaEvt* pNew = (HwaEvt*)SIM_NewEvt(HW_HWA, pstReq->nDuration);
	pNew->eHwaId = eId;
	pNew->stReq = *pstReq;
	ganHwaAvail[eId]--;
	return pNew;
}

void hwa_HandleEvt(void* pEvt)
{
	HwaEvt* pstEvt = (HwaEvt*)pEvt;
	HwaReqInfo* pReq = &(pstEvt->stReq);
	ganHwaAvail[pstEvt->eHwaId]++;
	pReq->cbDone(pReq->nTag, 0);

	if (gaReq[pstEvt->eHwaId].Count() > 0)
	{
		HwaReqInfo stReq = gaReq[pstEvt->eHwaId].PopHead();
		hwa_StartNew(pstEvt->eHwaId, &stReq);
	}
}

// Try use the resource eID for Duration.
void HWA_Request(HwaId eId, uint32 nDuration, Cbf cbDone, uint32 nTag)
{
	HwaReqInfo stReq(nDuration, cbDone, nTag);
	if ((ganHwaAvail[eId] > 0) && (gaReq[eId].Count() <= 0))
	{
		hwa_StartNew(eId, &stReq);
	}
	else
	{
		gaReq[eId].PushTail(stReq);
	}
}

void HWA_Init()
{
	for (uint32 nId = 0; nId < NUM_HWA; nId++)
	{
		ganHwaAvail[nId] = anHwaCnt[nId];
	}
	SIM_AddHW(HW_HWA, hwa_HandleEvt);
}
