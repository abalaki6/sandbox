#include "shader.hpp"

shader::shader(const std::string fragment_shader_filename, const std::string vertex_shader_filename)
{
    // read source code from files
    auto vcode = load_file(vertex_shader_filename);
    auto fcode = load_file(fragment_shader_filename);
    const char* vertex_code = vcode.c_str();
    const char* fragment_code = fcode.c_str();
    // compile and link shaders to new program
    vertex = compile_shader(vertex_code, GL_VERTEX_SHADER);
    fragment = compile_shader(fragment_code, GL_FRAGMENT_SHADER);
    id = create_program();
    // clean up memory
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

std::string shader::load_file(const std::string file_name)
{
    std::string code;
    std::fstream file;
    std::stringstream stream;
    try
    {
        file.open(file_name);
        stream << file.rdbuf();
        code = stream.str();
    }
    catch(std::ifstream::failure& e)
    {
        std::cerr << "SHADER::LOAD_CODE: failed to read the file: " << e.what() << std::endl;
        exit(1);
    }
    catch(std::exception& e)
    {
        std::cerr << "SHADER::LOAD_CODE: unhandled exception: " << e.what() << std::endl;
        exit(1);
    }
    catch(...)
    {
        std::cerr << "SHADER::LOAD_CODE: unknown exception." << std::endl;
        exit(1);
    }

    return code;
}

GLuint shader::compile_shader(const char* code, GLenum shader_type)
{
    GLuint shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &code, nullptr);
    glCompileShader(shader);
    check_compilation_errors(shader, true);

    return shader;
}

GLuint shader::create_program()
{
    GLuint id = glCreateProgram();
    glAttachShader(id, vertex);
    glAttachShader(id, fragment);
    glLinkProgram(id);
    check_compilation_errors(id, false);

    return id;
}

void shader::check_compilation_errors(GLuint id, bool is_shader)
{
    GLint status;
    GLchar buffer[512];
    
    if(is_shader)
    {
        glGetShaderiv(id, GL_COMPILE_STATUS, &status);
        if(!status)
        {
            glGetShaderInfoLog(id, 512, 0, buffer);
            std::cerr << "SHADER::CHECK_COMPILATION_ERRORS::SHADER: " << std::endl << buffer << std::endl;
            exit(1);  
        }
    }
    else
    {
        glGetProgramiv(id, GL_LINK_STATUS, &status);
        if(!status)
        {
            glGetProgramInfoLog(id, 512, 0, buffer);
            std::cerr << "SHADER::CHECK_COMPILATION_ERRORS::PROGRAM: " << std::endl << buffer << std::endl;
            exit(1);  
        }
    }

}

void shader::use_program() const
{
    glUseProgram(id);
}

void shader::set_window(GLFWwindow *window)
{
    this->window = window;
}

GLFWwindow* shader::get_window() const
{
    return window;
}

GLuint shader::get_id() const
{
    return id;
}

void shader::set_mat4(const std::string& name, const glm::mat4& mat) const
{
    glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, &mat[0][0]);
}