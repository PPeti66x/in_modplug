#ifndef CONFIG_HPP
#define CONFIG_HPP

#define NOMINMAX
#include <afxcmn.h>

#include <string>
#include "main.h"

//namespace libmodplug {
//namespace plugin {


void DllMainAttach();
void DllMainDetach();

void gui_edit_settings( ModPlug_Settings * s, Plugin_Settings * p, HWND parent, std::string title );
void gui_edit_formats( Plugin_Settings * fp, HWND parent, std::string title );

//tooltips related
afx_msg BOOL OnTtnNeedText(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
afx_msg BOOL OnTtnFmtNeedText(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
//afx_msg void OnKillFocus(CWnd* pNewWnd);
//Edit boxes - group1
//afx_msg void OnKillFocusStereoSeparation();
//afx_msg void OnKillFocusMaxMixChannels();
//afx_msg void OnKillFocusReverbDepth();
//afx_msg void OnKillFocusReverbDelay();
//Edit boxes - group2
//afx_msg void OnKillFocusBassAmount();
//afx_msg void OnKillFocusBassRange();
//afx_msg void OnKillFocusSurroundDepth();
afx_msg void OnKillFocusSurroundDelay();
//Edit boxes - group3
afx_msg void OnKillFocusLoopCount();
afx_msg void OnKillFocusPreAmp();
//afx_msg void OnKillFocusInfoRefresh();
afx_msg void OnKillFocusInfoBuffering();
//Modplug flags
//afx_msg void OnOversamplingClicked();
//afx_msg void OnNoiseReductionClicked();
afx_msg void OnReverbClicked();
afx_msg void OnMegabassClicked();
afx_msg void OnSurroundClicked();
//afx_msg void OnUseAGCClicked();
//Plugin flags
//afx_msg void OnIntVolumeCtrlClicked();
//afx_msg void OnSeekByOrdersClicked();
//Channels
afx_msg void OnChannelsMonoClicked();
afx_msg void OnChannelsStereoClicked();
afx_msg void OnChannelsQuadClicked();
//Bit depth
afx_msg void OnBitDepth8Clicked();
afx_msg void OnBitDepth16Clicked();
afx_msg void OnBitDepth32Clicked();
//Interpolation
afx_msg void OnResamplingNoneClicked();
afx_msg void OnResamplingLinearClicked();
afx_msg void OnResamplingCubicClicked();
afx_msg void OnResampling8TapFIRClicked();
//For all slider controls
afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
//Edit boxes - group1
afx_msg void OnEnChangeEditStereoSeparation();
afx_msg void OnEnChangeEditMaxMixChannels();
afx_msg void OnEnChangeEditReverbDepth();
afx_msg void OnEnChangeEditReverbDelay();
//Edit boxes - group2
afx_msg void OnEnChangeEditBassAmount();
afx_msg void OnEnChangeEditBassRange();
afx_msg void OnEnChangeEditSurroundDepth();
afx_msg void OnEnChangeEditSurroundDelay();
//Edit boxes - group3
afx_msg void OnEnChangeEditLoopCount();
afx_msg void OnEnChangeEditPreAmp();
afx_msg void OnEnChangeEditInfoRefresh();
afx_msg void OnEnChangeEditInfoBuffering();
afx_msg void OnFormatsClicked();
afx_msg void OnCfgResetClicked();
//Formats dialog window messages
afx_msg void OnEnChangeEditFormatsAddon();
afx_msg void OnFmtResetClicked();


//} // namespace plugin
//} // namespace libmodplug

#endif // CONFIG_HPP
