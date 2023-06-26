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
    , _vertexDataFile(new QFile("./vertex.data"))
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

    QPlainTextEdit* vertexDataEditor = new QPlainTextEdit(this);
    vertexDataEditor->setPlainText(_vertexDataFile->readAll());
    tabWidget->addTab(vertexDataEditor, "Vertex data");

    _vertexFile->close();
    _fragmentFile->close();
    _vertexDataFile->close();

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
            [ this, vertexShaderEditor, fragmentShaderEditor, vertexDataEditor ]()
            {
        _vertexFile->open(QIODevice::ReadOnly);
        if (vertexShaderEditor->toPlainText().toUtf8() !=
            _vertexFile->readAll())
        {
            _vertexFile->close();
            _vertexFile->open(QIODevice::WriteOnly);
            _vertexFile->write(vertexShaderEditor->toPlainText().toUtf8());
            _vertexFile->close();
        }

        _fragmentFile->open(QIODevice::ReadOnly);
        if (fragmentShaderEditor->toPlainText().toUtf8() !=
            _fragmentFile->readAll())
        {
            _fragmentFile->close();
            _fragmentFile->open(QIODevice::WriteOnly);
            _fragmentFile->write(fragmentShaderEditor->toPlainText().toUtf8());
            _fragmentFile->close();
        }

        _vertexDataFile->open(QIODevice::ReadOnly);
        if (vertexDataEditor->toPlainText().toUtf8() != _vertexDataFile->readAll())
        {
            _vertexDataFile->close();
            _vertexDataFile->open(QIODevice::WriteOnly);
            _vertexDataFile->write(vertexDataEditor->toPlainText().toUtf8());
            _vertexDataFile->close();
        }
    });

    QToolButton* load = new QToolButton(toolBar);
    load->setText("Load");
    toolBar->addWidget(load);

    connect(
        load,
        &QToolButton::clicked,
        [ this, vertexShaderEditor, fragmentShaderEditor, vertexDataEditor, view ]()
        {
        _vertexFile->open(QIODevice::ReadOnly);
        vertexShaderEditor->setPlainText(_vertexFile->readAll());
        _vertexFile->close();

        _fragmentFile->open(QIODevice::ReadOnly);
        fragmentShaderEditor->setPlainText(_fragmentFile->readAll());
        _fragmentFile->close();

        _vertexDataFile->open(QIODevice::ReadOnly);
        vertexDataEditor->setPlainText(_vertexDataFile->readAll());
        _vertexDataFile->close();
        });

    QToolButton* updateVertexData = new QToolButton(toolBar);
    updateVertexData->setText("Update vertex data");
    toolBar->addWidget(updateVertexData);

    connect(updateVertexData,
            &QToolButton::clicked,
            [ this, view, vertexDataEditor ]()
            { parseAndSetVertexData(vertexDataEditor->toPlainText(), view); });
}

MainWindow::~MainWindow() { }

void MainWindow::parseAndSetVertexData(QString vertexData, View* view)
{
    QStringList vertexDataList = vertexData.split("\n\n");
    QStringList vertices = vertexDataList[ 0 ].split(",");
    QStringList vertexSizes = vertexDataList[ 1 ].split(",");
    QStringList indices = vertexDataList[ 2 ].split(",");

    std::vector<float> verticesVector;
    std::vector<size_t> vertexSizesVector;
    std::vector<unsigned int> indicesVector;

    for (const QString& vertex : vertices)
    {
        verticesVector.push_back(vertex.toFloat());
    }

    for (const QString& vertexSize : vertexSizes)
    {
        vertexSizesVector.push_back(vertexSize.toUInt());
    }

    for (const QString& index : indices)
    {
        indicesVector.push_back(index.toUInt());
    }

    size_t sum = 0;
    for (size_t vertexSize : vertexSizesVector)
    {
        sum += vertexSize;
    }

    if (verticesVector.size() % sum != 0)
    {
        logger->error("Vertex count is not an integer: there may be a "
                      "missing attribute");
        return;
    }

    view->setVertexData(verticesVector, vertexSizesVector, indicesVector);
}
