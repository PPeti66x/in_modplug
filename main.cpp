//
// in_modplug v0.3_1
// Written by w8m
// Bugfixes by dsp2003
// Configuration GUI by PPeti66x
//

/*
#define XSTR(x) STR(x)
#define STR(x) #x
#pragma message ("The value of _MSC_VER: " XSTR(_MSC_VER))
#pragma message ("The value of _MSC_FULL_VER: " XSTR(_MSC_FULL_VER))
#pragma message ("The value of _MSC_BUILD: " XSTR(_MSC_BUILD))
*/

#ifdef LEGACYEXTENDER
#if _MSC_VER < 1400 //VC2005-
#undef LEGACYEXTENDER
#endif //VC2005-
#if _MSC_VER > 1500 //VC2008+
#undef LEGACYEXTENDER
#endif //VC2008+
#ifdef LEGACYFIX
#error ERROR: LEGACYFIX and LEGACYEXTENDER options are excusive! You can set only one of the both.
#else //LEGACYFIX
#pragma message( "Using Cloanto's Legacy Extender..." )
//NOTE: Discarding the LEGACYEXTENDER definition will not remove it.
//To remove the Legacy Extender you need to remove the .lib file in the
//linker settings and the postbuild command!
#endif //LEGACYFIX
#endif //LEGACYEXTENDER

#ifdef MFCGUI
//MFC header must be before <windows.h>!
#include <afx.h>
#endif

//If LEGACYFIX is not set, disable the post-build event command!
//In VS2008+ LEGACYFIX requires "unicows.lib" before "kernel32.lib" and
//   any MFC-dependent libraries to retain Win9x compatibility!
#ifdef LEGACYFIX
#if _MSC_VER >= 1400 //VC2005+
#if _MSC_VER <= 1600 //VC2010-
#pragma message( "Using BlackWingCat's VCFix..." )
#ifdef WINVER
#define WINVER2 WINVER
#else
#define WINVER2 0x0400
#endif
#define DLLMODE
#include "vcfix\commonfix.h"
#endif //VC2010-
#endif //VC2005+
#endif //LEGACYFIX

#define NOGDI
#define WIN32_LEAN_AND_MEAN

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <windows.h> //required by commctrl.h
//#include <commctrl.h> //this must be also included in main.rc

#include "stdint.h"
#include "libmodplug_0_8_8_x\src\modplug.h"
#include "resource.h"
#include "main.h"
#include "version.h"

#ifndef _tcscpy
  #ifdef UNICODE
    #define _tcscpy wcscpy
  #else
    #define _tcscpy strcpy
  #endif
#endif
#ifndef _tcsncpy
  #ifdef UNICODE
    #define _tcsncpy wcsncpy
  #else
    #define _tcsncpy strncpy
  #endif
#endif
#ifndef _tcslen
  #ifdef UNICODE
    #define _tcslen wcslen
  #else
    #define _tcslen strlen
  #endif
#endif

#ifdef __cplusplus
#ifndef EXTERN_C
#define EXTERN_C extern "C"
#endif
#endif

#ifdef MFCGUI
#include "config_mfc.hpp"
#endif

#undef _MSC_VER
#include "Winamp/in2.h"
#include "Winamp/wa_ipc.h"

// post this to the main window at end of file (after playback is stopped)
#define WM_WA_MPEG_EOF WM_USER+2

#define NCH mp_settings.mChannels
#define SAMPLERATE mp_settings.mFrequency
#define BPS mp_settings.mBits

#define SHORT_TITLE "in_modplug"

extern In_Module mod;

ModPlugFile *mp_file;
ModPlug_Settings mp_settings;

double decode_pos_ms;
int paused, sample_size, mp_flags, lastsize;
#ifdef INFOTIMER //defined or not in the resource.h
volatile int mp_infoflag = 0;
int mp_refresh, mp_buflen;
#endif
volatile int seek_needed, mp_exitflag;
HANDLE mp_thread = INVALID_HANDLE_VALUE;
CRITICAL_SECTION mp_lock;
HINSTANCE mp_module;

char mp_buf[576 * 8 * 4 * 2];

#ifndef VS_VERSION_INFO
#define VS_VERSION_INFO 1
#endif
#ifndef RT_VERSION
#define RT_VERSION 16
#endif
int mo3flag = 0;
HINSTANCE mo3lib = 0;
typedef int (WINAPI *UNMO3_Decode) (void **data, int *len, unsigned flags); //v2.4.0.5 and later
typedef int (WINAPI *UNMO3_DecodeOld) (void **data, int *len); //v2.4.0.3 and older
typedef void (WINAPI *UNMO3_Free) (void *data);
typedef unsigned (WINAPI *UNMO3_GetVersion) ();
void *mo3dec;
void *mo3free;

char inifn[MAX_PATH];
char lastfn[MAX_PATH];
char fileformats[256]; //this should be enough for cca. 60 extensions
#ifdef USEEXTRAFEATURES
int mp_useagc, mp_preamp;
#endif

#define LOCK EnterCriticalSection(&mp_lock)
#define UNLOCK LeaveCriticalSection(&mp_lock)

#ifndef _MFC_VER
BOOL WINAPI DllMain(HINSTANCE hinstDLL, ULONG fdwReason, LPVOID lpvReserved) {
	if(fdwReason == DLL_PROCESS_ATTACH) {
	  int n = GetModuleFileName(hinstDLL, inifn, MAX_PATH);
	  if(n) {
	    do if(inifn[--n] == '.') break; while(n);
	    if(n) strcpy(inifn + n + 1, "ini");
	  }
	  if(!n) return 0;
	  mp_module = hinstDLL;
	}
	return 1;
}
#endif

