#ifdef NOGUICONFIG
//Due to compliler bug (in VC6), this must be checked 2 times!
#endif

#ifndef NOGUICONFIG
#ifndef MFCGUI
#pragma message( "Native Config GUI code compilation..." )

#include <stdio.h> //required for sndfile.h 'FILE' declaration
#include <windows.h> //required by commctrl.h
#include <commctrl.h> //this must be also included in main.rc
#include "libmodplug_0_8_8_x\src\modplug.h"
#include "resource.h"
#include "main.h"
#include "formats.h"

extern HINSTANCE mp_module;
extern CRITICAL_SECTION mp_lock;

void AdjustTTTimeAndWidth(HWND hwndTip, int strlenght, DWORD dwWidths)
{
		if (!hwndTip || (strlenght <= 0))
		{
			return;
		}
		//The default display times are optimized for short strings (up to 80 characters).
		//The default time is calculated as DoubleClickTime * 10, which is by default: 500*10=5000ms
		//The default initial time is 500ms, the default reshow time is 100ms.
		int i, j;
		j = (strlenght - 60);
		if (j > 0) {
			i = j / 20;
			j = j - (20 * i);
			if (j > 0) j = i + 1; else j = i; //will round up if there is a remainder
		} else j = 0;
		i = 4800 + (j * 1200);
		if (i > 65535) i = 65535;
		SendMessage(hwndTip, TTM_SETDELAYTIME, (WPARAM)TTDT_AUTOPOP, (LPARAM)LOWORD(i));
		SendMessage(hwndTip, TTM_SETDELAYTIME, (WPARAM)TTDT_INITIAL, (LPARAM)LOWORD(1200));
		//Set the width by a static value or dynamically depending on the number of characters.
		if (dwWidths != 0) {
			DWORD ttWidth;
			if (HIWORD(dwWidths) == 0) {
				ttWidth=LOWORD(dwWidths); //set width directly
			} else {
				//set width dependent on number of characters
				int basewidth, basechnum, incwidth, incchnum;
				basewidth = LOBYTE(LOWORD(dwWidths));
				basechnum = HIBYTE(LOWORD(dwWidths));
				if ((basewidth == 0) || (basechnum == 0)) {
					basewidth = 0;
					basechnum = 0;
				}
				incwidth = LOBYTE(HIWORD(dwWidths));
				incchnum = HIBYTE(HIWORD(dwWidths));
				if ((incwidth == 0) || (incchnum == 0)) {
					incwidth = 0;
					incchnum = 0;
				}
				j = (strlenght - basechnum);
				if ((j > 0) && (incchnum > 0)) {
					i = j / incchnum;
					j = j - (incchnum * i);
					if (j > 0) j = i + 1; else j = i; //will round up if there is a remainder
				} else j = 0;
				ttWidth = basewidth + (j * incwidth);
				if (ttWidth > 480) ttWidth = 480;
			}
			SendMessage(hwndTip, TTM_SETMAXTIPWIDTH, (WPARAM)0, (LPARAM)ttWidth);
		}
}

HWND CreateToolTip(int toolID, HWND hDlg, PTSTR pszText, DWORD dwStyles, DWORD dwWidths, BOOL isStaticCtrl)
// Based on: "https://learn.microsoft.com/en-us/windows/win32/controls/create-a-tooltip-for-a-control"
// Based on: "https://learn.microsoft.com/en-us/windows/win32/controls/create-a-tooltip-for-a-rectangular-area"
// Based on: "https://stackoverflow.com/questions/18034975/how-do-i-find-position-of-a-win32-control-window-relative-to-its-parent-window"
// Description:
//   Creates a tooltip for an item in a dialog box. 
// Parameters:
//   idTool - identifier of an dialog box item.
//   nDlg - window handle of the dialog box.
//   pszText - string to use as the tooltip text.
//   dwStyles - OR-ed styles as a single DWord value.
//   dwWidths - custom width. 0=automatic(single line up to the screen width).
//     if HIWORD == 0, LOWORD contains width value.
//     if HIWORD != 0, the dwWidths is splitted into a 4 BYTE vales:
//       LOBYTE(LOWORD) - basic width up to (and including) the basic num.of characters reached.
//       HIBYTE(LOWORD) - basic num.of characters.
//       LOBYTE(HIWORD) - incremental width - width will be extended by this value for every.
//                        group of chracters that overflows the basic num.of characters.
//       HIBYTE(HIWORD) - incremental num.of characters.
//       If one of the BYTE pairs is 0, the remained BYTE is also treated as 0.
//       The final width value will be limited to the 480 in this mode.
//   isStaticCtrl - FALSE for normal controls (Buttons, TrackBars, EditBoxes,...) 
//     and TRUE for static controls. But normal controls inside the area will override it.
// Returns:
//   The handle to the tooltip.
//
{
	if (!toolID || !hDlg || !pszText)
	{
		return (HWND)NULL;
	}
	// Get the window of the tool.
	HWND hwndTool = GetDlgItem(hDlg, toolID);

	// Create the tooltip. g_hInst (now mp_module) is the global instance handle.
/*
	DWORD dwExStyle;
	if (isStaticCtrl) {
		dwExStyle = WS_EX_TOPMOST;
	} else {
		dwExStyle = 0;
	}
	HWND hwndTip = CreateWindowEx(dwExStyle, TOOLTIPS_CLASS, NULL,
							dwStyles,
							CW_USEDEFAULT, CW_USEDEFAULT,
							CW_USEDEFAULT, CW_USEDEFAULT,
							hDlg, NULL,
							mp_module, NULL);
*/
	//CreateWindowEx not available on Win95
	HWND hwndTip = CreateWindow(TOOLTIPS_CLASS, NULL,
							dwStyles,
							CW_USEDEFAULT, CW_USEDEFAULT,
							CW_USEDEFAULT, CW_USEDEFAULT,
							hDlg, NULL,
							mp_module, NULL);

	if (!hwndTool || !hwndTip)
	{
		return (HWND)NULL;
	}
	SetWindowPos(hwndTip, HWND_TOPMOST, 0, 0, 0, 0, 
				SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
	// Associate the tooltip with the tool.
	TOOLINFO toolInfo = { 0 };
	toolInfo.cbSize = sizeof(toolInfo);
	toolInfo.hwnd = hDlg;
	if (isStaticCtrl) {
		toolInfo.uFlags = TTF_SUBCLASS;
	} else {
		toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
		toolInfo.uId = (UINT_PTR)hwndTool;
	}
	toolInfo.lpszText = pszText;
	toolInfo.hinst = mp_module;
	if (isStaticCtrl) {
		GetWindowRect(hwndTool, &toolInfo.rect);
		MapWindowPoints(HWND_DESKTOP, hDlg, (LPPOINT) &toolInfo.rect, 2);
	}
	if (SendMessage(hwndTip, TTM_ADDTOOL, 0, (LPARAM)&toolInfo)) {
		int strlenght = strlen(toolInfo.lpszText);
		AdjustTTTimeAndWidth(hwndTip, strlenght, dwWidths);
		return hwndTip;
	} else {
		DestroyWindow(hwndTip);
		return (HWND)NULL;
	}
}

BOOL AddToolToToolTip(HWND hwndTip, int toolID, HWND hDlg, PTSTR pszText, DWORD dwWidths) {
	if (!hwndTip || !toolID || !hDlg || !pszText)
	{
		return FALSE;
	}
	HWND hwndTool = GetDlgItem(hDlg, toolID);
	if (!hwndTool || !hwndTip)
	{
		return FALSE;
	}
	// Associate the tooltip with the tool.
	TOOLINFO toolInfo = { 0 };
	toolInfo.cbSize = sizeof(toolInfo);
	toolInfo.hwnd = hDlg;
	toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
	toolInfo.uId = (UINT_PTR)hwndTool;
	toolInfo.lpszText = pszText;
	toolInfo.hinst = mp_module;
	if (SendMessage(hwndTip, TTM_ADDTOOL, 0, (LPARAM)&toolInfo)) {
		int strlenght = strlen(toolInfo.lpszText);
		AdjustTTTimeAndWidth(hwndTip, strlenght, dwWidths);
		return TRUE;
	} else {
		return FALSE;
	}
}


BOOL FormatsDlgInit(HWND hwnd, Plugin_Settings * curpcfg) {
try {
	//Filling up the listbox item
	HWND CurDlgItem;
	CurDlgItem = GetDlgItem(hwnd, IDC_LST_FORMATS);
	if (CurDlgItem != NULL) {
		int pos, count;
		count = (int)SendMessage(CurDlgItem, LB_GETCOUNT, 0, (LPARAM)0);
		if (count < 1) { //we do not want add items multiple times when re-initializating
			int n = sizeof(Formats) / sizeof(Formats[0]);
			int c = 0;
			while (c < n) {
				pos = (int)SendMessage(CurDlgItem, LB_ADDSTRING, 0, (LPARAM)Formats[c].name);
				if (pos >= 0) {SendMessage(CurDlgItem, LB_SETITEMDATA, pos, (LPARAM)Formats[c].ext);}
				c++;
			}
		}
		SendMessage (GetDlgItem(hwnd, IDC_EDIT_FORMATSADDON), EM_SETLIMITTEXT, (WPARAM)63, (LPARAM)0);
		SendMessage(CurDlgItem, LB_SETSEL, (WPARAM)FALSE, (LPARAM)-1); //unselect all

		count = (int)SendMessage(CurDlgItem, LB_GETCOUNT, 0, (LPARAM)0);
		if (count > 0) {
			unsigned char startpos[64], lenght[64], isfound[64], n, nformats, spos, cpos, ldlen, patchsemicolon;
			memset(startpos, 0 ,sizeof(startpos));
			memset(lenght, 0 ,sizeof(lenght));
			memset(isfound, 0 ,sizeof(isfound));
			if (count > 64) count = 64; //64 listitem limit
			curpcfg->pFormats[255] = '\0'; //to be certain that next cycle ends without overrun
			//convert to uppercase (only extensions part)
			n = strlen(curpcfg->pFormats);
			cpos = 0;
			//this uppercase conversion may be not perfect, but it should be sufficient for file extensions
			while (cpos < n) {
				if (curpcfg->pFormats[cpos] > 0x60) curpcfg->pFormats[cpos] = toupper(curpcfg->pFormats[cpos]);
				cpos++;
			}
			patchsemicolon = 0;
			spos = 0;
			cpos = 0;
			n = 0;
			while ((curpcfg->pFormats[cpos] > 0) && (n < 64)) {
				while ((curpcfg->pFormats[cpos] != ';') && (curpcfg->pFormats[cpos] > 0)) cpos++;
				startpos[n] = spos;
				lenght[n] = (cpos - spos) + 1;
				if (curpcfg->pFormats[cpos] == 0) {
					patchsemicolon = cpos;
				} else cpos++;
				if (lenght[n] > 0) n++; //to ignore multiple semicolons
				spos = cpos;
			}
			nformats = n; //num. of used array elements
			if (patchsemicolon > 0) curpcfg->pFormats[patchsemicolon] = ';'; //all extensions must end with semicolon
			pos = count;
			while (pos > 0) {
				pos--;
				char * clistdata;
				clistdata = (char*)SendMessage(CurDlgItem, LB_GETITEMDATA, (WPARAM)pos, (LPARAM)0);
				ldlen = strlen(clistdata);
				n = 0;
				char found = 0;
				while ((n < nformats) && (found == 0)) {
					cpos = ldlen;
					if (cpos == lenght[n]) {
						unsigned char ind = 0;
						while ((ind == 0) && (cpos > 0)) {
							cpos--;
							ind = (curpcfg->pFormats[startpos[n]+cpos] ^ clistdata[cpos]); //will be zero when values are identical
							//if (curpcfg->pFormats[startpos[n]+cpos] != clistdata[cpos]) ind++;
						}
						if (ind == 0) {
							SendMessage(CurDlgItem, LB_SETSEL, (WPARAM)TRUE, (LPARAM)pos);
							isfound[n]++;
							found = 1;
						} else n++;
					} else n++;
				}
			}
			//Gather custom file extensions (duplicates also goes here)
			char customformats[64];
			memset(customformats, 0 ,sizeof(customformats));
			n = 0;
			pos = 0;
			while (n < nformats) {
				if (isfound[n] == 0) {
					if ((pos + lenght[n]) < sizeof(customformats)) {
						memcpy(&customformats[pos], &curpcfg->pFormats[startpos[n]], lenght[n]);
						pos = pos + lenght[n];
					}
				}
				n++;
			}
			//Set the editbox value if necessary
			if (strlen(customformats) > 0) {
				SetWindowText(GetDlgItem(hwnd, IDC_EDIT_FORMATSADDON), customformats);
			}

			if (patchsemicolon > 0) curpcfg->pFormats[patchsemicolon] = '\0'; //revert to original state
		}
	}
	//Adding tooltips
	//HWND CurTTHandle;
	DWORD ttstyles = WS_POPUP | ES_MULTILINE | TTS_ALWAYSTIP;
	BYTE basewidth, basechnum, incwidth, incchnum;
	basewidth=160;
	basechnum=80;
	incwidth=32;
	incchnum=40;
	DWORD ttwidths =((incchnum << 24) | (incwidth << 16) | (basechnum << 8) | basewidth);
	char buffer [256]; //255 character + null-terminator - this should be enough.
	DWORD buffsize = sizeof(buffer);
	DWORD ntbuffsize = buffsize - 1; //note: we need null-terminated string, which is not assured by the LoadString.
	//Formats list
	memset(&buffer[0], 0, buffsize);
	LoadString(mp_module, IDS_HPLLST_FMTLIST, buffer, ntbuffsize);
	CreateToolTip(IDC_LST_FORMATS, hwnd, buffer, ttstyles, ttwidths, FALSE);
	//Additional formats
	memset(&buffer[0], 0, buffsize);
	LoadString(mp_module, IDS_HPLEDT_FMTADDONS, buffer, ntbuffsize);
	CreateToolTip(IDC_EDIT_FORMATSADDON, hwnd, buffer, ttstyles, ttwidths, FALSE);
	//Reset button
	memset(&buffer[0], 0, buffsize);
	LoadString(mp_module, IDS_HPLBTN_FMTRESET, buffer, ntbuffsize);
	CreateToolTip(IDC_BUTTON_FMTRESET, hwnd, buffer, ttstyles, ttwidths, FALSE);
	//Unselect button
	memset(&buffer[0], 0, buffsize);
	LoadString(mp_module, IDS_HPLBTN_FMTUNSELALL, buffer, ntbuffsize);
	CreateToolTip(IDC_BUTTON_FMTUNSELALL, hwnd, buffer, ttstyles, ttwidths, FALSE);
	//OK button
	memset(&buffer[0], 0, buffsize);
	LoadString(mp_module, IDS_HPLBTN_FMTIDOK, buffer, ntbuffsize);
	CreateToolTip(IDOK, hwnd, buffer, ttstyles, ttwidths, FALSE);
	//Cancel button
	memset(&buffer[0], 0, buffsize);
	LoadString(mp_module, IDS_HPLBTN_FMTIDCANCEL, buffer, ntbuffsize);
	CreateToolTip(IDCANCEL, hwnd, buffer, ttstyles, ttwidths, FALSE);
	return TRUE;
}
catch ( ... ) {
	MessageBox(hwnd, "ERROR: Config::Formats dialog initialization failed!", "Modplug Settings", 0);
	return FALSE;
}
}

BOOL FormatsAcceptSettings(HWND hwnd, Plugin_Settings * curpcfg) {
try {
	HWND CurDlgItem;
	CurDlgItem = GetDlgItem(hwnd, IDC_LST_FORMATS);
	if (CurDlgItem != NULL) {
		int pos, count;
		unsigned char n, cpos, ldlen;
		count = (int)SendMessage(CurDlgItem, LB_GETCOUNT, (WPARAM)0, (LPARAM)0);
		if (count > 64) count = 64; //64 listitem limit
		memset(curpcfg->pFormats, 0, curpcfg->pFormatsSize);
		char * clistdata;
		pos = 0;
		cpos = 0;
		while (pos < count) {
			if (SendMessage(CurDlgItem, LB_GETSEL, (WPARAM)pos, (LPARAM)0) != 0) {
				clistdata = (char*)SendMessage(CurDlgItem, LB_GETITEMDATA, (WPARAM)pos, (LPARAM)0);
				ldlen = strlen(clistdata);
				n = 0;
				if (ldlen > 0) {
					memcpy(&curpcfg->pFormats[cpos], clistdata, ldlen);
					cpos = cpos + ldlen;
				}
			}
			pos++;
		}
		char customformats[64];
		GetWindowText(GetDlgItem(hwnd, IDC_EDIT_FORMATSADDON), customformats, 64);
		ldlen = strlen(customformats);
		if (ldlen < 63) {
			if ((customformats[ldlen-1] != 0) && (customformats[ldlen-1] != ';')) customformats[ldlen] = ';';
		}
		int ns, nl, nmax;
		char * tmpdeffmtname = DEFAULTFORMATSNAME "\0";
		ns = strlen(tmpdeffmtname);
		nmax = (curpcfg->pFormatsSize - ldlen - ns - 2);
		if (cpos > nmax) {
			MessageBox(hwnd, "WARNING: One or more file extensions was removed from the selection because the internal text buffer size is not enought to hold all of them!", "Modplug Settings::Formats", MB_OK | MB_ICONEXCLAMATION);
			cpos = nmax;
			while ((curpcfg->pFormats[cpos] != ';') && (cpos > 0)) cpos--; //go to the end of the previous extension (prevents partial extension strings)
			if (cpos > 0) cpos++;
		}
		if (ldlen > 0) {
			if (cpos > (curpcfg->pFormatsSize - ldlen)) cpos = (curpcfg->pFormatsSize - ldlen);
			memcpy(&curpcfg->pFormats[cpos], customformats, ldlen);
			cpos = cpos + ldlen;
		}
		if (cpos > 0) {
			nl = (curpcfg->pFormatsSize - ns - 2);
			if (cpos > nl) cpos = nl;
			memcpy(&curpcfg->pFormats[cpos+1], tmpdeffmtname, ns);
		}
	}
	return TRUE;
}
catch ( ... ) {
	MessageBox(hwnd, "ERROR: Config::Formats settings processing failed!", "Modplug Settings", 0);
	return FALSE;
}
}

DWORD WINAPI FormatsDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	//NOTE: lParam at this stage is empty! It is only accessible at WM_INITDIALOG stage.
	switch (msg) {
		case WM_INITDIALOG:
			SetWindowLongPtr(hwnd, GWL_USERDATA, lParam);
			FormatsDlgInit(hwnd, (Plugin_Settings*)lParam);
		case WM_SETFOCUS:
		case WM_KILLFOCUS:
			return 1;

		case WM_COMMAND:
			if(HIWORD(wParam) == BN_CLICKED) {
				//unsigned int i;
				if(LOWORD(wParam) == IDCANCEL) goto fmtclose;
				if(LOWORD(wParam) == IDOK) goto fmtaccept;
				if(LOWORD(wParam) == IDC_BUTTON_FMTRESET) goto fmtreset;
				if(LOWORD(wParam) == IDC_BUTTON_FMTUNSELALL) goto fmtunselall;
				goto End_BN_CLICKED;

			fmtreset:
				{
					Plugin_Settings* pcurpcfg = (Plugin_Settings*)GetWindowLongPtr(hwnd, GWL_USERDATA);
					char * tmpdeffmt = DEFAULTFORMATS "\0";
					char * tmpdeffmtname = DEFAULTFORMATSNAME "\0";
					Plugin_Settings defpcfg = {4, 125, 500, (*pcurpcfg).pUseInfotimer, 0, 0, (*pcurpcfg).pFormatsSize, (*pcurpcfg).pIniFNSize, 0, (*pcurpcfg).isReset, (*pcurpcfg).pFormats, (*pcurpcfg).pIniFN};
					*pcurpcfg = defpcfg;
					memset(&(*pcurpcfg).pFormats[0], 0, (*pcurpcfg).pFormatsSize);
					int n = strlen(tmpdeffmt);
					int ns = strlen(tmpdeffmtname);
					int nl = ((*pcurpcfg).pFormatsSize - ns - 2);
					if (n > nl) n = nl;
					memcpy(&(*pcurpcfg).pFormats[0], tmpdeffmt, n);
					n++;
					memcpy(&(*pcurpcfg).pFormats[n], tmpdeffmtname, ns);
					FormatsDlgInit(hwnd, pcurpcfg);
				}
				goto End_BN_CLICKED;

			fmtunselall:
				SendMessage(GetDlgItem(hwnd, IDC_LST_FORMATS), LB_SETSEL, (WPARAM)FALSE, (LPARAM)-1); //unselect all
				goto End_BN_CLICKED;

			fmtaccept:
				{
					Plugin_Settings* pcurpcfg = (Plugin_Settings*)GetWindowLongPtr(hwnd, GWL_USERDATA);
					FormatsAcceptSettings(hwnd, pcurpcfg);
				}
				EndDialog(hwnd, 1);
				goto End_BN_CLICKED;

			End_BN_CLICKED:
			; }
			break;


		case WM_CLOSE:
		fmtclose:
			EndDialog(hwnd, 1);
			break;
	}
	return 0;
}


