#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLVersionFunctionsFactory>

#include "view.hpp"

#include "logger.hpp"
#include "texture.hpp"

static auto logger = get_logger("opengl");

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

void View::addTexture(Texture texture)
{
    logger->info("Adding texture: {}", texture.name());
    auto* f = getGLFunctions();

    unsigned int tex;
    f->glGenTextures(1, &tex);
    f->glBindTexture(GL_TEXTURE_2D, tex);
    f->glTexImage2D(GL_TEXTURE_2D,
                    0,
                    GL_RGBA,
                    texture.width(),
                    texture.height(),
                    0,
                    GL_RGBA,
                    GL_UNSIGNED_BYTE,
                    texture.data());
    f->glGenerateMipmap(GL_TEXTURE_2D);

    _textures.emplace_back(tex, texture.name());
    update();
}
void View::setVertexData(std::vector<float> vertices,
                         std::vector<size_t> vertexSizes,
                         std::vector<unsigned int> indices)
{
    _vertices = std::move(vertices);
    _vertexSizes = std::move(vertexSizes);
    _indices = std::move(indices);

    auto* f = getGLFunctions();

    f->glBindVertexArray(_vao);
    f->glBindBuffer(GL_ARRAY_BUFFER, _vbo);

    f->glBufferData(GL_ARRAY_BUFFER,
                    sizeof(float) * _vertices.size(),
                    _vertices.data(),
                    GL_STATIC_DRAW);

    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    f->glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                    sizeof(int) * _indices.size(),
                    _indices.data(),
                    GL_STATIC_DRAW);

    size_t sum = 0;
    for (auto size : _vertexSizes)
    {
        sum += size;
    }

    size_t offset = 0;
    for (size_t i = 0; i < _vertexSizes.size(); ++i)
    {
        // TODO: Old attributes must be cleared
        f->glVertexAttribPointer(i,
                                 _vertexSizes[ i ],
                                 GL_FLOAT,
                                 GL_FALSE,
                                 sum * sizeof(float),
                                 (void*)(offset * sizeof(float)));
        offset += _vertexSizes[ i ];
        f->glEnableVertexAttribArray(i);
    }
    update();
}

std::vector<float> View::vertices() const { return _vertices; }

std::vector<size_t> View::vertexSizes() const { return _vertexSizes; }

std::vector<unsigned int> View::indices() const { return _indices; }

void View::initializeGL()
{
    logger->debug("Initializing OpenGL");
    auto* f = getGLFunctions();
    f->glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    f->glGenVertexArrays(1, &_vao);
    f->glGenBuffers(1, &_vbo);
    f->glGenBuffers(1, &_ebo);

    f->glBindVertexArray(_vao);

    f->glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    f->glBufferData(GL_ARRAY_BUFFER,
                    sizeof(float) * _vertices.size(),
                    _vertices.data(),
                    GL_STATIC_DRAW);

    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    f->glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                    sizeof(int) * _indices.size(),
                    _indices.data(),
                    GL_STATIC_DRAW);

    f->glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
    f->glEnableVertexAttribArray(0);
    f->glVertexAttribPointer(1,
                             3,
                             GL_FLOAT,
                             GL_FALSE,
                             8 * sizeof(float),
                             (void*)(3 * sizeof(float)));
    f->glEnableVertexAttribArray(1);
    f->glVertexAttribPointer(2,
                             2,
                             GL_FLOAT,
                             GL_FALSE,
                             8 * sizeof(float),
                             (void*)(6 * sizeof(float)));
    f->glEnableVertexAttribArray(2);
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

    if (!_textures.empty())
    {
        f->glBindTexture(GL_TEXTURE_2D, std::get<0>(_textures[ 0 ]));
    }

    f->glBindVertexArray(_vao);
    f->glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, nullptr);
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
