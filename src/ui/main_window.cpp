#include "main_window.hpp"

#include "view.hpp"

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
{
	setCentralWidget(new View(this));
}

MainWindow::~MainWindow()
{
}
