
#include "sim_hw.h"
#include "rsc_model.h"
#include "hwa_model.h"

/*
* 이 Memory 할당 --> HWA 를 차례 사용 --> Memory Free.
*/

struct TestEvt
{
	uint32 nSeq;
};


void test_HwaDone(uint32 nTag, uint32 nResult);
void test_AllocCbf(uint32 nRscId, uint32 nTag);


void test_NewEvt(uint32 nSeqId)
{
	TestEvt* pNew = (TestEvt*)SIM_NewEvt(HW_TEST, 0);
	pNew->nSeq = nSeqId;
}


void test_HandleEvt(void* pEvt)
{
	TestEvt* pstEvt = (TestEvt*)pEvt;
	uint32 nSeq = pstEvt->nSeq;

	SIM_Print("T Evt: %d\n", nSeq);

	RSC_Allocate(RscId::RSC_MEM, test_AllocCbf, nSeq);
}


void test_HwaDone(uint32 nTag, uint32 nResult)
{
	SIM_Print("HW DONE: %d, %d\n", nTag, nResult);
	nTag++;
	if(nTag < HwaId::NUM_HWA)
	{
		HWA_Request(HwaId::HWA_COMP, SIM_MSEC(10), test_HwaDone, nTag);
	}
	else
	{
		RSC_Release(RscId::RSC_MEM, 0);
	}
}


void test_AllocCbf(uint32 nRscId, uint32 nTag)
{
	SIM_Print("AL DONE: %d, %d\n", nTag, nRscId);
	HWA_Request(HwaId::HWA_COMP, SIM_MSEC(10), test_HwaDone, HwaId::HWA_NCH);
}


void Sample_Init(void)
{
	SIM_AddHW(HW_TEST, test_HandleEvt);
	test_NewEvt(0);
}

