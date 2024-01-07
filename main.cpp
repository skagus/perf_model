#include "sim.h"
#include "hwa_model.h"
#include "rsc_model.h"
#include "sample_hw.h"

#define PRINTF(...)		SIM_Print(__VA_ARGS__)


int main(int argc, char* argv[])
{
	SIM_Init(0,0);

	HWA_Init();
	RSC_Init();
	
	Sample_Init();

	SIM_Run();

	SIM_Fin();
	return 0;
}
