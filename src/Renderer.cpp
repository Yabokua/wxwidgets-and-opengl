#include <GL/glew.h>
#include "Renderer.h"
#include <wx/image.h>
#include <wx/log.h>
#include <cmath>

const std::string triangleVertexShader = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

uniform float rotation;
uniform bool useFixedSize;
uniform vec2 viewport;
uniform float fixedSize;

out vec3 vertexColor;

void main()
{
    vec3 pos = aPos;
    
    float cosR = cos(radians(rotation));
    float sinR = sin(radians(rotation));
    
    mat2 rotMatrix = mat2(cosR, -sinR, sinR, cosR);
    vec2 rotatedPos = rotMatrix * pos.xy;
    
    if (useFixedSize) {
        float aspectRatio = viewport.x / viewport.y;
        float scale = fixedSize / min(viewport.x, viewport.y);
        
        rotatedPos *= scale;
        
        if (aspectRatio > 1.0) {
            rotatedPos.x /= aspectRatio;
        } else {
            rotatedPos.y *= aspectRatio;
        }
    }
    
    gl_Position = vec4(rotatedPos, pos.z, 1.0);
    vertexColor = aColor;
}
)";

const std::string triangleFragmentShader = R"(
#version 330 core
in vec3 vertexColor;
out vec4 FragColor;

void main()
{
    FragColor = vec4(vertexColor, 1.0);
}
)";

const std::string buttonVertexShader = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform vec2 viewport;
uniform vec2 buttonPos;
uniform vec2 buttonSize;

out vec2 TexCoord;

void main()
{
    vec2 pixelPos = buttonPos + aPos * buttonSize;
    vec2 normalizedPos = (pixelPos / viewport) * 2.0 - 1.0;
    normalizedPos.y = -normalizedPos.y;
    
    gl_Position = vec4(normalizedPos, 0.0, 1.0);
    TexCoord = aTexCoord;
}
)";

const std::string buttonFragmentShader = R"(
#version 330 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D buttonTexture;
uniform bool hovered;

void main()
{
    vec4 texColor = texture(buttonTexture, TexCoord);
    if (hovered)
    {
        FragColor = texColor * vec4(1.15, 1.15, 1.15, 1.0); // Button hover
    }
    else
    {
        FragColor = texColor;
    }
}
)";

Renderer::Renderer()
    : m_triangleVAO(0), m_triangleVBO(0), m_triangleShaderProgram(0)
    , m_buttonVAO(0), m_buttonVBO(0), m_buttonShaderProgram(0)
    , m_rotation(0.0f), m_triangleVisible(false)
    , m_viewportWidth(800), m_viewportHeight(600)
    , m_useCustomColor(false)
    , m_fixedTriangleSize(800.0f)  // Triangle size
    , m_useFixedSize(true)   
{
    // Vertex colors by default
    // Up - red
    m_vertexColors.vertex1[0] = 1.0f; m_vertexColors.vertex1[1] = 0.0f; m_vertexColors.vertex1[2] = 0.0f;
    // Left - greed  
    m_vertexColors.vertex2[0] = 0.0f; m_vertexColors.vertex2[1] = 1.0f; m_vertexColors.vertex2[2] = 0.0f;
    // right - blue
    m_vertexColors.vertex3[0] = 0.0f; m_vertexColors.vertex3[1] = 0.0f; m_vertexColors.vertex3[2] = 1.0f;
    
    // button
    m_button.x = -0.7f;
    m_button.y = 0.7f;
    m_button.width = 0.15f;
    m_button.height = 0.15f;
    m_button.hovered = false;
    m_button.textureId = 0;
}

Renderer::~Renderer()
{
    // Clear OpenGL
    if (m_triangleVAO) glDeleteVertexArrays(1, &m_triangleVAO);
    if (m_triangleVBO) glDeleteBuffers(1, &m_triangleVBO);
    if (m_triangleShaderProgram) glDeleteProgram(m_triangleShaderProgram);
    
    if (m_buttonVAO) glDeleteVertexArrays(1, &m_buttonVAO);
    if (m_buttonVBO) glDeleteBuffers(1, &m_buttonVBO);
    if (m_buttonShaderProgram) glDeleteProgram(m_buttonShaderProgram);
    
    if (m_button.textureId) glDeleteTextures(1, &m_button.textureId);
}

bool Renderer::Initialize()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Main windows background
    glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
    
    if (!InitializeShaders())
    {
        wxLogError("Failed to initialize shaders");
        return false;
    }
    
    if (!InitializeGeometry())
    {
        wxLogError("Failed to initialize geometry");
        return false;
    }
    
    if (!LoadButtonTexture())
    {
        wxLogError("Failed to load button texture");
        return false;
    }
    
    return true;
}