BOOL ConfigDlgInit(HWND hwnd, ModPlug_Settings * curcfg, Plugin_Settings * curpcfg) {
try {
	//Modplug flags
	if ((curcfg->mFlags & MODPLUG_ENABLE_OVERSAMPLING) != 0) {
		CheckDlgButton(hwnd, IDC_CHECK_OVERSAMPLING, BST_CHECKED);
	} else {
		CheckDlgButton(hwnd, IDC_CHECK_OVERSAMPLING, BST_UNCHECKED);
	}
	if ((curcfg->mFlags & MODPLUG_ENABLE_NOISE_REDUCTION) != 0) {
		CheckDlgButton(hwnd, IDC_CHECK_NOISEREDUCTION, BST_CHECKED);
	} else {
		CheckDlgButton(hwnd, IDC_CHECK_NOISEREDUCTION, BST_UNCHECKED);
	}
	if ((curcfg->mFlags & MODPLUG_ENABLE_REVERB) != 0) {
		CheckDlgButton(hwnd, IDC_CHECK_REVERB, BST_CHECKED);
	} else {
		CheckDlgButton(hwnd, IDC_CHECK_REVERB, BST_UNCHECKED);
	}
	if ((curcfg->mFlags & MODPLUG_ENABLE_MEGABASS) != 0) {
		CheckDlgButton(hwnd, IDC_CHECK_MEGABASS, BST_CHECKED);
	} else {
		CheckDlgButton(hwnd, IDC_CHECK_MEGABASS, BST_UNCHECKED);
	}
	if ((curcfg->mFlags & MODPLUG_ENABLE_SURROUND) != 0) {
		CheckDlgButton(hwnd, IDC_CHECK_SURROUND, BST_CHECKED);
	} else {
		CheckDlgButton(hwnd, IDC_CHECK_SURROUND, BST_UNCHECKED);
	}
	if (curpcfg->pUseAGC != 0) {
		CheckDlgButton(hwnd, IDC_CHECK_USEAGC, BST_CHECKED);
	} else {
		CheckDlgButton(hwnd, IDC_CHECK_USEAGC, BST_UNCHECKED);
	}
	//Plugin flags
	if ((curpcfg->pFlags & 1) != 0) {
		CheckDlgButton(hwnd, IDC_CHECK_INTVOLUMECTRL, BST_CHECKED);
	} else {
		CheckDlgButton(hwnd, IDC_CHECK_INTVOLUMECTRL, BST_UNCHECKED);
	}
	if ((curpcfg->pFlags & 4) != 0) {
		CheckDlgButton(hwnd, IDC_CHECK_SEEKBYORDERS, BST_CHECKED);
	} else {
		CheckDlgButton(hwnd, IDC_CHECK_SEEKBYORDERS, BST_UNCHECKED);
	}
	//Channels
	if (curcfg->mChannels == 1) {
		CheckDlgButton(hwnd, IDC_RADIO_CHANNELSMONO, BST_CHECKED);
	} else if (curcfg->mChannels == 4) {
		CheckDlgButton(hwnd, IDC_RADIO_CHANNELSQUAD, BST_CHECKED);
	} else {
		CheckDlgButton(hwnd, IDC_RADIO_CHANNELSSTEREO, BST_CHECKED);
	}
	if (curcfg->mChannels != 4) {
		CheckDlgButton(hwnd, IDC_RADIO_CHANNELSQUAD, BST_UNCHECKED);
	}
	if (curcfg->mChannels != 2) {
		CheckDlgButton(hwnd, IDC_RADIO_CHANNELSSTEREO, BST_UNCHECKED);
	}
	if (curcfg->mChannels != 1) {
		CheckDlgButton(hwnd, IDC_RADIO_CHANNELSMONO, BST_UNCHECKED);
	}
	//Bit depth
	if (curcfg->mBits == 32) {
		CheckDlgButton(hwnd, IDC_RADIO_BITDEPTH32, BST_CHECKED);
	} else if (curcfg->mBits == 8) {
		CheckDlgButton(hwnd, IDC_RADIO_BITDEPTH8, BST_CHECKED);
	} else {
		CheckDlgButton(hwnd, IDC_RADIO_BITDEPTH16, BST_CHECKED);
	}
	if (curcfg->mBits != 32) {
		CheckDlgButton(hwnd, IDC_RADIO_BITDEPTH32, BST_UNCHECKED);
	}
	if (curcfg->mBits != 16) {
		CheckDlgButton(hwnd, IDC_RADIO_BITDEPTH16, BST_UNCHECKED);
	}
	if (curcfg->mBits != 8) {
		CheckDlgButton(hwnd, IDC_RADIO_BITDEPTH8, BST_UNCHECKED);
	}
	//Interpolation
	if (curcfg->mResamplingMode == 3) {
		CheckDlgButton(hwnd, IDC_RADIO_RESAMPLING8TAPFIR, BST_CHECKED);
	} else if (curcfg->mResamplingMode == 1) {
		CheckDlgButton(hwnd, IDC_RADIO_RESAMPLINGLINEAR, BST_CHECKED);
	} else if (curcfg->mResamplingMode == 0) {
		CheckDlgButton(hwnd, IDC_RADIO_RESAMPLINGNONE, BST_CHECKED);
	} else {
		CheckDlgButton(hwnd, IDC_RADIO_RESAMPLINGCUBIC, BST_CHECKED);
	}
	if (curcfg->mResamplingMode != 3) {
		CheckDlgButton(hwnd, IDC_RADIO_RESAMPLING8TAPFIR, BST_UNCHECKED);
	}
	if (curcfg->mResamplingMode != 2) {
		CheckDlgButton(hwnd, IDC_RADIO_RESAMPLINGCUBIC, BST_UNCHECKED);
	}
	if (curcfg->mResamplingMode != 1) {
		CheckDlgButton(hwnd, IDC_RADIO_RESAMPLINGLINEAR, BST_UNCHECKED);
	}
	if (curcfg->mResamplingMode != 0) {
		CheckDlgButton(hwnd, IDC_RADIO_RESAMPLINGNONE, BST_UNCHECKED);
	}
	//Filling up the listbox item
	char numstr [33];
	HWND CurDlgItem;
	CurDlgItem = GetDlgItem(hwnd, IDC_LST_SAMPLINGRATE);
	if (CurDlgItem != NULL) {
		int pos, count;
		count = (int)SendMessage(CurDlgItem, LB_GETCOUNT, 0, (LPARAM)0);
		if (count < 1) { //we do not want add items multiple times when re-initializating
			pos = (int)SendMessage(CurDlgItem, LB_ADDSTRING, 0, (LPARAM)TEXT(" 8000"));
			if (pos >= 0) {SendMessage(CurDlgItem, LB_SETITEMDATA, pos, (LPARAM) 8000);}
			pos = (int)SendMessage(CurDlgItem, LB_ADDSTRING, 0, (LPARAM)TEXT("11025"));
			if (pos >= 0) {SendMessage(CurDlgItem, LB_SETITEMDATA, pos, (LPARAM) 11025);}
			pos = (int)SendMessage(CurDlgItem, LB_ADDSTRING, 0, (LPARAM)TEXT("12000"));
			if (pos >= 0) {SendMessage(CurDlgItem, LB_SETITEMDATA, pos, (LPARAM) 12000);}
			pos = (int)SendMessage(CurDlgItem, LB_ADDSTRING, 0, (LPARAM)TEXT("16000"));
			if (pos >= 0) {SendMessage(CurDlgItem, LB_SETITEMDATA, pos, (LPARAM) 16000);}
			pos = (int)SendMessage(CurDlgItem, LB_ADDSTRING, 0, (LPARAM)TEXT("22050"));
			if (pos >= 0) {SendMessage(CurDlgItem, LB_SETITEMDATA, pos, (LPARAM) 22050);}
			pos = (int)SendMessage(CurDlgItem, LB_ADDSTRING, 0, (LPARAM)TEXT("24000"));
			if (pos >= 0) {SendMessage(CurDlgItem, LB_SETITEMDATA, pos, (LPARAM) 24000);}
			pos = (int)SendMessage(CurDlgItem, LB_ADDSTRING, 0, (LPARAM)TEXT("32000"));
			if (pos >= 0) {SendMessage(CurDlgItem, LB_SETITEMDATA, pos, (LPARAM) 32000);}
			pos = (int)SendMessage(CurDlgItem, LB_ADDSTRING, 0, (LPARAM)TEXT("44100"));
			if (pos >= 0) {SendMessage(CurDlgItem, LB_SETITEMDATA, pos, (LPARAM) 44100);}
			pos = (int)SendMessage(CurDlgItem, LB_ADDSTRING, 0, (LPARAM)TEXT("48000"));
			if (pos >= 0) {SendMessage(CurDlgItem, LB_SETITEMDATA, pos, (LPARAM) 48000);}
			pos = (int)SendMessage(CurDlgItem, LB_ADDSTRING, 0, (LPARAM)TEXT("88200"));
			if (pos >= 0) {SendMessage(CurDlgItem, LB_SETITEMDATA, pos, (LPARAM) 88200);}
			pos = (int)SendMessage(CurDlgItem, LB_ADDSTRING, 0, (LPARAM)TEXT("96000"));
			if (pos >= 0) {SendMessage(CurDlgItem, LB_SETITEMDATA, pos, (LPARAM) 96000);}
		}
		BOOL selected = false;
		count = (int)SendMessage(CurDlgItem, LB_GETCOUNT, 0, (LPARAM)0);
		if (count > 0) {
			pos = count - 1;
			while ((pos >= 0) && (!selected)) {
				count = SendMessage(CurDlgItem, LB_GETITEMDATA, pos, 0);
				if (count == curcfg->mFrequency) {
					SendMessage(CurDlgItem, LB_SETCURSEL, pos, (LPARAM)0);
					selected = true;
				} else pos--;
			}
			if ( !selected ) {
				SendMessage(CurDlgItem, LB_SELECTSTRING, -1, (LPARAM)TEXT("44100")); //last try, should never get here
			}
		}
	}

	//Verify and correct the initial values for EditBoxes to prevent crashes
	//Group 1
	if (curcfg->mStereoSeparation < 1) {
		curcfg->mStereoSeparation = 1;
	} else if (curcfg->mStereoSeparation > 256) {
		curcfg->mStereoSeparation = 256;
	}
	if (curcfg->mMaxMixChannels < 32) {
		curcfg->mMaxMixChannels = 32;
	} else if (curcfg->mMaxMixChannels > 256) {
		curcfg->mMaxMixChannels = 256;
	}
	if (curcfg->mReverbDepth < 0) {
		curcfg->mReverbDepth = 0;
	} else if (curcfg->mReverbDepth > 100) {
		curcfg->mReverbDepth = 100;
	}
	if (curcfg->mReverbDelay < 40) {
		curcfg->mReverbDelay = 40;
	} else if (curcfg->mReverbDelay > 250) {
		curcfg->mReverbDelay = 250;
	}
	//Group 2
	if (curcfg->mBassAmount < 0) {
		curcfg->mBassAmount = 0;
	} else if (curcfg->mBassAmount > 100) {
		curcfg->mBassAmount = 100;
	}
	if (curcfg->mBassRange < 10) {
		curcfg->mBassRange = 10;
	} else if (curcfg->mBassRange > 100) {
		curcfg->mBassRange = 100;
	}
	if (curcfg->mSurroundDepth < 0) {
		curcfg->mSurroundDepth = 0;
	} else if (curcfg->mSurroundDepth > 100) {
		curcfg->mSurroundDepth = 100;
	}
	if (curcfg->mSurroundDelay < 4) {
		curcfg->mSurroundDelay = 4;
	} else if (curcfg->mSurroundDelay > 50) {
		curcfg->mSurroundDelay = 50;
	}
	//Group 3
	if (curcfg->mLoopCount < -1) {
		curcfg->mLoopCount = -1;
	} else if (curcfg->mLoopCount > 32767) {
		curcfg->mLoopCount = 32767;
	}
	if (curpcfg->pPreAmp < 0) {
		curpcfg->pPreAmp = 0;
	} else if ((curpcfg->pPreAmp > 0) && (curpcfg->pPreAmp < 16)) {
		curpcfg->pPreAmp = 16;
	} else if (curpcfg->pPreAmp > 128) {
		curpcfg->pPreAmp = 128;
	}
	if (curpcfg->pInfoRefresh < 0) {
		curpcfg->pInfoRefresh = 0;
	} else if (curpcfg->pInfoRefresh > 2000) {
		curpcfg->pInfoRefresh = 2000;
	}
	if (curpcfg->pInfoBuffering < 100) {
		if (curpcfg->pInfoBuffering < 1) {
			curpcfg->pInfoBuffering = 0;
		} else {
			curpcfg->pInfoBuffering = 100;
		}
	} else if (curpcfg->pInfoBuffering > 2000) {
		curpcfg->pInfoBuffering = 2000;
	}

	//Trackbar (slider) group 1
	CurDlgItem = GetDlgItem(hwnd, IDC_SLIDER_STEREOSEPARATION);
	if (CurDlgItem != NULL) {
		SendMessage(CurDlgItem, TBM_SETRANGE, (WPARAM)FALSE, MAKELPARAM(1, 256));
		SendMessage(CurDlgItem, TBM_SETTICFREQ, (WPARAM)16, (LPARAM)0);
		SendMessage(CurDlgItem, TBM_SETPAGESIZE, (WPARAM)0, (LPARAM)16);
		SendMessage(CurDlgItem, TBM_SETLINESIZE, (WPARAM)0, (LPARAM)4);
		SendMessage(CurDlgItem, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)curcfg->mStereoSeparation);
	}
	CurDlgItem = GetDlgItem(hwnd, IDC_EDIT_STEREOSEPARATION);
	if (CurDlgItem != NULL) {
		SendMessage (CurDlgItem, EM_SETLIMITTEXT, (WPARAM)3, (LPARAM)0);
		_itoa( curcfg->mStereoSeparation, numstr, 10 );
		SetWindowText(CurDlgItem, numstr);
	}

	CurDlgItem = GetDlgItem(hwnd, IDC_SLIDER_MAXMIXCHANNELS);
	if (CurDlgItem != NULL) {
		SendMessage(CurDlgItem, TBM_SETRANGE, (WPARAM)FALSE, MAKELPARAM(32, 256));
		SendMessage(CurDlgItem, TBM_SETTICFREQ, (WPARAM)16, (LPARAM)0);
		SendMessage(CurDlgItem, TBM_SETPAGESIZE, (WPARAM)0, (LPARAM)16);
		SendMessage(CurDlgItem, TBM_SETLINESIZE, (WPARAM)0, (LPARAM)4);
		SendMessage(CurDlgItem, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)curcfg->mMaxMixChannels);
	}
	CurDlgItem = GetDlgItem(hwnd, IDC_EDIT_MAXMIXCHANNELS);
	if (CurDlgItem != NULL) {
		SendMessage (CurDlgItem, EM_SETLIMITTEXT, (WPARAM)3, (LPARAM)0);
		_itoa( curcfg->mMaxMixChannels, numstr, 10 );
		SetWindowText(CurDlgItem, numstr);
	}

	CurDlgItem = GetDlgItem(hwnd, IDC_SLIDER_REVERBDEPTH);
	if (CurDlgItem != NULL) {
		SendMessage(CurDlgItem, TBM_SETRANGE, (WPARAM)FALSE, MAKELPARAM(0, 100));
		SendMessage(CurDlgItem, TBM_SETTICFREQ, (WPARAM)10, (LPARAM)0);
		SendMessage(CurDlgItem, TBM_SETPAGESIZE, (WPARAM)0, (LPARAM)20);
		SendMessage(CurDlgItem, TBM_SETLINESIZE, (WPARAM)0, (LPARAM)5);
		SendMessage(CurDlgItem, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)curcfg->mReverbDepth);
	}
	CurDlgItem = GetDlgItem(hwnd, IDC_EDIT_REVERBDEPTH);
	if (CurDlgItem != NULL) {
		SendMessage (CurDlgItem, EM_SETLIMITTEXT, (WPARAM)3, (LPARAM)0);
		_itoa( curcfg->mReverbDepth, numstr, 10 );
		SetWindowText(CurDlgItem, numstr);
	}

	CurDlgItem = GetDlgItem(hwnd, IDC_SLIDER_REVERBDELAY);
	if (CurDlgItem != NULL) {
		SendMessage(CurDlgItem, TBM_SETRANGE, (WPARAM)FALSE, MAKELPARAM(40, 250));
		SendMessage(CurDlgItem, TBM_SETTICFREQ, (WPARAM)20, (LPARAM)0);
		SendMessage(CurDlgItem, TBM_SETPAGESIZE, (WPARAM)0, (LPARAM)40);
		SendMessage(CurDlgItem, TBM_SETLINESIZE, (WPARAM)0, (LPARAM)10);
		SendMessage(CurDlgItem, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)curcfg->mReverbDelay);
	}
	CurDlgItem = GetDlgItem(hwnd, IDC_EDIT_REVERBDELAY);
	if (CurDlgItem != NULL) {
		SendMessage (CurDlgItem, EM_SETLIMITTEXT, (WPARAM)3, (LPARAM)0);
		_itoa( curcfg->mReverbDelay, numstr, 10 );
		SetWindowText(CurDlgItem, numstr);
	}

	//Trackbar (slider) group 2
	CurDlgItem = GetDlgItem(hwnd, IDC_SLIDER_BASSAMOUNT);
	if (CurDlgItem != NULL) {
		SendMessage(CurDlgItem, TBM_SETRANGE, (WPARAM)FALSE, MAKELPARAM(0, 100));
		SendMessage(CurDlgItem, TBM_SETTICFREQ, (WPARAM)10, (LPARAM)0);
		SendMessage(CurDlgItem, TBM_SETPAGESIZE, (WPARAM)0, (LPARAM)20);
		SendMessage(CurDlgItem, TBM_SETLINESIZE, (WPARAM)0, (LPARAM)5);
		SendMessage(CurDlgItem, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)curcfg->mBassAmount);
	}
	CurDlgItem = GetDlgItem(hwnd, IDC_EDIT_BASSAMOUNT);
	if (CurDlgItem != NULL) {
		SendMessage (CurDlgItem, EM_SETLIMITTEXT, (WPARAM)3, (LPARAM)0);
		_itoa( curcfg->mBassAmount, numstr, 10 );
		SetWindowText(CurDlgItem, numstr);
	}

	CurDlgItem = GetDlgItem(hwnd, IDC_SLIDER_BASSRANGE);
	if (CurDlgItem != NULL) {
		SendMessage(CurDlgItem, TBM_SETRANGE, (WPARAM)FALSE, MAKELPARAM(10, 100));
		SendMessage(CurDlgItem, TBM_SETTICFREQ, (WPARAM)10, (LPARAM)0);
		SendMessage(CurDlgItem, TBM_SETPAGESIZE, (WPARAM)0, (LPARAM)20);
		SendMessage(CurDlgItem, TBM_SETLINESIZE, (WPARAM)0, (LPARAM)5);
		SendMessage(CurDlgItem, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)curcfg->mBassRange);
	}
	CurDlgItem = GetDlgItem(hwnd, IDC_EDIT_BASSRANGE);
	if (CurDlgItem != NULL) {
		SendMessage (CurDlgItem, EM_SETLIMITTEXT, (WPARAM)3, (LPARAM)0);
		_itoa( curcfg->mBassRange, numstr, 10 );
		SetWindowText(CurDlgItem, numstr);
	}

	CurDlgItem = GetDlgItem(hwnd, IDC_SLIDER_SURROUNDDEPTH);
	if (CurDlgItem != NULL) {
		SendMessage(CurDlgItem, TBM_SETRANGE, (WPARAM)FALSE, MAKELPARAM(0, 100));
		SendMessage(CurDlgItem, TBM_SETTICFREQ, (WPARAM)10, (LPARAM)0);
		SendMessage(CurDlgItem, TBM_SETPAGESIZE, (WPARAM)0, (LPARAM)20);
		SendMessage(CurDlgItem, TBM_SETLINESIZE, (WPARAM)0, (LPARAM)5);
		SendMessage(CurDlgItem, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)curcfg->mSurroundDepth);
	}
	CurDlgItem = GetDlgItem(hwnd, IDC_EDIT_SURROUNDDEPTH);
	if (CurDlgItem != NULL) {
		SendMessage (CurDlgItem, EM_SETLIMITTEXT, (WPARAM)3, (LPARAM)0);
		_itoa( curcfg->mSurroundDepth, numstr, 10 );
		SetWindowText(CurDlgItem, numstr);
	}

	CurDlgItem = GetDlgItem(hwnd, IDC_SLIDER_SURROUNDDELAY);
	if (CurDlgItem != NULL) {
		SendMessage(CurDlgItem, TBM_SETRANGE, (WPARAM)FALSE, MAKELPARAM(0, 50));
		SendMessage(CurDlgItem, TBM_SETTICFREQ, (WPARAM)5, (LPARAM)0);
		SendMessage(CurDlgItem, TBM_SETPAGESIZE, (WPARAM)0, (LPARAM)10);
		SendMessage(CurDlgItem, TBM_SETLINESIZE, (WPARAM)0, (LPARAM)5);
		SendMessage(CurDlgItem, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)curcfg->mSurroundDelay);
		//realign ticks
		//TBM_GETPTICS is a dead end: array contains elements with values: min.+1 to max.-1
		//It seems to be the best solution is set range 0 - 50, and then limit it.
	}
	CurDlgItem = GetDlgItem(hwnd, IDC_EDIT_SURROUNDDELAY);
	if (CurDlgItem != NULL) {
		SendMessage (CurDlgItem, EM_SETLIMITTEXT, (WPARAM)2, (LPARAM)0);
		_itoa( curcfg->mSurroundDelay, numstr, 10 );
		SetWindowText(CurDlgItem, numstr);
	}
	//Trackbar (slider) group 3
	CurDlgItem = GetDlgItem(hwnd, IDC_SLIDER_LOOPCOUNT);
	if (CurDlgItem != NULL) {
		SendMessage(CurDlgItem, TBM_SETRANGE, (WPARAM)FALSE, MAKELPARAM(-1, 20));
		SendMessage(CurDlgItem, TBM_SETTICFREQ, (WPARAM)1, (LPARAM)0);
		SendMessage(CurDlgItem, TBM_SETPAGESIZE, (WPARAM)0, (LPARAM)5);
		SendMessage(CurDlgItem, TBM_SETLINESIZE, (WPARAM)0, (LPARAM)1);
		SendMessage(CurDlgItem, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)curcfg->mLoopCount);
	}
	CurDlgItem = GetDlgItem(hwnd, IDC_EDIT_LOOPCOUNT);
	if (CurDlgItem != NULL) {
		SendMessage (CurDlgItem, EM_SETLIMITTEXT, (WPARAM)5, (LPARAM)0);
		_itoa( curcfg->mLoopCount, numstr, 10 );
		SetWindowText(CurDlgItem, numstr);
	}

	CurDlgItem = GetDlgItem(hwnd, IDC_SLIDER_PREAMP);
	if (CurDlgItem != NULL) {
		SendMessage(CurDlgItem, TBM_SETRANGE, (WPARAM)FALSE, MAKELPARAM(0, 128));
		SendMessage(CurDlgItem, TBM_SETTICFREQ, (WPARAM)8, (LPARAM)0);
		SendMessage(CurDlgItem, TBM_SETPAGESIZE, (WPARAM)0, (LPARAM)16);
		SendMessage(CurDlgItem, TBM_SETLINESIZE, (WPARAM)0, (LPARAM)4);
		SendMessage(CurDlgItem, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)curpcfg->pPreAmp);
	}
	CurDlgItem = GetDlgItem(hwnd, IDC_EDIT_PREAMP);
	if (CurDlgItem != NULL) {
		SendMessage (CurDlgItem, EM_SETLIMITTEXT, (WPARAM)3, (LPARAM)0);
		_itoa( curpcfg->pPreAmp, numstr, 10 );
		SetWindowText(CurDlgItem, numstr);
	}

	CurDlgItem = GetDlgItem(hwnd, IDC_SLIDER_INFOREFRESH);
	if (CurDlgItem != NULL) {
		SendMessage(CurDlgItem, TBM_SETRANGE, (WPARAM)FALSE, MAKELPARAM(0, 2000));
		SendMessage(CurDlgItem, TBM_SETTICFREQ, (WPARAM)200, (LPARAM)0);
		SendMessage(CurDlgItem, TBM_SETPAGESIZE, (WPARAM)0, (LPARAM)100);
		SendMessage(CurDlgItem, TBM_SETLINESIZE, (WPARAM)0, (LPARAM)25);
		SendMessage(CurDlgItem, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)curpcfg->pInfoRefresh);
	}
	CurDlgItem = GetDlgItem(hwnd, IDC_EDIT_INFOREFRESH);
	if (CurDlgItem != NULL) {
		SendMessage (CurDlgItem, EM_SETLIMITTEXT, (WPARAM)4, (LPARAM)0);
		_itoa( curpcfg->pInfoRefresh, numstr, 10 );
		SetWindowText(CurDlgItem, numstr);
	}

	CurDlgItem = GetDlgItem(hwnd, IDC_SLIDER_INFOBUFFERING);
	if (CurDlgItem != NULL) {
		SendMessage(CurDlgItem, TBM_SETRANGE, (WPARAM)FALSE, MAKELPARAM(0, 2000));
		SendMessage(CurDlgItem, TBM_SETTICFREQ, (WPARAM)200, (LPARAM)0);
		SendMessage(CurDlgItem, TBM_SETPAGESIZE, (WPARAM)0, (LPARAM)100);
		SendMessage(CurDlgItem, TBM_SETLINESIZE, (WPARAM)0, (LPARAM)25);
		SendMessage(CurDlgItem, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)curpcfg->pInfoBuffering);
	}
	CurDlgItem = GetDlgItem(hwnd, IDC_EDIT_INFOBUFFERING);
	if (CurDlgItem != NULL) {
		SendMessage (CurDlgItem, EM_SETLIMITTEXT, (WPARAM)4, (LPARAM)0);
		_itoa( curpcfg->pInfoBuffering, numstr, 10 );
		SetWindowText(CurDlgItem, numstr);
	}
	//Disable controls
	UINT nCheck;
	nCheck = SendMessage(GetDlgItem(hwnd, IDC_CHECK_REVERB), BM_GETCHECK, (WPARAM)0, (LPARAM)0);
	if ((nCheck & BST_CHECKED) == 0) {
		EnableWindow(GetDlgItem(hwnd, IDC_EDIT_REVERBDEPTH), FALSE);
		EnableWindow(GetDlgItem(hwnd, IDC_SLIDER_REVERBDEPTH), FALSE);
		EnableWindow(GetDlgItem(hwnd, IDC_EDIT_REVERBDELAY), FALSE);
		EnableWindow(GetDlgItem(hwnd, IDC_SLIDER_REVERBDELAY), FALSE);
	} else {
		EnableWindow(GetDlgItem(hwnd, IDC_EDIT_REVERBDEPTH), TRUE);
		EnableWindow(GetDlgItem(hwnd, IDC_SLIDER_REVERBDEPTH), TRUE);
		EnableWindow(GetDlgItem(hwnd, IDC_EDIT_REVERBDELAY), TRUE);
		EnableWindow(GetDlgItem(hwnd, IDC_SLIDER_REVERBDELAY), TRUE);
	}
	nCheck = SendMessage(GetDlgItem(hwnd, IDC_CHECK_MEGABASS), BM_GETCHECK, (WPARAM)0, (LPARAM)0);
	if ((nCheck & BST_CHECKED) == 0) {
		EnableWindow(GetDlgItem(hwnd, IDC_EDIT_BASSAMOUNT), FALSE);
		EnableWindow(GetDlgItem(hwnd, IDC_SLIDER_BASSAMOUNT), FALSE);
		EnableWindow(GetDlgItem(hwnd, IDC_EDIT_BASSRANGE), FALSE);
		EnableWindow(GetDlgItem(hwnd, IDC_SLIDER_BASSRANGE), FALSE);
	} else {
		EnableWindow(GetDlgItem(hwnd, IDC_EDIT_BASSAMOUNT), TRUE);
		EnableWindow(GetDlgItem(hwnd, IDC_SLIDER_BASSAMOUNT), TRUE);
		EnableWindow(GetDlgItem(hwnd, IDC_EDIT_BASSRANGE), TRUE);
		EnableWindow(GetDlgItem(hwnd, IDC_SLIDER_BASSRANGE), TRUE);
	}
	nCheck = SendMessage(GetDlgItem(hwnd, IDC_CHECK_SURROUND), BM_GETCHECK, (WPARAM)0, (LPARAM)0);
	if ((nCheck & BST_CHECKED) == 0) {
		EnableWindow(GetDlgItem(hwnd, IDC_EDIT_SURROUNDDEPTH), FALSE);
		EnableWindow(GetDlgItem(hwnd, IDC_SLIDER_SURROUNDDEPTH), FALSE);
		EnableWindow(GetDlgItem(hwnd, IDC_EDIT_SURROUNDDELAY), FALSE);
		EnableWindow(GetDlgItem(hwnd, IDC_SLIDER_SURROUNDDELAY), FALSE);
	} else {
		EnableWindow(GetDlgItem(hwnd, IDC_EDIT_SURROUNDDEPTH), TRUE);
		EnableWindow(GetDlgItem(hwnd, IDC_SLIDER_SURROUNDDEPTH), TRUE);
		EnableWindow(GetDlgItem(hwnd, IDC_EDIT_SURROUNDDELAY), TRUE);
		EnableWindow(GetDlgItem(hwnd, IDC_SLIDER_SURROUNDDELAY), TRUE);
	}
	if ((curpcfg->pUseInfotimer) == 0) {
		EnableWindow(GetDlgItem(hwnd, IDC_EDIT_INFOREFRESH), FALSE);
		EnableWindow(GetDlgItem(hwnd, IDC_SLIDER_INFOREFRESH), FALSE);
		EnableWindow(GetDlgItem(hwnd, IDC_EDIT_INFOBUFFERING), FALSE);
		EnableWindow(GetDlgItem(hwnd, IDC_SLIDER_INFOBUFFERING), FALSE);
	} else {
		EnableWindow(GetDlgItem(hwnd, IDC_EDIT_INFOREFRESH), TRUE);
		EnableWindow(GetDlgItem(hwnd, IDC_SLIDER_INFOREFRESH), TRUE);
		EnableWindow(GetDlgItem(hwnd, IDC_EDIT_INFOBUFFERING), TRUE);
		EnableWindow(GetDlgItem(hwnd, IDC_SLIDER_INFOBUFFERING), TRUE);
	}
