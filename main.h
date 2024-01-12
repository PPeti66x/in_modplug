#ifndef MAINHEADERINCLUDED
#define MAINHEADERINCLUDED

DWORD WINAPI ConfigDlgProc(HWND, UINT, WPARAM, LPARAM);
#define DEFAULTFORMATS "MOD;STM;S3M;FST;XM;IT;669;AMF;AMS;DBM;DMF;DSM;FAR;M15;MDL;MED;MT2;MTM;NST;OKT;PTM;STK;ULT;WOW;PSM;UMX;MO3;"
#define DEFAULTFORMATSNAME "Music Modules"

typedef struct _Plugin_Settings
{
	//mFlags: 1 - Use volume control on ModPlug's internal mixer instead of Winamp's.
	//mFlags: 2 - internal use only
	//mFlags: 4 - Seek by orders instead of samples, more accurate playback when seeking.
	int pFlags;          /* One or more bitwise-OR'ed values */
	int pInfoRefresh;    /* Refresh time for file info dialog, in ms */
	int pInfoBuffering;  /* Buffer length when dialog window is active, in ms */
	int pUseInfotimer;   /* internal use only: 1 if INFOTIMER is defined, otherwise 0 */
	int pUseAGC;         /* Automatic Gain Control: 0 = disable (default), 1 = enable */
	int pPreAmp;         /* Pre-amplification: 0 = disabled, reange 32 - 100 overrides the default value */
	int pFormatsSize;    /* Allocated Char array length for pFormats */
	int pIniFNSize;      /* Allocated Char array length for pIniFN */
	int isChanged;       /* internal use only: indicates that settings was changed */
	int isReset;         /* internal use only: indicates that reset button was clicked */
	char * pFormats;     /* spaceless list of semicolon separated file extensions inc. format name at end */
	char * pIniFN;       /* internal use only: pointer to the INI file */
} Plugin_Settings;

typedef struct _All_Settings
{
	ModPlug_Settings * mp_settings; /* holds the immediate (temporary) configuration values */
	Plugin_Settings * mpp_settings; /* holds the immediate (temporary) configuration values */
	ModPlug_Settings * mp_backup;   /* holds the initial values, then returns the confirmed values */
	Plugin_Settings * mpp_backup;   /* holds the initial values, then returns the confirmed values */
} All_Settings;

#ifdef USEEXTRAFEATURES
//sndfile.h requires C++ compilation
#include "libmodplug_0_8_8_x\src\libmodplug\sndfile.h"
typedef struct _ModPlugFile
{
	CSoundFile mSoundFile;
} ModPlugFile;
#endif

#endif