void Renderer::Render()
{
    glClear(GL_COLOR_BUFFER_BIT);
    
    if (m_triangleVisible)
    {
        RenderTriangle();
    }
    
    RenderButton();
}

void Renderer::SetViewport(int width, int height)
{
    m_viewportWidth = width;
    m_viewportHeight = height;
    glViewport(0, 0, width, height);
}

void Renderer::SetRotation(float rotation)
{
    m_rotation = rotation;
}

void Renderer::SetUseCustomColor(bool useCustom)
{
    m_useCustomColor = useCustom;
    UpdateTriangleGeometry();
}

void Renderer::SetVertexColor(int vertexIndex, float r, float g, float b)
{
    switch(vertexIndex)
    {
        case 0: // Up
            m_vertexColors.vertex1[0] = r;
            m_vertexColors.vertex1[1] = g;
            m_vertexColors.vertex1[2] = b;
            break;
        case 1: // Left
            m_vertexColors.vertex2[0] = r;
            m_vertexColors.vertex2[1] = g;
            m_vertexColors.vertex2[2] = b;
            break;
        case 2: // Right
            m_vertexColors.vertex3[0] = r;
            m_vertexColors.vertex3[1] = g;
            m_vertexColors.vertex3[2] = b;
            break;
    }
    
    if (m_useCustomColor)
    {
        UpdateTriangleGeometry();
    }
}

