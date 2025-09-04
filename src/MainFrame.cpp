#include "MainFrame.h"
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/statline.h>

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_MENU(wxID_EXIT, MainFrame::OnExit)
    EVT_SLIDER(ID_SLIDER, MainFrame::OnSliderChange)
    EVT_CHECKBOX(ID_CHECKBOX, MainFrame::OnCheckboxToggle)
    EVT_COLOURPICKER_CHANGED(ID_COLOR_PICKER_1, MainFrame::OnColorChanged1)
    EVT_COLOURPICKER_CHANGED(ID_COLOR_PICKER_2, MainFrame::OnColorChanged2)
    EVT_COLOURPICKER_CHANGED(ID_COLOR_PICKER_3, MainFrame::OnColorChanged3)
wxEND_EVENT_TABLE()

MainFrame::MainFrame()
    : wxFrame(nullptr, wxID_ANY, "OpenGL Application", wxDefaultPosition, wxSize(1000, 800)),
      m_sidePanelVisible(false)
{
    // Создать меню
    wxMenu* menuFile = new wxMenu;
    menuFile->Append(ID_Hello, "&Just click on triangle\t", "");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);

    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    SetMenuBar(menuBar);

    CreateStatusBar();
    SetStatusText("Click the button to show/hide control panel");

    // Main panel
    m_mainPanel = new wxPanel(this, wxID_ANY);
    
    // Sizer for main window
    wxBoxSizer* frameSizer = new wxBoxSizer(wxVERTICAL);
    frameSizer->Add(m_mainPanel, 1, wxEXPAND);
    SetSizer(frameSizer);

    // Sizer for main panel
    m_mainSizer = new wxBoxSizer(wxHORIZONTAL);

    // Create OpenGL canvas
    m_glCanvas = new GLCanvas(m_mainPanel);
    m_glCanvas->SetToggleTriangleCallback([this]() { OnToggleSidePanel(); });
    
    // Canvas to full main window
    m_mainSizer->Add(m_glCanvas, 1, wxEXPAND);
    
    m_mainPanel->SetSizer(m_mainSizer);
    
    CreateSidePanel();
    
    m_mainPanel->Bind(wxEVT_SIZE, &MainFrame::OnMainPanelResize, this);
}

void MainFrame::CreateSidePanel()
{
    m_sidePanel = new wxPanel(m_mainPanel, wxID_ANY);
    m_sidePanel->SetBackgroundColour(wxColour(40, 40, 40));
    
    m_sidePanel->SetSize(wxSize(250, 600));
    
    wxBoxSizer* panelSizer = new wxBoxSizer(wxVERTICAL);
    
    wxStaticText* title = new wxStaticText(m_sidePanel, wxID_ANY, "Triangle Controls");
    wxFont titleFont = title->GetFont();
    titleFont.SetWeight(wxFONTWEIGHT_BOLD);
    titleFont.SetPointSize(12);
    title->SetFont(titleFont);
    title->SetForegroundColour(wxColour(255, 255, 255));
    panelSizer->Add(title, 0, wxALL | wxALIGN_CENTER, 15);
    
    wxStaticLine* line = new wxStaticLine(m_sidePanel, wxID_ANY);
    panelSizer->Add(line, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);
    
    panelSizer->AddSpacer(10);
    
    //Create rotation slider
    wxStaticText* rotationLabel = new wxStaticText(m_sidePanel, wxID_ANY, "Rotation Angle:");
    rotationLabel->SetForegroundColour(wxColour(255, 255, 255));
    panelSizer->Add(rotationLabel, 0, wxLEFT | wxRIGHT, 15);
    
    m_rotationSlider = new wxSlider(m_sidePanel, ID_SLIDER, 0, 0, 360, 
                                   wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_LABELS);
    panelSizer->Add(m_rotationSlider, 0, wxALL | wxEXPAND, 15);
    
    panelSizer->AddSpacer(10);
    
    // Checkbox to show triangle
    m_visibilityCheckbox = new wxCheckBox(m_sidePanel, ID_CHECKBOX, "Show Triangle");
    m_visibilityCheckbox->SetValue(false);
    m_visibilityCheckbox->SetForegroundColour(wxColour(255, 255, 255));
    panelSizer->Add(m_visibilityCheckbox, 0, wxALL, 15);
    
    panelSizer->AddSpacer(15);
    
    wxStaticText* colorLabel = new wxStaticText(m_sidePanel, wxID_ANY, "Vertex Colors:");
    colorLabel->SetForegroundColour(wxColour(255, 255, 255));
    panelSizer->Add(colorLabel, 0, wxLEFT | wxRIGHT, 15);

    // Color changer for each triangle point
    wxStaticText* vertex1Label = new wxStaticText(m_sidePanel, wxID_ANY, "Top Vertex:");
    vertex1Label->SetForegroundColour(wxColour(255, 255, 255));
    panelSizer->Add(vertex1Label, 0, wxLEFT | wxRIGHT, 15);
    m_colorPicker1 = new wxColourPickerCtrl(m_sidePanel, ID_COLOR_PICKER_1, 
                                           wxColour(255, 0, 0),
                                           wxDefaultPosition, wxDefaultSize,
                                           wxCLRP_DEFAULT_STYLE);
    panelSizer->Add(m_colorPicker1, 0, wxALL | wxEXPAND, 15);

    wxStaticText* vertex2Label = new wxStaticText(m_sidePanel, wxID_ANY, "Left Vertex:");
    vertex2Label->SetForegroundColour(wxColour(255, 255, 255));
    panelSizer->Add(vertex2Label, 0, wxLEFT | wxRIGHT, 15);
    m_colorPicker2 = new wxColourPickerCtrl(m_sidePanel, ID_COLOR_PICKER_2, 
                                           wxColour(0, 255, 0),
                                           wxDefaultPosition, wxDefaultSize,
                                           wxCLRP_DEFAULT_STYLE);
    panelSizer->Add(m_colorPicker2, 0, wxALL | wxEXPAND, 15);

    wxStaticText* vertex3Label = new wxStaticText(m_sidePanel, wxID_ANY, "Right Vertex:");
    vertex3Label->SetForegroundColour(wxColour(255, 255, 255));
    panelSizer->Add(vertex3Label, 0, wxLEFT | wxRIGHT, 15);
    m_colorPicker3 = new wxColourPickerCtrl(m_sidePanel, ID_COLOR_PICKER_3, 
                                           wxColour(0, 0, 255),
                                           wxDefaultPosition, wxDefaultSize,
                                           wxCLRP_DEFAULT_STYLE);
    panelSizer->Add(m_colorPicker3, 0, wxALL | wxEXPAND, 15);

    // Checkbox to apply user's colors
    wxCheckBox* customColorCheckbox = new wxCheckBox(m_sidePanel, wxID_ANY, "Use Custom Colors");
    customColorCheckbox->SetValue(false);
    customColorCheckbox->SetForegroundColour(wxColour(255, 255, 255));
    customColorCheckbox->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, 
        [this, customColorCheckbox](wxCommandEvent& event) {
            if (m_glCanvas)
            {
                bool useCustom = customColorCheckbox->GetValue();
                m_glCanvas->SetUseCustomColor(useCustom);
                
                if (useCustom)
                {
                    for(int i = 0; i < 3; i++)
                    {
                        wxColourPickerCtrl* picker = (i == 0) ? m_colorPicker1 : 
                                                   (i == 1) ? m_colorPicker2 : m_colorPicker3;
                        wxColour color = picker->GetColour();
                        float r = color.Red() / 255.0f;
                        float g = color.Green() / 255.0f;
                        float b = color.Blue() / 255.0f;
                        m_glCanvas->SetVertexColor(i, r, g, b);
                    }
                }
                
                SetStatusText(useCustom ? "Using custom vertex colors" : "Using default vertex colors");
            }
        });

    panelSizer->Add(customColorCheckbox, 0, wxALL, 15);
    
    m_sidePanel->SetSizer(panelSizer);
    m_sidePanel->Hide();
}


