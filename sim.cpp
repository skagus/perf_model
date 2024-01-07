
#include <queue>
#include <list>
#include <random>

#include "templ.h"
#include "sim_hw.h"

using namespace std;

/**
* Normally, one Event for each HW. (caution: count instance! NOT type)
*/
#define NUM_EVENT				(128)


struct Evt
{
public:
	SimTick	nTick;			///< Time of event occur.
	HwID	nOwner;			///< HW of the event.
	uint32	nSeqNo;
	uint8	aParams[BYTE_PER_EVT];		///< Event information for each HW.

	/**
		Event queue must be sorted with time(tick)
	*/
	bool operator()(const Evt* lhs, const Evt* rhs) const
	{
		return lhs->nTick > rhs->nTick;
	}
};


static SimTick gnTick;			///< Simulation time.
static EvtHdr gfEvtHdr[NUM_HW];	///< Event handler of each HW.
static bool gbPowerOn;			///< Power on state.
static uint32 gnCycle;
static HANDLE ghSimThread;

/// Event repository.
static Evt gaEvts[NUM_EVENT];
static std::priority_queue<Evt*, std::vector<Evt*>, Evt> gEvtQue;
static Queue<Evt*, NUM_EVENT + 1> gEvtPool;

/**
* Add Hardware.
* HW is defined by handler.
*/
void SIM_AddHW(HwID id, EvtHdr pfEvtHandler)
{
	gfEvtHdr[id] = pfEvtHandler;
}

void* SIM_NewEvt(HwID eOwn, SimTick nTick)
{
	Evt* pEvt = gEvtPool.PopHead();
	pEvt->nTick = gnTick + nTick;
	pEvt->nOwner = eOwn;
	pEvt->nSeqNo = SIM_IncSeqNo();
	gEvtQue.push(pEvt);
	return pEvt->aParams;
}

SimTick SIM_GetTick()
{
	return gnTick;
}

uint32 SIM_GetCycle()
{
	return gnCycle;
}

/**
* Check whether it can bypass tick & then bypass tick.
* @return true if peek the tick.
*/
bool SIM_PeekTick(SimTick nTick)
{
	Evt* pEvt = gEvtQue.top();
	if (gnTick + nTick < pEvt->nTick)
	{
		gnTick += nTick;
		return true;
	}
	return false;
}

/**
Initiate system. (aka. power on)
*/
static void sim_Reset()
{
	gbPowerOn = true;
	while (gEvtQue.size())
	{
		gEvtQue.pop();
	}
	gEvtPool.Init();
	for (uint32 nIdx = 0; nIdx < NUM_EVENT; nIdx++)
	{
		gEvtPool.PushTail(gaEvts + nIdx);
	}
	gnTick = 0;
}

void SIM_SwitchToSim()
{
	SwitchToFiber(ghSimThread);
}


void SIM_PowerDown()
{
	gbPowerOn = false;
}


void SIM_Run()
{
	while (true)
	{
//		sim_PowerUp();
//		SIM_Print("[SIM] ============== Power up %d =================\n", gnCycle);
		while (gbPowerOn)
		{
			if (gEvtQue.empty())
			{
				return;
			}
			Evt* pEvt = gEvtQue.top();
			gnTick = pEvt->nTick;
//			SIM_Print("EVT: %d\n", gnTick);
			gEvtQue.pop();
			gfEvtHdr[pEvt->nOwner](pEvt->aParams);
			gEvtPool.PushTail(pEvt);
		}
		gnCycle++;
	}
}


////////////////////////////////////////


static std::mt19937_64 gRand;		///< Random number generator.
static uint32 gnSeqNo;			///< Sequence number for debug.
static uint32 gnBrkSN;
static FILE* fpLog;

uint32 SIM_GetRand(uint32 nMod)
{
	return gRand() % nMod;
}

/**
* GetSeqNo와 IncSeqNo의 용법.
* IncSeqNo()는 호출을 안하면 SeqNo기반 break위치가 바뀔 수 있으므로 조심해서 사용.
* GetSeqNo()는 호출되던 곳을 제외해도 재현에 영향이 없으므로  추가/삭제가 자유로워서 print같은데 사용 가능.
*/
uint32 SIM_GetSeqNo()
{
	if (gnBrkSN == gnSeqNo)
	{
		__debugbreak();
	}
	return gnSeqNo;
}

uint32 SIM_IncSeqNo()
{
	gnSeqNo++;
	if (gnBrkSN == gnSeqNo)
	{
		__debugbreak();
	}
	return gnSeqNo;
}

#define MAX_BUF_SIZE	(512)
static char saBuf[MAX_BUF_SIZE];
void SIM_Print(const char* szFormat, ...)
{
	va_list stAP;
	va_start(stAP, szFormat);
	vsprintf_s(saBuf, MAX_BUF_SIZE, szFormat, stAP);
	va_end(stAP);
	fprintf(stdout, "%8lld.%06lld: ", SIM_GetTick() / SIM_MSEC(1), SIM_GetTick() % SIM_MSEC(1));
	fprintf(stdout, saBuf);
	fprintf(fpLog, "%8lld.%06lld: ", SIM_GetTick() / SIM_MSEC(1), SIM_GetTick() % SIM_MSEC(1));
	fprintf(fpLog, saBuf);
	fflush(fpLog);
}

void SIM_Init(uint32 nSeed, uint32 nBrkNo)
{
	gnSeqNo = 0;
	gnBrkSN = nBrkNo;

	time_t nCurTime;
	time(&nCurTime);
	struct tm stTime;
	localtime_s(&stTime, &nCurTime);
	char szName[20];
	sprintf_s(szName, 20, "sim_%02d%02d_%02d%02d%02d.log",
		stTime.tm_mon + 1, stTime.tm_mday, stTime.tm_hour, stTime.tm_min, stTime.tm_sec);
	fpLog = _fsopen(szName, "w", _SH_DENYWR);
	if (0 == nSeed)
	{
		nSeed = (uint32)nCurTime;
	}
	SIM_Print("SEED: %X\n", nSeed);
	gRand.seed(nSeed);
	sim_Reset();
}

void SIM_Fin()
{
	fflush(fpLog);
	fclose(fpLog);
}