#ifndef USEEXTRAFEATURES
	EnableWindow(GetDlgItem(hwnd, IDC_CHECK_USEAGC), FALSE);
	EnableWindow(GetDlgItem(hwnd, IDC_EDIT_PREAMP), FALSE);
	EnableWindow(GetDlgItem(hwnd, IDC_SLIDER_PREAMP), FALSE);
#endif

	//Adding tooltips
	HWND CurTTHandle;
	DWORD ttstyles = WS_POPUP | ES_MULTILINE | TTS_ALWAYSTIP;
	BYTE basewidth, basechnum, incwidth, incchnum;
	basewidth=160;
	basechnum=80;
	incwidth=32;
	incchnum=40;
	DWORD ttwidths =((incchnum << 24) | (incwidth << 16) | (basechnum << 8) | basewidth);
	char buffer [256]; //255 character + null-terminator - this should be enough.
	DWORD buffsize = sizeof(buffer);
	DWORD ntbuffsize = buffsize - 1; //note: we need null-terminated string, which is not assured by the LoadString.
	//Modplug flags
	memset(&buffer[0], 0, buffsize);
	LoadString(mp_module, IDS_HMPFLG_OVERSAMPLING, buffer, ntbuffsize);
	CreateToolTip(IDC_CHECK_OVERSAMPLING, hwnd, buffer, ttstyles, ttwidths, FALSE); //string is copied to the tooltip's buffer, so we can reuse/destroy it
	memset(&buffer[0], 0, buffsize);
	LoadString(mp_module, IDS_HMPFLG_NOISEREDUCTION, buffer, ntbuffsize);
	CreateToolTip(IDC_CHECK_NOISEREDUCTION, hwnd, buffer, ttstyles, ttwidths, FALSE);
	memset(&buffer[0], 0, buffsize);
	LoadString(mp_module, IDS_HMPFLG_REVERB, buffer, ntbuffsize);
	CreateToolTip(IDC_CHECK_REVERB, hwnd, buffer, ttstyles, ttwidths, FALSE);
	memset(&buffer[0], 0, buffsize);
	LoadString(mp_module, IDS_HMPFLG_MEGABASS, buffer, ntbuffsize);
	CreateToolTip(IDC_CHECK_MEGABASS, hwnd, buffer, ttstyles, ttwidths, FALSE);
	memset(&buffer[0], 0, buffsize);
	LoadString(mp_module, IDS_HMPFLG_SURROUND, buffer, ntbuffsize);
	CreateToolTip(IDC_CHECK_SURROUND, hwnd, buffer, ttstyles, ttwidths, FALSE);
	memset(&buffer[0], 0, buffsize);
	LoadString(mp_module, IDS_HMPFLG_USEAGC, buffer, ntbuffsize);
	CreateToolTip(IDC_CHECK_USEAGC, hwnd, buffer, ttstyles, ttwidths, FALSE);
	//Plugin flags
	memset(&buffer[0], 0, buffsize);
	LoadString(mp_module, IDS_HPLFLG_INTVOLUMECTRL, buffer, ntbuffsize);
	CreateToolTip(IDC_CHECK_INTVOLUMECTRL, hwnd, buffer, ttstyles, ttwidths, FALSE);
	memset(&buffer[0], 0, buffsize);
	LoadString(mp_module, IDS_HPLFLG_SEEKBYORDERS, buffer, ntbuffsize);
	CreateToolTip(IDC_CHECK_SEEKBYORDERS, hwnd, buffer, ttstyles, ttwidths, FALSE);
	//Sampling rate
	memset(&buffer[0], 0, buffsize);
	LoadString(mp_module, IDS_HMPLST_FREQUENCY, buffer, ntbuffsize);
	CurTTHandle = CreateToolTip(IDC_LST_SAMPLINGRATE, hwnd, buffer, ttstyles, ttwidths, FALSE);
	//Channels
	memset(&buffer[0], 0, buffsize);
	LoadString(mp_module, IDS_HMPRDO_CHANNELSMONO, buffer, ntbuffsize);
	CreateToolTip(IDC_RADIO_CHANNELSMONO, hwnd, buffer, ttstyles, ttwidths, FALSE);
	memset(&buffer[0], 0, buffsize);
	LoadString(mp_module, IDS_HMPRDO_CHANNELSSTEREO, buffer, ntbuffsize);
	CreateToolTip(IDC_RADIO_CHANNELSSTEREO, hwnd, buffer, ttstyles, ttwidths, FALSE);
	memset(&buffer[0], 0, buffsize);
	LoadString(mp_module, IDS_HMPRDO_CHANNELSQUAD, buffer, ntbuffsize);
	CreateToolTip(IDC_RADIO_CHANNELSQUAD, hwnd, buffer, ttstyles, ttwidths, FALSE);
	//Bit depth
	memset(&buffer[0], 0, buffsize);
	LoadString(mp_module, IDS_HMPRDO_BITDEPTH, buffer, ntbuffsize);
	CurTTHandle = CreateToolTip(IDC_STATIC_BITDEPTH, hwnd, buffer, ttstyles, ttwidths, TRUE);
	AddToolToToolTip(CurTTHandle, IDC_RADIO_BITDEPTH8, hwnd, buffer, ttwidths);
	AddToolToToolTip(CurTTHandle, IDC_RADIO_BITDEPTH16, hwnd, buffer, ttwidths);
	AddToolToToolTip(CurTTHandle, IDC_RADIO_BITDEPTH32, hwnd, buffer, ttwidths);
	//CreateToolTip(IDC_RADIO_BITDEPTH8, hwnd, buffer, ttstyles, ttwidths, FALSE);
	//CreateToolTip(IDC_RADIO_BITDEPTH16, hwnd, buffer, ttstyles, ttwidths, FALSE);
	//CreateToolTip(IDC_RADIO_BITDEPTH32, hwnd, buffer, ttstyles, ttwidths, FALSE);
	//Interpolation (resampling)
	memset(&buffer[0], 0, buffsize);
	LoadString(mp_module, IDS_HMPRDO_RESAMPLINGNONE, buffer, ntbuffsize);
	CreateToolTip(IDC_RADIO_RESAMPLINGNONE, hwnd, buffer, ttstyles, ttwidths, FALSE);
	memset(&buffer[0], 0, buffsize);
	LoadString(mp_module, IDS_HMPRDO_RESAMPLINGLINEAR, buffer, ntbuffsize);
	CreateToolTip(IDC_RADIO_RESAMPLINGLINEAR, hwnd, buffer, ttstyles, ttwidths, FALSE);
	memset(&buffer[0], 0, buffsize);
	LoadString(mp_module, IDS_HMPRDO_RESAMPLINGCUBIC, buffer, ntbuffsize);
	CreateToolTip(IDC_RADIO_RESAMPLINGCUBIC, hwnd, buffer, ttstyles, ttwidths, FALSE);
	memset(&buffer[0], 0, buffsize);
	LoadString(mp_module, IDS_HMPRDO_RESAMPLING8TAPFIR, buffer, ntbuffsize);
	CreateToolTip(IDC_RADIO_RESAMPLING8TAPFIR, hwnd, buffer, ttstyles, ttwidths, FALSE);
	//Trackbars(sliders) - group 1
	memset(&buffer[0], 0, buffsize);
	LoadString(mp_module, IDS_HMPRNG_STEREOSEPARATION, buffer, ntbuffsize);
	CreateToolTip(IDC_SLIDER_STEREOSEPARATION, hwnd, buffer, ttstyles, ttwidths, FALSE);
	memset(&buffer[0], 0, buffsize);
	LoadString(mp_module, IDS_HMPRNG_MAXMIXCHANNELS, buffer, ntbuffsize);
	CreateToolTip(IDC_SLIDER_MAXMIXCHANNELS, hwnd, buffer, ttstyles, ttwidths, FALSE);
	memset(&buffer[0], 0, buffsize);
	LoadString(mp_module, IDS_HMPRNG_REVERBDEPTH, buffer, ntbuffsize);
	CreateToolTip(IDC_SLIDER_REVERBDEPTH, hwnd, buffer, ttstyles, ttwidths, FALSE);
	memset(&buffer[0], 0, buffsize);
	LoadString(mp_module, IDS_HMPRNG_REVERBDELAY, buffer, ntbuffsize);
	CreateToolTip(IDC_SLIDER_REVERBDELAY, hwnd, buffer, ttstyles, ttwidths, FALSE);
	//Trackbars(sliders) - group 2
	memset(&buffer[0], 0, buffsize);
	LoadString(mp_module, IDS_HMPRNG_BASSAMOUNT, buffer, ntbuffsize);
	CreateToolTip(IDC_SLIDER_BASSAMOUNT, hwnd, buffer, ttstyles, ttwidths, FALSE);
	memset(&buffer[0], 0, buffsize);
	LoadString(mp_module, IDS_HMPRNG_BASSRANGE, buffer, ntbuffsize);
	CreateToolTip(IDC_SLIDER_BASSRANGE, hwnd, buffer, ttstyles, ttwidths, FALSE);
	memset(&buffer[0], 0, buffsize);
	LoadString(mp_module, IDS_HMPRNG_SURROUNDDEPTH, buffer, ntbuffsize);
	CreateToolTip(IDC_SLIDER_SURROUNDDEPTH, hwnd, buffer, ttstyles, ttwidths, FALSE);
	memset(&buffer[0], 0, buffsize);
	LoadString(mp_module, IDS_HMPRNG_SURROUNDDELAY, buffer, ntbuffsize);
	CreateToolTip(IDC_SLIDER_SURROUNDDELAY, hwnd, buffer, ttstyles, ttwidths, FALSE);
	//Trackbars(sliders) - group 3
	memset(&buffer[0], 0, buffsize);
	LoadString(mp_module, IDS_HMPRNG_LOOPCOUNT , buffer, ntbuffsize);
	CreateToolTip(IDC_SLIDER_LOOPCOUNT, hwnd, buffer, ttstyles, ttwidths, FALSE);
	memset(&buffer[0], 0, buffsize);
	LoadString(mp_module, IDS_HMPRNG_PREAMP , buffer, ntbuffsize);
	CreateToolTip(IDC_SLIDER_PREAMP, hwnd, buffer, ttstyles, ttwidths, FALSE);
	memset(&buffer[0], 0, buffsize);
	LoadString(mp_module, IDS_HPLRNG_INFOREFRESH, buffer, ntbuffsize);
	CreateToolTip(IDC_SLIDER_INFOREFRESH, hwnd, buffer, ttstyles, ttwidths, FALSE);
	memset(&buffer[0], 0, buffsize);
	LoadString(mp_module, IDS_HPLRNG_INFOBUFFERING, buffer, ntbuffsize);
	CreateToolTip(IDC_SLIDER_INFOBUFFERING, hwnd, buffer, ttstyles, ttwidths, FALSE);
	//Formats button
	memset(&buffer[0], 0, buffsize);
	LoadString(mp_module, IDS_HPLBTN_FORMATS, buffer, ntbuffsize);
	CreateToolTip(IDC_BUTTON_FORMATS, hwnd, buffer, ttstyles, ttwidths, FALSE);
	//Reset button
	memset(&buffer[0], 0, buffsize);
	LoadString(mp_module, IDS_HPLBTN_CFGRESET, buffer, ntbuffsize);
	CreateToolTip(IDC_BUTTON_CFGRESET, hwnd, buffer, ttstyles, ttwidths, FALSE);
	//OK button
	memset(&buffer[0], 0, buffsize);
	LoadString(mp_module, IDS_HPLBTN_CFGIDOK, buffer, ntbuffsize);
	CreateToolTip(IDOK, hwnd, buffer, ttstyles, ttwidths, FALSE);
	//Cancel button
	memset(&buffer[0], 0, buffsize);
	LoadString(mp_module, IDS_HPLBTN_CFGIDCANCEL, buffer, ntbuffsize);
	CreateToolTip(IDCANCEL, hwnd, buffer, ttstyles, ttwidths, FALSE);

	return TRUE;
}
catch ( ... ) {
	MessageBox(hwnd, "ERROR: Config dialog initialization failed!", "Modplug Settings", 0);
	return FALSE;
}
}

