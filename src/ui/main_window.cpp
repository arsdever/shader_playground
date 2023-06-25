#include <QDockWidget>
#include <QFileDialog>
#include <QPlainTextEdit>
#include <QTabWidget>
#include <QToolBar>
#include <QToolButton>
#include <qfiledialog.h>
#include <qtoolbutton.h>

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
    , _vertexFile(new QFile("./default.vert"))
    , _fragmentFile(new QFile("./default.frag"))
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

    _vertexFile->open(QIODevice::ReadOnly);
    _fragmentFile->open(QIODevice::ReadOnly);

    QPlainTextEdit* vertexShaderEditor = new QPlainTextEdit(this);
    vertexShaderEditor->setPlainText(_vertexFile->readAll());
    tabWidget->addTab(vertexShaderEditor, "Vertex shader");

    QPlainTextEdit* fragmentShaderEditor = new QPlainTextEdit(this);
    fragmentShaderEditor->setPlainText(_fragmentFile->readAll());
    tabWidget->addTab(fragmentShaderEditor, "Fragment shader");

    _vertexFile->close();
    _fragmentFile->close();

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

    QToolButton* save = new QToolButton(toolBar);
    save->setText("Save");
    toolBar->addWidget(save);

    connect(save,
            &QToolButton::clicked,
            [ vertexFile = _vertexFile,
              vertexShaderEditor,
              fragmentFile = _fragmentFile,
              fragmentShaderEditor ]()
            {
        vertexFile->open(QIODevice::ReadOnly);
        if (vertexShaderEditor->toPlainText().toUtf8() != vertexFile->readAll())
        {
            vertexFile->close();
            vertexFile->open(QIODevice::WriteOnly);
            vertexFile->write(vertexShaderEditor->toPlainText().toUtf8());
            vertexFile->close();
        }

        fragmentFile->open(QIODevice::ReadOnly);
        if (fragmentShaderEditor->toPlainText().toUtf8() !=
            fragmentFile->readAll())
        {
            fragmentFile->close();
            fragmentFile->open(QIODevice::WriteOnly);
            fragmentFile->write(fragmentShaderEditor->toPlainText().toUtf8());
            fragmentFile->close();
        }
    });

    QToolButton* load = new QToolButton(toolBar);
    load->setText("Load");
    toolBar->addWidget(load);

    connect(load,
            &QToolButton::clicked,
            [ vertexFile = _vertexFile,
              vertexShaderEditor,
              fragmentFile = _fragmentFile,
              fragmentShaderEditor ]()
            {
        vertexFile->open(QIODevice::ReadOnly);
        vertexShaderEditor->setPlainText(vertexFile->readAll());
        vertexFile->close();

        fragmentFile->open(QIODevice::ReadOnly);
        fragmentShaderEditor->setPlainText(fragmentFile->readAll());
        fragmentFile->close();
    });
}

MainWindow::~MainWindow() { }