static void RemoveLeadingTrailingSpaces( const TCHAR * src, TCHAR * buffer, int bufflen, bool purge=false )
{
	//bufflen is in Bytes! buffer must have length srclength*sizeof(TCHAR)+1 or greater!
	//Note: The "\u00A0" (non-breaking space) character is not filtered because it is safe only when processed as unicode 
	//or ANSI, but not for ASCII (DOS) codepage! But there is no way to distinguish ASCII and ANSI encoded strings.
	#ifdef UNICODE
		#define _filter_str_nbsp || (src[startpos] == '0xA0')
		#define _filter_end_nbsp || (src[endpos] == '0xA0')
	#else
		#define _filter_str_nbsp  
		#define _filter_end_nbsp  
	#endif
	int charsize = sizeof(TCHAR);
	int bufflench = bufflen / charsize;
	int stringlen = _tcslen(src);
	int startpos = 0;
	int endpos = stringlen - 1;
	if ( stringlen > 0 ) {
		while ( (startpos < stringlen) && ( (src[startpos] == ' ') || (src[startpos] == '\r') || (src[startpos] == '\n') || (src[startpos] == '\t') || (src[startpos] == '\v') || (src[startpos] == '\f') _filter_str_nbsp ) ) startpos++;
		while ( (endpos >= 0) && ( (src[endpos] == ' ') || (src[endpos] == '\r') || (src[endpos] == '\n') || (src[endpos] == '\t') || (src[endpos] == '\v') || (src[endpos] == '\f') _filter_str_nbsp ) ) endpos--;
	}
	int newlen = 0;
	if (startpos > endpos) {
		//stripped string is empty
		if (purge) memset(&buffer[0], 0, bufflen); else buffer[0] = '\0';
	} else {
		newlen = (endpos + 1) - startpos;
		if (newlen >= bufflench) newlen = (bufflench - 1);
		_tcsncpy(&buffer[0], &src[startpos], newlen);
		if (purge) memset(&buffer[newlen], 0, ((bufflench - newlen) * charsize)); else buffer[newlen] = '\0';
	}
}

ModPlugFile* mp_load_mo3(char *m, int size) {
	ModPlugFile *mp_file = 0; char *d = m;

	if(!mo3flag) {
	  char fn[MAX_PATH];
	  int a, i = 0, x = 0;

	  strcpy(fn, inifn);
	  do {
	    a = fn[i++];
	    if(a == '\\' || a == '/') x = i;
	  } while(a);
	  strcpy(fn + x, "unmo3.dll");

	  mo3flag++;
	  if(mo3lib = LoadLibrary(fn)) {
	    if(mo3dec = GetProcAddress(mo3lib, "UNMO3_Decode")) mo3flag++;
	    if(mo3free = GetProcAddress(mo3lib, "UNMO3_Free")) mo3flag++;
	    void *mo3getver;
	    if(mo3getver = GetProcAddress(mo3lib, "UNMO3_GetVersion")) mo3flag++;
	    mo3flag++; //indicates that version check was done
	  }
	}
/*
	if(mo3flag == 3) {
		//version check (dll file is already loaded, so we do not want to use the GetFileVersionInfo)
		//Checking the presence of the "UNMO3_GetVersion" function seems to be a better solution than the code below.
		//The DLL remains loaded so this is a one-shot check.
		HRSRC VIrsrc = FindResource(mo3lib, MAKEINTRESOURCE(VS_VERSION_INFO), MAKEINTRESOURCE(RT_VERSION)); // handle to the resource's information block
		HGLOBAL hVIrsrc = LoadResource(mo3lib, VIrsrc); // handle to the resource itself
		DWORD VIrsrcSize = SizeofResource(mo3lib, VIrsrc); // 92 bytes is the minimal size? Or it can be 90 or even 88?
		char* ptrVIrsrc = (char*)LockResource(hVIrsrc); // pointer to the 1st byte of the resource itself
		VS_FIXEDFILEINFO VSFixedVI = *(VS_FIXEDFILEINFO*) malloc(sizeof(VS_FIXEDFILEINFO));
		memset(&VSFixedVI, 0, sizeof(VS_FIXEDFILEINFO));
		int i;
		if (ptrVIrsrc[40] = (char)0xBD) i = 40; else i = 36; //normally this should be 40
		bool b = false; //indicates that VSFixedVI is successfully filled
		DWORD dw = 0;
		while ((i <= 48) && (dw == 0)) {
			if (ptrVIrsrc[i] = (char)0xBD) {
				memcpy(&dw, &ptrVIrsrc[i], 4);
				if (dw == 0xFEEF04BD) {
					if (VIrsrcSize >= (i + sizeof(VS_FIXEDFILEINFO))) {
						memcpy(&VSFixedVI, &ptrVIrsrc[i], sizeof(VS_FIXEDFILEINFO));
						b = true;
					}
				} else {
					dw = 0;
					i++;
				}
			} else i++;
		}
		mo3flag++; //indicates that version check was done
		if ((VSFixedVI.dwFileVersionMS > 0x00020004) || ((VSFixedVI.dwFileVersionMS == 0x00020004) && (VSFixedVI.dwFileVersionLS >= 0x00000005))) mo3flag++;
		free(&VSFixedVI);
	}
*/
	if(mo3flag >= 4) {
		//this must be in separated block (out of version checking block scope) to unlock the dll.
		if (mo3flag == 5) {
			if(!((UNMO3_Decode)mo3dec)((void **)&d, &size, 0)) mp_file = ModPlug_Load(d, size); //v2.4.0.5+
		} else {
			if(!((UNMO3_DecodeOld)mo3dec)((void **)&d, &size)) mp_file = ModPlug_Load(d, size); //v2.4.0.3- and unknown
		}
		if(d && d != m) ((UNMO3_Free)mo3free)(d);
	}

	return mp_file;
}