BOOL AcceptSettings(HWND hwnd, All_Settings * pmpas) {
	ModPlug_Settings *curcfg, *bupcfg;
	Plugin_Settings *curpcfg, *buppcfg;
	curcfg = (ModPlug_Settings*)(*(All_Settings*)pmpas).mp_settings;
	curpcfg = (Plugin_Settings*)(*(All_Settings*)pmpas).mpp_settings;
	bupcfg = (ModPlug_Settings*)(*(All_Settings*)pmpas).mp_backup;
	buppcfg = (Plugin_Settings*)(*(All_Settings*)pmpas).mpp_backup;
	HWND CurDlgItem;
	int pos, val;
	char strnum [12];
	UINT nCheck;
	//Modplug flags
	curcfg->mFlags = 0;

	nCheck = SendMessage(GetDlgItem(hwnd, IDC_CHECK_OVERSAMPLING), BM_GETCHECK, (WPARAM)0, (LPARAM)0);
	if ((nCheck & BST_CHECKED) != 0) {
		curcfg->mFlags = (curcfg->mFlags | MODPLUG_ENABLE_OVERSAMPLING);
	}
	nCheck = SendMessage(GetDlgItem(hwnd, IDC_CHECK_NOISEREDUCTION), BM_GETCHECK, (WPARAM)0, (LPARAM)0);
	if ((nCheck & BST_CHECKED) != 0) {
		curcfg->mFlags = (curcfg->mFlags | MODPLUG_ENABLE_NOISE_REDUCTION);
	}
	nCheck = SendMessage(GetDlgItem(hwnd, IDC_CHECK_REVERB), BM_GETCHECK, (WPARAM)0, (LPARAM)0);
	if ((nCheck & BST_CHECKED) != 0) {
		curcfg->mFlags = (curcfg->mFlags | MODPLUG_ENABLE_REVERB);
	}
	nCheck = SendMessage(GetDlgItem(hwnd, IDC_CHECK_MEGABASS), BM_GETCHECK, (WPARAM)0, (LPARAM)0);
	if ((nCheck & BST_CHECKED) != 0) {
		curcfg->mFlags = (curcfg->mFlags | MODPLUG_ENABLE_MEGABASS);
	}
	nCheck = SendMessage(GetDlgItem(hwnd, IDC_CHECK_SURROUND), BM_GETCHECK, (WPARAM)0, (LPARAM)0);
	if ((nCheck & BST_CHECKED) != 0) {
		curcfg->mFlags = (curcfg->mFlags | MODPLUG_ENABLE_SURROUND);
	}
	nCheck = SendMessage(GetDlgItem(hwnd, IDC_CHECK_USEAGC), BM_GETCHECK, (WPARAM)0, (LPARAM)0);
	if ((nCheck & BST_CHECKED) != 0) {
		curpcfg->pUseAGC = 1;
	} else {
		curpcfg->pUseAGC = 0;
	}
	//Plugin flags
	curpcfg->pFlags = 0;
	nCheck = SendMessage(GetDlgItem(hwnd, IDC_CHECK_INTVOLUMECTRL), BM_GETCHECK, (WPARAM)0, (LPARAM)0);
	if ((nCheck & BST_CHECKED) != 0) {
		curpcfg->pFlags = (curpcfg->pFlags | 1);
	}
	nCheck = SendMessage(GetDlgItem(hwnd, IDC_CHECK_SEEKBYORDERS), BM_GETCHECK, (WPARAM)0, (LPARAM)0);
	if ((nCheck & BST_CHECKED) != 0) {
		curpcfg->pFlags = (curpcfg->pFlags | 4);
	}
	//Sampling rate
	val = 44100;
	CurDlgItem = GetDlgItem(hwnd, IDC_LST_SAMPLINGRATE);
	if (CurDlgItem != NULL) {
		pos = (int)SendMessage(CurDlgItem, LB_GETCURSEL, (WPARAM)0, (LPARAM)0);
		val = SendMessage(CurDlgItem, LB_GETITEMDATA, (WPARAM)pos, (LPARAM)0);
	}
	if ((val < 8000) || (val > 96000)) {
		val = 44100;
	}
	curcfg->mFrequency = val;
	//Channels
	nCheck = SendMessage(GetDlgItem(hwnd, IDC_RADIO_CHANNELSMONO), BM_GETCHECK, (WPARAM)0, (LPARAM)0);
	if ((nCheck & BST_CHECKED) != 0) {
		curcfg->mChannels = 1;
	} else {
		nCheck = SendMessage(GetDlgItem(hwnd, IDC_RADIO_CHANNELSQUAD), BM_GETCHECK, (WPARAM)0, (LPARAM)0);
		if ((nCheck & BST_CHECKED) != 0) {
			curcfg->mChannels = 4;
		} else {
			curcfg->mChannels = 2;
		}
	}
	//Bit depth
	nCheck = SendMessage(GetDlgItem(hwnd, IDC_RADIO_BITDEPTH32), BM_GETCHECK, (WPARAM)0, (LPARAM)0);
	if ((nCheck & BST_CHECKED) != 0) {
		curcfg->mBits = 32;
	} else {
		nCheck = SendMessage(GetDlgItem(hwnd, IDC_RADIO_BITDEPTH8), BM_GETCHECK, (WPARAM)0, (LPARAM)0);
		if ((nCheck & BST_CHECKED) != 0) {
			curcfg->mBits = 8;
		} else {
			curcfg->mBits = 16;
		}
	}
	//Interpolation (resampling)
	nCheck = SendMessage(GetDlgItem(hwnd, IDC_RADIO_RESAMPLING8TAPFIR), BM_GETCHECK, (WPARAM)0, (LPARAM)0);
	if ((nCheck & BST_CHECKED) != 0) {
		curcfg->mResamplingMode = MODPLUG_RESAMPLE_FIR;
	} else {
		nCheck = SendMessage(GetDlgItem(hwnd, IDC_RADIO_RESAMPLINGLINEAR), BM_GETCHECK, (WPARAM)0, (LPARAM)0);
		if ((nCheck & BST_CHECKED) != 0) {
			curcfg->mResamplingMode = MODPLUG_RESAMPLE_LINEAR;
		} else {
			nCheck = SendMessage(GetDlgItem(hwnd, IDC_RADIO_RESAMPLINGNONE), BM_GETCHECK, (WPARAM)0, (LPARAM)0);
			if ((nCheck & BST_CHECKED) != 0) {
				curcfg->mResamplingMode = MODPLUG_RESAMPLE_NEAREST;
			} else {
				curcfg->mResamplingMode = MODPLUG_RESAMPLE_SPLINE;
			}
		}
	}
	//Sliders and edit boxes - group1
	curcfg->mStereoSeparation = SendMessage(GetDlgItem(hwnd, IDC_SLIDER_STEREOSEPARATION), TBM_GETPOS, (WPARAM)0, (LPARAM)0);
	curcfg->mMaxMixChannels = SendMessage(GetDlgItem(hwnd, IDC_SLIDER_MAXMIXCHANNELS), TBM_GETPOS, (WPARAM)0, (LPARAM)0);
	curcfg->mReverbDepth = SendMessage(GetDlgItem(hwnd, IDC_SLIDER_REVERBDEPTH), TBM_GETPOS, (WPARAM)0, (LPARAM)0);
	curcfg->mReverbDelay = SendMessage(GetDlgItem(hwnd, IDC_SLIDER_REVERBDELAY), TBM_GETPOS, (WPARAM)0, (LPARAM)0);
	//Sliders and edit boxes - group2
	curcfg->mBassAmount = SendMessage(GetDlgItem(hwnd, IDC_SLIDER_BASSAMOUNT), TBM_GETPOS, (WPARAM)0, (LPARAM)0);
	curcfg->mBassRange = SendMessage(GetDlgItem(hwnd, IDC_SLIDER_BASSRANGE), TBM_GETPOS, (WPARAM)0, (LPARAM)0);
	curcfg->mSurroundDepth = SendMessage(GetDlgItem(hwnd, IDC_SLIDER_SURROUNDDEPTH), TBM_GETPOS, (WPARAM)0, (LPARAM)0);
	curcfg->mSurroundDelay = SendMessage(GetDlgItem(hwnd, IDC_SLIDER_SURROUNDDELAY), TBM_GETPOS, (WPARAM)0, (LPARAM)0);
	//Sliders and edit boxes - group3
	//curcfg->mLoopCount = SendMessage(GetDlgItem(hwnd, IDC_SLIDER_LOOPCOUNT), TBM_GETPOS, (WPARAM)0, (LPARAM)0); //EditText field can hold more values
	SendMessage(GetDlgItem(hwnd, IDC_EDIT_LOOPCOUNT), WM_GETTEXT, (WPARAM)12, (LPARAM)strnum);
	curcfg->mLoopCount = atoi(strnum);
	curpcfg->pPreAmp = SendMessage(GetDlgItem(hwnd, IDC_SLIDER_PREAMP), TBM_GETPOS, (WPARAM)0, (LPARAM)0);
	curpcfg->pInfoRefresh = SendMessage(GetDlgItem(hwnd, IDC_SLIDER_INFOREFRESH), TBM_GETPOS, (WPARAM)0, (LPARAM)0);
	curpcfg->pInfoBuffering = SendMessage(GetDlgItem(hwnd, IDC_SLIDER_INFOBUFFERING), TBM_GETPOS, (WPARAM)0, (LPARAM)0);

	//NOTE: Invalid values automatically replaced by a default value will be not saved until a value changed by the user!
	int changes = 0;
	if (bupcfg->mFlags != curcfg->mFlags) changes++;
	if (buppcfg->pUseAGC != curpcfg->pUseAGC) changes++;
	if (bupcfg->mChannels != curcfg->mChannels) changes++;
	if (bupcfg->mBits != curcfg->mBits) changes++;
	if (bupcfg->mFrequency != curcfg->mFrequency) changes++;
	if (bupcfg->mResamplingMode != curcfg->mResamplingMode) changes++;
	if (bupcfg->mStereoSeparation != curcfg->mStereoSeparation) changes++;
	if (bupcfg->mMaxMixChannels != curcfg->mMaxMixChannels) changes++;
	if (bupcfg->mReverbDepth != curcfg->mReverbDepth) changes++;
	if (bupcfg->mReverbDelay != curcfg->mReverbDelay) changes++;
	if (bupcfg->mBassAmount != curcfg->mBassAmount) changes++;
	if (bupcfg->mBassRange != curcfg->mBassRange) changes++;
	if (bupcfg->mSurroundDepth != curcfg->mSurroundDepth) changes++;
	if (bupcfg->mSurroundDelay != curcfg->mSurroundDelay) changes++;
	if (bupcfg->mLoopCount != curcfg->mLoopCount) changes++;
	if (buppcfg->pPreAmp != curpcfg->pPreAmp) changes++;
	if (buppcfg->pInfoRefresh != curpcfg->pInfoRefresh) changes++;
	if (buppcfg->pInfoBuffering != curpcfg->pInfoBuffering) changes++;
	if (buppcfg->pFlags != curpcfg->pFlags) changes++;
	//check for changes in fileformats
	unsigned char n, nb, cpos, ind;
	n = strlen(curpcfg->pFormats);
	nb = strlen(buppcfg->pFormats);
	BOOL isFileformatsChanged = FALSE;
	if ((n == nb) || ((n == (nb + 1)) && (buppcfg->pFormats[nb-1] != ';'))) {
		if (n == (nb + 1)) n--;
		cpos = 0;
		ind = 0;
		//this uppercase conversion may be not perfect, but it should be sufficient for file extensions
		while ((cpos < n) && (ind == 0)) {
			if (buppcfg->pFormats[cpos] > 0x60) ind = (toupper(buppcfg->pFormats[cpos]) ^ toupper(curpcfg->pFormats[cpos]));
			else ind = ((buppcfg->pFormats[cpos]) ^ (curpcfg->pFormats[cpos]));
			cpos++;
		}
		if (ind != 0) isFileformatsChanged = TRUE;
	} else isFileformatsChanged = TRUE;
	if (isFileformatsChanged) changes++;
	if ((curpcfg->isReset > 0) || (changes > 0)) {
	try {
		if (changes > 0) curpcfg->isChanged = 1;
		const char *IniSection = "ModPlug";
#define WRITEINITEXT(key, value) WritePrivateProfileString(IniSection, key, value, curpcfg->pIniFN)
		//InitializeCriticalSection(&mp_lock); //already initialized in main.cpp
		EnterCriticalSection(&mp_lock);
		if ((curpcfg->isReset > 0) || (bupcfg->mFlags != curcfg->mFlags)) {
			_itoa(curcfg->mFlags, strnum, 10);
			WRITEINITEXT("Flags", strnum);
		}
		if ((curpcfg->isReset > 0) || (bupcfg->mChannels != curcfg->mChannels)) {
			_itoa(curcfg->mChannels, strnum, 10);
			WRITEINITEXT("Channels", strnum);
		}
		if ((curpcfg->isReset > 0) || (bupcfg->mBits != curcfg->mBits)) {
			_itoa(curcfg->mBits, strnum, 10);
			WRITEINITEXT("Bits", strnum);
		}
		if ((curpcfg->isReset > 0) || (bupcfg->mFrequency != curcfg->mFrequency)) {
			_itoa(curcfg->mFrequency, strnum, 10);
			WRITEINITEXT("Frequency", strnum);
		}
		if ((curpcfg->isReset > 0) || (bupcfg->mResamplingMode != curcfg->mResamplingMode)) {
			_itoa(curcfg->mResamplingMode, strnum, 10);
			WRITEINITEXT("ResamplingMode", strnum);
		}
		if ((curpcfg->isReset > 0) || (bupcfg->mStereoSeparation != curcfg->mStereoSeparation)) {
			_itoa(curcfg->mStereoSeparation, strnum, 10);
			WRITEINITEXT("StereoSeparation", strnum);
		}
		if ((curpcfg->isReset > 0) || (bupcfg->mMaxMixChannels != curcfg->mMaxMixChannels)) {
			_itoa(curcfg->mMaxMixChannels, strnum, 10);
			WRITEINITEXT("MaxMixChannels", strnum);
		}
		if ((curpcfg->isReset > 0) || (bupcfg->mReverbDepth != curcfg->mReverbDepth)) {
			_itoa(curcfg->mReverbDepth, strnum, 10);
			WRITEINITEXT("ReverbDepth", strnum);
		}
		if ((curpcfg->isReset > 0) || (bupcfg->mReverbDelay != curcfg->mReverbDelay)) {
			_itoa(curcfg->mReverbDelay, strnum, 10);
			WRITEINITEXT("ReverbDelay", strnum);
		}
		if ((curpcfg->isReset > 0) || (bupcfg->mBassAmount != curcfg->mBassAmount)) {
			_itoa(curcfg->mBassAmount, strnum, 10);
			WRITEINITEXT("BassAmount", strnum);
		}
		if ((curpcfg->isReset > 0) || (bupcfg->mBassRange != curcfg->mBassRange)) {
			_itoa(curcfg->mBassRange, strnum, 10);
			WRITEINITEXT("BassRange", strnum);
		}
		if ((curpcfg->isReset > 0) || (bupcfg->mSurroundDepth != curcfg->mSurroundDepth)) {
			_itoa(curcfg->mSurroundDepth, strnum, 10);
			WRITEINITEXT("SurroundDepth", strnum);
		}
		if ((curpcfg->isReset > 0) || (bupcfg->mSurroundDelay != curcfg->mSurroundDelay)) {
			_itoa(curcfg->mSurroundDelay, strnum, 10);
			WRITEINITEXT("SurroundDelay", strnum);
		}
		if ((curpcfg->isReset > 0) || (bupcfg->mLoopCount != curcfg->mLoopCount)) {
			_itoa(curcfg->mLoopCount, strnum, 10);
			WRITEINITEXT("LoopCount", strnum);
		}
		if ((curpcfg->isReset > 0) || (buppcfg->pInfoRefresh != curpcfg->pInfoRefresh)) {
			_itoa(curpcfg->pInfoRefresh, strnum, 10);
			WRITEINITEXT("InfoRefresh", strnum);
		}
		if ((curpcfg->isReset > 0) || (buppcfg->pInfoBuffering != curpcfg->pInfoBuffering)) {
			_itoa(curpcfg->pInfoBuffering, strnum, 10);
			WRITEINITEXT("InfoBuffering", strnum);
		}
		if ((curpcfg->isReset > 0) || (buppcfg->pFlags != curpcfg->pFlags)) {
			_itoa(curpcfg->pFlags, strnum, 10);
			WRITEINITEXT("PluginFlags", strnum);
		}
		if ((curpcfg->isReset > 0) || (buppcfg->pUseAGC != curpcfg->pUseAGC)) {
			if (curpcfg->pUseAGC == 0) WRITEINITEXT("UseAGC", "0");
			else WRITEINITEXT("UseAGC", "1");
		}
		if ((curpcfg->isReset > 0) || (buppcfg->pPreAmp != curpcfg->pPreAmp)) {
			_itoa(curpcfg->pPreAmp, strnum, 10);
			WRITEINITEXT("PreAmp", strnum);
		}
		if ((curpcfg->isReset > 0) || (isFileformatsChanged)) {
			WRITEINITEXT("Formats", curpcfg->pFormats);
		}
		LeaveCriticalSection(&mp_lock);
	}
	catch ( ... ) {
		MessageBox(hwnd, "ERROR: Failed to write settings to the INI file!", "Modplug Settings", 0);
		return FALSE;
	}
#undef WRITEINITEXT
	}
	char* tmpFormats = buppcfg->pFormats;
	char* tmpIniFN = buppcfg->pIniFN;
	int tmpFormatsSize = buppcfg->pFormatsSize;
	int tmpIniFNSize = buppcfg->pIniFNSize;
	*bupcfg = *curcfg;
	*buppcfg = *curpcfg;
	buppcfg->pFormats = tmpFormats;
	buppcfg->pIniFN = tmpIniFN;
	buppcfg->pFormatsSize = tmpFormatsSize;
	buppcfg->pIniFNSize = tmpIniFNSize;
	memcpy(&buppcfg->pFormats[0], curpcfg->pFormats, tmpFormatsSize);
	memcpy(&buppcfg->pIniFN[0], curpcfg->pIniFN, tmpIniFNSize);
	return TRUE;
}

