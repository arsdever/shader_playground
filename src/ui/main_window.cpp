#include <QDockWidget>
#include <QFileDialog>
#include <QPlainTextEdit>
#include <QTabWidget>
#include <QToolBar>
#include <QToolButton>
#include <qfiledialog.h>

#include "main_window.hpp"

#include "logger.hpp"
#include "qspdlog/qspdlog.hpp"
#include "texture.hpp"
#include "view.hpp"
#include <spdlog/sinks/sink.h>
#include <spdlog/spdlog.h>

static auto logger = get_logger("ui");
static unsigned int textureCounter = 0;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    View* view = new View(this);
    view->setFixedSize(800, 600);
    setCentralWidget(view);

    QDockWidget* loggerDockWidget = new QDockWidget("Logger", this);
    loggerDockWidget->setAllowedAreas(Qt::BottomDockWidgetArea);

    QSpdLog* qspdlog = new QSpdLog(loggerDockWidget);
    auto& registry = spdlog::details::registry::instance();
    registry.apply_all([ qspdlog ](std::shared_ptr<spdlog::logger> logger)
                       { logger->sinks().push_back(qspdlog->sink()); });

    loggerDockWidget->setWidget(qspdlog);
    addDockWidget(Qt::BottomDockWidgetArea, loggerDockWidget);

    QDockWidget* codeEditor = new QDockWidget("Code editor", this);
    QTabWidget* tabWidget = new QTabWidget(codeEditor);
    codeEditor->setWidget(tabWidget);
    addDockWidget(Qt::RightDockWidgetArea, codeEditor);

    QPlainTextEdit* vertexShaderEditor = new QPlainTextEdit(this);
    vertexShaderEditor->setPlainText(
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\n");
    tabWidget->addTab(vertexShaderEditor, "Vertex shader");

    QPlainTextEdit* fragmentShaderEditor = new QPlainTextEdit(this);
    fragmentShaderEditor->setPlainText(
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
        "}\n");
    tabWidget->addTab(fragmentShaderEditor, "Fragment shader");

    QToolBar* toolBar = new QToolBar(this);
    addToolBar(toolBar);

    QToolButton* compileButton = new QToolButton(toolBar);
    compileButton->setText("Compile");
    toolBar->addWidget(compileButton);

    connect(compileButton,
            &QToolButton::clicked,
            [ view, vertexShaderEditor, fragmentShaderEditor ]()
            {
        try
        {
            view->recompileShaders(
                vertexShaderEditor->toPlainText().toStdString(),
                fragmentShaderEditor->toPlainText().toStdString());
        }
        catch (std::exception& e)
        {
        }
    });

    QToolButton* loadTexture = new QToolButton(toolBar);
    loadTexture->setText("Load texture");
    toolBar->addWidget(loadTexture);

    connect(loadTexture,
            &QToolButton::clicked,
            [ this, view ]()
            {
        logger->info("Loading a texture");
        QString textureFileName = QFileDialog::getOpenFileName(
            this, "Select texture", "", "Images (*.png *.jpg)");
        if (textureFileName.isEmpty())
        {
            logger->info("No texture selected");
            return;
        }

        QImage textureImage(textureFileName);
        if (textureImage.isNull())
        {
            logger->error("Failed to load selected image");
            return;
        }

        Texture texture = Texture::fromQImage(textureImage);
        texture.setName(fmt::format("tex_{}", textureCounter));
        logger->info("Loaded texture with id {}", textureCounter++);

        view->addTexture(std::move(texture));
    });
}

MainWindow::~MainWindow() { }
