#include <QOpenGLWidget>
#include <tuple>
#include <vector>

class QOpenGLFunctions_3_3_Core;
class Texture;

class View : public QOpenGLWidget
{
public:
    View(QWidget* parent = nullptr);
    ~View();

    void recompileShaders(std::string_view vertex, std::string_view fragment);
    void addTexture(Texture texture);

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
	unsigned int _ebo;
    unsigned int _shaderProgram;

    std::vector<std::tuple<unsigned int, std::string>> _textures;
};
