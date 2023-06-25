#pragma once

#include <QMainWindow>

class QFile;

class MainWindow : public QMainWindow
{
public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    QFile* _vertexFile;
    QFile* _fragmentFile;
};
