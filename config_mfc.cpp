#ifndef NOGUICONFIG
#ifdef MFCGUI
#pragma message( "MFC Config GUI code compilation..." )

#ifdef _MSC_VER
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif // _MSC_VER

#include <afxwin.h>
#include <afxcmn.h>
#include <string>

//#include "main.h" //already included in the config_mfc.hpp
#include "formats.h"
#include "resource.h"
#include "libmodplug_0_8_8_x\src\modplug.h"
#include "config_mfc.hpp"

extern HINSTANCE mp_module;
CToolTipCtrl* ttcontrol;
CToolTipCtrl* fttcontrol;

// shared helper functions for tooltips - start
	void AdjustTTTimeAndWidth(CToolTipCtrl* ttctrl, int strlenght)
	{
		//requires a ttcontrol pointer to the current CToolTipCtrl
		if (ttctrl != NULL) {
			int i, j;
			j = (strlenght - 60);
			if (j > 0) {
				i = j / 20;
				j = j - (20 * i);
				if (j > 0) j = i + 1; else j = i; //will round up if there is a remainder
			} else j = 0;
			i = 4800 + (j * 1200);
			if (i > 65535) i = 65535;
			ttctrl->SetDelayTime(TTDT_AUTOPOP, i);

			//set width dependent on number of characters
			DWORD ttWidth;
			int basewidth, basechnum, incwidth, incchnum;
			basewidth = 160;
			basechnum = 80;
			if ((basewidth == 0) || (basechnum == 0)) {
				basewidth = 0;
				basechnum = 0;
			}
			incwidth = 32;
			incchnum = 40;
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
			ttctrl->SetMaxTipWidth(ttWidth);
		}
	}

	BOOL SetTootipTextFromRes(NMTTDISPINFO* pTTT, UINT ResID, CToolTipCtrl* ttctrl, char * CharBuff, DWORD BuffSize)
	{
		//requires a ttcontrol pointer to the current CToolTipCtrl
		memset(CharBuff, 0, BuffSize);
		LoadString(mp_module, ResID, CharBuff, BuffSize - 1);
		int strlenght = strlen(CharBuff);
		if (strlenght > 80) {
			pTTT->lpszText = CharBuff;
		} else {
			LoadString(mp_module, ResID, pTTT->szText, 80);
		}
		AdjustTTTimeAndWidth(ttctrl, strlenght);
		return TRUE;
	}
// shared helper functions for tooltips - end


class CSettingsApp : public CWinApp {
public:

	CSettingsApp() {
		return;
	}

public:

	virtual BOOL InitInstance() {
		if ( !CWinApp::InitInstance() )
		{
			return FALSE;
		}
		DllMainAttach();
		return TRUE;
	}

	virtual int ExitInstance() {
		DllMainDetach();
		return CWinApp::ExitInstance();
	}

	DECLARE_MESSAGE_MAP()

};

class CFormatsApp : public CWinApp {
public:

	CFormatsApp() {
		return;
	}

public:

	virtual BOOL InitInstance() {
		if ( !CWinApp::InitInstance() )
		{
			return FALSE;
		}
		DllMainAttach();
		return TRUE;
	}

	virtual int ExitInstance() {
		DllMainDetach();
		return CWinApp::ExitInstance();
	}

	DECLARE_MESSAGE_MAP()

};

BEGIN_MESSAGE_MAP(CSettingsApp, CWinApp)
END_MESSAGE_MAP()


CSettingsApp theSettingsApp;


class CSettingsDialog : public CDialog {

protected:

	DECLARE_MESSAGE_MAP()
	ModPlug_Settings* s, curcfg; // declared in modplug.h
	Plugin_Settings* p, curpcfg; // declared in main.h
	char* tmpfileformats;
	char* tmpinifn;

	CToolTipCtrl m_ToolTip;
	CString m_Title;

	CButton m_CheckBoxOversampling;
	CButton m_CheckBoxNoiseReduction;
	CButton m_CheckBoxReverb;
	CButton m_CheckBoxMegabass;
	CButton m_CheckBoxSurroundSound;
	CButton m_CheckBoxUseAGC;
	CListBox m_ListBoxSampleRate;
	CButton m_RadioBtnChannelsMono;
	CButton m_RadioBtnChannelsStereo;
	CButton m_RadioBtnChannelsQuad;
	CButton m_RadioBtnBitDepth8;
	CButton m_RadioBtnBitDepth16;
	CButton m_RadioBtnBitDepth32;
	CButton m_RadioBtnResamplingNone;
	CButton m_RadioBtnResamplingLinear;
	CButton m_RadioBtnResamplingCubic;
	CButton m_RadioBtnResampling8TapFIR;
	CButton m_CheckBoxIntVolumeCtrl;
	CButton m_CheckBoxSeekByOrders;
	CSliderCtrl m_SliderCtrlStereoSeparation;
	CEdit m_CEditStereoSeparation;
	CSliderCtrl m_SliderCtrlMaxMixChannels;
	CEdit m_CEditMaxMixChannels;
	CSliderCtrl m_SliderCtrlReverbDepth;
	CEdit m_CEditReverbDepth;
	CSliderCtrl m_SliderCtrlReverbDelay;
	CEdit m_CEditReverbDelay;
	CSliderCtrl m_SliderCtrlBassAmount;
	CEdit m_CEditBassAmount;
	CSliderCtrl m_SliderCtrlBassRange;
	CEdit m_CEditBassRange;
	CSliderCtrl m_SliderCtrlSurroundDepth;
	CEdit m_CEditSurroundDepth;
	CSliderCtrl m_SliderCtrlSurroundDelay;
	CEdit m_CEditSurroundDelay;
	CSliderCtrl m_SliderCtrlLoopCount;
	CEdit m_CEditLoopCount;
	CSliderCtrl m_SliderCtrlPreAmp;
	CEdit m_CEditPreAmp;
	CSliderCtrl m_SliderCtrlInfoRefresh;
	CEdit m_CEditInfoRefresh;
	CSliderCtrl m_SliderCtrlInfoBuffering;
	CEdit m_CEditInfoBuffering;

public:

	CSettingsDialog( ModPlug_Settings * s_, Plugin_Settings * p_, CString title, CWnd * parent = NULL )
		: CDialog( IDD_CONFIG, parent )
		, s( s_ )
		, p( p_ )
		, m_Title( title )
	{
		return;
	}

protected:

	virtual void DoDataExchange( CDataExchange * pDX )
	{
		CDialog::DoDataExchange( pDX );

		DDX_Control( pDX, IDC_CHECK_OVERSAMPLING, m_CheckBoxOversampling );
		DDX_Control( pDX, IDC_CHECK_NOISEREDUCTION, m_CheckBoxNoiseReduction );
		DDX_Control( pDX, IDC_CHECK_REVERB, m_CheckBoxReverb );
		DDX_Control( pDX, IDC_CHECK_MEGABASS, m_CheckBoxMegabass );
		DDX_Control( pDX, IDC_CHECK_SURROUND, m_CheckBoxSurroundSound );
		DDX_Control( pDX, IDC_CHECK_USEAGC, m_CheckBoxUseAGC );

		DDX_Control( pDX, IDC_LST_SAMPLINGRATE, m_ListBoxSampleRate );
		DDX_Control( pDX, IDC_RADIO_CHANNELSMONO, m_RadioBtnChannelsMono );
		DDX_Control( pDX, IDC_RADIO_CHANNELSSTEREO, m_RadioBtnChannelsStereo );
		DDX_Control( pDX, IDC_RADIO_CHANNELSQUAD, m_RadioBtnChannelsQuad );
		DDX_Control( pDX, IDC_RADIO_BITDEPTH8, m_RadioBtnBitDepth8 );
		DDX_Control( pDX, IDC_RADIO_BITDEPTH16, m_RadioBtnBitDepth16 );
		DDX_Control( pDX, IDC_RADIO_BITDEPTH32, m_RadioBtnBitDepth32 );

		DDX_Control( pDX, IDC_RADIO_RESAMPLINGNONE, m_RadioBtnResamplingNone );
		DDX_Control( pDX, IDC_RADIO_RESAMPLINGLINEAR, m_RadioBtnResamplingLinear );
		DDX_Control( pDX, IDC_RADIO_RESAMPLINGCUBIC, m_RadioBtnResamplingCubic );
		DDX_Control( pDX, IDC_RADIO_RESAMPLING8TAPFIR, m_RadioBtnResampling8TapFIR );
		DDX_Control( pDX, IDC_CHECK_INTVOLUMECTRL, m_CheckBoxIntVolumeCtrl );
		DDX_Control( pDX, IDC_CHECK_SEEKBYORDERS, m_CheckBoxSeekByOrders );

		DDX_Control( pDX, IDC_SLIDER_STEREOSEPARATION, m_SliderCtrlStereoSeparation );
		DDX_Control( pDX, IDC_EDIT_STEREOSEPARATION, m_CEditStereoSeparation );
		DDX_Control( pDX, IDC_SLIDER_MAXMIXCHANNELS, m_SliderCtrlMaxMixChannels );
		DDX_Control( pDX, IDC_EDIT_MAXMIXCHANNELS, m_CEditMaxMixChannels );
		DDX_Control( pDX, IDC_SLIDER_REVERBDEPTH, m_SliderCtrlReverbDepth );
		DDX_Control( pDX, IDC_EDIT_REVERBDEPTH, m_CEditReverbDepth );
		DDX_Control( pDX, IDC_SLIDER_REVERBDELAY, m_SliderCtrlReverbDelay );
		DDX_Control( pDX, IDC_EDIT_REVERBDELAY, m_CEditReverbDelay );

		DDX_Control( pDX, IDC_SLIDER_BASSAMOUNT, m_SliderCtrlBassAmount );
		DDX_Control( pDX, IDC_EDIT_BASSAMOUNT, m_CEditBassAmount );
		DDX_Control( pDX, IDC_SLIDER_BASSRANGE, m_SliderCtrlBassRange );
		DDX_Control( pDX, IDC_EDIT_BASSRANGE, m_CEditBassRange );
		DDX_Control( pDX, IDC_SLIDER_SURROUNDDEPTH, m_SliderCtrlSurroundDepth );
		DDX_Control( pDX, IDC_EDIT_SURROUNDDEPTH, m_CEditSurroundDepth );
		DDX_Control( pDX, IDC_SLIDER_SURROUNDDELAY, m_SliderCtrlSurroundDelay );
		DDX_Control( pDX, IDC_EDIT_SURROUNDDELAY, m_CEditSurroundDelay );

		DDX_Control( pDX, IDC_SLIDER_LOOPCOUNT, m_SliderCtrlLoopCount );
		DDX_Control( pDX, IDC_EDIT_LOOPCOUNT, m_CEditLoopCount );
		DDX_Control( pDX, IDC_SLIDER_PREAMP, m_SliderCtrlPreAmp );
		DDX_Control( pDX, IDC_EDIT_PREAMP, m_CEditPreAmp );
		DDX_Control( pDX, IDC_SLIDER_INFOREFRESH, m_SliderCtrlInfoRefresh );
		DDX_Control( pDX, IDC_EDIT_INFOREFRESH, m_CEditInfoRefresh );
		DDX_Control( pDX, IDC_SLIDER_INFOBUFFERING, m_SliderCtrlInfoBuffering );
		DDX_Control( pDX, IDC_EDIT_INFOBUFFERING, m_CEditInfoBuffering );
	}

