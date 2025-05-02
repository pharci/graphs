#include "app.h"
#include "board.h"
#include "sidebar.h"

#include <QHBoxLayout>
#include <QWidget>
#include <QMenuBar>
#include <QMenu>

#include <QFileDialog>
#include <QMessageBox>

#include "json.hpp"
#include <fstream>

App::App(QWidget *parent) : QMainWindow(parent)
{
    QMenuBar *menuBar = new QMenuBar(this);
    QMenu *fileMenu = new QMenu("Файл", this);

    QAction *openAction = new QAction("Открыть", this);
    QAction *saveAction = new QAction("Сохранить", this);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(openAction);
    connect(openAction, &QAction::triggered, this, &App::openMatrixFromFile);
    connect(saveAction, &QAction::triggered, this, &App::saveMatrixToFile);

    menuBar->addMenu(fileMenu);
    setMenuBar(menuBar);

    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QHBoxLayout *layout = new QHBoxLayout(central);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    Board *board = new Board(this);
    sidebar = new Sidebar(board, this);

    layout->addWidget(sidebar);
    layout->addWidget(board);

    sidebar->setFixedWidth(300);
    board->setMinimumWidth(500);
    board->setMinimumHeight(500);
}

App::~App() {}

void App::openMatrixFromFile() {
    QString filename = QFileDialog::getOpenFileName(this, "Открыть матрицу", "", "Text Files (*.json)");
    if (filename.isEmpty()) return;

    std::ifstream file(filename.toStdString());
    if (!file.is_open()) {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл.");
        return;
    }

    nlohmann::json j;
    try {
        file >> j;
    } catch (const nlohmann::json::parse_error& e) {
        QMessageBox::critical(this, "Ошибка", QString("Ошибка чтения JSON:\n%1").arg(e.what()));
        return;
    }

    if (j.find("matrix") == j.end()) {
        QMessageBox::warning(this, "Ошибка", "Файл не содержит матрицу.");
        return;
    }
    if (j.find("size") == j.end()) {
        QMessageBox::warning(this, "Ошибка", "Файл не содержит размер.");
        return;
    }

    auto matrix_json = j["matrix"];
    int size = j["size"];
    bool oriented = j["oriented"];
    bool weighted = j["weighted"];

    sidebar->setSize(size);
    sidebar->setWeighted(oriented);
    sidebar->setOriented(weighted);
    sidebar->createMatrix();
    QVector<QVector<QLineEdit *>>* matrix =  sidebar->getMatrix();

    for (int row = 0; row < size; row++) {
        for (int col = 0; col < size; col++) {
            if (matrix_json[row][col].is_number()) {
                int cell = matrix_json[row][col];
                (*matrix)[row][col]->setText(QString::number(cell));
            } else {
                (*matrix)[row][col]->setText("0");  // или другое значение по умолчанию
            }
        }
    }

    sidebar->drawMatrix();
}

void App::saveMatrixToFile() {
    QVector<QVector<QLineEdit *>>* matrix =  sidebar->getMatrix();

    QString filename = QFileDialog::getSaveFileName(this, "Сохранить матрицу", "", "Text Files (*.json)");
    if (filename.isEmpty()) return;

    nlohmann::json j;

    j["matrix"] = nlohmann::json::array();

    for (const auto& row : *matrix) {
        std::vector<int> new_row;
        for (const auto& cell : row) {
            bool ok;
            int value = cell->text().toInt(&ok);
            new_row.push_back(ok ? value : 0);
        }
        j["matrix"].push_back(new_row);
    }

    j["oriented"] = sidebar->isOriented();
    j["weighted"] = sidebar->isWeighted();
    j["size"] = matrix->size();

    std::ofstream file(filename.toStdString());
    file << j.dump(2);

    if (!file.is_open()) {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл для записи.");
        return;
    }
    QMessageBox::information(this, "Успех", "Матрица успешно сохранена.");
}