ModPlugFile* mp_load_file(const char *fn, int *x) {
	unsigned long size; HANDLE fh; char *m = 0;
	ModPlugFile *mp_file = 0;

	fh = CreateFile(fn, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
	if(fh == INVALID_HANDLE_VALUE) return mp_file;
	size = GetFileSize(fh, 0);
	if(x) *x = size;
	if(size == -1) return mp_file;

	if(size && !(size >> 24)) m = (char *)malloc(size); 
	if(m) {
	  if(ReadFile(fh, m, size, &size, 0))
	    if(size > 4) {
	      if(m[0] == 'M' && m[1] == 'O' && m[2] == '3')
	        mp_file = mp_load_mo3(m, size);
	      else mp_file = ModPlug_Load(m, size);
	    }
	  free(m);
	}
	CloseHandle(fh);
	return mp_file;
}

DWORD WINAPI DecodeThread(LPVOID lpParameter) {
	int done = 0, n;

	while(!mp_exitflag) {

	  if(seek_needed != -1) {
	    int pos = seek_needed; seek_needed = -1; done = 0;
	    decode_pos_ms = pos;
	    LOCK;
	    ModPlug_Seek(mp_file, pos);
	    if(mp_flags & 4) {
	      int order, row;
	      ModPlug_Read(mp_file, mp_buf, sample_size);
	      order = ModPlug_GetCurrentOrder(mp_file);
	      row = ModPlug_GetCurrentRow(mp_file);

	      ModPlug_SeekOrder(mp_file, order);
	      for(;;) {
	        if(!ModPlug_Read(mp_file, mp_buf, 576 * sample_size)) break;
	        if(order != ModPlug_GetCurrentOrder(mp_file)) break;
	        if(row <= ModPlug_GetCurrentRow(mp_file)) break;
              }
	    }
	    mod.outMod->Flush(pos);
	    UNLOCK;
	  }

	  if(done) {
	    mod.outMod->CanWrite();
	    if(!mod.outMod->IsPlaying()) {
	      PostMessage(mod.hMainWindow, WM_WA_MPEG_EOF, 0, 0);
	      return 0;
	    }
	    Sleep(10);
	    continue;
	  }


#ifdef INFOTIMER
	  if(mp_buflen && mp_infoflag) {
	    n = mod.outMod->GetWrittenTime() - mod.outMod->GetOutputTime();
	    if(n > mp_buflen) {
	      Sleep(10);
	      continue;
	    }
	  }
#endif

	  if(mod.outMod->CanWrite() >= ((576 * sample_size) * (mod.dsp_isactive() ? 2 : 1))) {	
	    LOCK;
	    n = ModPlug_Read(mp_file, mp_buf, 576 * sample_size) / sample_size;
	    UNLOCK;
	    if(n) {
	      mod.SAAddPCMData(mp_buf, NCH, BPS, (int)decode_pos_ms);	
	      mod.VSAAddPCMData(mp_buf, NCH, BPS, (int)decode_pos_ms);
	      decode_pos_ms += (576 * 1000) / (double)SAMPLERATE;	

	      if(mod.dsp_isactive()) n = mod.dsp_dosamples((short *)mp_buf, n, BPS, NCH, SAMPLERATE);
	      mod.outMod->Write(mp_buf, n * sample_size);
	    } else done = -1;
	  } else Sleep(20);
	}
	return 0;
}

BOOL WriteNewINI(char * inifn) {
	BOOL isSuccess = FALSE;
	EnterCriticalSection(&mp_lock);
	HANDLE hFile = CreateFile(inifn, GENERIC_WRITE, 
		0, NULL, CREATE_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile==INVALID_HANDLE_VALUE) {
		MessageBox(0, "ERROR: Unable to open/create a new INI file!", 0, MB_OK);
	} else {
		char * defaultini = "[ModPlug]\r\n"
		";	One or more of the MODPLUG_ENABLE_* flags below, bitwise-OR'ed:\r\n"
		";	 1 - Enable oversampling (*highly* recommended)\r\n"
		";	 2 - Enable noise reduction\r\n"
		";	 4 - Enable reverb\r\n"
		";	 8 - Enable megabass\r\n"
		";	16 - Enable surround sound.\r\n"
		"Flags=3\r\n"
		"\r\n"
		";	Number of channels: 1 for mono, 2 for stereo, 4 for quad\r\n"
		"Channels=2\r\n"
		"\r\n"
		";	Bits per sample: 8, 16, or 32\r\n"
		"Bits=16\r\n"
		"\r\n"
		";	Sampling rate: 8000, 11025, 12000, 16000, 22050, 24000, 32000, \r\n"
		";	44100, 48000, 88200, 96000 (Plugin-restricted, modplug engine \r\n"
		";	itself accepts anything between 4000 - 192000.)\r\n"
		"Frequency=44100\r\n"
		"\r\n"
		";	One of MODPLUG_RESAMPLE_*, below:\r\n"
		";	0 - No interpolation (very fast, extremely bad sound quality)\r\n"
		";	1 - Linear interpolation (fast, good quality)\r\n"
		";	2 - Cubic spline interpolation (high quality)\r\n"
		";	3 - 8-tap fir filter (slow, extremely high quality)\r\n"
		"ResamplingMode=1\r\n"
		"\r\n"
		";	Stereo separation: 1 - 256\r\n"
		";	(128 is default, but 96 for headphones may be better.)\r\n"
		"StereoSeparation=128\r\n"
		"\r\n"
		";	Maximum number of mixing channels (polyphony): 32 - 256\r\n"
		"MaxMixChannels=32\r\n"
		"\r\n"
		";	Reverb level: 0 (quiet) - 100 (loud)\r\n"
		"ReverbDepth=0\r\n"
		"\r\n"
		";	Reverb delay in ms: 40 - 250 (usually 40-200ms)\r\n"
		"ReverbDelay=0\r\n"
		"\r\n"
		";	XBass level: 0 (quiet) - 100 (loud)\r\n"
		"BassAmount=0\r\n"
		"\r\n"
		";	XBass cutoff in Hz: 10 - 100\r\n"
		"BassRange=0\r\n"
		"\r\n"
		";	Surround level: 0 (quiet) - 100 (heavy)\r\n"
		"SurroundDepth=0\r\n"
		"\r\n"
		";	Surround delay in ms: 4 - 50 (usually 5-40 ms)\r\n"
		"SurroundDelay=0\r\n"
		"\r\n"
		";	Number of times to loop: -1 - 32767\r\n"
		";	Zero prevents looping, -1 loops forever.\r\n"
		"LoopCount=0\r\n"
		"\r\n"
		";	Refresh time for file info dialog, in ms: \r\n"
		";	0 (disabled) or 1 - 2000 (def.: 125)\r\n"
		"InfoRefresh=125\r\n"
		"\r\n"
		";	Buffer length when dialog window is active, in ms: \r\n"
		";	0 (no buffering) or 100 - 2000 (default: 500)\r\n"
		"InfoBuffering=500\r\n"
		"\r\n"
		";	Plugin flags:\r\n"
		";	1 - Use volume control on ModPlug's internal mixer instead of \r\n"
		";	    Winamp's. WARNING! Breaks playback when changing volume.\r\n"
		";	4 - Seek by orders instead of samples, more accurate playback \r\n"
		";	    when seeking.\r\n"
		"PluginFlags=4\r\n"
		"\r\n"
		";	Automatic Gain Control: 0 - disabled, 1 - enabled\r\n"
		";	Applies a compressor / limiter-type effect that monitors and \r\n"
		";	keeps the volume within a more-or-less steady amplification. \r\n"
		";	Softer sections will be made louder and loud sections will be \r\n"
		";	softened in volume."
		"UseAGC=0\r\n"
		"\r\n"
		";	Pre-Amp level: 0 (default) or 16 - 128.\r\n"
		";	Overrides the default pre-amplification value. This range \r\n"
		";	matches to cca. 13.25 dB. The default value is around 48, but \r\n"
		";	it depends on number of channels, and other factors.\r\n"
		"PreAmp=0\r\n"
		"\r\n"
		";	Supported file formats: A spaceless semicolon-separated list of \r\n"
		";	the file extensions, reported to Winamp. Max. 241 characters.\r\n"
		";	Changes takes effect when Winamp is restarted.\r\n"
		";	Formats: MOD;STM;S3M;FST;XM;IT;669;AMF;AMS;DBM;DMF;DSM;FAR;M15;\r\n"
		";	    MDL;MED;MT2;MTM;NST;OKT;PTM;STK;ULT;WOW;PSM;UMX;MO3;MID;\r\n"
		";	    RMI;SMF;ABC;\r\n"
		"Formats=" DEFAULTFORMATS "\r\n";

		DWORD written;
		if (WriteFile(hFile, defaultini, strlen(defaultini), &written, NULL)) {
			isSuccess = TRUE;
		} else {
			MessageBox(0, "ERROR: Unable to write to the opened INI file!", 0, MB_OK);
		}
		CloseHandle(hFile);
	}
    LeaveCriticalSection(&mp_lock);
	return isSuccess;
};

void Config(HWND hwnd) {
#ifdef NOGUICONFIG
	MessageBox(hwnd, "Use \"in_modplug.ini\" to configure plugin.", "Modplug Settings", 0);
#else
#ifndef INFOTIMER
	int	mp_refresh = 0;
	int mp_buflen = 0;
	int mp_useinfotimer = 0;
#else
	int mp_useinfotimer = 1;
#endif
#ifndef USEEXTRAFEATURES
	unsigned int mp_useagc = 0;
	unsigned int mp_preamp = 0;
#endif
	Plugin_Settings mpp_settings = {mp_flags, mp_refresh, mp_buflen, mp_useinfotimer, mp_useagc, mp_preamp, sizeof(fileformats), sizeof(inifn), 0, 0, fileformats, inifn};
#ifdef MFCGUI
	gui_edit_settings(&mp_settings, &mpp_settings, hwnd, TEXT(SHORT_TITLE));
#else //MFCGUI
	ModPlug_Settings mp_temporary = mp_settings;
	Plugin_Settings mpp_temporary = mpp_settings;
	//by assigning struct to struct integer values are copied, but content of pointers are not!
	char tmpfileformats[sizeof(fileformats)];
	char tmpinifn[MAX_PATH];
	memcpy(tmpfileformats, fileformats, sizeof(fileformats));
	memcpy(tmpinifn, inifn, sizeof(inifn));
	mpp_temporary.pFormats = tmpfileformats;
	mpp_temporary.pIniFN = tmpinifn;
    All_Settings mpas = {&mp_temporary, &mpp_temporary, &mp_settings, &mpp_settings};
	DialogBoxParam(mp_module, MAKEINTRESOURCE(IDD_CONFIG), hwnd, (DLGPROC)ConfigDlgProc, (LPARAM)&mpas);
#endif //MFCGUI
	mp_flags = mpp_settings.pFlags;
	//fileformats = mpp_settings.pFormats;
#ifdef INFOTIMER
	mp_refresh = mpp_settings.pInfoRefresh;
	mp_buflen = mpp_settings.pInfoBuffering;
#endif
	if (mpp_settings.isChanged != 0) {
		ModPlug_SetSettings(&mp_settings);
#ifdef USEEXTRAFEATURES
		if(mp_file) {
			if (mp_useagc != mpp_settings.pUseAGC) {
				if (mp_useagc != 0) mp_file->mSoundFile.SetAGC(TRUE);
				else mp_file->mSoundFile.SetAGC(FALSE);
			}
			if (mp_preamp != mpp_settings.pPreAmp) {
				if (mp_preamp > 0) mp_file->mSoundFile.m_nSongPreAmp = mp_preamp;
			}
		}
		mp_useagc = mpp_settings.pUseAGC;
		mp_preamp = mpp_settings.pPreAmp;
#endif
		sample_size = (NCH * BPS) >> 3;
	}
#endif //NOGUICONFIG
}

void About(HWND hwnd) {
	MessageBox(hwnd, "ModPlug wrapper for Winamp / libmodplug v" LIBMODPLUG_TVERSION " (" LIBMODPLUG_TDATE ")\r\n"
	  "Based on the ModPlug sound engine by Olivier Lapicque <olivierl@jps.net>,\r\n"
	  "now maintained by Konstanty Bialkowski <konstanty@ieee.org>:\r\n"
	  "https://github.com/Konstanty/libmodplug\r\n"
	  "Plugin written by w8m, updated and compiled by dsp2003 and PPeti66x.\r\n"
	  "\r\n"
	  "Version " IN_MODPLUG_TVERSION " (" IN_MODPLUG_TDATE ")\r\n"
	  "\r\n"
	  "© 2011-2012 WinKiller Studio, © 2023-2024 PPeti66x. Open Source.\r\n"
	  "http://wks.arai-kibou.ru\r\n"
	  "https://github.com/PPeti66x/in_modplug\r\n"
	  "\r\n"
#ifdef WINVER2
	  "Uses BlackWingCat's VCFix:\r\n"
	  "http://blog.livedoor.jp/blackwingcat/archives/1452362.html\r\n"
#endif //WINVER2
#ifdef LEGACYEXTENDER
	  "Uses Cloanto's Legacy Extender: http://www.legacyextender.com/\r\n"
#endif //LEGACYEXTENDER
	  ,"About Modplug Plugin", 0);
}

void Init() {
	unsigned int a; const char *iniapp = "ModPlug";

	InitializeCriticalSection(&mp_lock);
	LOCK;
	ModPlug_GetSettings(&mp_settings);

#define READINT(name, def) GetPrivateProfileInt(iniapp, name, def, inifn)
#define RANGE(val, min, max) ((unsigned int)(val - min) <= (max - min))

	a = READINT("Flags", MODPLUG_ENABLE_OVERSAMPLING | MODPLUG_ENABLE_NOISE_REDUCTION);
	mp_settings.mFlags = a & 0x1F;

	a = READINT("Channels", 2);
	if(RANGE(a, 1, 8)) mp_settings.mChannels = a;

	a = READINT("Bits", 16);
	if(RANGE(a, 8, 32) && !(a & 7)) mp_settings.mBits = a;

	a = READINT("Frequency", 48000);
	if(RANGE(a, 8000, 192000)) mp_settings.mFrequency = a;

	a = READINT("ResamplingMode", MODPLUG_RESAMPLE_LINEAR);
	if(RANGE(a, 0, 3)) mp_settings.mResamplingMode = a;

	a = READINT("StereoSeparation", 96);
	if(RANGE(a, 1, 256)) mp_settings.mStereoSeparation = a;

	a = READINT("MaxMixChannels", 128);
	if(RANGE(a, 16, 256)) mp_settings.mMaxMixChannels = a;


	a = READINT("ReverbDepth", 0);
	mp_settings.mReverbDepth = a;

	a = READINT("ReverbDelay", 0);
	mp_settings.mReverbDelay = a;

	a = READINT("BassAmount", 0);
	mp_settings.mBassAmount = a;

	a = READINT("BassRange", 0);
	mp_settings.mBassRange = a;

	a = READINT("SurroundDepth", 0);
	mp_settings.mSurroundDepth = a;

	a = READINT("SurroundDelay", 0);
	mp_settings.mSurroundDelay = a;

	a = READINT("LoopCount", 0);
	mp_settings.mLoopCount = a;

#ifdef INFOTIMER
	a = READINT("InfoRefresh", 125);
	if(!RANGE(a, 1, 2000)) a = 0;
	mp_refresh = a;

	a = READINT("InfoBuffering", 500);
	if(!RANGE(a, 100, 2000)) a = 0;
	mp_buflen = a;
#endif

	a = READINT("PluginFlags", 4);
	mp_flags = a;

#ifdef USEEXTRAFEATURES
	a = READINT("PreAmp", 0);
	if(!RANGE(a, 16, 128)) a = 0;
	mp_preamp = a;
	a = READINT("UseAGC", 0);
	mp_useagc = a;
#endif

#undef RANGE
#undef READINT

	ModPlug_SetSettings(&mp_settings);
	sample_size = (NCH * BPS) >> 3;
	UNLOCK;
}

void Quit() {
	if(mo3lib) FreeLibrary(mo3lib);
}

int GetLength() {
	int result = 0;
	if(mp_file) { LOCK; result = ModPlug_GetLength(mp_file); UNLOCK; }
	return result;
}

int GetOutputTime() {
	int time;
	LOCK;
	time = decode_pos_ms + (mod.outMod->GetOutputTime() - mod.outMod->GetWrittenTime()); 
	UNLOCK;
	return time; 
}

void SetOutputTime(int time_in_ms) {
	seek_needed = time_in_ms;
}

void SetVolume(int volume) {
	if(mp_flags & 1) {
	  if(mp_file) { LOCK; ModPlug_SetMasterVolume(mp_file, (volume << 9) / 512); UNLOCK; }
	  if(mp_flags & 2) seek_needed = GetOutputTime();
	} else {
	  mod.outMod->SetVolume(volume);
	}
}

void SetPan(int pan) {
	mod.outMod->SetPan(pan);
}

int mp_fntest(const char *fn) {
	if(!mp_file) return 1;
	if(fn && *fn) return strcmp(fn, lastfn);
	else return 0;
}

void GetFileInfo(const char *fn, char *title, int *length_in_ms) {
	ModPlugFile *mp_file_tmp = 0, *mp_file_cur;

	LOCK;
	mp_file_cur = mp_file;
	if(mp_fntest(fn)) {
		mp_file_cur = mp_file_tmp = mp_load_file(fn, 0);
	}
	if(mp_file_cur) {
		if(length_in_ms) *length_in_ms = ModPlug_GetLength(mp_file_cur);
		if(title) {
			const char *src = ModPlug_GetName(mp_file_cur);
			if(src) {
				int titlelen = _tcslen(src);
				if (titlelen > 0) {
					//NOTE: in_modplug interprets titles as ANSI (Windows) string in the current codepage
					RemoveLeadingTrailingSpaces(src, title, GETFILEINFO_TITLE_LENGTH);
				} else {
					const char * tmpfn = fn;
					int pathlen = _tcslen(tmpfn);
					if ( pathlen <= 0 ) {
						tmpfn = lastfn;
						pathlen = _tcslen(tmpfn);
					}
					if ( pathlen > 0 ) {
						int pos = pathlen - 1;
						while ((pos >= 0) && (tmpfn[pos] != '\\') && (tmpfn[pos] != '/')) pos--;
						pos++;
						pathlen = _tcslen(&tmpfn[pos]);
						if (pathlen >= GETFILEINFO_TITLE_LENGTH) pathlen = (GETFILEINFO_TITLE_LENGTH - 1);
						strncpy(title, &tmpfn[pos], pathlen);
						title[pathlen] = '\0'; //strncpy does not guarantees the null termination!
					} else {
						strcpy(title, TEXT("GetTitleError!\0"));
					}
				}
			}
		}
	}
	if(mp_file_tmp) ModPlug_Unload(mp_file_tmp);
	UNLOCK;
}

const char *mp_types[] = {
/* NONE */ "None",

/* MOD */ "ProTracker",
/* S3M */ "ScreamTracker III",
/* XM */ "FastTracker II",
/* MED */ "OctaMed",

/* MTM */ "MultiTracker",
/* IT */ "Impulse Tracker",
/* 669 */ "UNIS 669",
/* ULT */ "UltraTracker",

/* STM */ "ScreamTracker II",
/* FAR */ "Farandole Composer",
/* WAV */ "Wave",
/* AMF */ "DSMI",

/* AMS */ "Extreme's Tracker",
/* DSM */ "DSIK Format",
/* MDL */ "DigiTracker",
/* OKT */ "Oktalyzer",

/* MID */ "Midi",
/* DMF */ "X-Tracker",
/* PTM */ "PolyTracker",
/* DBM */ "DigiBooster Pro",

/* MT2 */ "MadTracker 2",
/* AMF0 */ "Asylum",
/* PSM */ "Epic Megagames MASI",
/* J2B */ "Galaxy Sound System",

/* ABC */ "ABC",
/* PAT */ "PAT",
/* NONE */ "None",
/* UMX */ "Unreal Music" };

void InfoBoxInit(HWND hwnd, ModPlugFile *mp_file, const char *fn, int size) {
	unsigned int a = size, i = 0, x = 0;
	const char *str; char buf[512];

	if(a < 1000) sprintf(buf, "%u bytes", a);
	else if(a < 1000000) sprintf(buf, "%u %03u bytes", a / 1000, a % 1000);
	else sprintf(buf, "%u %03u %03u bytes", a / 1000000, (a / 1000) % 1000, a % 1000);
	SetDlgItemText(hwnd, IDC_FILESIZE, buf);

	do {
	  a = fn[i++];
	  if(a == '\\' || a == '/') x = i;
	} while(a);
	SetDlgItemText(hwnd, IDC_FILENAME, fn + x);

	if(str = ModPlug_GetName(mp_file)) SetDlgItemText(hwnd, IDC_TITLE, str);

	a = ModPlug_GetModuleType(mp_file);
	i = 0; if(a) do i++; while(a >>= 1);
	if(i > 24) i = 0; //value 24 is defined in sndfile.h, MAX_MODTYPE
	SetDlgItemText(hwnd, IDC_MODTYPE, mp_types[i]);

	a = ModPlug_GetLength(mp_file) / 1000;
	if(a < 60) sprintf(buf, "%u seconds", a);
	else sprintf(buf, "%u:%02u minutes", a / 60, a % 60);
	SetDlgItemText(hwnd, IDC_LENGTH, buf);

	a = ModPlug_NumChannels(mp_file);
	SetDlgItemInt(hwnd, IDC_CHANNELS, a, 0);

	x = ModPlug_NumSamples(mp_file);
	sprintf(buf, "Samples (%u)", x);
	SetDlgItemText(hwnd, IDC_SAMPLES, buf);
	if(!x) EnableWindow(GetDlgItem(hwnd, IDC_SAMPLES), 0);

	for(i = 0; i < x+1; i++) {
	  ModPlug_SampleName(mp_file, i, buf + sprintf(buf, "%02u: ", i));
	  SendDlgItemMessage(hwnd, IDC_LIST1, LB_ADDSTRING, 0, (LPARAM)(LPCTSTR)buf);
	}

	x = ModPlug_NumInstruments(mp_file);
	sprintf(buf, "Instruments (%u)", x);
	SetDlgItemText(hwnd, IDC_INSTRUMENTS, buf);
	if(!x) EnableWindow(GetDlgItem(hwnd, IDC_INSTRUMENTS), 0);

	for(i = 0; i < x+1; i++) {
	  ModPlug_InstrumentName(mp_file, i, buf + sprintf(buf, "%02u: ", i));
	  SendDlgItemMessage(hwnd, IDC_LIST2, LB_ADDSTRING, 0, (LPARAM)(LPCTSTR)buf);
	}


	if(str = ModPlug_GetMessage(mp_file)) {
	  char *m; int j;
	  x = 0; i = 0; a = 0; do {
	    j = a; a = str[i++];
	    if(a == '\r' || a == '\n') if((j ^ 7) != a) x++;
	  } while(a);
	  x += i;

	  if(x && (m = (char *)malloc(x))) {
	    i = 0; a = 0; do {
	      j = a; a = *str++;
	      if(a == '\r' || a == '\n') {
	        if((j ^ 7) != a) { m[i++] = '\r'; m[i++] = '\n'; }
	      } else m[i++] = a;
	    } while(a);

	    EnableWindow(GetDlgItem(hwnd, IDC_COMMENT), 1);
	    SetDlgItemText(hwnd, IDC_EDIT, m);
	    free(m);
	  }
	}

	SendDlgItemMessage(hwnd, IDC_SAMPLES, BM_SETCHECK, BST_CHECKED, 0);
}

#ifdef INFOTIMER
void InfoBoxTimer(HWND hwnd) {
	char buf[512];
//	int buflen = mod.outMod->GetWrittenTime() - mod.outMod->GetOutputTime();

	sprintf(buf, "Pattern: %03u [%03u], Row: %03u\nTempo: %03u [%03u], Voices: %03u",
	  ModPlug_GetCurrentOrder(mp_file),
	  ModPlug_GetCurrentPattern(mp_file),
	  ModPlug_GetCurrentRow(mp_file),
	  ModPlug_GetCurrentTempo(mp_file),
	  ModPlug_GetCurrentSpeed(mp_file),
	  ModPlug_GetPlayingChannels(mp_file));
	SetDlgItemText(hwnd, IDC_INFO, buf);
}
#endif

DWORD WINAPI InfoBoxProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	static char curpage;

	switch (msg) {
	  case WM_INITDIALOG:
	    curpage = 0;
	    LOCK;
	    if(!mp_file || mp_fntest((const char*)lParam)) {
	      int size; ModPlugFile *mp_file_tmp;
	      if(!(mp_file_tmp = mp_load_file((const char*)lParam, &size))) { UNLOCK; return 0; }
	      InfoBoxInit(hwnd, mp_file_tmp, (const char*)lParam, size);
	      ModPlug_Unload(mp_file_tmp);
	    } else {
	      InfoBoxInit(hwnd, mp_file, lastfn, lastsize);
#ifdef INFOTIMER
	      if(mp_refresh) {
	        mp_infoflag = 1;
	        SetTimer(hwnd, 1, mp_refresh, (TIMERPROC)InfoBoxProc);
	      }
#endif
	    }
	    UNLOCK;
	  case WM_SETFOCUS:
	  case WM_KILLFOCUS:
	    return 1;

	  case WM_COMMAND:
	    if(HIWORD(wParam) == BN_CLICKED) {
	      unsigned int i;
	      if(LOWORD(wParam) == IDC_CLOSE) goto close;
	      i = LOWORD(wParam) - IDC_SAMPLES;
	      if(i < 3 && curpage != i) {
	        ShowWindow(GetDlgItem(hwnd, IDC_LIST1 + i), SW_SHOW);
	        ShowWindow(GetDlgItem(hwnd, IDC_LIST1 + curpage), SW_HIDE);
	        curpage = i;
	      }
	    }
	    break;

#ifdef INFOTIMER
	  case WM_TIMER:
	    LOCK;
	    if(mp_infoflag && mp_file) InfoBoxTimer(hwnd);
	    UNLOCK;
	    break;
#endif

	  case WM_CLOSE:
		close:
#ifdef INFOTIMER
		mp_infoflag = 0;
#endif
	    EndDialog(hwnd, 1);
	    break;
	}
	return 0;
}

