#include <QDockWidget>

#include "main_window.hpp"

#include "qspdlog/qspdlog.hpp"
#include "view.hpp"
#include <spdlog/sinks/sink.h>
#include <spdlog/spdlog.h>

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
}

MainWindow::~MainWindow() { }
