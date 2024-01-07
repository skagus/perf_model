#pragma once


#define SIM_NSEC(x)		(SimTick)(x)
#define SIM_USEC(x)		SIM_NSEC((x)*1000UL)
#define SIM_MSEC(x)		SIM_USEC((x)*1000UL)
#define SIM_SEC(x)		SIM_MSEC((x)*1000UL)


/**
HW ID: 같은 handler를 사용하게 된다.
*/
enum HwID
{
	HW_HWA,
	HW_RSC,
	HW_TEST,	///< 
	NUM_HW,
};