int InfoBox(const char *fn, HWND hwnd) {
	DialogBoxParam(mp_module, MAKEINTRESOURCE(IDD_INFO), hwnd, (DLGPROC)InfoBoxProc, (LPARAM)(LPCTSTR)fn);
	return 0;
}

int IsOurFile(const char *fn) {
	return 0; 
}

int Play(const char *fn) { 
	int maxlatency;
	unsigned long tmp;

	LOCK;
	if(mp_thread != INVALID_HANDLE_VALUE) { UNLOCK; return 1; }

	paused = 0; decode_pos_ms = 0; seek_needed = -1; mp_exitflag = 0;

	mp_file = mp_load_file(fn, &lastsize);
	if(!mp_file) { UNLOCK; return 1; }
	strcpy(lastfn,fn);

	maxlatency = mod.outMod->Open(SAMPLERATE, NCH, BPS, -1, -1); 
	if(maxlatency < 0) { ModPlug_Unload(mp_file); mp_file = 0; UNLOCK; return 1; }

	mod.SetInfo((SAMPLERATE * BPS * NCH) / 1000, SAMPLERATE / 1000, NCH, 1);
	mod.SAVSAInit(maxlatency, SAMPLERATE);
	mod.VSASetInfo(SAMPLERATE, NCH);
#ifdef USEEXTRAFEATURES
	if (mp_useagc != 0) mp_file->mSoundFile.SetAGC(TRUE);
	else mp_file->mSoundFile.SetAGC(FALSE);
	if (mp_preamp > 0) mp_file->mSoundFile.m_nSongPreAmp = mp_preamp;
#endif
	if(mp_flags & 1) {
	  mod.outMod->SetVolume(255);
	  SetVolume(SendMessage(mod.hMainWindow, WM_WA_IPC, -666, IPC_SETVOLUME));
	} else {
	  mod.outMod->SetVolume(-666);
	}

	mp_thread = (HANDLE) CreateThread(0, 0, (LPTHREAD_START_ROUTINE)DecodeThread, 0, 0, &tmp);
	UNLOCK;
	return 0;
}

