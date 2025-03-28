#include "shader.h"

GLuint ShaderLoader::LoadShader(const std::string_view vertexPath, const std::string_view fragmentPath) {

    std::string vertexCode;
    std::string fragmentCode;

    std::ifstream vertexFile(vertexPath.data());
    std::ifstream fragmentFile(fragmentPath.data());

    std::string line;
    while (std::getline(vertexFile, line)) {
        vertexCode += line + "\n";
    }

    while (std::getline(fragmentFile, line)) {
        fragmentCode += line + "\n";
    }

    vertexFile.close();
    fragmentFile.close();

    GLuint vertexShader, fragmentShader;

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    const char* vertex = vertexCode.c_str();
    const char* fragment = fragmentCode.c_str();
    glShaderSource(vertexShader, 1, &vertex, NULL);
    glShaderSource(fragmentShader, 1, &fragment, NULL);
    
    glCompileShader(vertexShader);
    glCompileShader(fragmentShader);
    
    int  success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }


    GLuint program;
    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}
