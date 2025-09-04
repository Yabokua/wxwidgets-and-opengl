#include <GL/glew.h>
#include "GLCanvas.h"
#include <wx/dcclient.h>

wxBEGIN_EVENT_TABLE(GLCanvas, wxGLCanvas)
    EVT_PAINT(GLCanvas::OnPaint)
    EVT_SIZE(GLCanvas::OnSize)
    EVT_LEFT_DOWN(GLCanvas::OnMouseDown)
    EVT_MOTION(GLCanvas::OnMouseMove)
wxEND_EVENT_TABLE()

GLCanvas::GLCanvas(wxWindow* parent)
    : wxGLCanvas(parent, wxID_ANY, nullptr, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE)
    , m_context(nullptr)
    , m_renderer(nullptr)
    , m_glInitialized(false)
    , m_width(0)
    , m_height(0)
{
    // OpenGL context
    wxGLContextAttrs ctxAttrs;
    ctxAttrs.PlatformDefaults().CoreProfile().OGLVersion(3, 3).EndList();
    m_context = new wxGLContext(this, nullptr, &ctxAttrs);

    // Render
    m_renderer = new Renderer();
}

GLCanvas::~GLCanvas()
{
    delete m_renderer;
    delete m_context;
}

void GLCanvas::SetUseCustomColor(bool useCustom)
{
    if (m_renderer)
    {
        m_renderer->SetUseCustomColor(useCustom);
        Refresh();
    }
}

void GLCanvas::SetToggleTriangleCallback(std::function<void()> callback)
{
    m_toggleTriangleCallback = callback;
}

void GLCanvas::SetRotation(float rotation)
{
    if (m_renderer)
    {
        m_renderer->SetRotation(rotation);
        Refresh();
    }
}

void GLCanvas::SetTriangleVisible(bool visible)
{
    if (m_renderer)
    {
        m_renderer->SetTriangleVisible(visible);
        Refresh();
    }
}

void GLCanvas::SetVertexColor(int vertexIndex, float r, float g, float b)
{
    if (m_renderer)
    {
        m_renderer->SetVertexColor(vertexIndex, r, g, b);
        Refresh();
    }
}

void GLCanvas::OnPaint(wxPaintEvent& event)
{
    wxPaintDC dc(this);
    
    if (!IsShownOnScreen())
        return;

    SetCurrent(*m_context);
    
    if (!m_glInitialized)
    {
        InitGL();
        m_glInitialized = true;
    }

    Render();
    SwapBuffers();
}

void GLCanvas::OnSize(wxSizeEvent& event)
{
    wxSize size = event.GetSize();
    m_width = size.GetWidth();
    m_height = size.GetHeight();

    if (m_renderer && m_glInitialized)
    {
        SetCurrent(*m_context);
        m_renderer->SetViewport(m_width, m_height);
    }

    event.Skip();
}

void GLCanvas::OnMouseDown(wxMouseEvent& event)
{
    if (!m_renderer)
        return;

    wxPoint pos = event.GetPosition();
    
    // Mouse position 
    float x = (2.0f * pos.x) / m_width - 1.0f;
    float y = 1.0f - (2.0f * pos.y) / m_height;

    // Button click listener
    if (m_renderer->IsButtonClicked(x, y))
    {
        if (m_toggleTriangleCallback)
        {
            m_toggleTriangleCallback();
        }
    }
}

void GLCanvas::OnMouseMove(wxMouseEvent& event)
{
    if (!m_renderer)
        return;

    wxPoint pos = event.GetPosition();
    
    // Mouse position
    float x = (2.0f * pos.x) / m_width - 1.0f;
    float y = 1.0f - (2.0f * pos.y) / m_height;

    // Hover
    bool wasHovered = m_renderer->IsButtonHovered();
    m_renderer->UpdateButtonHover(x, y);
    bool isHovered = m_renderer->IsButtonHovered();

    if (wasHovered != isHovered)
    {
        Refresh();
    }

    if (isHovered)
    {
        SetCursor(wxCursor(wxCURSOR_HAND));
    }
    else
    {
        SetCursor(wxCursor(wxCURSOR_ARROW));
    }
}

void GLCanvas::InitGL()
{
    SetCurrent(*m_context);
    
    // GLEW
    if (glewInit() != GLEW_OK)
    {
        wxLogError("Failed to initialize GLEW");
        return;
    }

    // Render
    if (m_renderer)
    {
        m_renderer->Initialize();
        m_renderer->SetViewport(m_width, m_height);
    }
}

void GLCanvas::Render()
{
    if (m_renderer)
    {
        m_renderer->Render();
    }
}