void Pause() {
	paused = 1; mod.outMod->Pause(1);
}

void UnPause() {
	paused = 0; mod.outMod->Pause(0);
}

int IsPaused() {
	return paused;
}

void Stop() {
	if(mp_thread != INVALID_HANDLE_VALUE) {
	  mp_exitflag = 1;
	  WaitForSingleObject(mp_thread, -1);
	  CloseHandle(mp_thread);
	  mp_thread = INVALID_HANDLE_VALUE;
	}
	mod.outMod->Close();
	mod.SAVSADeInit();
	LOCK;
#ifdef INFOTIMER
	mp_infoflag = 0;
#endif
	if(mp_file) { ModPlug_Unload(mp_file); mp_file = 0; }
	UNLOCK;
}

void EQSet(int on, char data[10], int preamp) {

}

In_Module mod = {
	IN_VER,	// defined in IN2.H
	"Modplug wrapper v" IN_MODPLUG_TVERSION,
	0,	// hMainWindow (filled in by winamp)
	0,	// hDllInstance (filled in by winamp)
	&fileformats[0],
	1,	// is_seekable
	1,	// uses output plug-in system
	Config, About, Init, Quit,
	GetFileInfo, InfoBox, IsOurFile,
	Play, Pause, UnPause, IsPaused, Stop,
	GetLength, GetOutputTime, SetOutputTime,
	SetVolume, SetPan,

	0,0,0,0,0,0,0,0,0,	// visualization calls filled in by winamp
	0,0,		// dsp calls filled in by winamp
	EQSet,
	NULL,		// setinfo call filled in by winamp
	0		// out_mod filled in by winamp
};