	afx_msg BOOL OnInitDialog() {

		if ( !CDialog::OnInitDialog() ) {
			return false;
		}

		ttcontrol = NULL;
		//SetWindowText( m_Title ); //it is already set in the dialog resource
		//by assigning struct to struct integer values are copied, but content of pointers are not!
		tmpfileformats = (char*)malloc((*p).pFormatsSize);
		tmpinifn = (char*)malloc((*p).pIniFNSize);
		memcpy(tmpfileformats, (*p).pFormats, (*p).pFormatsSize);
		memcpy(tmpinifn, (*p).pIniFN, (*p).pIniFNSize);
		curcfg = *s; //we preserving original values until OK clicked
		curpcfg = *p; //we preserving original values until OK clicked
		curpcfg.pFormats = tmpfileformats;
		curpcfg.pIniFN = tmpinifn;
		DoInitControls();

		//Create the ToolTip control
		//CDialog::OnInitDialog();
		if( !m_ToolTip.Create(this)) {
			TRACE0("Unable to create the ToolTip!");
		} else {
			// Add tool tips to the controls, either by hard coded string 
			// or using the string table resource.
			// NOTE: When we do not passing the ControlID (or we passing NULL or 0),
			//       the OnToolTipNotify will return a handle to the control instead
			//       of the ControlID! If CRect (parameter 3) specified, a valid  ControlID
			//       (parameter 4) is required, otherwise the tooltip will not work.
			ttcontrol = &m_ToolTip; //we need to make ToolTipControl accessible outside of this function
			m_ToolTip.SetMaxTipWidth(240); // enables automatic line breaks (requires VC6 and later)
			//The default initial time is 500ms, the default reshow time is 100ms.
			m_ToolTip.SetDelayTime(TTDT_INITIAL, 1200); // time that pointer must remain stationary before showing the tooltip
			//Modplug flags
			m_ToolTip.AddTool( GetDlgItem(IDC_CHECK_OVERSAMPLING), LPSTR_TEXTCALLBACK);
			m_ToolTip.AddTool( GetDlgItem(IDC_CHECK_NOISEREDUCTION), LPSTR_TEXTCALLBACK);
			m_ToolTip.AddTool( GetDlgItem(IDC_CHECK_REVERB), LPSTR_TEXTCALLBACK);
			m_ToolTip.AddTool( GetDlgItem(IDC_CHECK_MEGABASS), LPSTR_TEXTCALLBACK);
			m_ToolTip.AddTool( GetDlgItem(IDC_CHECK_SURROUND), LPSTR_TEXTCALLBACK);
			m_ToolTip.AddTool( GetDlgItem(IDC_CHECK_USEAGC), LPSTR_TEXTCALLBACK);
			//Plugin flags
			m_ToolTip.AddTool( GetDlgItem(IDC_CHECK_INTVOLUMECTRL), LPSTR_TEXTCALLBACK);
			m_ToolTip.AddTool( GetDlgItem(IDC_CHECK_SEEKBYORDERS), LPSTR_TEXTCALLBACK);
			//Samplerate
			m_ToolTip.AddTool( GetDlgItem(IDC_LST_SAMPLINGRATE), LPSTR_TEXTCALLBACK);
			//Channels
			m_ToolTip.AddTool( GetDlgItem(IDC_RADIO_CHANNELSMONO), LPSTR_TEXTCALLBACK);
			m_ToolTip.AddTool( GetDlgItem(IDC_RADIO_CHANNELSSTEREO), LPSTR_TEXTCALLBACK);
			m_ToolTip.AddTool( GetDlgItem(IDC_RADIO_CHANNELSQUAD), LPSTR_TEXTCALLBACK);
			//Bit depth
			CRect rcStatic;
			GetDlgItem(IDC_STATIC_BITDEPTH)->GetWindowRect(rcStatic);
			ScreenToClient(rcStatic);
			m_ToolTip.AddTool( this, LPSTR_TEXTCALLBACK, rcStatic, IDC_STATIC_BITDEPTH);
			m_ToolTip.AddTool( GetDlgItem(IDC_RADIO_BITDEPTH8), LPSTR_TEXTCALLBACK);
			m_ToolTip.AddTool( GetDlgItem(IDC_RADIO_BITDEPTH16), LPSTR_TEXTCALLBACK);
			m_ToolTip.AddTool( GetDlgItem(IDC_RADIO_BITDEPTH32), LPSTR_TEXTCALLBACK);
			//Resampling (interpolation)
			m_ToolTip.AddTool( GetDlgItem(IDC_RADIO_RESAMPLINGNONE), LPSTR_TEXTCALLBACK);
			m_ToolTip.AddTool( GetDlgItem(IDC_RADIO_RESAMPLINGLINEAR), LPSTR_TEXTCALLBACK);
			m_ToolTip.AddTool( GetDlgItem(IDC_RADIO_RESAMPLINGCUBIC), LPSTR_TEXTCALLBACK);
			m_ToolTip.AddTool( GetDlgItem(IDC_RADIO_RESAMPLING8TAPFIR), LPSTR_TEXTCALLBACK);
			//Trackbars(sliders) - group 1
			m_ToolTip.AddTool( GetDlgItem(IDC_SLIDER_STEREOSEPARATION), LPSTR_TEXTCALLBACK);
			m_ToolTip.AddTool( GetDlgItem(IDC_SLIDER_MAXMIXCHANNELS), LPSTR_TEXTCALLBACK);
			m_ToolTip.AddTool( GetDlgItem(IDC_SLIDER_REVERBDEPTH), LPSTR_TEXTCALLBACK);
			m_ToolTip.AddTool( GetDlgItem(IDC_SLIDER_REVERBDELAY), LPSTR_TEXTCALLBACK);
			//Trackbars(sliders) - group 2
			m_ToolTip.AddTool( GetDlgItem(IDC_SLIDER_BASSAMOUNT), LPSTR_TEXTCALLBACK);
			m_ToolTip.AddTool( GetDlgItem(IDC_SLIDER_BASSRANGE), LPSTR_TEXTCALLBACK);
			m_ToolTip.AddTool( GetDlgItem(IDC_SLIDER_SURROUNDDEPTH), LPSTR_TEXTCALLBACK);
			m_ToolTip.AddTool( GetDlgItem(IDC_SLIDER_SURROUNDDELAY), LPSTR_TEXTCALLBACK);
			//Trackbars(sliders) - group 3
			m_ToolTip.AddTool( GetDlgItem(IDC_SLIDER_LOOPCOUNT), LPSTR_TEXTCALLBACK);
			m_ToolTip.AddTool( GetDlgItem(IDC_SLIDER_PREAMP), LPSTR_TEXTCALLBACK);
			m_ToolTip.AddTool( GetDlgItem(IDC_SLIDER_INFOREFRESH), LPSTR_TEXTCALLBACK);
			m_ToolTip.AddTool( GetDlgItem(IDC_SLIDER_INFOBUFFERING), LPSTR_TEXTCALLBACK);
			//Buttons
			m_ToolTip.AddTool( GetDlgItem(IDC_BUTTON_FORMATS), LPSTR_TEXTCALLBACK);
			m_ToolTip.AddTool( GetDlgItem(IDC_BUTTON_CFGRESET), LPSTR_TEXTCALLBACK);
			m_ToolTip.AddTool( GetDlgItem(IDOK), LPSTR_TEXTCALLBACK);
			m_ToolTip.AddTool( GetDlgItem(IDCANCEL), LPSTR_TEXTCALLBACK);
			m_ToolTip.Activate(TRUE);
		}
		return TRUE;
	}

	BOOL OnTtnNeedText(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
	{
		//NOTE: TOOLTIPTEXT type in some cases (compiler bug???) have only 3 members:
		//    HWND hwndFrom; UINT idFrom; UINT code; (missing: HINSTANCE hinst; UINT uFlags;)
		//    In this case we can not distinguish that idFrom member contains a handle or ControlID.
		//    Solutions:
		//    1.) Use NMTTDISPINFO instead of TOOLTIPTEXT, and check the TTF_IDISHWND bit
		//        in the uFlags member: if it is set, we have a HWND, else ControlID.
		//    2.) Guarantee to contain ControlID by passing a ControlID in the AddTool function 
		//        or force HWND by discarding the ControlID.
		//    3.) Test the returned value (is reliable?!):
		//        CWnd* testID = GetDlgItem(pNMHDR->idFrom);
		//        if (testID == NULL) nID = ::GetDlgCtrlID((HWND)pNMHDR->idFrom) else nID = pNMHDR->idFrom;

		NMTTDISPINFO* pTTT = (NMTTDISPINFO*)pNMHDR;
		UINT nID;
		if (pTTT->uFlags & TTF_IDISHWND) {
			nID = ::GetDlgCtrlID((HWND)pNMHDR->idFrom);
		} else {
			nID = pNMHDR->idFrom;
		}

		char buffer [256]; //255 character + null-terminator - this should be enough.
		DWORD buffsize = sizeof(buffer);
		DWORD ntbuffsize = buffsize - 1; //note: we need null-terminated string, which is not assured by the LoadString.

		if (nID == IDC_CHECK_OVERSAMPLING)	{
			if (SetTootipTextFromRes(pTTT, IDS_HMPFLG_OVERSAMPLING, ttcontrol, &buffer[0], buffsize)) return TRUE;
		}
		if (nID == IDC_CHECK_NOISEREDUCTION) {
			if (SetTootipTextFromRes(pTTT, IDS_HMPFLG_NOISEREDUCTION, ttcontrol, &buffer[0], buffsize)) return TRUE;
		}
		if (nID == IDC_CHECK_REVERB) {
			if (SetTootipTextFromRes(pTTT, IDS_HMPFLG_REVERB, ttcontrol, &buffer[0], buffsize)) return TRUE;
		}
		if (nID == IDC_CHECK_MEGABASS) {
			if (SetTootipTextFromRes(pTTT, IDS_HMPFLG_MEGABASS, ttcontrol, &buffer[0], buffsize)) return TRUE;
		}
		if (nID == IDC_CHECK_SURROUND) {
			if (SetTootipTextFromRes(pTTT, IDS_HMPFLG_SURROUND, ttcontrol, &buffer[0], buffsize)) return TRUE;
		}
		if (nID == IDC_CHECK_USEAGC) {
			if (SetTootipTextFromRes(pTTT, IDS_HMPFLG_USEAGC, ttcontrol, &buffer[0], buffsize)) return TRUE;
		}
		//Plugin flags
		if (nID == IDC_CHECK_INTVOLUMECTRL) {
			if (SetTootipTextFromRes(pTTT, IDS_HPLFLG_INTVOLUMECTRL, ttcontrol, &buffer[0], buffsize)) return TRUE;
		}
		if (nID == IDC_CHECK_SEEKBYORDERS) {
			if (SetTootipTextFromRes(pTTT, IDS_HPLFLG_SEEKBYORDERS, ttcontrol, &buffer[0], buffsize)) return TRUE;
		}
		//Sampling rate
		if (nID == IDC_LST_SAMPLINGRATE) {
			if (SetTootipTextFromRes(pTTT, IDS_HMPLST_FREQUENCY, ttcontrol, &buffer[0], buffsize)) return TRUE;
		}
		//Channels
		if (nID == IDC_RADIO_CHANNELSMONO) {
			if (SetTootipTextFromRes(pTTT, IDS_HMPRDO_CHANNELSMONO, ttcontrol, &buffer[0], buffsize)) return TRUE;
		}
		if (nID == IDC_RADIO_CHANNELSSTEREO) {
			if (SetTootipTextFromRes(pTTT, IDS_HMPRDO_CHANNELSSTEREO, ttcontrol, &buffer[0], buffsize)) return TRUE;
		}
		if (nID == IDC_RADIO_CHANNELSQUAD) {
			if (SetTootipTextFromRes(pTTT, IDS_HMPRDO_CHANNELSQUAD, ttcontrol, &buffer[0], buffsize)) return TRUE;
		}
		//Bit depth
		if (nID == IDC_STATIC_BITDEPTH) {
			if (SetTootipTextFromRes(pTTT, IDS_HMPRDO_BITDEPTH, ttcontrol, &buffer[0], buffsize)) return TRUE;
		}
		if (nID == IDC_RADIO_BITDEPTH8) {
			if (SetTootipTextFromRes(pTTT, IDS_HMPRDO_BITDEPTH, ttcontrol, &buffer[0], buffsize)) return TRUE;
		}
		if (nID == IDC_RADIO_BITDEPTH16) {
			if (SetTootipTextFromRes(pTTT, IDS_HMPRDO_BITDEPTH, ttcontrol, &buffer[0], buffsize)) return TRUE;
		}
		if (nID == IDC_RADIO_BITDEPTH32) {
			if (SetTootipTextFromRes(pTTT, IDS_HMPRDO_BITDEPTH, ttcontrol, &buffer[0], buffsize)) return TRUE;
		}
		//Interpolation (resampling)
		if (nID == IDC_RADIO_RESAMPLINGNONE) {
			if (SetTootipTextFromRes(pTTT, IDS_HMPRDO_RESAMPLINGNONE, ttcontrol, &buffer[0], buffsize)) return TRUE;
		}
		if (nID == IDC_RADIO_RESAMPLINGLINEAR) {
			if (SetTootipTextFromRes(pTTT, IDS_HMPRDO_RESAMPLINGLINEAR, ttcontrol, &buffer[0], buffsize)) return TRUE;
		}
		if (nID == IDC_RADIO_RESAMPLINGCUBIC) {
			if (SetTootipTextFromRes(pTTT, IDS_HMPRDO_RESAMPLINGCUBIC, ttcontrol, &buffer[0], buffsize)) return TRUE;
		}
		if (nID == IDC_RADIO_RESAMPLING8TAPFIR) {
			if (SetTootipTextFromRes(pTTT, IDS_HMPRDO_RESAMPLING8TAPFIR, ttcontrol, &buffer[0], buffsize)) return TRUE;
		}
		//Trackbars(sliders) - group 1
		if (nID == IDC_SLIDER_STEREOSEPARATION) {
			if (SetTootipTextFromRes(pTTT, IDS_HMPRNG_STEREOSEPARATION, ttcontrol, &buffer[0], buffsize)) return TRUE;
		}
		if (nID == IDC_SLIDER_MAXMIXCHANNELS) {
			if (SetTootipTextFromRes(pTTT, IDS_HMPRNG_MAXMIXCHANNELS, ttcontrol, &buffer[0], buffsize)) return TRUE;
		}
		if (nID == IDC_SLIDER_REVERBDEPTH) {
			if (SetTootipTextFromRes(pTTT, IDS_HMPRNG_REVERBDEPTH, ttcontrol, &buffer[0], buffsize)) return TRUE;
		}
		if (nID == IDC_SLIDER_REVERBDELAY) {
			if (SetTootipTextFromRes(pTTT, IDS_HMPRNG_REVERBDELAY, ttcontrol, &buffer[0], buffsize)) return TRUE;
		}
		//Trackbars(sliders) - group 2
		if (nID == IDC_SLIDER_BASSAMOUNT) {
			if (SetTootipTextFromRes(pTTT, IDS_HMPRNG_BASSAMOUNT, ttcontrol, &buffer[0], buffsize)) return TRUE;
		}
		if (nID == IDC_SLIDER_BASSRANGE) {
			if (SetTootipTextFromRes(pTTT, IDS_HMPRNG_BASSRANGE, ttcontrol, &buffer[0], buffsize)) return TRUE;
		}
		if (nID == IDC_SLIDER_SURROUNDDEPTH) {
			if (SetTootipTextFromRes(pTTT, IDS_HMPRNG_SURROUNDDEPTH, ttcontrol, &buffer[0], buffsize)) return TRUE;
		}
		if (nID == IDC_SLIDER_SURROUNDDELAY) {
			if (SetTootipTextFromRes(pTTT, IDS_HMPRNG_SURROUNDDELAY, ttcontrol, &buffer[0], buffsize)) return TRUE;
		}
		//Trackbars(sliders) - group 3
		if (nID == IDC_SLIDER_LOOPCOUNT) {
			if (SetTootipTextFromRes(pTTT, IDS_HMPRNG_LOOPCOUNT, ttcontrol, &buffer[0], buffsize)) return TRUE;
		}
		if (nID == IDC_SLIDER_PREAMP) {
			if (SetTootipTextFromRes(pTTT, IDS_HMPRNG_PREAMP, ttcontrol, &buffer[0], buffsize)) return TRUE;
		}
		if (nID == IDC_SLIDER_INFOREFRESH) {
			if (SetTootipTextFromRes(pTTT, IDS_HPLRNG_INFOREFRESH, ttcontrol, &buffer[0], buffsize)) return TRUE;
		}
		if (nID == IDC_SLIDER_INFOBUFFERING) {
			if (SetTootipTextFromRes(pTTT, IDS_HPLRNG_INFOBUFFERING, ttcontrol, &buffer[0], buffsize)) return TRUE;
		}
		//buttons
		if (nID == IDC_BUTTON_FORMATS) {
			if (SetTootipTextFromRes(pTTT, IDS_HPLBTN_FORMATS, ttcontrol, &buffer[0], buffsize)) return TRUE;
		}
		if (nID == IDC_BUTTON_CFGRESET) {
			if (SetTootipTextFromRes(pTTT, IDS_HPLBTN_CFGRESET, ttcontrol, &buffer[0], buffsize)) return TRUE;
		}
		if (nID == IDOK) {
			if (SetTootipTextFromRes(pTTT, IDS_HPLBTN_CFGIDOK, ttcontrol, &buffer[0], buffsize)) return TRUE;
		}
		if (nID == IDCANCEL) {
			if (SetTootipTextFromRes(pTTT, IDS_HPLBTN_CFGIDCANCEL, ttcontrol, &buffer[0], buffsize)) return TRUE;
		}
		return FALSE;
	}

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		m_ToolTip.RelayEvent(pMsg);
		if (pMsg->message == WM_KEYDOWN)
	    {
		    if (pMsg->wParam == VK_RETURN)
	        {
				AfxGetMainWnd()->SetFocus(); //changes focus, so result of the OnKillFocus will be not reverted
		    }
	    }
		return CDialog::PreTranslateMessage(pMsg);
	}

