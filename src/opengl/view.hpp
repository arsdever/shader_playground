#include <QOpenGLWidget>

class QOpenGLFunctions_3_3_Core;

class View : public QOpenGLWidget
{
public:
    View(QWidget* parent = nullptr);
    ~View();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    QOpenGLFunctions_3_3_Core* getGLFunctions();
};