void Renderer::UpdateTriangleGeometry()
{
    float triangleVertices[] = {
        // Positions         // Colors
         0.0f,  0.5f, 0.0f,  m_useCustomColor ? m_vertexColors.vertex1[0] : 1.0f, 
                             m_useCustomColor ? m_vertexColors.vertex1[1] : 0.0f, 
                             m_useCustomColor ? m_vertexColors.vertex1[2] : 0.0f, // Up
        -0.5f, -0.5f, 0.0f,  m_useCustomColor ? m_vertexColors.vertex2[0] : 0.0f,  
                             m_useCustomColor ? m_vertexColors.vertex2[1] : 1.0f, 
                             m_useCustomColor ? m_vertexColors.vertex2[2] : 0.0f, // Left
         0.5f, -0.5f, 0.0f,  m_useCustomColor ? m_vertexColors.vertex3[0] : 0.0f, 
                             m_useCustomColor ? m_vertexColors.vertex3[1] : 0.0f, 
                             m_useCustomColor ? m_vertexColors.vertex3[2] : 1.0f  // Right
    };
    
    // update VBO
    if (m_triangleVBO != 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_triangleVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(triangleVertices), triangleVertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

void Renderer::SetTriangleVisible(bool visible)
{
    m_triangleVisible = visible;
}

bool Renderer::IsButtonClicked(float x, float y)
{   // Transform positions
    float pixelX = (x + 1.0f) * 0.5f * m_viewportWidth;
    float pixelY = (1.0f - y) * 0.5f * m_viewportHeight;
    
    // Check button click
    return (pixelX >= 20.0f && pixelX <= 80.0f && 
            pixelY >= 20.0f && pixelY <= 80.0f);
}

void Renderer::UpdateButtonHover(float x, float y)
{
    m_button.hovered = IsButtonClicked(x, y);
}

bool Renderer::IsButtonHovered() const
{
    return m_button.hovered;
}

bool Renderer::InitializeShaders()
{
    m_triangleShaderProgram = CreateShaderProgram(triangleVertexShader, triangleFragmentShader);
    if (m_triangleShaderProgram == 0)
        return false;
    
    m_buttonShaderProgram = CreateShaderProgram(buttonVertexShader, buttonFragmentShader);
    if (m_buttonShaderProgram == 0)
        return false;
    
    return true;
}

bool Renderer::InitializeGeometry()
{
    float triangleVertices[] = {
        // Positions           // Colors
         0.0f,  0.577f, 0.0f,  1.0f, 0.0f, 0.0f, // Up
        -0.5f, -0.289f, 0.0f,  0.0f, 1.0f, 0.0f, // Left
         0.5f, -0.289f, 0.0f,  0.0f, 0.0f, 1.0f  // Right
    };
    
    // VAO, VBO for triangle 
    glGenVertexArrays(1, &m_triangleVAO);
    glGenBuffers(1, &m_triangleVBO);
    
    glBindVertexArray(m_triangleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_triangleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_DYNAMIC_DRAW);
    
    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // Button positions
    float buttonVertices[] = {
        0.0f, 0.0f,              0.0f, 0.0f, // Лево-низ
        1.0f, 0.0f,              1.0f, 0.0f, // Право-низ
        1.0f, 1.0f,              1.0f, 1.0f, // Право-верх
        
        0.0f, 0.0f,              0.0f, 0.0f, // Лево-низ
        1.0f, 1.0f,              1.0f, 1.0f, // Право-верх
        0.0f, 1.0f,              0.0f, 1.0f  // Лево-верх
    };    
    
    // VAO, VBO for button
    glGenVertexArrays(1, &m_buttonVAO);
    glGenBuffers(1, &m_buttonVBO);
    
    glBindVertexArray(m_buttonVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_buttonVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(buttonVertices), buttonVertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
    
    return true;
}

bool Renderer::LoadButtonTexture()
{
    m_button.textureId = LoadTexture("icon/button_icon.png");
    return m_button.textureId != 0;
}

unsigned int Renderer::CompileShader(unsigned int type, const std::string& source)
{
    unsigned int shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        wxLogError("Shader compilation failed: %s", infoLog);
        glDeleteShader(shader);
        return 0;
    }
    
    return shader;
}

unsigned int Renderer::CreateShaderProgram(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
    
    if (vs == 0 || fs == 0)
    {
        if (vs) glDeleteShader(vs);
        if (fs) glDeleteShader(fs);
        return 0;
    }
    
    unsigned int program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    
    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        wxLogError("Shader program linking failed: %s", infoLog);
        glDeleteProgram(program);
        program = 0;
    }
    
    glDeleteShader(vs);
    glDeleteShader(fs);
    
    return program;
}

void Renderer::RenderTriangle()
{
    glUseProgram(m_triangleShaderProgram);
    
    int rotationLoc = glGetUniformLocation(m_triangleShaderProgram, "rotation");
    glUniform1f(rotationLoc, m_rotation);
    
    int viewportLoc = glGetUniformLocation(m_triangleShaderProgram, "viewport");
    glUniform2f(viewportLoc, (float)m_viewportWidth, (float)m_viewportHeight);
    
    int useFixedSizeLoc = glGetUniformLocation(m_triangleShaderProgram, "useFixedSize");
    glUniform1i(useFixedSizeLoc, m_useFixedSize ? 1 : 0);
    
    int fixedSizeLoc = glGetUniformLocation(m_triangleShaderProgram, "fixedSize");
    glUniform1f(fixedSizeLoc, m_fixedTriangleSize);
    
    glBindVertexArray(m_triangleVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}

void Renderer::RenderButton()
{
    glUseProgram(m_buttonShaderProgram);
    
    int viewportLoc = glGetUniformLocation(m_buttonShaderProgram, "viewport");
    glUniform2f(viewportLoc, (float)m_viewportWidth, (float)m_viewportHeight);
    
    float buttonPixelX = 20.0f;
    float buttonPixelY = 20.0f;
    int buttonPosLoc = glGetUniformLocation(m_buttonShaderProgram, "buttonPos");
    glUniform2f(buttonPosLoc, buttonPixelX, buttonPixelY);
    
    // Size
    float buttonPixelWidth = 60.0f;
    float buttonPixelHeight = 60.0f;
    int buttonSizeLoc = glGetUniformLocation(m_buttonShaderProgram, "buttonSize");
    glUniform2f(buttonSizeLoc, buttonPixelWidth, buttonPixelHeight);
    
    // Texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_button.textureId);
    glUniform1i(glGetUniformLocation(m_buttonShaderProgram, "buttonTexture"), 0);
    
    // Hover
    glUniform1i(glGetUniformLocation(m_buttonShaderProgram, "hovered"), m_button.hovered ? 1 : 0);
    
    glBindVertexArray(m_buttonVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

unsigned int Renderer::LoadTexture(const std::string& path)
{
    wxInitAllImageHandlers();
    
    wxImage image;
    if (!image.LoadFile(path, wxBITMAP_TYPE_PNG))
    {
        wxLogError("Failed to load texture: %s", path);
        return 0;
    }
    
    int width = image.GetWidth();
    int height = image.GetHeight();
    unsigned char* data = image.GetData();
    unsigned char* alpha = image.GetAlpha();
    
    unsigned char* rgba_data = new unsigned char[width * height * 4];
    for (int i = 0; i < width * height; ++i)
    {
        rgba_data[i * 4 + 0] = data[i * 3 + 0]; // R
        rgba_data[i * 4 + 1] = data[i * 3 + 1]; // G
        rgba_data[i * 4 + 2] = data[i * 3 + 2]; // B
        rgba_data[i * 4 + 3] = alpha ? alpha[i] : 255; // A
    }
    
    unsigned int textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba_data);
    
    delete[] rgba_data;
    
    return textureId;
}