	BOOL DoInitControls() {
		//Modplug flags
		if ((curcfg.mFlags & MODPLUG_ENABLE_OVERSAMPLING) != 0) {m_CheckBoxOversampling.SetCheck(BST_CHECKED);} else {m_CheckBoxOversampling.SetCheck(BST_UNCHECKED);}
		if ((curcfg.mFlags & MODPLUG_ENABLE_NOISE_REDUCTION) != 0) {m_CheckBoxNoiseReduction.SetCheck(BST_CHECKED);} else {m_CheckBoxNoiseReduction.SetCheck(BST_UNCHECKED);}
		if ((curcfg.mFlags & MODPLUG_ENABLE_REVERB) != 0) {m_CheckBoxReverb.SetCheck(BST_CHECKED);} else {m_CheckBoxReverb.SetCheck(BST_UNCHECKED);}
		if ((curcfg.mFlags & MODPLUG_ENABLE_MEGABASS) != 0) {m_CheckBoxMegabass.SetCheck(BST_CHECKED);} else {m_CheckBoxMegabass.SetCheck(BST_UNCHECKED);}
		if ((curcfg.mFlags & MODPLUG_ENABLE_SURROUND) != 0) {m_CheckBoxSurroundSound.SetCheck(BST_CHECKED);} else {m_CheckBoxSurroundSound.SetCheck(BST_UNCHECKED);}
		if (curpcfg.pUseAGC == 0) {m_CheckBoxUseAGC.SetCheck(BST_UNCHECKED);} else {m_CheckBoxUseAGC.SetCheck(BST_CHECKED);}
		//Plugin flags
		if ((curpcfg.pFlags & 1) != 0) {m_CheckBoxIntVolumeCtrl.SetCheck(BST_CHECKED);} else {m_CheckBoxIntVolumeCtrl.SetCheck(BST_UNCHECKED);}
		if ((curpcfg.pFlags & 4) != 0) {m_CheckBoxSeekByOrders.SetCheck(BST_CHECKED);} else {m_CheckBoxSeekByOrders.SetCheck(BST_UNCHECKED);}

		BOOL selected = false;

		selected = false;
		//m_ListBoxSampleRate.sorted = false;
		m_ListBoxSampleRate.SetItemData( m_ListBoxSampleRate.AddString( " 8000" ), 8000 );
		m_ListBoxSampleRate.SetItemData( m_ListBoxSampleRate.AddString( "11025" ), 11025 );
		m_ListBoxSampleRate.SetItemData( m_ListBoxSampleRate.AddString( "12000" ), 12000 );
		m_ListBoxSampleRate.SetItemData( m_ListBoxSampleRate.AddString( "16000" ), 16000 );
		m_ListBoxSampleRate.SetItemData( m_ListBoxSampleRate.AddString( "22050" ), 22050 );
		m_ListBoxSampleRate.SetItemData( m_ListBoxSampleRate.AddString( "24000" ), 24000 );
		m_ListBoxSampleRate.SetItemData( m_ListBoxSampleRate.AddString( "32000" ), 32000 );
		m_ListBoxSampleRate.SetItemData( m_ListBoxSampleRate.AddString( "44100" ), 44100 );
		m_ListBoxSampleRate.SetItemData( m_ListBoxSampleRate.AddString( "48000" ), 48000 );
		m_ListBoxSampleRate.SetItemData( m_ListBoxSampleRate.AddString( "88200" ), 88200 );
		m_ListBoxSampleRate.SetItemData( m_ListBoxSampleRate.AddString( "96000" ), 96000 );
		for ( int index = 0; index < m_ListBoxSampleRate.GetCount(); ++index ) {
			if ( m_ListBoxSampleRate.GetItemData( index ) == curcfg.mFrequency ) {
				m_ListBoxSampleRate.SetCurSel( index );
				selected = true;
			}
		}
		if ( !selected ) {
			m_ListBoxSampleRate.SelectString( 0, "48000" );
		}

		CButton* pButton;
		//Channels
		if (curcfg.mChannels == 1) {
			pButton = (CButton*)GetDlgItem(IDC_RADIO_CHANNELSMONO);
			pButton->SetCheck(BST_CHECKED);
		} else if (curcfg.mChannels == 4) {
			pButton = (CButton*)GetDlgItem(IDC_RADIO_CHANNELSQUAD);
			pButton->SetCheck(BST_CHECKED);
		} else {
			pButton = (CButton*)GetDlgItem(IDC_RADIO_CHANNELSSTEREO);
			pButton->SetCheck(BST_CHECKED);
		}
		if (curcfg.mChannels != 1) {
			pButton = (CButton*)GetDlgItem(IDC_RADIO_CHANNELSMONO);
			pButton->SetCheck(BST_UNCHECKED);
		}
		if (curcfg.mChannels != 2) {
			pButton = (CButton*)GetDlgItem(IDC_RADIO_CHANNELSSTEREO);
			pButton->SetCheck(BST_UNCHECKED);
		}
		if (curcfg.mChannels != 4) {
			pButton = (CButton*)GetDlgItem(IDC_RADIO_CHANNELSQUAD);
			pButton->SetCheck(BST_UNCHECKED);
		}
		//Bit depth
		if (curcfg.mBits == 32) {
			pButton = (CButton*)GetDlgItem(IDC_RADIO_BITDEPTH32);
			pButton->SetCheck(BST_CHECKED);
		} else if (curcfg.mBits == 8) {
			pButton = (CButton*)GetDlgItem(IDC_RADIO_BITDEPTH8);
			pButton->SetCheck(BST_CHECKED);
		} else {
			pButton = (CButton*)GetDlgItem(IDC_RADIO_BITDEPTH16);
			pButton->SetCheck(BST_CHECKED);
		}
		if (curcfg.mBits != 32) {
			pButton = (CButton*)GetDlgItem(IDC_RADIO_BITDEPTH32);
			pButton->SetCheck(BST_UNCHECKED);
		}
		if (curcfg.mBits != 16) {
			pButton = (CButton*)GetDlgItem(IDC_RADIO_BITDEPTH16);
			pButton->SetCheck(BST_UNCHECKED);
		}
		if (curcfg.mBits != 8) {
			pButton = (CButton*)GetDlgItem(IDC_RADIO_BITDEPTH8);
			pButton->SetCheck(BST_UNCHECKED);
		}
		//Interpolation
		if (curcfg.mResamplingMode == 3) {
			m_RadioBtnResampling8TapFIR.SetCheck(BST_CHECKED);
		} else if (curcfg.mResamplingMode == 1) {
			m_RadioBtnResamplingLinear.SetCheck(BST_CHECKED);
		} else if (curcfg.mResamplingMode == 0) {
			m_RadioBtnResamplingNone.SetCheck(BST_CHECKED);
		} else {
			m_RadioBtnResamplingCubic.SetCheck(BST_CHECKED);
		}
		//Verify and correct the initial values for EditBoxes to prevent crashes
		//Group 1
		if (curcfg.mStereoSeparation < 1) {
			curcfg.mStereoSeparation = 1;
		} else if (curcfg.mStereoSeparation > 256) {
			curcfg.mStereoSeparation = 256;
		}
		if (curcfg.mMaxMixChannels < 32) {
			curcfg.mMaxMixChannels = 32;
		} else if (curcfg.mMaxMixChannels > 256) {
			curcfg.mMaxMixChannels = 256;
		}
		if (curcfg.mReverbDepth < 0) {
			curcfg.mReverbDepth = 0;
		} else if (curcfg.mReverbDepth > 100) {
			curcfg.mReverbDepth = 100;
		}
		if (curcfg.mReverbDelay < 40) {
			curcfg.mReverbDelay = 40;
		} else if (curcfg.mReverbDelay > 250) {
			curcfg.mReverbDelay = 250;
		}
		//Group 2
		if (curcfg.mBassAmount < 0) {
			curcfg.mBassAmount = 0;
		} else if (curcfg.mBassAmount > 100) {
			curcfg.mBassAmount = 100;
		}
		if (curcfg.mBassRange < 10) {
			curcfg.mBassRange = 10;
		} else if (curcfg.mBassRange > 100) {
			curcfg.mBassRange = 100;
		}
		if (curcfg.mSurroundDepth < 0) {
			curcfg.mSurroundDepth = 0;
		} else if (curcfg.mSurroundDepth > 100) {
			curcfg.mSurroundDepth = 100;
		}
		if (curcfg.mSurroundDelay < 4) {
			curcfg.mSurroundDelay = 4;
		} else if (curcfg.mSurroundDelay > 50) {
			curcfg.mSurroundDelay = 50;
		}
		//Group 3
		if (curcfg.mLoopCount < -1) {
			curcfg.mLoopCount = -1;
		} else if (curcfg.mLoopCount > 32767) {
			curcfg.mLoopCount = 32767;
		}
		if (curpcfg.pInfoRefresh < 0) {
			curpcfg.pInfoRefresh = 0;
		} else if (curpcfg.pInfoRefresh > 2000) {
			curpcfg.pInfoRefresh = 2000;
		}
		if (curpcfg.pInfoBuffering < 100) {
			if (curpcfg.pInfoBuffering < 1) {
				curpcfg.pInfoBuffering = 0;
			} else {
				curpcfg.pInfoBuffering = 100;
			}
		} else if (curpcfg.pInfoBuffering > 2000) {
			curpcfg.pInfoBuffering = 2000;
		}

		char buffer [33];
		//Sliders and edit boxes - group1
		m_SliderCtrlStereoSeparation.SetRange( 1, 256 );
		m_SliderCtrlStereoSeparation.SetTicFreq( 16 );
		m_SliderCtrlStereoSeparation.SetPageSize( 16 );
		m_SliderCtrlStereoSeparation.SetLineSize( 4 );
		m_SliderCtrlStereoSeparation.SetPos( curcfg.mStereoSeparation );
		m_CEditStereoSeparation.SetLimitText(3);
		_itoa( curcfg.mStereoSeparation, buffer, 10 );
		m_CEditStereoSeparation.SetWindowText( buffer );

		m_SliderCtrlMaxMixChannels.SetRange( 32, 256 );
		m_SliderCtrlMaxMixChannels.SetTicFreq( 16 );
		m_SliderCtrlMaxMixChannels.SetPageSize( 16 );
		m_SliderCtrlMaxMixChannels.SetLineSize( 4 );
		m_SliderCtrlMaxMixChannels.SetPos( curcfg.mMaxMixChannels );
		m_CEditMaxMixChannels.SetLimitText(3);
		_itoa( curcfg.mMaxMixChannels, buffer, 10 );
		m_CEditMaxMixChannels.SetWindowText( buffer );

		m_SliderCtrlReverbDepth.SetRange( 0, 100 );
		m_SliderCtrlReverbDepth.SetTicFreq( 10 );
		m_SliderCtrlReverbDepth.SetPageSize( 20 );
		m_SliderCtrlReverbDepth.SetLineSize( 5 );
		m_SliderCtrlReverbDepth.SetPos( curcfg.mReverbDepth );
		m_CEditReverbDepth.SetLimitText(3);
		_itoa( curcfg.mReverbDepth, buffer, 10 );
		m_CEditReverbDepth.SetWindowText( buffer );

		m_SliderCtrlReverbDelay.SetRange( 40, 250 );
		m_SliderCtrlReverbDelay.SetTicFreq( 20 );
		m_SliderCtrlReverbDelay.SetPageSize( 40 );
		m_SliderCtrlReverbDelay.SetLineSize( 10 );
		m_SliderCtrlReverbDelay.SetPos( curcfg.mReverbDelay );
		m_CEditReverbDelay.SetLimitText(3);
		_itoa( curcfg.mReverbDelay, buffer, 10 );
		m_CEditReverbDelay.SetWindowText( buffer );

		//Sliders and edit boxes - group2
		m_SliderCtrlBassAmount.SetRange( 0, 100 );
		m_SliderCtrlBassAmount.SetTicFreq( 10 );
		m_SliderCtrlBassAmount.SetPageSize( 20 );
		m_SliderCtrlBassAmount.SetLineSize( 5 );
		m_SliderCtrlBassAmount.SetPos( curcfg.mBassAmount );
		m_CEditBassAmount.SetLimitText(3);
		_itoa( curcfg.mBassAmount, buffer, 10 );
		m_CEditBassAmount.SetWindowText( buffer );

		m_SliderCtrlBassRange.SetRange( 10, 100 );
		m_SliderCtrlBassRange.SetTicFreq( 10 );
		m_SliderCtrlBassRange.SetPageSize( 20 );
		m_SliderCtrlBassRange.SetLineSize( 5 );
		m_SliderCtrlBassRange.SetPos( curcfg.mBassRange );
		m_CEditBassRange.SetLimitText(3);
		_itoa( curcfg.mBassRange, buffer, 10 );
		m_CEditBassRange.SetWindowText( buffer );

		m_SliderCtrlSurroundDepth.SetRange( 0, 100 );
		m_SliderCtrlSurroundDepth.SetTicFreq( 10 );
		m_SliderCtrlSurroundDepth.SetPageSize( 20 );
		m_SliderCtrlSurroundDepth.SetLineSize( 5 );
		m_SliderCtrlSurroundDepth.SetPos( curcfg.mSurroundDepth );
		m_CEditSurroundDepth.SetLimitText(3);
		_itoa( curcfg.mSurroundDepth, buffer, 10 );
		m_CEditSurroundDepth.SetWindowText( buffer );

		m_SliderCtrlSurroundDelay.SetRange( 0, 50 );
		m_SliderCtrlSurroundDelay.SetTicFreq( 5 );
		m_SliderCtrlSurroundDelay.SetPageSize( 10 );
		m_SliderCtrlSurroundDelay.SetLineSize( 5 );
		m_SliderCtrlSurroundDelay.SetPos( curcfg.mSurroundDelay );
		m_CEditSurroundDelay.SetLimitText(2);
		_itoa( curcfg.mSurroundDelay, buffer, 10 );
		m_CEditSurroundDelay.SetWindowText( buffer );

		//Sliders and edit boxes - group3
		m_SliderCtrlLoopCount.SetRange( -1, 20 );
		m_SliderCtrlLoopCount.SetTicFreq( 1 );
		m_SliderCtrlLoopCount.SetPageSize( 5 );
		m_SliderCtrlLoopCount.SetLineSize( 1 );
		m_SliderCtrlLoopCount.SetPos( curcfg.mLoopCount );
		m_CEditLoopCount.SetLimitText(5);
		_itoa( curcfg.mLoopCount, buffer, 10 );
		m_CEditLoopCount.SetWindowText( buffer );

		m_SliderCtrlPreAmp.SetRange( 0, 128 );
		m_SliderCtrlPreAmp.SetTicFreq( 8 );
		m_SliderCtrlPreAmp.SetPageSize( 16 );
		m_SliderCtrlPreAmp.SetLineSize( 4 );
		m_SliderCtrlPreAmp.SetPos( curpcfg.pPreAmp );
		m_CEditPreAmp.SetLimitText(3);
		_itoa( curpcfg.pPreAmp, buffer, 10 );
		m_CEditPreAmp.SetWindowText( buffer );

		m_SliderCtrlInfoRefresh.SetRange( 0, 2000 );
		m_SliderCtrlInfoRefresh.SetTicFreq( 200 );
		m_SliderCtrlInfoRefresh.SetPageSize( 100 );
		m_SliderCtrlInfoRefresh.SetLineSize( 25 );
		m_SliderCtrlInfoRefresh.SetPos( curpcfg.pInfoRefresh );
		m_CEditInfoRefresh.SetLimitText(4);
		_itoa( curpcfg.pInfoRefresh, buffer, 10 );
		m_CEditInfoRefresh.SetWindowText( buffer );

		m_SliderCtrlInfoBuffering.SetRange( 0, 2000 );
		m_SliderCtrlInfoBuffering.SetTicFreq( 200 );
		m_SliderCtrlInfoBuffering.SetPageSize( 100 );
		m_SliderCtrlInfoBuffering.SetLineSize( 25 );
		m_SliderCtrlInfoBuffering.SetPos( curpcfg.pInfoBuffering );
		m_CEditInfoBuffering.SetLimitText(4);
		_itoa( curpcfg.pInfoBuffering, buffer, 10 );
		m_CEditInfoBuffering.SetWindowText( buffer );
		//Disable controls
		UINT nCheck;
		nCheck = m_CheckBoxReverb.GetCheck();
		if ((nCheck & BST_CHECKED) == 0) {
			m_SliderCtrlReverbDepth.EnableWindow(FALSE);
			m_CEditReverbDepth.EnableWindow(FALSE);
			m_SliderCtrlReverbDelay.EnableWindow(FALSE);
			m_CEditReverbDelay.EnableWindow(FALSE);
		}
		nCheck = m_CheckBoxMegabass.GetCheck();
		if ((nCheck & BST_CHECKED) == 0) {
			m_SliderCtrlBassAmount.EnableWindow(FALSE);
			m_CEditBassAmount.EnableWindow(FALSE);
			m_SliderCtrlBassRange.EnableWindow(FALSE);
			m_CEditBassRange.EnableWindow(FALSE);
		}
		nCheck = m_CheckBoxSurroundSound.GetCheck();
		if ((nCheck & BST_CHECKED) == 0) {
			m_SliderCtrlSurroundDepth.EnableWindow(FALSE);
			m_CEditSurroundDepth.EnableWindow(FALSE);
			m_SliderCtrlSurroundDelay.EnableWindow(FALSE);
			m_CEditSurroundDelay.EnableWindow(FALSE);
		}
		if (curpcfg.pUseInfotimer == 0) {
			m_SliderCtrlInfoRefresh.EnableWindow(FALSE);
			m_CEditInfoRefresh.EnableWindow(FALSE);
			m_SliderCtrlInfoBuffering.EnableWindow(FALSE);
			m_CEditInfoBuffering.EnableWindow(FALSE);
		}
#ifndef USEEXTRAFEATURES
		m_CheckBoxUseAGC.EnableWindow(FALSE);
		m_SliderCtrlPreAmp.EnableWindow(FALSE);
		m_CEditPreAmp.EnableWindow(FALSE);
#endif
		return TRUE;
	}

