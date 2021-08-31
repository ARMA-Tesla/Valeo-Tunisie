//#include "extcode.h"
#pragma pack(push)
#pragma pack(1)

#ifdef __cplusplus
extern "C" {
#endif
typedef uint16_t  Enum;
#define Enum_ButterworthFilter 0
#define Enum_ChebyshevFilter 1
#define Enum_InverseChebyshevFilter 2
#define Enum_EllipticFilter 3
#define Enum_BesselFilter 4
typedef uint32_t  Enum1;
#define Enum1_Lowpass 0
#define Enum1_Highpass 1
#define Enum1_Bandpass 2
#define Enum1_Bandstop 3
typedef uint32_t  Enum2;
#define Enum2_Peaks 0
#define Enum2_Valleys 1

/*!
 * Filtre
 */
void __cdecl Filtre(double TorqueIN[], int32_t len_TorqueIN, Enum FiltreType, 
	Enum1 FilterBandType, int32_t Order, double highCutoffFreqFh, 
	double lowCutoffFreqFl, double samplingFreqFs, double FilteredSignalOUT[], 
	int32_t *len_FilteredSignalOUT);
/*!
 * Peaks_Detactor
 */
void __cdecl Peaks_Detactor(double FiltredSignal[], 
	int32_t len_FiltredSignal, Enum2 peaksValleys, int32_t width, 
	double threshold, int32_t *NumOfPeaksValleys, int32_t Locations[], 
	int32_t *len_Locations);

//MgErr __cdecl LVDLLStatus(char *errStr, int errStrLen, void *module);

void __cdecl SetExcursionFreeExecutionSetting(int value);

#ifdef __cplusplus
} // extern "C"
#endif

#pragma pack(pop)

