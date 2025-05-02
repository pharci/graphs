#pragma once

#include <QtWidgets/QMainWindow>
#include "sidebar.h"

class App : public QMainWindow
{
    Q_OBJECT
public:
    App(QWidget *parent = nullptr);
    Sidebar *sidebar;
    ~App();
    void openMatrixFromFile();
    void saveMatrixToFile();
};
