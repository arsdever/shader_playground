#pragma once

#include <QMainWindow>

class QFile;
class View;

class MainWindow : public QMainWindow
{
public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    void parseAndSetVertexData(QString vertexData, View* view);

private:
    QFile* _vertexFile;
    QFile* _fragmentFile;
    QFile* _vertexDataFile;
};