	virtual void OnOK() {
		extern CRITICAL_SECTION mp_lock;
		char strnum [33];
		UINT nCheck;
		//Modplug flags
		curcfg.mFlags = 0;
		nCheck = m_CheckBoxOversampling.GetCheck();
		if ((nCheck & BST_CHECKED) != 0) {
			curcfg.mFlags = (curcfg.mFlags | MODPLUG_ENABLE_OVERSAMPLING);
		}
		nCheck = m_CheckBoxNoiseReduction.GetCheck();
		if ((nCheck & BST_CHECKED) != 0) {
			curcfg.mFlags = (curcfg.mFlags | MODPLUG_ENABLE_NOISE_REDUCTION);
		}
		nCheck = m_CheckBoxReverb.GetCheck();
		if ((nCheck & BST_CHECKED) != 0) {
			curcfg.mFlags = (curcfg.mFlags | MODPLUG_ENABLE_REVERB);
		}
		nCheck = m_CheckBoxMegabass.GetCheck();
		if ((nCheck & BST_CHECKED) != 0) {
			curcfg.mFlags = (curcfg.mFlags | MODPLUG_ENABLE_MEGABASS);
		}
		nCheck = m_CheckBoxSurroundSound.GetCheck();
		if ((nCheck & BST_CHECKED) != 0) {
			curcfg.mFlags = (curcfg.mFlags | MODPLUG_ENABLE_SURROUND);
		}
		nCheck = m_CheckBoxUseAGC.GetCheck();
		if ((nCheck & BST_CHECKED) != 0) {
			curpcfg.pUseAGC = 1;
		} else {
			curpcfg.pUseAGC = 0;
		}
		//Plugin flags
		curpcfg.pFlags = 0;
		nCheck = m_CheckBoxIntVolumeCtrl.GetCheck();
		if ((nCheck & BST_CHECKED) != 0) {
			curpcfg.pFlags = (curpcfg.pFlags | 1);
		}
		nCheck = m_CheckBoxSeekByOrders.GetCheck();
		if ((nCheck & BST_CHECKED) != 0) {
			curpcfg.pFlags = (curpcfg.pFlags | 4);
		}
		//Sampling rate
		curcfg.mFrequency = m_ListBoxSampleRate.GetItemData( m_ListBoxSampleRate.GetCurSel() );
		//Radio buttons are setting values immediately
		//Sliders and edit boxes - group1
		curcfg.mStereoSeparation = m_SliderCtrlStereoSeparation.GetPos();
		curcfg.mMaxMixChannels = m_SliderCtrlMaxMixChannels.GetPos();
		curcfg.mReverbDepth = m_SliderCtrlReverbDepth.GetPos();
		curcfg.mReverbDelay = m_SliderCtrlReverbDelay.GetPos();
		//Sliders and edit boxes - group2
		curcfg.mBassAmount = m_SliderCtrlBassAmount.GetPos();
		curcfg.mBassRange = m_SliderCtrlBassRange.GetPos();
		curcfg.mSurroundDepth = m_SliderCtrlSurroundDepth.GetPos();
		curcfg.mSurroundDelay = m_SliderCtrlSurroundDelay.GetPos();
		//Sliders and edit boxes - group3

		//curcfg.mLoopCount = m_SliderCtrlLoopCount.GetPos(); //EditText field can hold more values
		CString strEditValue;
		m_CEditLoopCount.GetWindowText(strEditValue);
		curcfg.mLoopCount = _ttoi(strEditValue);
		curpcfg.pPreAmp = m_SliderCtrlPreAmp.GetPos();
		curpcfg.pInfoRefresh = m_SliderCtrlInfoRefresh.GetPos();
		curpcfg.pInfoBuffering = m_SliderCtrlInfoBuffering.GetPos();

//NOTE: Invalid values automatically replaced by a default value will be not saved until a value changed by the user!
		int changes = 0;
		if ((*s).mFlags != curcfg.mFlags) changes++;
		if ((*s).mChannels != curcfg.mChannels) changes++;
		if ((*s).mBits != curcfg.mBits) changes++;
		if ((*s).mFrequency != curcfg.mFrequency) changes++;
		if ((*s).mResamplingMode != curcfg.mResamplingMode) changes++;
		if ((*s).mStereoSeparation != curcfg.mStereoSeparation) changes++;
		if ((*s).mMaxMixChannels != curcfg.mMaxMixChannels) changes++;
		if ((*s).mReverbDepth != curcfg.mReverbDepth) changes++;
		if ((*s).mReverbDelay != curcfg.mReverbDelay) changes++;
		if ((*s).mBassAmount != curcfg.mBassAmount) changes++;
		if ((*s).mBassRange != curcfg.mBassRange) changes++;
		if ((*s).mSurroundDepth != curcfg.mSurroundDepth) changes++;
		if ((*s).mSurroundDelay != curcfg.mSurroundDelay) changes++;
		if ((*s).mLoopCount != curcfg.mLoopCount) changes++;
		if ((*p).pUseAGC != curpcfg.pUseAGC) changes++;
		if ((*p).pPreAmp != curpcfg.pPreAmp) changes++;
		if ((*p).pInfoRefresh != curpcfg.pInfoRefresh) changes++;
		if ((*p).pInfoBuffering != curpcfg.pInfoBuffering) changes++;
		if ((*p).pFlags != curpcfg.pFlags) changes++;
		//check for changes in fileformats
		unsigned char n, nb, cpos, ind;
		n = strlen(curpcfg.pFormats);
		nb = strlen(p->pFormats);
		BOOL isFileformatsChanged = FALSE;
		if ((n == nb) || ((n == (nb + 1)) && (p->pFormats[nb-1] != ';'))) {
			if (n == (nb + 1)) n--;
			cpos = 0;
			ind = 0;
			//this uppercase conversion may be not perfect, but it should be sufficient for file extensions
			while ((cpos < n) && (ind == 0)) {
				if (p->pFormats[cpos] > 0x60) ind = (toupper(p->pFormats[cpos]) ^ toupper(curpcfg.pFormats[cpos]));
				else ind = ((p->pFormats[cpos]) ^ (curpcfg.pFormats[cpos]));
				cpos++;
			}
			if (ind != 0) isFileformatsChanged = TRUE;
		} else isFileformatsChanged = TRUE;
		if (isFileformatsChanged) changes++;
		if ((curpcfg.isReset > 0) || (changes > 0)) {
			if (changes > 0) curpcfg.isChanged = 1;
			const char *IniSection = "ModPlug";
#define WRITEINITEXT(key, value) WritePrivateProfileString(IniSection, key, value, curpcfg.pIniFN)
			//InitializeCriticalSection(&mp_lock); //already initialized in main.cpp
			EnterCriticalSection(&mp_lock);
			if ((curpcfg.isReset > 0) || ((*s).mFlags != curcfg.mFlags)) {
				_itoa(curcfg.mFlags, strnum, 10);
				WRITEINITEXT("Flags", strnum);
			}
			if ((curpcfg.isReset > 0) || ((*s).mChannels != curcfg.mChannels)) {
				_itoa(curcfg.mChannels, strnum, 10);
				WRITEINITEXT("Channels", strnum);
			}
			if ((curpcfg.isReset > 0) || ((*s).mBits != curcfg.mBits)) {
				_itoa(curcfg.mBits, strnum, 10);
				WRITEINITEXT("Bits", strnum);
			}
			if ((curpcfg.isReset > 0) || ((*s).mFrequency != curcfg.mFrequency)) {
				_itoa(curcfg.mFrequency, strnum, 10);
				WRITEINITEXT("Frequency", strnum);
			}
			if ((curpcfg.isReset > 0) || ((*s).mResamplingMode != curcfg.mResamplingMode)) {
				_itoa(curcfg.mResamplingMode, strnum, 10);
				WRITEINITEXT("ResamplingMode", strnum);
			}
			if ((curpcfg.isReset > 0) || ((*s).mStereoSeparation != curcfg.mStereoSeparation)) {
				_itoa(curcfg.mStereoSeparation, strnum, 10);
				WRITEINITEXT("StereoSeparation", strnum);
			}
			if ((curpcfg.isReset > 0) || ((*s).mMaxMixChannels != curcfg.mMaxMixChannels)) {
				_itoa(curcfg.mMaxMixChannels, strnum, 10);
				WRITEINITEXT("MaxMixChannels", strnum);
			}
			if ((curpcfg.isReset > 0) || ((*s).mReverbDepth != curcfg.mReverbDepth)) {
				_itoa(curcfg.mReverbDepth, strnum, 10);
				WRITEINITEXT("ReverbDepth", strnum);
			}
			if ((curpcfg.isReset > 0) || ((*s).mReverbDelay != curcfg.mReverbDelay)) {
				_itoa(curcfg.mReverbDelay, strnum, 10);
				WRITEINITEXT("ReverbDelay", strnum);
			}
			if ((curpcfg.isReset > 0) || ((*s).mBassAmount != curcfg.mBassAmount)) {
				_itoa(curcfg.mBassAmount, strnum, 10);
				WRITEINITEXT("BassAmount", strnum);
			}
			if ((curpcfg.isReset > 0) || ((*s).mBassRange != curcfg.mBassRange)) {
				_itoa(curcfg.mBassRange, strnum, 10);
				WRITEINITEXT("BassRange", strnum);
			}
			if ((curpcfg.isReset > 0) || ((*s).mSurroundDepth != curcfg.mSurroundDepth)) {
				_itoa(curcfg.mSurroundDepth, strnum, 10);
				WRITEINITEXT("SurroundDepth", strnum);
			}
			if ((curpcfg.isReset > 0) || ((*s).mSurroundDelay != curcfg.mSurroundDelay)) {
				_itoa(curcfg.mSurroundDelay, strnum, 10);
				WRITEINITEXT("SurroundDelay", strnum);
			}
			if ((curpcfg.isReset > 0) || ((*s).mLoopCount != curcfg.mLoopCount)) {
				_itoa(curcfg.mLoopCount, strnum, 10);
				WRITEINITEXT("LoopCount", strnum);
			}
			if ((curpcfg.isReset > 0) || ((*p).pInfoRefresh != curpcfg.pInfoRefresh)) {
				_itoa(curpcfg.pInfoRefresh, strnum, 10);
				WRITEINITEXT("InfoRefresh", strnum);
			}
			if ((curpcfg.isReset > 0) || ((*p).pInfoBuffering != curpcfg.pInfoBuffering)) {
				_itoa(curpcfg.pInfoBuffering, strnum, 10);
				WRITEINITEXT("InfoBuffering", strnum);
			}
			if ((curpcfg.isReset > 0) || ((*p).pFlags != curpcfg.pFlags)) {
				_itoa(curpcfg.pFlags, strnum, 10);
				WRITEINITEXT("PluginFlags", strnum);
			}
			if ((curpcfg.isReset > 0) || ((*p).pUseAGC != curpcfg.pUseAGC)) {
				_itoa(curpcfg.pUseAGC, strnum, 10);
				WRITEINITEXT("UseAGC", strnum);
			}
			if ((curpcfg.isReset > 0) || ((*p).pPreAmp != curpcfg.pPreAmp)) {
				_itoa(curpcfg.pPreAmp, strnum, 10);
				WRITEINITEXT("PreAmp", strnum);
			}
			if ((curpcfg.isReset > 0) || (isFileformatsChanged)) {
				WRITEINITEXT("Formats", curpcfg.pFormats);
			}
			LeaveCriticalSection(&mp_lock);
#undef WRITEINITEXT
		}
		char* tmpFormats = curpcfg.pFormats;
		char* tmpIniFN = curpcfg.pIniFN;
		int tmpFormatsSize = curpcfg.pFormatsSize;
		int tmpIniFNSize = curpcfg.pIniFNSize;
		*s = curcfg;
		*p = curpcfg;
		memcpy(&curpcfg.pFormats[0], tmpFormats, tmpFormatsSize);
		memcpy(&curpcfg.pIniFN[0], tmpFormats, tmpIniFNSize);
		free(tmpfileformats);
		free(tmpinifn);
		CDialog::OnOK();
	}

/*
	void CSettingsDialog::OnOversamplingClicked()
	{
		UINT nCheck = m_CheckBoxOversampling.GetCheck();
		if ((nCheck & BST_CHECKED) != 0) {
			curcfg.mFlags = (curcfg.mFlags | MODPLUG_ENABLE_OVERSAMPLING);
		} else {
			curcfg.mFlags = (curcfg.mFlags & (~ MODPLUG_ENABLE_OVERSAMPLING));
		}
	}
*/
/*
	void CSettingsDialog::OnNoiseReductionClicked()
	{
		UINT nCheck = m_CheckBoxNoiseReduction.GetCheck();
		if ((nCheck & BST_CHECKED) != 0) {
			curcfg.mFlags = (curcfg.mFlags | MODPLUG_ENABLE_NOISE_REDUCTION);
		} else {
			curcfg.mFlags = (curcfg.mFlags & (~ MODPLUG_ENABLE_NOISE_REDUCTION));
		}
	}
*/
	void CSettingsDialog::OnReverbClicked()
	{
		UINT nCheck = m_CheckBoxReverb.GetCheck();
		if ((nCheck & BST_CHECKED) != 0) {
			//curcfg.mFlags = (curcfg.mFlags | MODPLUG_ENABLE_REVERB);
			m_SliderCtrlReverbDepth.EnableWindow(TRUE);
			m_CEditReverbDepth.EnableWindow(TRUE);
			m_SliderCtrlReverbDelay.EnableWindow(TRUE);
			m_CEditReverbDelay.EnableWindow(TRUE);
		} else {
			//curcfg.mFlags = (curcfg.mFlags & (~ MODPLUG_ENABLE_REVERB));
			m_SliderCtrlReverbDepth.EnableWindow(FALSE);
			m_CEditReverbDepth.EnableWindow(FALSE);
			m_SliderCtrlReverbDelay.EnableWindow(FALSE);
			m_CEditReverbDelay.EnableWindow(FALSE);
		}
	}
	void CSettingsDialog::OnMegabassClicked()
	{
		UINT nCheck = m_CheckBoxMegabass.GetCheck();
		if ((nCheck & BST_CHECKED) != 0) {
			//curcfg.mFlags = (curcfg.mFlags | MODPLUG_ENABLE_MEGABASS);
			m_SliderCtrlBassAmount.EnableWindow(TRUE);
			m_CEditBassAmount.EnableWindow(TRUE);
			m_SliderCtrlBassRange.EnableWindow(TRUE);
			m_CEditBassRange.EnableWindow(TRUE);
		} else {
			//curcfg.mFlags = (curcfg.mFlags & (~ MODPLUG_ENABLE_MEGABASS));
			m_SliderCtrlBassAmount.EnableWindow(FALSE);
			m_CEditBassAmount.EnableWindow(FALSE);
			m_SliderCtrlBassRange.EnableWindow(FALSE);
			m_CEditBassRange.EnableWindow(FALSE);
		}
	}
	void CSettingsDialog::OnSurroundClicked()
	{
		UINT nCheck = m_CheckBoxSurroundSound.GetCheck();
		if ((nCheck & BST_CHECKED) != 0) {
			//curcfg.mFlags = (curcfg.mFlags | MODPLUG_ENABLE_SURROUND);
			m_SliderCtrlSurroundDepth.EnableWindow(TRUE);
			m_CEditSurroundDepth.EnableWindow(TRUE);
			m_SliderCtrlSurroundDelay.EnableWindow(TRUE);
			m_CEditSurroundDelay.EnableWindow(TRUE);
		} else {
			//curcfg.mFlags = (curcfg.mFlags & (~ MODPLUG_ENABLE_SURROUND));
			m_SliderCtrlSurroundDepth.EnableWindow(FALSE);
			m_CEditSurroundDepth.EnableWindow(FALSE);
			m_SliderCtrlSurroundDelay.EnableWindow(FALSE);
			m_CEditSurroundDelay.EnableWindow(FALSE);
		}
	}
/*
	void CSettingsDialog::OnUseAGCClicked()
	{
		UINT nCheck = m_CheckBoxUseAGC.GetCheck();
		if ((nCheck & BST_CHECKED) != 0) {
			curpcfg.pUseAG = 1;
		} else {
			curpcfg.pUseAG = 0;
		}
	}
/*
/*
	void CSettingsDialog::OnIntVolumeCtrlClicked()
	{
		UINT nCheck = m_CheckBoxIntVolumeCtrl.GetCheck();
		if ((nCheck & BST_CHECKED) != 0) {
			curpcfg.pFlags = (curpcfg.pFlags | 1);
		} else {
			curpcfg.pFlags = (curpcfg.pFlags & (~ 1));
		}
	}
*/
/*
	void CSettingsDialog::OnSeekByOrdersClicked()
	{
		UINT nCheck = m_CheckBoxSeekByOrders.GetCheck();
		if ((nCheck & BST_CHECKED) != 0) {
			curpcfg.pFlags = (curpcfg.pFlags | 4);
		} else {
			curpcfg.pFlags = (curpcfg.pFlags & (~ 4));
		}
	}
*/
	void CSettingsDialog::OnChannelsMonoClicked()
	{
		curcfg.mChannels = 1;
	}
	void CSettingsDialog::OnChannelsStereoClicked()
	{
		curcfg.mChannels = 2;
	}
	void CSettingsDialog::OnChannelsQuadClicked()
	{
		curcfg.mChannels = 4;
	}
	void CSettingsDialog::OnBitDepth8Clicked()
	{
		curcfg.mBits = 8;
	}
	void CSettingsDialog::OnBitDepth16Clicked()
	{
		curcfg.mBits = 16;
	}
	void CSettingsDialog::OnBitDepth32Clicked()
	{
		curcfg.mBits = 32;
	}
	void CSettingsDialog::OnResamplingNoneClicked()
	{
		curcfg.mResamplingMode = MODPLUG_RESAMPLE_NEAREST;
	}
	void CSettingsDialog::OnResamplingLinearClicked()
	{
		curcfg.mResamplingMode = MODPLUG_RESAMPLE_LINEAR;
	}
	void CSettingsDialog::OnResamplingCubicClicked()
	{
		curcfg.mResamplingMode = MODPLUG_RESAMPLE_SPLINE;
	}
	void CSettingsDialog::OnResampling8TapFIRClicked()
	{
		curcfg.mResamplingMode = MODPLUG_RESAMPLE_FIR;
	}

