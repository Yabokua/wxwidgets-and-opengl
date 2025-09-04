#pragma once
#include <GL/glew.h>
#include <wx/wx.h>
#include <wx/glcanvas.h>
#include <functional>
#include "Renderer.h"

class GLCanvas : public wxGLCanvas
{
public:
    GLCanvas(wxWindow* parent);
    ~GLCanvas();

    void SetToggleTriangleCallback(std::function<void()> callback);
    void SetRotation(float rotation);
    void SetTriangleVisible(bool visible);
    void SetVertexColor(int vertexIndex, float r, float g, float b);
    void SetUseCustomColor(bool useCustom);

private:
    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void InitGL();
    void Render();

    wxGLContext* m_context;
    Renderer* m_renderer;
    std::function<void()> m_toggleTriangleCallback;
    
    bool m_glInitialized;
    int m_width, m_height;

    wxDECLARE_EVENT_TABLE();
};

