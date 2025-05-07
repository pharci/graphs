#include <QPainter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QVector>
#include <QLabel>

#include <QLineEdit>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

#include <QFileDialog>
#include <QFile>
#include <QTextStream>

#include <QCheckBox>

#include "sidebar.h"

Sidebar::Sidebar(Board *board, QWidget *parent)
    : QWidget(parent), board(board)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(15, 15, 15, 15);

    QHBoxLayout *SizeLayout = new QHBoxLayout();
    SizeLayout->setContentsMargins(0, 0, 0, 10);

    QLabel *SizeLabel = new QLabel("Size: ", this);
    SizeLayout->addWidget(SizeLabel);

    SizeEdit = new QLineEdit(this);
    SizeEdit->setPlaceholderText("Input size");
    SizeEdit->setText("5");

    QPushButton *SizeBtn = new QPushButton("Update", this);
    connect(SizeBtn, &QPushButton::clicked, this, &Sidebar::setSize);

    SizeLayout->addWidget(SizeEdit);
    SizeLayout->addWidget(SizeBtn);
    layout->addLayout(SizeLayout);

    grid = new QGridLayout();
    grid->setContentsMargins(0, 0, 0, 10);
    layout->addLayout(grid);

    QPushButton *genBtn = new QPushButton("Generate", this);
    connect(genBtn, &QPushButton::clicked, this, &Sidebar::Generate);
    layout->addWidget(genBtn);

    layout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

    // init
    createMatrix();
    drawMatrix();
    board->setParams(&matrix, &nodes, &size);
}

Sidebar::~Sidebar() {}

void Sidebar::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
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
        NodeLabel->setFixedWidth(40);
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

    QLabel *GridLabel = new QLabel("State", this);
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
        nodes[i]->setFixedWidth(50);
        connect(nodes[i], &QLineEdit::textChanged, this, &Sidebar::onMatrixChanged);

        matrix[i].resize(size);
        for (int j = 0; j < size; j++)
        {
            matrix[i][j] = new QLineEdit;
            matrix[i][j]->setText("0");
            matrix[i][j]->setAlignment(Qt::AlignCenter);
            matrix[i][j]->setFixedSize(40, 40);
            QRegularExpression rx("^[01]{0,1}$");
            matrix[i][j]->setValidator(new QRegularExpressionValidator(rx, this));

            connect(matrix[i][j], &QLineEdit::textChanged, this, &Sidebar::onMatrixChanged);
        }
    }
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