DWORD WINAPI ConfigDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	//NOTE: lParam at this stage is empty! It is accessible only at WM_INITDIALOG stage.
	ModPlug_Settings *pcurcfg, *pbupcfg;
	Plugin_Settings *pcurpcfg, *pbuppcfg;
	UINT nCheck;

	switch (msg) {
		case WM_INITDIALOG:
			SetWindowLongPtr(hwnd, GWL_USERDATA, lParam);
			pcurcfg = (*(All_Settings*)lParam).mp_settings;
			pcurpcfg = (*(All_Settings*)lParam).mpp_settings;
			ConfigDlgInit(hwnd, pcurcfg, pcurpcfg);
		case WM_SETFOCUS:
		case WM_KILLFOCUS:
			return 1;

		case WM_COMMAND:
			if(HIWORD(wParam) == BN_CLICKED) {
				//unsigned int i;
				if(LOWORD(wParam) == IDCANCEL) goto close;
				if(LOWORD(wParam) == IDOK) goto accept;
				if(LOWORD(wParam) == IDC_BUTTON_FORMATS) goto formats;
				if(LOWORD(wParam) == IDC_BUTTON_CFGRESET) goto reset;
				if(LOWORD(wParam) == IDC_CHECK_REVERB) goto setreverbstatus;
				if(LOWORD(wParam) == IDC_CHECK_MEGABASS) goto setmegabassstatus;
				if(LOWORD(wParam) == IDC_CHECK_SURROUND) goto setsurroundstatus;
				goto End_BN_CLICKED;

			formats:
				{
					All_Settings* pmpas = (All_Settings*)GetWindowLongPtr(hwnd, GWL_USERDATA);
					pcurpcfg = (Plugin_Settings*)(*(All_Settings*)pmpas).mpp_settings;
					DialogBoxParam(mp_module, MAKEINTRESOURCE(IDD_FORMATS), hwnd, (DLGPROC)FormatsDlgProc, (LPARAM)pcurpcfg);
				}
				goto End_BN_CLICKED;

			reset:
				{
					All_Settings* pmpas = (All_Settings*)GetWindowLongPtr(hwnd, GWL_USERDATA);
					pcurcfg = (ModPlug_Settings*)(*(All_Settings*)pmpas).mp_settings;
					pcurpcfg = (Plugin_Settings*)(*(All_Settings*)pmpas).mpp_settings;
					pbupcfg = (ModPlug_Settings*)(*(All_Settings*)pmpas).mp_backup;
					pbuppcfg = (Plugin_Settings*)(*(All_Settings*)pmpas).mpp_backup;
					ModPlug_Settings defcfg = {3, 2, 16, 44100, 1, 128, 32, 0, 0, 0, 0, 0, 0, 0};
					Plugin_Settings defpcfg = {4, 125, 500, (*pcurpcfg).pUseInfotimer, 0, 0, (*pcurpcfg).pFormatsSize, (*pcurpcfg).pIniFNSize, 0, 1, (*pcurpcfg).pFormats, (*pcurpcfg).pIniFN};
					*pcurcfg = defcfg;
					*pcurpcfg = defpcfg;
					ConfigDlgInit(hwnd, pcurcfg, pcurpcfg);
				}
				goto End_BN_CLICKED;

			accept:
				{
					All_Settings* pmpas = (All_Settings*)GetWindowLongPtr(hwnd, GWL_USERDATA);
					AcceptSettings(hwnd, pmpas);
				}
				EndDialog(hwnd, 1);
				goto End_BN_CLICKED;

			setreverbstatus:
				nCheck = SendMessage(GetDlgItem(hwnd, IDC_CHECK_REVERB), BM_GETCHECK, (WPARAM)0, (LPARAM)0);
				if ((nCheck & BST_CHECKED) == 0) {
					EnableWindow(GetDlgItem(hwnd, IDC_EDIT_REVERBDEPTH), FALSE);
					EnableWindow(GetDlgItem(hwnd, IDC_SLIDER_REVERBDEPTH), FALSE);
					EnableWindow(GetDlgItem(hwnd, IDC_EDIT_REVERBDELAY), FALSE);
					EnableWindow(GetDlgItem(hwnd, IDC_SLIDER_REVERBDELAY), FALSE);
				} else {
					EnableWindow(GetDlgItem(hwnd, IDC_EDIT_REVERBDEPTH), TRUE);
					EnableWindow(GetDlgItem(hwnd, IDC_SLIDER_REVERBDEPTH), TRUE);
					EnableWindow(GetDlgItem(hwnd, IDC_EDIT_REVERBDELAY), TRUE);
					EnableWindow(GetDlgItem(hwnd, IDC_SLIDER_REVERBDELAY), TRUE);
				}
				goto End_BN_CLICKED;

			setmegabassstatus:
				nCheck = SendMessage(GetDlgItem(hwnd, IDC_CHECK_MEGABASS), BM_GETCHECK, (WPARAM)0, (LPARAM)0);
				if ((nCheck & BST_CHECKED) == 0) {
					EnableWindow(GetDlgItem(hwnd, IDC_EDIT_BASSAMOUNT), FALSE);
					EnableWindow(GetDlgItem(hwnd, IDC_SLIDER_BASSAMOUNT), FALSE);
					EnableWindow(GetDlgItem(hwnd, IDC_EDIT_BASSRANGE), FALSE);
					EnableWindow(GetDlgItem(hwnd, IDC_SLIDER_BASSRANGE), FALSE);
				} else {
					EnableWindow(GetDlgItem(hwnd, IDC_EDIT_BASSAMOUNT), TRUE);
					EnableWindow(GetDlgItem(hwnd, IDC_SLIDER_BASSAMOUNT), TRUE);
					EnableWindow(GetDlgItem(hwnd, IDC_EDIT_BASSRANGE), TRUE);
					EnableWindow(GetDlgItem(hwnd, IDC_SLIDER_BASSRANGE), TRUE);
				}
				goto End_BN_CLICKED;

			setsurroundstatus:
				nCheck = SendMessage(GetDlgItem(hwnd, IDC_CHECK_SURROUND), BM_GETCHECK, (WPARAM)0, (LPARAM)0);
				if ((nCheck & BST_CHECKED) == 0) {
					EnableWindow(GetDlgItem(hwnd, IDC_EDIT_SURROUNDDEPTH), FALSE);
					EnableWindow(GetDlgItem(hwnd, IDC_SLIDER_SURROUNDDEPTH), FALSE);
					EnableWindow(GetDlgItem(hwnd, IDC_EDIT_SURROUNDDELAY), FALSE);
					EnableWindow(GetDlgItem(hwnd, IDC_SLIDER_SURROUNDDELAY), FALSE);
				} else {
					EnableWindow(GetDlgItem(hwnd, IDC_EDIT_SURROUNDDEPTH), TRUE);
					EnableWindow(GetDlgItem(hwnd, IDC_SLIDER_SURROUNDDEPTH), TRUE);
					EnableWindow(GetDlgItem(hwnd, IDC_EDIT_SURROUNDDELAY), TRUE);
					EnableWindow(GetDlgItem(hwnd, IDC_SLIDER_SURROUNDDELAY), TRUE);
				}
				goto End_BN_CLICKED;

			End_BN_CLICKED:
			; } else if ((HIWORD(wParam) == EN_CHANGE) || (HIWORD(wParam) == EN_KILLFOCUS)) {
				int TrackBarID;
				HWND TrackBarHWND;
				int TrackBarMinVal, TrackBarMaxVal, curEditVal, bupEditVal;
				char strEditBoxValue[12];
				switch (LOWORD((WPARAM) wParam)) {
					case IDC_EDIT_STEREOSEPARATION:
						TrackBarID=IDC_SLIDER_STEREOSEPARATION;
						goto setTrackBar;
					case IDC_EDIT_MAXMIXCHANNELS:
						TrackBarID=IDC_SLIDER_MAXMIXCHANNELS;
						goto setTrackBar;
					case IDC_EDIT_REVERBDEPTH:
						TrackBarID=IDC_SLIDER_REVERBDEPTH;
						goto setTrackBar;
					case IDC_EDIT_REVERBDELAY:
						TrackBarID=IDC_SLIDER_REVERBDELAY;
						goto setTrackBar;

					case IDC_EDIT_BASSAMOUNT:
						TrackBarID=IDC_SLIDER_BASSAMOUNT;
						goto setTrackBar;
					case IDC_EDIT_BASSRANGE:
						TrackBarID=IDC_SLIDER_BASSRANGE;
						goto setTrackBar;
					case IDC_EDIT_SURROUNDDEPTH:
						TrackBarID=IDC_SLIDER_SURROUNDDEPTH;
						goto setTrackBar;
					case IDC_EDIT_SURROUNDDELAY:
						TrackBarID=IDC_SLIDER_SURROUNDDELAY;
						TrackBarHWND = GetDlgItem(hwnd, TrackBarID);
						TrackBarMinVal = SendMessage(TrackBarHWND, TBM_GETRANGEMIN, (WPARAM)0, (LPARAM)0);
						TrackBarMaxVal = SendMessage(TrackBarHWND, TBM_GETRANGEMAX, (WPARAM)0, (LPARAM)0);
						GetWindowText((HWND) lParam, strEditBoxValue, 12);
						curEditVal = atoi(strEditBoxValue);
						bupEditVal = curEditVal;
						if ((HIWORD(wParam) == EN_KILLFOCUS) && (curEditVal < 4)) {
							curEditVal = 4;
						}
						goto setTrackBarCustom;

					case IDC_EDIT_LOOPCOUNT:
						TrackBarID=IDC_SLIDER_LOOPCOUNT;
						TrackBarHWND = GetDlgItem(hwnd, TrackBarID);
						TrackBarMinVal = SendMessage(TrackBarHWND, TBM_GETRANGEMIN, (WPARAM)0, (LPARAM)0);
						TrackBarMaxVal = 32767; //greater numbers are allowed that are not displayed/preserved by the trackbar
						GetWindowText((HWND) lParam, strEditBoxValue, 12);
						curEditVal = atoi(strEditBoxValue);
						bupEditVal = curEditVal;
						goto setTrackBarCustom;
					case IDC_EDIT_PREAMP:
						TrackBarID=IDC_SLIDER_PREAMP;
						TrackBarHWND = GetDlgItem(hwnd, TrackBarID);
						TrackBarMinVal = SendMessage(TrackBarHWND, TBM_GETRANGEMIN, (WPARAM)0, (LPARAM)0);
						TrackBarMaxVal = SendMessage(TrackBarHWND, TBM_GETRANGEMAX, (WPARAM)0, (LPARAM)0);
						GetWindowText((HWND) lParam, strEditBoxValue, 12);
						curEditVal = atoi(strEditBoxValue);
						bupEditVal = curEditVal;
						if ((HIWORD(wParam) == EN_KILLFOCUS) && (curEditVal < 16) && (curEditVal > 0)) {
							curEditVal = 16;
						}
						goto setTrackBarCustom;
					case IDC_EDIT_INFOREFRESH:
						TrackBarID=IDC_SLIDER_INFOREFRESH;
						goto setTrackBar;
				    case IDC_EDIT_INFOBUFFERING:
						TrackBarID=IDC_SLIDER_INFOBUFFERING;
						TrackBarHWND = GetDlgItem(hwnd, TrackBarID);
						TrackBarMinVal = SendMessage(TrackBarHWND, TBM_GETRANGEMIN, (WPARAM)0, (LPARAM)0);
						TrackBarMaxVal = SendMessage(TrackBarHWND, TBM_GETRANGEMAX, (WPARAM)0, (LPARAM)0);
						GetWindowText((HWND) lParam, strEditBoxValue, 12);
						curEditVal = atoi(strEditBoxValue);
						bupEditVal = curEditVal;
						if ((HIWORD(wParam) == EN_KILLFOCUS) && (curEditVal < 100) && (curEditVal > 0)) {
							curEditVal = 100;
						}
						goto setTrackBarCustom;
					break;
				setTrackBar:
					TrackBarHWND = GetDlgItem(hwnd, TrackBarID);
					TrackBarMinVal = SendMessage(TrackBarHWND, TBM_GETRANGEMIN, (WPARAM)0, (LPARAM)0);
					TrackBarMaxVal = SendMessage(TrackBarHWND, TBM_GETRANGEMAX, (WPARAM)0, (LPARAM)0);
					GetWindowText((HWND) lParam, strEditBoxValue, 12);
					curEditVal = atoi(strEditBoxValue);
					bupEditVal = curEditVal;
				setTrackBarCustom:
					if (HIWORD(wParam) == EN_KILLFOCUS) {
						if (curEditVal < TrackBarMinVal) {
							curEditVal = TrackBarMinVal;
						} else if (curEditVal > TrackBarMaxVal) {
							curEditVal = TrackBarMaxVal;
						}
					} else {
						//EN_CHANGE will fire at every typed character - do not check the min. value
						if (curEditVal > TrackBarMaxVal) {
							curEditVal = TrackBarMaxVal;
						}
					}
					if (curEditVal != bupEditVal) {
						_itoa(curEditVal, strEditBoxValue, 10);
						SetWindowText((HWND) lParam, strEditBoxValue);
					}
					SendMessage(TrackBarHWND, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)curEditVal);
					return 0;

				}
				break;
			}
			break;

		case WM_HSCROLL:
			int intTrackBarValue;
			int EditBoxID;
			char strTrackBarValue[12]; //required buffer: radix 10: 7 for 16-bits, 12 for 32-bits; radix 2: 17 bytes for 16-bits 33 for 32-bits
			switch ((int)GetDlgCtrlID((HWND) lParam)) {
				case IDC_SLIDER_STEREOSEPARATION:
					EditBoxID=IDC_EDIT_STEREOSEPARATION;
					goto setEditBox;
				case IDC_SLIDER_MAXMIXCHANNELS:
					EditBoxID=IDC_EDIT_MAXMIXCHANNELS;
					goto setEditBox;
				case IDC_SLIDER_REVERBDEPTH:
					EditBoxID=IDC_EDIT_REVERBDEPTH;
					goto setEditBox;
				case IDC_SLIDER_REVERBDELAY:
					EditBoxID=IDC_EDIT_REVERBDELAY;
					goto setEditBox;

				case IDC_SLIDER_BASSAMOUNT:
					EditBoxID=IDC_EDIT_BASSAMOUNT;
					goto setEditBox;
				case IDC_SLIDER_BASSRANGE:
					EditBoxID=IDC_EDIT_BASSRANGE;
					goto setEditBox;
				case IDC_SLIDER_SURROUNDDEPTH:
					EditBoxID=IDC_EDIT_SURROUNDDEPTH;
					goto setEditBox;
				case IDC_SLIDER_SURROUNDDELAY:
					EditBoxID=IDC_EDIT_SURROUNDDELAY;
					intTrackBarValue = SendMessage((HWND) lParam, TBM_GETPOS, (WPARAM)0, (LPARAM)0);
					if (intTrackBarValue < 4) {
						intTrackBarValue = 4;
						SendMessage((HWND) lParam, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)intTrackBarValue);
					}
					goto setEditBoxCustom;

				case IDC_SLIDER_LOOPCOUNT:
					EditBoxID=IDC_EDIT_LOOPCOUNT;
					goto setEditBox;
				case IDC_SLIDER_PREAMP:
					EditBoxID=IDC_EDIT_PREAMP;
					intTrackBarValue = SendMessage((HWND) lParam, TBM_GETPOS, (WPARAM)0, (LPARAM)0);
					if ((intTrackBarValue < 16) && (intTrackBarValue > 0)) {
						//we want to be able to move out of the dead zone easily
						if (intTrackBarValue < SendMessage((HWND) lParam, TBM_GETLINESIZE, (WPARAM)0, (LPARAM)0)) intTrackBarValue = 0;
						else intTrackBarValue = 16;
						SendMessage((HWND) lParam, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)intTrackBarValue);
					}
					goto setEditBoxCustom;
				case IDC_SLIDER_INFOREFRESH:
					EditBoxID=IDC_EDIT_INFOREFRESH;
					goto setEditBox;
				case IDC_SLIDER_INFOBUFFERING:
					EditBoxID=IDC_EDIT_INFOBUFFERING;
					intTrackBarValue = SendMessage((HWND) lParam, TBM_GETPOS, (WPARAM)0, (LPARAM)0);
					if ((intTrackBarValue < 100) && (intTrackBarValue > 0)) {
						//we want to be able to move out of the dead zone easily
						if (intTrackBarValue < SendMessage((HWND) lParam, TBM_GETLINESIZE, (WPARAM)0, (LPARAM)0)) intTrackBarValue = 0;
						else intTrackBarValue = 100;
						SendMessage((HWND) lParam, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)intTrackBarValue);
					}
					goto setEditBoxCustom;
				break;
			setEditBox:
				intTrackBarValue = SendMessage((HWND) lParam, TBM_GETPOS, (WPARAM)0, (LPARAM)0);
			setEditBoxCustom:
				_itoa( intTrackBarValue, strTrackBarValue, 10);
				SetWindowText(GetDlgItem(hwnd, EditBoxID), strTrackBarValue);
				return 0;
			}
			break;

	case WM_NOTIFY:
	{
		switch (((LPNMHDR)lParam)->code)
		{
		case TTN_GETDISPINFO:
			LPNMTTDISPINFO pInfo = (LPNMTTDISPINFO)lParam;


			SendMessage(pInfo->hdr.hwndFrom, TTM_SETMAXTIPWIDTH, 0, 150);
/*
			wcscpy_s(pInfo->szText, ARRAYSIZE(pInfo->szText), 
				L"This\nis a very long text string " \
				L"that must be broken into several lines.");
*/
			break;
		}
		break;
	}

		case WM_CLOSE:
		close:
			EndDialog(hwnd, 1);
			break;
	}
	return 0;
}
#endif // !MFCGUI
#else // !NOGUICONFIG
#pragma message( "NOGUI code compilation..." )
#endif // !NOGUICONFIG