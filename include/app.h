#pragma once

#include <QtWidgets/QMainWindow>
#include "sidebar.h"

class App : public QMainWindow
{
    Q_OBJECT
public:
    App(QWidget *parent = nullptr);
    Sidebar *sidebar;
    Board *board;
    ~App();
    void changeEvent(QEvent *event) override;
    void openMatrixFromFile();
    void saveMatrixToFile();
    void sidebarPallet(QWidget* widget, bool darkTheme);
    void boardPallet(QWidget* widget, bool darkTheme);
    bool isDarkThemeActive();
};