	//Pressing the Enter button does not trigger a WM_KILLFOCUS event?!
	//Seems that id does. But the value is then reverted to the pre-KillFocus state!
	//We need to catch the Enter key pressed event and trigger the KillFocus event.
	//Edit boxes - group1
/*
	void OnKillFocusStereoSeparation()
	{
	}
*/
/*
	void OnKillFocusMaxMixChannels()
	{
	}
*/
/*
	void OnKillFocusReverbDepth()
	{
	}
*/
/*
	void OnKillFocusReverbDelay()
	{
	}
*/
	//Edit boxes - group2
/*
	void OnKillFocusBassAmount()
	{
	}
*/
/*
	void OnKillFocusBassRange()
	{
	}
*/
/*
	void OnKillFocusSurroundDepth()
	{
	}
*/
	void OnKillFocusSurroundDelay()
	{
		CString strEditValue;
		m_CEditSurroundDelay.GetWindowText(strEditValue);
		int iCEditValue = _ttoi(strEditValue);
		//limits according to CSoundFile::SetSurroundParameters in snd_dsp.cpp
		if (iCEditValue < 4) {
			iCEditValue = 4;
			m_CEditSurroundDelay.SetWindowText(TEXT("4"));
		} else if (iCEditValue > 50) {
			iCEditValue = 50;
			m_CEditSurroundDelay.SetWindowText(TEXT("50"));
		}
		//m_SliderCtrlSurroundDelay.SetPos(iCEditValue);
	}
	//Edit boxes - group3
	void OnKillFocusLoopCount()
	{
		CString strEditValue;
		m_CEditLoopCount.GetWindowText(strEditValue);
		int iCEditValue = _ttoi(strEditValue);
		if ((iCEditValue == 0) && (strEditValue != "0")) m_CEditLoopCount.SetWindowText(TEXT("0"));
	}
	void OnKillFocusPreAmp()
	{
		CString strEditValue;
		m_CEditPreAmp.GetWindowText(strEditValue);
		int iCEditValue = _ttoi(strEditValue);
		//range: 0 or 16..128
		if (iCEditValue < 16) {
			if (iCEditValue < 1) {
				if (iCEditValue != 0) {
					iCEditValue = 0;
					m_CEditPreAmp.SetWindowText(TEXT("0"));
				}
			} else {
				iCEditValue = 16;
				m_CEditPreAmp.SetWindowText(TEXT("16"));
			}
		} else if (iCEditValue > 128) {
			iCEditValue = 128;
			m_CEditPreAmp.SetWindowText(TEXT("128"));
		}
		//m_SliderCtrlPreAmp.SetPos(iCEditValue); // set slider value
	}
/*
	void OnKillFocusInfoRefresh()
	{
	}
*/
	void OnKillFocusInfoBuffering()
	{
		CString strEditValue;
		m_CEditInfoBuffering.GetWindowText(strEditValue);
		int iCEditValue = _ttoi(strEditValue);
		if (iCEditValue < 100) {
			if (iCEditValue < 1) {
				if (iCEditValue != 0) {
					iCEditValue = 0;
					m_CEditInfoBuffering.SetWindowText(TEXT("0"));
				}
			} else {
				iCEditValue = 100;
				m_CEditInfoBuffering.SetWindowText(TEXT("100"));
			}
		} else if (iCEditValue > 2000) {
			iCEditValue = 2000;
			m_CEditInfoBuffering.SetWindowText(TEXT("2000"));
		}
		//m_SliderCtrlInfoBuffering.SetPos(iCEditValue); // set slider value
	}