EXTERN_C __declspec( dllexport ) In_Module * winampGetInModule2() {
	//This function is called immediately after DllMain when Winamp loads. 
	//get ini size
	DWORD inisizelo, inisizehi;
	HANDLE hFile = CreateFile(inifn, GENERIC_READ, 
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile==INVALID_HANDLE_VALUE) {
		inisizelo = 0;
		inisizehi = 0;
	} else {
		inisizelo = GetFileSize(hFile, &inisizehi);
		CloseHandle(hFile);
	}
	InitializeCriticalSection(&mp_lock);
	LOCK;
	//write new ini if necessary
	BOOL isNewIniWritten = FALSE;
	if ((inisizelo < 264) && (inisizehi == 0)) {
		isNewIniWritten = WriteNewINI(inifn);
	}
	//constructing fileformats data
	memset(&fileformats[0], 0, sizeof(fileformats));
	int n, ns, nl;
	char * tmpdeffmtname = DEFAULTFORMATSNAME "\0";
	ns = strlen(tmpdeffmtname);
	nl = (sizeof(fileformats) - ns - 2);
	if (isNewIniWritten) {
		//get fileformats without file read from predefined default value
		char * tmpdeffmt = DEFAULTFORMATS "\0";
		n = strlen(tmpdeffmt);
		if (n > nl) n = nl;
		memcpy(fileformats, tmpdeffmt, n);
		n++;
		memcpy(&fileformats[n], tmpdeffmtname, ns);
	} else {
		//get fileformats from ini file
		n = GetPrivateProfileString("ModPlug", "Formats", tmpdeffmtname, &fileformats[0], sizeof(fileformats), inifn);
		if (n > nl) n = nl;
		fileformats[n] = '\0';
		n++;
		memcpy(&fileformats[n], tmpdeffmtname, ns);
		fileformats[n + ns] = '\0';
	}
	UNLOCK;
	DeleteCriticalSection(&mp_lock);
	return &mod;
}

#ifdef _MFC_VER
void DllMainAttach() {
	HINSTANCE hinstDLL = AfxGetInstanceHandle();
	int n = GetModuleFileName(hinstDLL, inifn, MAX_PATH);
	if(n) {
		do if(inifn[--n] == '.') break; while(n);
		if(n) strcpy(inifn + n + 1, "ini");
	}
	mp_module = hinstDLL;
}

void DllMainDetach() {
	// nothing
}
#endif