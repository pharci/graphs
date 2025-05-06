#include <QPainter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QValidator>
#include <QMessageBox>
#include <QVector>
#include <QLabel>

#include <QFileDialog>
#include <QFile>
#include <QTextStream>

#include <QCheckBox>

#include "sidebar.h"

Sidebar::Sidebar(Board *board, QWidget *parent)
    : QWidget(parent), board(board)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    // type
    weighted = new QCheckBox("Взвешенный", this);
    oriented = new QCheckBox("Ориентированный", this);
    layout->addWidget(weighted);
    layout->addWidget(oriented);
    connect(weighted, &QCheckBox::clicked, this, &Sidebar::onMatrixChanged);
    connect(oriented, &QCheckBox::clicked, this, &Sidebar::onOrientedChange);

    // размер матрицы
    QHBoxLayout *SizeLayout = new QHBoxLayout();
    QPushButton *SizeBtn = new QPushButton("Обновить", this);
    SizeEdit = new QLineEdit(this);
    SizeEdit->setPlaceholderText("Введите размер");
    SizeEdit->setText("5");
    SizeLayout->addWidget(SizeEdit);
    SizeLayout->addWidget(SizeBtn);
    layout->addLayout(SizeLayout);

    connect(SizeBtn, &QPushButton::clicked, this, &Sidebar::setSize);

    QPushButton *genBtn = new QPushButton("Сгенерировать", this);
    connect(genBtn, &QPushButton::clicked, this, &Sidebar::Generate);

    // матрица
    grid = new QGridLayout();
    layout->addLayout(grid);
    layout->addWidget(genBtn);

    layout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

    // init
    createMatrix();
    drawMatrix();
    board->setParams(&matrix, &nodes, &size, weighted, oriented);
}

Sidebar::~Sidebar() {}

void Sidebar::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPen pen = QPen(Qt::lightGray, 1);
    painter.setPen(pen);

    painter.drawLine(0, 0, width(), 0);
    painter.drawLine(width(), 0, width(), height());
}

void Sidebar::drawMatrix()
{
    QLayoutItem *item;
    while ((item = grid->takeAt(0)))
    {
        delete item->widget();
    }

    for (int i = 0; i < size; i++)
    {
        grid->addWidget(nodes[i], i + 1, 0);

        QLabel *NodeLabel = new QLabel(nodes[i]->text(), this);
        NodeLabel->setAlignment(Qt::AlignCenter);
        grid->addWidget(NodeLabel, 0, i + 1);

        connect(nodes[i], &QLineEdit::textChanged, this, [=](const QString &text)
        { NodeLabel->setText( nodes[i]->text()); });
    }

    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            grid->addWidget(matrix[i][j], i + 1, j + 1);
        }
    }

    QLabel *GridLabel = new QLabel("Имя", this);
    GridLabel->setAlignment(Qt::AlignCenter);
    grid->addWidget(GridLabel, 0, 0);
}

void Sidebar::createMatrix()
{
    matrix.clear();
    matrix.resize(size);

    nodes.clear();
    nodes.resize(size);

    for (int i = 0; i < size; i++)
    {
        nodes[i] = new QLineEdit;
        nodes[i]->setText(QString::number(i));
        nodes[i]->setAlignment(Qt::AlignCenter);
        connect(nodes[i], &QLineEdit::textChanged, this, &Sidebar::onMatrixChanged);

        matrix[i].resize(size);
        for (int j = 0; j < size; j++)
        {
            matrix[i][j] = new QLineEdit;
            matrix[i][j]->setText("0");
            matrix[i][j]->setAlignment(Qt::AlignCenter);
            matrix[i][j]->setFixedSize(40, 40);

            if (!oriented->isChecked())
            {
                connect(matrix[i][j], &QLineEdit::textChanged, this, [=](const QString &text)
                        { matrix[j][i]->setText(matrix[i][j]->text()); });
            }
            connect(matrix[i][j], &QLineEdit::textChanged, this, &Sidebar::onMatrixChanged);
        }
    }
}

void Sidebar::onOrientedChange() {
    for (int i = 0; i < matrix.size(); ++i) {
        for (int j = 0; j < matrix[i].size(); ++j) {
            if (i != j) {
                matrix[i][j]->disconnect();
                if (!oriented->isChecked())
                {
                    connect(matrix[i][j], &QLineEdit::textChanged, this, [=](const QString &text)
                            { matrix[j][i]->setText(matrix[i][j]->text()); });
                }
                connect(matrix[i][j], &QLineEdit::textChanged, this, &Sidebar::onMatrixChanged);
            }
        }
    }
    onMatrixChanged();
}

void Sidebar::setSize(int sizeParam)
{
    int newSize = SizeEdit->text().toInt();
    if (sizeParam > 1) {
        newSize = sizeParam;
        SizeEdit->setText(QString::number(newSize));
    }

    if (newSize > 1 && newSize < 10)
    {
        size = newSize;
        createMatrix();
        drawMatrix();
        board->update();
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Введите число от 2 до 9.");
        msgBox.exec();
    }
}

void Sidebar::Generate()
{
    QString filename = QFileDialog::getSaveFileName(this, "Сохранить матрицу", "", "Text Files (*.cpp)");
    if (filename.isEmpty()) return;


    QFile file(filename);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << "#include <iostream>\n";
        out << "enum class State { ";

        for (const auto& row : matrix) {
            std::vector<int> new_row;
            for (const auto& cell : row) {
                bool ok;
                int value = cell->text().toInt(&ok);
                out << (ok ? value : 0);
            }
        }

        out << " }";

        file.close();
    } else {
        QMessageBox::critical(this, "Ошибка", "Произошла ошибка при записи файла.");
    }
}

void Sidebar::onMatrixChanged()
{
    board->update();
}

QVector<QVector<QLineEdit *>>* Sidebar::getMatrix() {
    return &matrix;
}

bool Sidebar::isWeighted() { return weighted->isChecked(); }
bool Sidebar::isOriented() { return oriented->isChecked(); }

void Sidebar::setWeighted(bool flag) {
    if (flag) weighted->setChecked(true);
    else weighted->setChecked(false);
}
void Sidebar::setOriented(bool flag) {
    if (flag) oriented->setChecked(true);
    else oriented->setChecked(false);
}