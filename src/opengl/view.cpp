#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLVersionFunctionsFactory>

#include "view.hpp"

#include "logger.hpp"

auto logger = get_logger("view");

View::View(QWidget* parent)
    : QOpenGLWidget(parent)
{
}

View::~View() { }

void View::initializeGL()
{
    logger->debug("Initializing OpenGL");
    auto* f = getGLFunctions();
    f->glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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
