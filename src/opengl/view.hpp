#include <QOpenGLWidget>

class QOpenGLFunctions_3_3_Core;

class View : public QOpenGLWidget
{
public:
    View(QWidget* parent = nullptr);
    ~View();

    void recompileShaders(std::string_view vertex, std::string_view fragment);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    QOpenGLFunctions_3_3_Core* getGLFunctions();
	unsigned int compileShader(std::string_view source, unsigned int type);

private:
    unsigned int _vbo;
	unsigned int _vao;
	unsigned int _shaderProgram;
};