	void CSettingsDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
	{
		CSliderCtrl* pSlider = reinterpret_cast<CSliderCtrl*>(pScrollBar);
		// you can have multiple sliders: check which one sent the notification
		if (pSlider == &m_SliderCtrlStereoSeparation)
		{
			CString strSliderValue;
			int iValue = m_SliderCtrlStereoSeparation.GetPos();
			strSliderValue.Format(TEXT("%d"), iValue);
			m_CEditStereoSeparation.SetWindowText(strSliderValue);
		}
		else if (pSlider == &m_SliderCtrlMaxMixChannels)
		{
			CString strSliderValue;
			int iValue = m_SliderCtrlMaxMixChannels.GetPos();
			strSliderValue.Format(TEXT("%d"), iValue);
			m_CEditMaxMixChannels.SetWindowText(strSliderValue);
		}
		else if (pSlider == &m_SliderCtrlReverbDepth)
		{
			CString strSliderValue;
			int iValue = m_SliderCtrlReverbDepth.GetPos();
			strSliderValue.Format(TEXT("%d"), iValue);
			m_CEditReverbDepth.SetWindowText(strSliderValue);
		}
		else if (pSlider == &m_SliderCtrlReverbDelay)
		{
			CString strSliderValue;
			int iValue = m_SliderCtrlReverbDelay.GetPos();
			strSliderValue.Format(TEXT("%d"), iValue);
			m_CEditReverbDelay.SetWindowText(strSliderValue);
		}
		else if (pSlider == &m_SliderCtrlBassAmount)
		{
			CString strSliderValue;
			int iValue = m_SliderCtrlBassAmount.GetPos();
			strSliderValue.Format(TEXT("%d"), iValue);
			m_CEditBassAmount.SetWindowText(strSliderValue);
		}
		else if (pSlider == &m_SliderCtrlBassRange)
		{
			CString strSliderValue;
			int iValue = m_SliderCtrlBassRange.GetPos();
			strSliderValue.Format(TEXT("%d"), iValue);
			m_CEditBassRange.SetWindowText(strSliderValue);
		}
		else if (pSlider == &m_SliderCtrlSurroundDepth)
		{
			CString strSliderValue;
			int iValue = m_SliderCtrlSurroundDepth.GetPos();
			strSliderValue.Format(TEXT("%d"), iValue);
			m_CEditSurroundDepth.SetWindowText(strSliderValue);
		}
		else if (pSlider == &m_SliderCtrlSurroundDelay)
		{
			CString strSliderValue;
			int iValue = m_SliderCtrlSurroundDelay.GetPos();
			strSliderValue.Format(TEXT("%d"), iValue);
			m_CEditSurroundDelay.SetWindowText(strSliderValue);
		}
		else if (pSlider == &m_SliderCtrlLoopCount)
		{
			CString strSliderValue;
			int iValue = m_SliderCtrlLoopCount.GetPos();
			strSliderValue.Format(TEXT("%d"), iValue);
			m_CEditLoopCount.SetWindowText(strSliderValue);
		}
		else if (pSlider == &m_SliderCtrlPreAmp)
		{
			CString strSliderValue;
			int iValue = m_SliderCtrlPreAmp.GetPos();
			if ((iValue > 0) && (iValue < 16)) {
				if (iValue < m_SliderCtrlPreAmp.GetLineSize()) iValue = 0; //we want to be able to move out of the dead zone easily
				else iValue = 16;
				m_SliderCtrlPreAmp.SetPos(iValue);
			}
			strSliderValue.Format(TEXT("%d"), iValue);
			m_CEditPreAmp.SetWindowText(strSliderValue);
		}
		else if (pSlider == &m_SliderCtrlInfoRefresh)
		{
			CString strSliderValue;
			int iValue = m_SliderCtrlInfoRefresh.GetPos();
			strSliderValue.Format(TEXT("%d"), iValue);
			m_CEditInfoRefresh.SetWindowText(strSliderValue);
		}
		else if (pSlider == &m_SliderCtrlInfoBuffering)
		{
			CString strSliderValue;
			int iValue = m_SliderCtrlInfoBuffering.GetPos();
			if ((iValue > 0) && (iValue < 100)) {
				if (iValue < m_SliderCtrlInfoRefresh.GetLineSize()) iValue = 0; //we want to be able to move out of the dead zone easily
				else iValue = 100;
				m_SliderCtrlInfoBuffering.SetPos(iValue);
			}
			strSliderValue.Format(TEXT("%d"), iValue);
			m_CEditInfoBuffering.SetWindowText(strSliderValue);
		}
	}

