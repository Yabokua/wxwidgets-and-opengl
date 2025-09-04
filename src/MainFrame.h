#pragma once

#include <wx/wx.h>
#include <wx/slider.h>
#include <wx/checkbox.h>
#include <wx/clrpicker.h>
#include "GLCanvas.h"

enum
{
    ID_Hello = 1,
    ID_SLIDER = 2,
    ID_CHECKBOX = 3,
    ID_COLOR_PICKER_1 = 4,
    ID_COLOR_PICKER_2 = 5,
    ID_COLOR_PICKER_3 = 6
};

class MainFrame : public wxFrame
{
public:
    MainFrame();

private:
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnSliderChange(wxCommandEvent& event);
    void OnCheckboxToggle(wxCommandEvent& event);
    void OnColorChanged1(wxColourPickerEvent& event);
    void OnColorChanged2(wxColourPickerEvent& event);
    void OnColorChanged3(wxColourPickerEvent& event);
    void CreateSidePanel();
    void OnToggleSidePanel();
    void OnMainPanelResize(wxSizeEvent& event);
    void PositionSidePanel();

    // UI
    wxPanel* m_mainPanel;
    wxPanel* m_sidePanel;
    GLCanvas* m_glCanvas;
    wxBoxSizer* m_mainSizer;
    
    // Side panel
    wxSlider* m_rotationSlider;
    wxCheckBox* m_visibilityCheckbox;
    wxColourPickerCtrl* m_colorPicker1;
    wxColourPickerCtrl* m_colorPicker2;
    wxColourPickerCtrl* m_colorPicker3;
    bool m_sidePanelVisible;

    wxDECLARE_EVENT_TABLE();
};

