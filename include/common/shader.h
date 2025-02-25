#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
private:
    GLuint _programID;
    GLuint createShader(GLenum type, std::string shaderPath);
    static std::string _SHADER_BASE_PATH;
public:
    Shader(std::string vertexPath, std::string fragmentPath) : Shader(vertexPath, "", fragmentPath) {}
    Shader(std::string vertexPath, std::string geometryPath, std::string fragmentPath);
    ~Shader();
    GLvoid use();
    GLuint getProgramID() { return _programID; }
    GLint getUniformLocation(const GLchar *name) { return glGetUniformLocation(_programID, name); }
    GLvoid set1i(const GLchar *name, GLint value) { glUniform1i(getUniformLocation(name), value); }
    GLvoid set1f(const GLchar *name, GLfloat value) { glUniform1f(getUniformLocation(name), value); }
    GLvoid set2f(const GLchar *name, GLfloat x, GLfloat y) { glUniform2f(getUniformLocation(name), x, y); }
    GLvoid set3f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z) { glUniform3f(getUniformLocation(name), x, y, z); }
    GLvoid set4f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w) { glUniform4f(getUniformLocation(name), x, y, z, w); }
    GLvoid setmat3fv(const GLchar *name, const GLfloat *value) { glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, value); }
    GLvoid setmat4fv(const GLchar *name, const GLfloat *value) { glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, value); }
    GLvoid bindUniformBlock(const GLchar *name, GLuint index) { glUniformBlockBinding(_programID, glGetUniformBlockIndex(_programID, name), index); }
};

std::string Shader::_SHADER_BASE_PATH = "./../../../shader/";

Shader::Shader(std::string vertexPath, std::string geometryPath, std::string fragmentPath)
{
    GLuint vertexShader, geometryShader, fragmentShader;
    _programID = glCreateProgram();
    // 顶点着色器
    vertexShader = createShader(GL_VERTEX_SHADER, vertexPath);
    glAttachShader(_programID, vertexShader);
    // 几何着色器
    if (!geometryPath.empty())
    {
        geometryShader = createShader(GL_GEOMETRY_SHADER, geometryPath);
        glAttachShader(_programID, geometryShader);
    }
    // 片段着色器
    fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentPath);
    glAttachShader(_programID, fragmentShader);
    // 链接并检查错误
    GLint success;
    glLinkProgram(_programID);
    glGetProgramiv(_programID, GL_LINK_STATUS, &success);
    if(!success)
    {
        GLchar infoLog[512];
        glGetProgramInfoLog(_programID, 512, NULL, infoLog);
        std::cout << "Link Shader Error" << std::endl;
        std::cout << infoLog << std::endl;
    }
    // 链接成功后删除着色器
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    if (!geometryPath.empty())
    {
        glDeleteShader(geometryShader);
    }
}

Shader::~Shader()
{
    glDeleteProgram(_programID);
    _programID = 0; 
}

GLvoid Shader::use()
{
    glUseProgram(_programID);
}

GLuint Shader::createShader(GLenum type, std::string shaderPath)
{
    std::string code;
    std::ifstream shaderFile;
    // 保证ifstream对象可以抛出异常
    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // 打开文件
        shaderFile.open(_SHADER_BASE_PATH + shaderPath);
        // 读取文件的缓冲内容到数据流中
        std::stringstream buffer;
        buffer << shaderFile.rdbuf();
        // 关闭文件
        shaderFile.close();
        // 转换数据流到string
        code = buffer.str();
    }
    catch(std::ifstream::failure e)
    {
        std::cout << "Load Shader File Error: " << shaderPath << std::endl;
        std::cout << e.what() << std::endl;
        return 0;
    }
    // 创建着色器    
    GLuint shaderID;
    shaderID = glCreateShader(type);
    // 绑定着色器源码
    const GLchar *shaderCode = code.c_str();
    glShaderSource(shaderID, 1, &shaderCode, NULL);
    // 编译着色器
    glCompileShader(shaderID);
    // 检查着色器是否编译成功
    GLint success;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        GLchar infoLog[512];
        glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
        std::cout << "Compile Shader Error: " << shaderPath << std::endl;
        std::cout << infoLog << std::endl;
        return 0;
    }
    return shaderID;
}

#endif