void MainFrame::OnColorChanged1(wxColourPickerEvent& event)
{
    if (m_glCanvas)
    {
        wxColour color = m_colorPicker1->GetColour();
        float r = color.Red() / 255.0f;
        float g = color.Green() / 255.0f;
        float b = color.Blue() / 255.0f;
        m_glCanvas->SetVertexColor(0, r, g, b);// 0 = Up side
        SetStatusText(wxString::Format("Top vertex color: RGB(%d, %d, %d)", 
                     color.Red(), color.Green(), color.Blue()));
    }
}

void MainFrame::OnColorChanged2(wxColourPickerEvent& event)
{
    if (m_glCanvas)
    {
        wxColour color = m_colorPicker2->GetColour();
        float r = color.Red() / 255.0f;
        float g = color.Green() / 255.0f;
        float b = color.Blue() / 255.0f;
        m_glCanvas->SetVertexColor(1, r, g, b); // 1 = left side
        SetStatusText(wxString::Format("Left vertex color: RGB(%d, %d, %d)", 
                     color.Red(), color.Green(), color.Blue()));
    }
}

void MainFrame::OnColorChanged3(wxColourPickerEvent& event)
{
    if (m_glCanvas)
    {
        wxColour color = m_colorPicker3->GetColour();
        float r = color.Red() / 255.0f;
        float g = color.Green() / 255.0f;
        float b = color.Blue() / 255.0f;
        m_glCanvas->SetVertexColor(2, r, g, b); // 2 = right side
        SetStatusText(wxString::Format("Right vertex color: RGB(%d, %d, %d)", 
                     color.Red(), color.Green(), color.Blue()));
    }
}

void MainFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}

void MainFrame::OnSliderChange(wxCommandEvent& event)
{
    if (m_glCanvas)
    {
        float rotation = m_rotationSlider->GetValue();
        m_glCanvas->SetRotation(rotation);
        
        SetStatusText(wxString::Format("Triangle rotation: %.0f degrees", rotation));
    }
}

void MainFrame::OnCheckboxToggle(wxCommandEvent& event)
{
    if (m_glCanvas)
    {
        bool visible = m_visibilityCheckbox->GetValue();
        m_glCanvas->SetTriangleVisible(visible);
        
        SetStatusText(visible ? "Triangle is visible" : "Triangle is hidden");
    }
}

void MainFrame::OnToggleSidePanel()
{
    m_sidePanelVisible = !m_sidePanelVisible;
    
    if (m_sidePanelVisible)
    {
        PositionSidePanel();
        m_sidePanel->Show();
        m_sidePanel->Raise();
        SetStatusText("Control panel opened");
    }
    else
    {
        m_sidePanel->Hide();
        SetStatusText("Control panel closed");
    }
}

void MainFrame::OnMainPanelResize(wxSizeEvent& event)
{
    if (m_sidePanelVisible)
    {
        PositionSidePanel();
    }
    event.Skip();
}

void MainFrame::PositionSidePanel()
{
    if (!m_sidePanel) return;

    wxSize mainPanelSize = m_mainPanel->GetSize();
    wxSize sidePanelSize = m_sidePanel->GetSize();

    int x = mainPanelSize.GetWidth() - sidePanelSize.GetWidth();
    int y = 0;

    m_sidePanel->SetSize(sidePanelSize.GetWidth(), mainPanelSize.GetHeight());
    m_sidePanel->SetPosition(wxPoint(x, y));
}