	//Edit boxes - group1
	void CSettingsDialog::OnEnChangeEditStereoSeparation()
	{
		CString strEditValue;
		m_CEditStereoSeparation.GetWindowText(strEditValue);
		int iCEditValue = _ttoi(strEditValue);
		if (iCEditValue < 1) {
			iCEditValue = 1;
			m_CEditStereoSeparation.SetWindowText(TEXT("1"));
		} else if (iCEditValue > 256) {
			iCEditValue = 256;
			m_CEditStereoSeparation.SetWindowText(TEXT("256"));
		}
		m_SliderCtrlStereoSeparation.SetPos(iCEditValue);
	}
	void CSettingsDialog::OnEnChangeEditMaxMixChannels()
	{
		CString strEditValue;
		m_CEditMaxMixChannels.GetWindowText(strEditValue);
		int iCEditValue = _ttoi(strEditValue);
		if (iCEditValue < 32) {
			iCEditValue = 32;
			m_CEditMaxMixChannels.SetWindowText(TEXT("32"));
		} else if (iCEditValue > 256) {
			iCEditValue = 256;
			m_CEditMaxMixChannels.SetWindowText(TEXT("256"));
		}
		m_SliderCtrlMaxMixChannels.SetPos(iCEditValue);
	}
	void CSettingsDialog::OnEnChangeEditReverbDepth()
	{
		CString strEditValue;
		m_CEditReverbDepth.GetWindowText(strEditValue);
		int iCEditValue = _ttoi(strEditValue);
		if (iCEditValue < 0) {
			iCEditValue = 0;
			m_CEditReverbDepth.SetWindowText(TEXT("0"));
		} else if (iCEditValue > 100) {
			iCEditValue = 100;
			m_CEditReverbDepth.SetWindowText(TEXT("100"));
		}
		m_SliderCtrlReverbDepth.SetPos(iCEditValue);
	}
	void CSettingsDialog::OnEnChangeEditReverbDelay()
	{
		CString strEditValue;
		m_CEditReverbDelay.GetWindowText(strEditValue);
		int iCEditValue = _ttoi(strEditValue);
		//limits according to CSoundFile::SetReverbParameters in snd_dsp.cpp
		if (iCEditValue < 40) {
			iCEditValue = 40;
			m_CEditReverbDelay.SetWindowText(TEXT("40"));
		} else if (iCEditValue > 250) {
			iCEditValue = 250;
			m_CEditReverbDelay.SetWindowText(TEXT("250"));
		}
		m_SliderCtrlReverbDelay.SetPos(iCEditValue);
	}
	//Edit boxes - group2
	void CSettingsDialog::OnEnChangeEditBassAmount()
	{
		CString strEditValue;
		m_CEditBassAmount.GetWindowText(strEditValue);
		int iCEditValue = _ttoi(strEditValue);
		if (iCEditValue < 0) {
			iCEditValue = 0;
			m_CEditBassAmount.SetWindowText(TEXT("0"));
		} else if (iCEditValue > 100) {
			iCEditValue = 100;
			m_CEditBassAmount.SetWindowText(TEXT("100"));
		}
		m_SliderCtrlBassAmount.SetPos(iCEditValue);
	}
	void CSettingsDialog::OnEnChangeEditBassRange()
	{
		CString strEditValue;
		m_CEditBassRange.GetWindowText(strEditValue);
		int iCEditValue = _ttoi(strEditValue);
		//seems that real range is 10 - 84  (2*5)..((16*5)+max_roundig_remainder)
		if (iCEditValue < 10) {
			iCEditValue = 10;
			m_CEditBassRange.SetWindowText(TEXT("10"));
		} else if (iCEditValue > 100) {
			iCEditValue = 100;
			m_CEditBassRange.SetWindowText(TEXT("100"));
		}
		m_SliderCtrlBassRange.SetPos(iCEditValue);
	}
	void CSettingsDialog::OnEnChangeEditSurroundDepth()
	{
		CString strEditValue;
		m_CEditSurroundDepth.GetWindowText(strEditValue);
		int iCEditValue = _ttoi(strEditValue);
		if (iCEditValue < 0) {
			iCEditValue = 0;
			m_CEditSurroundDepth.SetWindowText(TEXT("0"));
		} else if (iCEditValue > 100) {
			iCEditValue = 100;
			m_CEditSurroundDepth.SetWindowText(TEXT("100"));
		}
		m_SliderCtrlSurroundDepth.SetPos(iCEditValue);
	}
	void CSettingsDialog::OnEnChangeEditSurroundDelay()
	{
		CString strEditValue;
		m_CEditSurroundDelay.GetWindowText(strEditValue);
		int iCEditValue = _ttoi(strEditValue);
		//limits according to CSoundFile::SetSurroundParameters in snd_dsp.cpp
		//We can do here only a simple min.-max. range check,
		//advanced checking should be done when ON_EN_KILLFOCUS is triggered.
		if (iCEditValue < 1) {
			iCEditValue = 4;
			m_CEditSurroundDelay.SetWindowText(TEXT("4"));
		} else if (iCEditValue > 50) {
			iCEditValue = 50;
			m_CEditSurroundDelay.SetWindowText(TEXT("50"));
		}
		m_SliderCtrlSurroundDelay.SetPos(iCEditValue);
	}
	//Edit boxes - group3
	void CSettingsDialog::OnEnChangeEditLoopCount()
	{
		CString strEditValue;
		m_CEditLoopCount.GetWindowText(strEditValue);
		int iCEditValue = _ttoi(strEditValue);
		if (iCEditValue < -1) {
			iCEditValue = -1;
			m_CEditLoopCount.SetWindowText(TEXT("-1"));
		} else if (iCEditValue > 32767) {
			iCEditValue = 32767;
			m_CEditLoopCount.SetWindowText(TEXT("32767"));
		} else if (iCEditValue == 0) {
			if ((strEditValue != "0") && (strEditValue != "-")) m_CEditLoopCount.SetWindowText(TEXT("0"));
		}
		m_SliderCtrlLoopCount.SetPos(iCEditValue); // set slider value
	}
	void CSettingsDialog::OnEnChangeEditPreAmp()
	{
		CString strEditValue;
		m_CEditPreAmp.GetWindowText(strEditValue);
		int iCEditValue = _ttoi(strEditValue);
		//We can do here only a simple min.-max. range check,
		//advanced checking should be done when ON_EN_KILLFOCUS is triggered.
		if (iCEditValue < 0) {
			iCEditValue = 0;
			m_CEditPreAmp.SetWindowText(TEXT("0"));
		} else if (iCEditValue > 128) {
			iCEditValue = 128;
			m_CEditPreAmp.SetWindowText(TEXT("128"));
		}
		m_SliderCtrlPreAmp.SetPos(iCEditValue); // set slider value
	}
	void CSettingsDialog::OnEnChangeEditInfoRefresh()
	{
		CString strEditValue;
		m_CEditInfoRefresh.GetWindowText(strEditValue);
		int iCEditValue = _ttoi(strEditValue);
		//range: 0 or 1..2000
		if (iCEditValue < 0) {
			iCEditValue = 0;
			m_CEditInfoRefresh.SetWindowText(TEXT("0"));
		} else if (iCEditValue > 2000) {
			iCEditValue = 2000;
			m_CEditInfoRefresh.SetWindowText(TEXT("2000"));
		}
		m_SliderCtrlInfoRefresh.SetPos(iCEditValue); // set slider value
	}
	void CSettingsDialog::OnEnChangeEditInfoBuffering()
	{
		CString strEditValue;
		m_CEditInfoBuffering.GetWindowText(strEditValue);
		int iCEditValue = _ttoi(strEditValue);
		//We can do here only a simple min.-max. range check,
		//advanced checking should be done when ON_EN_KILLFOCUS is triggered.
		if (iCEditValue < 0) {
			iCEditValue = 0;
			m_CEditInfoBuffering.SetWindowText(TEXT("0"));
		} else if (iCEditValue > 2000) {
			iCEditValue = 2000;
			m_CEditInfoBuffering.SetWindowText(TEXT("2000"));
		}
		m_SliderCtrlInfoBuffering.SetPos(iCEditValue); // set slider value
	}
	//Formats button
	void CSettingsDialog::OnFormatsClicked()
	{
		gui_edit_formats( &curpcfg, CWnd::m_hWnd, (LPCTSTR)m_Title );
	}
	//Reset button
	void CSettingsDialog::OnCfgResetClicked()
	{
		ModPlug_Settings defcfg = {3, 2, 16, 44100, 1, 128, 32, 0, 0, 0, 0, 0, 0, 0};
		Plugin_Settings defpcfg = {4, 125, 500, curpcfg.pUseInfotimer, 0, 0, curpcfg.pFormatsSize, curpcfg.pIniFNSize, 0, 1, curpcfg.pFormats, curpcfg.pIniFN};
		curcfg = defcfg;
		curpcfg = defpcfg;
		CSettingsDialog::DoInitControls();
	}
};


BEGIN_MESSAGE_MAP(CFormatsApp, CWinApp)
END_MESSAGE_MAP()


CFormatsApp theFormatsApp;

class CFormatsDialog : public CDialog {

protected:

	DECLARE_MESSAGE_MAP()
	Plugin_Settings* fp, curpcfg; // declared in main.h
	char* ftmpfileformats;
	char* ftmpinifn;

	CToolTipCtrl mf_ToolTip;
	CString m_Title;

	CListBox m_ListBoxFormats;
	CEdit m_CEditFormatsAddon;

public:
	CFormatsDialog( Plugin_Settings * fp_, CString title, CWnd * parent = NULL )
		: CDialog( IDD_FORMATS, parent )
		, fp( fp_ )
		, m_Title( title )
	{
		return;
	}

protected:

	virtual void DoDataExchange( CDataExchange * pDX )
	{
		CDialog::DoDataExchange( pDX );

		DDX_Control( pDX, IDC_LST_FORMATS, m_ListBoxFormats );
		DDX_Control( pDX, IDC_EDIT_FORMATSADDON, m_CEditFormatsAddon );
	}

	afx_msg BOOL OnInitDialog() {

		if ( !CDialog::OnInitDialog() ) {
			return false;
		}

		//SetWindowText( m_Title ); //it is already set in the dialog resource
		ftmpfileformats = (char*)malloc((*fp).pFormatsSize);
		ftmpinifn = (char*)malloc((*fp).pIniFNSize);
		memcpy(ftmpfileformats, (*fp).pFormats, (*fp).pFormatsSize);
		memcpy(ftmpinifn, (*fp).pIniFN, (*fp).pIniFNSize);
		curpcfg = *fp; //we preserving original values until OK clicked
		curpcfg.pFormats = ftmpfileformats;
		curpcfg.pIniFN = ftmpinifn;
		DoInitControls();

		//Create the ToolTip control
		//CDialog::OnInitDialog();
		if( !mf_ToolTip.Create(this))
		{
		   TRACE0("Unable to create the ToolTip!");
		}
		else
		{
			// Add tool tips to the controls, either by hard coded string 
			// or using the string table resource.
			fttcontrol = &mf_ToolTip; //we need to make ToolTipControl accessible outside of this function
			mf_ToolTip.SetMaxTipWidth(240); // enables automatic line breaks (requires VC6 and later)
			mf_ToolTip.AddTool( GetDlgItem(IDC_LST_FORMATS), LPSTR_TEXTCALLBACK);
			mf_ToolTip.AddTool( GetDlgItem(IDC_EDIT_FORMATSADDON), LPSTR_TEXTCALLBACK);
			mf_ToolTip.AddTool( GetDlgItem(IDC_BUTTON_FMTRESET), LPSTR_TEXTCALLBACK);
			mf_ToolTip.AddTool( GetDlgItem(IDC_BUTTON_FMTUNSELALL), LPSTR_TEXTCALLBACK);
			mf_ToolTip.AddTool( GetDlgItem(IDOK), LPSTR_TEXTCALLBACK);
			mf_ToolTip.AddTool( GetDlgItem(IDCANCEL), LPSTR_TEXTCALLBACK);
			mf_ToolTip.Activate(TRUE);
		}
		return TRUE;
	}

	BOOL OnTtnFmtNeedText(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
	{
		//NOTE: TOOLTIPTEXT type in some cases (compiler bug???) have only 3 members:
		//    HWND hwndFrom; UINT idFrom; UINT code; (missing: HINSTANCE hinst; UINT uFlags;)
		//    In this case we can not distinguish that idFrom member contains a handle or ControlID.
		//    Solutions:
		//    1.) Use NMTTDISPINFO instead of TOOLTIPTEXT, and check the TTF_IDISHWND bit
		//        in the uFlags member: if it is set, we have a HWND, else ControlID.
		//    2.) Guarantee to contain ControlID by passing a ControlID in the AddTool function 
		//        or force HWND by discarding the ControlID.
		//    3.) Test the returned value (is reliable?!):
		//        CWnd* testID = GetDlgItem(pNMHDR->idFrom);
		//        if (testID == NULL) nID = ::GetDlgCtrlID((HWND)pNMHDR->idFrom) else nID = pNMHDR->idFrom;

		NMTTDISPINFO* pTTT = (NMTTDISPINFO*)pNMHDR;
		UINT nID;
		if (pTTT->uFlags & TTF_IDISHWND) {
			nID = ::GetDlgCtrlID((HWND)pNMHDR->idFrom);
		} else {
			nID = pNMHDR->idFrom;
		}

		char buffer [256]; //255 character + null-terminator - this should be enough.
		DWORD buffsize = sizeof(buffer);
		DWORD ntbuffsize = buffsize - 1; //note: we need null-terminated string, which is not assured by the LoadString.

	    if (nID == IDC_LST_FORMATS)	{
			if (SetTootipTextFromRes(pTTT, IDS_HPLLST_FMTLIST, fttcontrol, &buffer[0], buffsize)) return TRUE;
		}
		if (nID == IDC_EDIT_FORMATSADDON) {
			if (SetTootipTextFromRes(pTTT, IDS_HPLEDT_FMTADDONS, fttcontrol, &buffer[0], buffsize)) return TRUE;
		}
		if (nID == IDC_BUTTON_FMTRESET) {
			if (SetTootipTextFromRes(pTTT, IDS_HPLBTN_FMTRESET, fttcontrol, &buffer[0], buffsize)) return TRUE;
		}
		if (nID == IDC_BUTTON_FMTUNSELALL) {
			if (SetTootipTextFromRes(pTTT, IDS_HPLBTN_FMTUNSELALL, fttcontrol, &buffer[0], buffsize)) return TRUE;
		}
		if (nID == IDOK) {
			if (SetTootipTextFromRes(pTTT, IDS_HPLBTN_FMTIDOK, fttcontrol, &buffer[0], buffsize)) return TRUE;
		}
		if (nID == IDCANCEL) {
			if (SetTootipTextFromRes(pTTT, IDS_HPLBTN_FMTIDCANCEL, fttcontrol, &buffer[0], buffsize)) return TRUE;
		}
	    return FALSE;
	}

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		mf_ToolTip.RelayEvent(pMsg);
		if (pMsg->message == WM_KEYDOWN)
	    {
		    if (pMsg->wParam == VK_RETURN)
	        {
				AfxGetMainWnd()->SetFocus(); //changes focus, so result of the OnKillFocus will be not reverted
		    }
	    }
		return CDialog::PreTranslateMessage(pMsg);
	}

