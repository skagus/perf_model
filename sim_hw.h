#pragma once
/**
* HW 전용 include file.
* 
*/

#include "types.h"
#include "sim.h"
#include "sim_conf.h"

#define BYTE_PER_EVT	(32)	///< Max sizeof event.


typedef void (*EvtHdr)(void* pEvt);

//////////////////////////////////////

void SIM_AddHW(HwID id, EvtHdr pfEvtHandler);
void* SIM_NewEvt(HwID eOwn, SimTick time);

bool SIM_PeekTick(SimTick nTick);
void SIM_SwitchToSim();		///< used by CPU.

