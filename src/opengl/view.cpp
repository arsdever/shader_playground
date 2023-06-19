#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLVersionFunctionsFactory>

#include "view.hpp"

#include "logger.hpp"

auto logger = get_logger("view");

View::View(QWidget* parent)
    : QOpenGLWidget(parent)
{
}

View::~View()
{
    auto* f = getGLFunctions();
    f->glDeleteVertexArrays(1, &_vao);
    f->glDeleteBuffers(1, &_vbo);
    f->glDeleteProgram(_shaderProgram);
}

void View::recompileShaders(std::string_view vertex, std::string_view fragment)
{
    logger->info("Recompiling shaders");

    auto* f = getGLFunctions();

    f->glDeleteProgram(_shaderProgram);
    unsigned int vertexShader = compileShader(vertex, GL_VERTEX_SHADER);
    unsigned int fragmentShader = compileShader(fragment, GL_FRAGMENT_SHADER);

    _shaderProgram = f->glCreateProgram();
    f->glAttachShader(_shaderProgram, vertexShader);
    f->glAttachShader(_shaderProgram, fragmentShader);
    f->glLinkProgram(_shaderProgram);

    int success;
    std::string infoLog;
    infoLog.resize(512);
    f->glGetProgramiv(_shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        f->glGetProgramInfoLog(_shaderProgram, 512, nullptr, infoLog.data());
        logger->error("Shader linking failed: {}", infoLog);
        throw std::runtime_error("Shader linking failed");
    }

    f->glDeleteShader(vertexShader);
    f->glDeleteShader(fragmentShader);
    update();
}

void View::initializeGL()
{
    logger->debug("Initializing OpenGL");
    auto* f = getGLFunctions();
    f->glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // rectangle points
    float points[] = {
        -0.7f, -0.7f, 0.0f, // bottom left
        0.7f,  -0.7f, 0.0f, // bottom right
        0.7f,  0.7f,  0.0f, // top right

        0.7f,  0.7f,  0.0f, // top right
        -0.7f, 0.7f,  0.0f, // top left
        -0.7f, -0.7f, 0.0f  // bottom left
    };

    f->glGenVertexArrays(1, &_vao);
    f->glGenBuffers(1, &_vbo);

    f->glBindVertexArray(_vao);

    f->glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    f->glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

    f->glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    f->glEnableVertexAttribArray(0);
}

void View::resizeGL(int w, int h)
{
    logger->debug("Resizing OpenGL");
    auto* f = getGLFunctions();
    f->glViewport(0, 0, w, h);
}

void View::paintGL()
{
    logger->debug("Painting OpenGL");
    auto* f = getGLFunctions();
    f->glClear(GL_COLOR_BUFFER_BIT);

    f->glUseProgram(_shaderProgram);
    f->glBindVertexArray(_vao);
    f->glDrawArrays(GL_TRIANGLES, 0, 6);
}

QOpenGLFunctions_3_3_Core* View::getGLFunctions()
{
    QOpenGLFunctions_3_3_Core* f =
        QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_3_3_Core>(
            context());
    if (!f)
    {
        logger->error("No OpenGL context found");
        throw std::runtime_error("No OpenGL context found");
    }
    return f;
}

unsigned int View::compileShader(std::string_view source, unsigned int type)
{
    logger->debug("Compiling {} shader",
                  type == GL_VERTEX_SHADER ? "vertex" : "fragment");

    auto* f = getGLFunctions();
    unsigned int shader = f->glCreateShader(type);
    const char* src = source.data();
    f->glShaderSource(shader, 1, &src, nullptr);
    f->glCompileShader(shader);

    int success;
    std::string infoLog;
    infoLog.resize(512);
    f->glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        f->glGetShaderInfoLog(shader, 512, nullptr, infoLog.data());
        logger->error("Shader compilation failed: {}", infoLog);
        throw std::runtime_error("Shader compilation failed");
    }

    return shader;
}