	BOOL DoInitControls() {
		int pos, count;
		count = m_ListBoxFormats.GetCount();
		if (count < 1) { //we do not want add items multiple times when re-initializating
			int n = sizeof(Formats) / sizeof(Formats[0]);
			int c = 0;
			while (c < n) {
				m_ListBoxFormats.SetItemData( m_ListBoxFormats.AddString( Formats[c].name ), c );
				c++;
			}
		}
		m_CEditFormatsAddon.SetLimitText(63);
		m_ListBoxFormats.SetSel(-1, FALSE); //unselect all

		count = m_ListBoxFormats.GetCount();
		if (count > 0) {
			unsigned char startpos[64], lenght[64], isfound[64], n, nformats, spos, cpos, ldlen, patchsemicolon;
			memset(startpos, 0 ,sizeof(startpos));
			memset(lenght, 0 ,sizeof(lenght));
			memset(isfound, 0 ,sizeof(isfound));
			if (count > 64) count = 64; //64 listitem limit
			curpcfg.pFormats[255] = '\0'; //to be certain that next cycle ends without overrun
			//convert to uppercase (only extensions part)
			n = strlen(curpcfg.pFormats);
			cpos = 0;
			//this uppercase conversion may be not perfect, but it should be sufficient for file extensions
			while (cpos < n) {
				if (curpcfg.pFormats[cpos] > 0x60) curpcfg.pFormats[cpos] = toupper(curpcfg.pFormats[cpos]);
				cpos++;
			}
			patchsemicolon = 0;
			spos = 0;
			cpos = 0;
			n = 0;
			while ((curpcfg.pFormats[cpos] > 0) && (n < 64)) {
				while ((curpcfg.pFormats[cpos] != ';') && (curpcfg.pFormats[cpos] > 0)) cpos++;
				startpos[n] = spos;
				lenght[n] = (cpos - spos) + 1;
				if (curpcfg.pFormats[cpos] == 0) {
					patchsemicolon = cpos;
				} else cpos++;
				if (lenght[n] > 0) n++; //to ignore multiple semicolons
				spos = cpos;
			}
			nformats = n; //num. of used array elements
			if (patchsemicolon > 0) curpcfg.pFormats[patchsemicolon] = ';'; //all extensions must end with semicolon
			pos = count;
			while (pos > 0) {
				pos--;
				char * clistdata;
				clistdata = Formats[m_ListBoxFormats.GetItemData(pos)].ext;
				ldlen = strlen(clistdata);
				n = 0;
				char found = 0;
				while ((n < nformats) && (found == 0)) {
					cpos = ldlen;
					if (cpos == lenght[n]) {
						unsigned char ind = 0;
						while ((ind == 0) && (cpos > 0)) {
							cpos--;
							ind = (curpcfg.pFormats[startpos[n]+cpos] ^ clistdata[cpos]); //will be zero when values are identical
							//if (curpcfg.pFormats[startpos[n]+cpos] != clistdata[cpos]) ind++;
						}
						if (ind == 0) {
							m_ListBoxFormats.SetSel( pos, TRUE );
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
						memcpy(&customformats[pos], &curpcfg.pFormats[startpos[n]], lenght[n]);
						pos = pos + lenght[n];
					}
				}
				n++;
			}
			//Set the editbox value if necessary
			if (strlen(customformats) > 0) {
				m_CEditFormatsAddon.SetWindowText(customformats);
			}

			if (patchsemicolon > 0) curpcfg.pFormats[patchsemicolon] = '\0'; //revert to original state
		}
		return TRUE;
	}

	virtual void OnOK() {
		int pos, count;
		unsigned char n, cpos, ldlen;
		count = m_ListBoxFormats.GetCount();
		if (count > 64) count = 64; //64 listitem limit
		memset(curpcfg.pFormats, 0 ,curpcfg.pFormatsSize);
		char * clistdata;
		pos = 0;
		cpos = 0;
		while (pos < count) {
			if (m_ListBoxFormats.GetSel(pos) != 0) {
				clistdata = Formats[m_ListBoxFormats.GetItemData(pos)].ext;
				ldlen = strlen(clistdata);
				n = 0;
				if (ldlen > 0) {
					memcpy(&curpcfg.pFormats[cpos], clistdata, ldlen);
					cpos = cpos + ldlen;
				}
			}
			pos++;
		}
		char customformats[64];
		m_CEditFormatsAddon.GetWindowText(customformats, 64);
		ldlen = strlen(customformats);
		if (ldlen < 63) {
			if ((customformats[ldlen-1] != 0) && (customformats[ldlen-1] != ';')) customformats[ldlen] = ';';
		}
		int ns, nl, nmax;
		char * tmpdeffmtname = DEFAULTFORMATSNAME "\0";
		ns = strlen(tmpdeffmtname);
		nmax = ((*fp).pFormatsSize - ldlen - ns - 2);
		if (cpos > nmax) {
			MessageBox("WARNING: One or more file extensions was removed from the selection because the internal text buffer size is not enought to hold all of them!", "Modplug Settings::Formats", MB_OK | MB_ICONEXCLAMATION);
			cpos = nmax;
			while (((*fp).pFormats[cpos] != ';') && (cpos > 0)) cpos--; //go to the end of the previous extension (prevents partial extension strings)
			if (cpos > 0) cpos++;
		}
		if (ldlen > 0) {
			if (cpos > ((*fp).pFormatsSize - ldlen)) cpos = ((*fp).pFormatsSize - ldlen);
			memcpy(&curpcfg.pFormats[cpos], customformats, ldlen);
			cpos = cpos + ldlen;
		}
		if (cpos > 0) {
			nl = (curpcfg.pFormatsSize - ns - 2);
			if (cpos > nl) cpos = nl;
			memcpy(&curpcfg.pFormats[cpos+1], tmpdeffmtname, ns);
		}
		char* tmpFormats = (*fp).pFormats;
		char* tmpIniFN = (*fp).pIniFN;
		int tmpFormatsSize = (*fp).pFormatsSize;
		int tmpIniFNSize = (*fp).pIniFNSize;
		*fp = curpcfg;
		(*fp).pFormats = tmpFormats;
		(*fp).pIniFN = tmpIniFN;
		(*fp).pFormatsSize = tmpFormatsSize;
		(*fp).pIniFNSize = tmpIniFNSize;
		memcpy(&(*fp).pFormats[0], curpcfg.pFormats, tmpFormatsSize);
		memcpy(&(*fp).pIniFN[0], curpcfg.pIniFN, tmpIniFNSize);
		free(ftmpfileformats);
		free(ftmpinifn);
		CDialog::OnOK();
	}

	void CFormatsDialog::OnEnChangeEditFormatsAddon()
	{
		CString strEditValue;
		m_CEditFormatsAddon.GetWindowText(strEditValue);

	}
	//Reset button
	void CFormatsDialog::OnFmtResetClicked()
	{
		char * tmpdeffmt = DEFAULTFORMATS "\0";
		char * tmpdeffmtname = DEFAULTFORMATSNAME "\0";
		Plugin_Settings defpcfg = {4, 125, 500, curpcfg.pUseInfotimer, 0, 0, curpcfg.pFormatsSize, curpcfg.pIniFNSize, 0, curpcfg.isReset, curpcfg.pFormats, curpcfg.pIniFN};
		curpcfg = defpcfg;
		memset(&curpcfg.pFormats[0], 0, curpcfg.pFormatsSize);
		int n = strlen(tmpdeffmt);
		int ns = strlen(tmpdeffmtname);
		int nl = (curpcfg.pFormatsSize - ns - 2);
		if (n > nl) n = nl;
		memcpy(&curpcfg.pFormats[0], tmpdeffmt, n);
		n++;
		memcpy(&curpcfg.pFormats[n], tmpdeffmtname, ns);
		CFormatsDialog::DoInitControls();
	}
	//Unselect all button
	void CFormatsDialog::OnFmtUnselAllClicked()
	{
		m_ListBoxFormats.SetSel(-1, FALSE); //unselect all
	}
};



BEGIN_MESSAGE_MAP(CSettingsDialog, CDialog)
	ON_NOTIFY_EX( TTN_NEEDTEXT, 0, OnTtnNeedText)
	//Edit boxes - group1
	//ON_EN_KILLFOCUS(IDC_EDIT_STEREOSEPARATION, OnKillFocusStereoSeparation)
	//ON_EN_KILLFOCUS(IDC_EDIT_MAXMIXCHANNELS, OnKillFocusMaxMixChannels)
	//ON_EN_KILLFOCUS(IDC_EDIT_REVERBDEPTH, OnKillFocusReverbDepth)
	//ON_EN_KILLFOCUS(IDC_EDIT_REVERBDELAY, OnKillFocusReverbDelay)
	//Edit boxes - group2
	//ON_EN_KILLFOCUS(IDC_EDIT_BASSAMOUNT, OnKillFocusBassAmount)
	//ON_EN_KILLFOCUS(IDC_EDIT_BASSRANGE, OnKillFocusBassRange)
	//ON_EN_KILLFOCUS(IDC_EDIT_SURROUNDDEPTH, OnKillFocusSurroundDepth)
	ON_EN_KILLFOCUS(IDC_EDIT_SURROUNDDELAY, OnKillFocusSurroundDelay)
	//Edit boxes - group3
	ON_EN_KILLFOCUS(IDC_EDIT_LOOPCOUNT, OnKillFocusLoopCount)
	ON_EN_KILLFOCUS(IDC_EDIT_PREAMP, OnKillFocusPreAmp)
	//ON_EN_KILLFOCUS(IDC_EDIT_INFOREFRESH, OnKillFocusInfoRefresh)
	ON_EN_KILLFOCUS(IDC_EDIT_INFOBUFFERING, OnKillFocusInfoBuffering)
	//Modplug flags
	//ON_BN_CLICKED(IDC_CHECK_OVERSAMPLING, OnOversamplingClicked)
	//ON_BN_CLICKED(IDC_CHECK_NOISEREDUCTION, OnNoiseReductionClicked)
	ON_BN_CLICKED(IDC_CHECK_REVERB, OnReverbClicked)
	ON_BN_CLICKED(IDC_CHECK_MEGABASS, OnMegabassClicked)
	ON_BN_CLICKED(IDC_CHECK_SURROUND, OnSurroundClicked)
	//ON_BN_CLICKED(IDC_CHECK_USEAGC, OnUseAGCClicked)
	//Plugin flags
	//ON_BN_CLICKED(IDC_CHECK_INTVOLUMECTRL, OnIntVolumeCtrlClicked)
	//ON_BN_CLICKED(IDC_CHECK_SEEKBYORDERS, OnSeekByOrdersClicked)
	//Channels
	ON_BN_CLICKED(IDC_RADIO_CHANNELSMONO, OnChannelsMonoClicked)
	ON_BN_CLICKED(IDC_RADIO_CHANNELSSTEREO, OnChannelsStereoClicked)
	ON_BN_CLICKED(IDC_RADIO_CHANNELSQUAD, OnChannelsQuadClicked)
	//Bit depth
	ON_BN_CLICKED(IDC_RADIO_BITDEPTH8, OnBitDepth8Clicked)
	ON_BN_CLICKED(IDC_RADIO_BITDEPTH16, OnBitDepth16Clicked)
	ON_BN_CLICKED(IDC_RADIO_BITDEPTH32, OnBitDepth32Clicked)
	//Interpolation
	ON_BN_CLICKED(IDC_RADIO_RESAMPLINGNONE, OnResamplingNoneClicked)
	ON_BN_CLICKED(IDC_RADIO_RESAMPLINGLINEAR, OnResamplingLinearClicked)
	ON_BN_CLICKED(IDC_RADIO_RESAMPLINGCUBIC, OnResamplingCubicClicked)
	ON_BN_CLICKED(IDC_RADIO_RESAMPLING8TAPFIR, OnResampling8TapFIRClicked)
	//For all slider controls
	ON_WM_HSCROLL() // called when slider is scrolled
	//IMPORTANT: For EditBoxes all initial values must be guaranteed to be
	//   within of the expected range! Otherwise it crashes when
	//   SetWindowText() is used at initialization stage.
	//Edit boxes - group1
	ON_EN_CHANGE(IDC_EDIT_STEREOSEPARATION, OnEnChangeEditStereoSeparation)
	ON_EN_CHANGE(IDC_EDIT_MAXMIXCHANNELS, OnEnChangeEditMaxMixChannels)
	ON_EN_CHANGE(IDC_EDIT_REVERBDEPTH, OnEnChangeEditReverbDepth)
	ON_EN_CHANGE(IDC_EDIT_REVERBDELAY, OnEnChangeEditReverbDelay)
	//Edit boxes - group2
	ON_EN_CHANGE(IDC_EDIT_BASSAMOUNT, OnEnChangeEditBassAmount)
	ON_EN_CHANGE(IDC_EDIT_BASSRANGE, OnEnChangeEditBassRange)
	ON_EN_CHANGE(IDC_EDIT_SURROUNDDEPTH, OnEnChangeEditSurroundDepth)
	ON_EN_CHANGE(IDC_EDIT_SURROUNDDELAY, OnEnChangeEditSurroundDelay)
	//Edit boxes - group3
	ON_EN_CHANGE(IDC_EDIT_LOOPCOUNT, OnEnChangeEditLoopCount)
	ON_EN_CHANGE(IDC_EDIT_PREAMP, OnEnChangeEditPreAmp)
	ON_EN_CHANGE(IDC_EDIT_INFOREFRESH, OnEnChangeEditInfoRefresh)
	ON_EN_CHANGE(IDC_EDIT_INFOBUFFERING, OnEnChangeEditInfoBuffering)
	//Formats button
	ON_BN_CLICKED(IDC_BUTTON_FORMATS, OnFormatsClicked)
	//Reset button
	ON_BN_CLICKED(IDC_BUTTON_CFGRESET, OnCfgResetClicked)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CFormatsDialog, CDialog)
	ON_NOTIFY_EX( TTN_NEEDTEXT, 0, OnTtnFmtNeedText)
	ON_EN_CHANGE(IDC_EDIT_FORMATSADDON, OnEnChangeEditFormatsAddon)
	//Reset button
	ON_BN_CLICKED(IDC_BUTTON_FMTRESET, OnFmtResetClicked)
	ON_BN_CLICKED(IDC_BUTTON_FMTUNSELALL, OnFmtUnselAllClicked)
END_MESSAGE_MAP()

void gui_edit_settings( ModPlug_Settings * s, Plugin_Settings * p, HWND parent, std::string title ) {
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );
	CSettingsDialog dlg( s, p, title.c_str(), parent ? CWnd::FromHandle( parent ) : NULL );
	dlg.DoModal();
}

void gui_edit_formats( Plugin_Settings * fp, HWND parent, std::string title ) {
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );
	CFormatsDialog dlg( fp, title.c_str(), parent ? CWnd::FromHandle( parent ) : NULL );
	dlg.DoModal();
}
#endif //MFCGUI
#endif //not NOGUICONFIG