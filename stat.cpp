#include "sim.h"
#include "models.h"

#define MAX_RSP_LOG				(1000000)
#define PRINTF(...)				SIM_Print(__VA_ARGS__)

uint32 gaRspLog[MAX_RSP_LOG];
SimTick gnAccResp;
SimTick gnStart;
uint32 gnCntResp;
SweepEntry* gpCurSweep;

SimTick gnAccDieBusy;
SimTick gnAccCmdBusy;
SimTick gnAccBufBusy;

void STAT_Cmd(CmdInfo* pCmd)
{
	uint32 nResp = (uint32)(SIM_GetTick() - pCmd->nIssueTime);

	if (gnCntResp < MAX_RSP_LOG)
	{
		gnAccResp += nResp;
		gaRspLog[gnCntResp] = nResp;
		gnCntResp++;
	}
	gnAccCmdBusy += nResp;
}

void STAT_Die(uint32 nDieId, uint32 nBusyTime)
{
	gnAccDieBusy += nBusyTime;
}

void STAT_Buf(uint32 nBufId, uint32 nBusyTime)
{
	gnAccBufBusy += nBusyTime;
}


void STAT_Start(SweepEntry* pEntry)
{
	gpCurSweep = pEntry;
	gnStart = SIM_GetTick();
	gnCntResp = 0;
	gnAccResp = 0;
	gnAccCmdBusy = 0;
	gnAccDieBusy = 0;
	gnAccBufBusy = 0;
}

extern uint32 gnRestCmd;

void STAT_PrintUtil(SweepEntry* pCur)
{
	ASSERT(0 == gnRestCmd);
	float fThrou = (float)gnCntResp * TIME_PROG / (SIM_GetTick() - gnStart);
	float fAveRsp = (float)gnAccResp / gnCntResp;
	SimTick nTotal = SIM_GetTick() - gnStart;
	PRINTF("\n");
	PRINTF("[%s] Die: %3d, Buf: %3d, QD*Size: %3d\n",
		gaszScheme[pCur->nScheme], pCur->nDie, pCur->nBuf, pCur->nQD * pCur->nCmdSize);
	PRINTF("[Utilization]: Die: %3.1f,  Buf=> %3.1f,  QD*Size=> %3.1f\n",
		(float)gnAccDieBusy / nTotal,
		(float)gnAccBufBusy / nTotal,
		(float)gnAccCmdBusy * pCur->nCmdSize / nTotal);
	PRINTF("[Throughput ]: % 4.1f, Resp Ave : % 8.1f\n", fThrou, fAveRsp);
}

int _comp(const void* pLeft, const void* pRight){return (*(int32*)pLeft - *(int32*)pRight);}
void STAT_PrintQoS()
{
	qsort(gaRspLog, gnCntResp, sizeof(uint32), _comp);

	char cMark = ' ';
	char cMark2 = ' ';
	if ((float)gaRspLog[(int)(gnCntResp * 0.9)] / gaRspLog[(int)(gnCntResp * 0.1)] > 1.3)
	{
		cMark = '*';
	}
	float fLastRsp = (float)gaRspLog[(int)(gnCntResp - 1)];
	float fHalfRsp = (float)gaRspLog[(int)(gnCntResp / 2)];
	if (fHalfRsp * 2 < fLastRsp)
	{
		cMark2 = '*';
	}
	PRINTF("[QoS] 1st: %3d, Last: %3d 10: %3d, 50: %3d\t 90: %3d %3d %3d %3d : 99.99\t(%c  %c)\n",
		gaRspLog[0], gaRspLog[(int)(gnCntResp - 1)],
		gaRspLog[(int)(gnCntResp * 0.1)],
		gaRspLog[(int)(gnCntResp * 0.5)],

		gaRspLog[(int)(gnCntResp * 0.9)],
		gaRspLog[(int)(gnCntResp * 0.99)],
		gaRspLog[(int)(gnCntResp * 0.999)],
		gaRspLog[(int)(gnCntResp * 0.9999)],
		
		cMark, cMark2);
}
