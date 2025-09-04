#pragma once
#include <string>

struct ButtonData
{
    float x, y;          // Positions
    float width, height; // Size
    bool hovered;        // Hover
    unsigned int textureId; // icon Id
};

struct VertexColors
{
    float vertex1[3]; // Up
    float vertex2[3]; // Left
    float vertex3[3]; // Right
};

class Renderer
{
public:
    Renderer();
    ~Renderer();

    bool Initialize();
    void Render();
    void SetViewport(int width, int height);

    // Triangle
    void SetRotation(float rotation);
    void SetTriangleVisible(bool visible);
    void SetVertexColor(int vertexIndex, float r, float g, float b); // 0=верх, 1=лево, 2=право
    void SetUseCustomColor(bool useCustom);
    void UpdateTriangleGeometry();
    
    // Button
    bool IsButtonClicked(float x, float y);
    void UpdateButtonHover(float x, float y);
    bool IsButtonHovered() const;

private:
    // Init
    bool InitializeShaders();
    bool InitializeGeometry();
    bool LoadButtonTexture();
    unsigned int CompileShader(unsigned int type, const std::string& source);
    unsigned int CreateShaderProgram(const std::string& vertexShader, const std::string& fragmentShader);
    
    // Render
    void RenderTriangle();
    void RenderButton();
    
    // Texture
    unsigned int LoadTexture(const std::string& path);

    // OpenGL for triangle
    unsigned int m_triangleVAO, m_triangleVBO;
    unsigned int m_triangleShaderProgram;
    
    // OpenGL for button
    unsigned int m_buttonVAO, m_buttonVBO;
    unsigned int m_buttonShaderProgram;
    
    float m_fixedTriangleSize;
    bool m_useFixedSize;
    float m_rotation;
    bool m_triangleVisible;
    int m_viewportWidth, m_viewportHeight;
    VertexColors m_vertexColors;
    bool m_useCustomColor;

    ButtonData m_button;
};

