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
    static std::string shaderBasePath;
public:
    Shader(std::string vertexPath, std::string fragmentPath);
    ~Shader();
    GLvoid use();
    GLuint getProgramID() { return _programID; }
    GLint getUniformLocation(const GLchar *name) { return glGetUniformLocation(_programID, name); }
    GLvoid setli(const GLchar *name, GLint value) { glUniform1i(getUniformLocation(name), value); }
    GLvoid set4f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w) { glUniform4f(getUniformLocation(name), x, y, z, w); }
};

std::string Shader::shaderBasePath = "./../../../shader/";

Shader::Shader(std::string vertexPath, std::string fragmentPath)
{
    GLuint vertexShader = createShader(GL_VERTEX_SHADER, vertexPath);
    GLuint fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentPath);
    _programID = glCreateProgram();
    glAttachShader(_programID, vertexShader);
    glAttachShader(_programID, fragmentShader);
    glLinkProgram(_programID);
    // 检查链接错误
    GLint success;
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
        shaderFile.open(shaderBasePath + shaderPath);
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