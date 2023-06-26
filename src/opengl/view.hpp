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
    void setVertexData(std::vector<float> vertices,
                       std::vector<size_t> vertexSizes,
                       std::vector<unsigned int> indices);
    std::vector<float> vertices() const;
    std::vector<size_t> vertexSizes() const;
    std::vector<unsigned int> indices() const;

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

    std::vector<float> _vertices {
        0.7f,  0.7f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
        0.7f,  -0.7f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
        -0.7f, -0.7f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
        -0.7f, 0.7f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top left
    };
    std::vector<size_t> _vertexSizes { 3, 3, 2 };
    std::vector<unsigned int> _indices { 0, 1, 3, 1, 2, 3 };

    std::vector<std::tuple<unsigned int, std::string>> _textures;
};
