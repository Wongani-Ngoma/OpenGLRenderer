#include "Shader.h"

Shader::Shader() {

}

Shader::Shader(const char* vertexShaderPath, const char* fragmentShaderPath) {
    generate(vertexShaderPath, fragmentShaderPath);

}

void Shader::generate(const char* vertexShaderPath, const char* fragmentShaderPath) {
    int success;
    char infoLog[1024];

    GLuint vertexShader = compileShader(vertexShaderPath, GL_VERTEX_SHADER);
    GLuint fragmentShader= compileShader(fragmentShaderPath, GL_FRAGMENT_SHADER);

    id = glCreateProgram();
    glAttachShader(id, vertexShader);
    glAttachShader(id, fragmentShader);
    glLinkProgram(id);

    //Catch Errors
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(id, 1024, NULL, infoLog);
        std::cout << "ERROR: FAILED TO LINK SHADER PROGRAM: \n" << infoLog << "\n";
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::activate() {
    glUseProgram(id);
}


std::string Shader::loadShaderSrc(const char* filename) {
    std::ifstream file;
    std::stringstream buff;
    std::string ret = "";

    file.open(filename);
    if (file.is_open()) {
        buff << file.rdbuf();
        ret = buff.str();
        file.close();
    }
    else {
        std::cout << "ERROR: COULD NOT OPEN FILE: " << filename << "\n";
    }

    return ret;
}

GLuint Shader::compileShader(const char* filepath, GLenum type) {

    int success;
    char infoLog[512];

    GLuint ret = glCreateShader(type);
    std::string shaderSrc = loadShaderSrc(filepath);
    const GLchar* shader = shaderSrc.c_str();
    glShaderSource(ret, 1, &shader, NULL);
    glCompileShader(ret);

    //Catch error
    glGetShaderiv(ret, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(ret, 512, NULL, infoLog);
        std::cout << "ERROR: FAILED TO COMPILE SHADER:\n" << infoLog << "\n";
    }
  
    return ret;
}

void Shader::setInt(std::string name, int val) {
    glUniform1i(glGetUniformLocation(id, name.c_str()), val);
}

void Shader::setFloat(std::string name, float val) {
    glUniform1f(glGetUniformLocation(id, name.c_str()), val);
}

void Shader::set3Float(std::string name, glm::vec3 v) {
    set3Float(name, v.x, v.y, v.z);
}

void Shader::set3Float(std::string name, float v1, float v2, float v3) {
    glUniform3f(glGetUniformLocation(id, name.c_str()), v1, v2, v3);
}

void Shader::set4Float(std::string name, aiColor4D color) {
    glUniform4f(glGetUniformLocation(id, name.c_str()), color.r, color.g, color.b, color.a);
}

void Shader::set4Float(std::string name, glm::vec4 v) {

    glUniform4f(glGetUniformLocation(id, name.c_str()), v.x, v.y, v.z, v.w);
}

void Shader::set4Float(std::string name, float v1, float v2, float v3, float v4) {
    glUniform4f(glGetUniformLocation(id, name.c_str()), v1, v2, v3, v4);
}

void Shader::setMat4(std::string name, glm::mat4 val) {
    glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(val));
}