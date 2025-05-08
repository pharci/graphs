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
        connect(nodes[i], &QLineEdit::textChanged, board, &Board::createNodes);

        matrix[i].resize(size);
        for (int j = 0; j < size; j++)
        {
            matrix[i][j] = new QLineEdit;
            matrix[i][j]->setText("0");
            matrix[i][j]->setAlignment(Qt::AlignCenter);
            matrix[i][j]->setFixedSize(40, 40);

            connect(matrix[i][j], &QLineEdit::textEdited, this, [edit = matrix[i][j]](const QString &text) {
                int pos = edit->cursorPosition();
                if (!text.isEmpty()) {
                    QChar inserted = text[pos - 1];
                    if (inserted == '0' || inserted == '1') {
                        edit->blockSignals(true);
                        edit->setText(QString(inserted));
                        edit->blockSignals(false);
                    } else {
                        edit->blockSignals(true);
                        edit->setText("0");
                        edit->blockSignals(false);
                    }
                }
            });

            connect(matrix[i][j], &QLineEdit::textChanged, board, &Board::createLines);
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

    if (newSize > 1 && newSize < 16)
    {
        size = newSize;
        createMatrix();
        drawMatrix();
        board->createNodes();
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Введите число от 2 до 15.");
        msgBox.exec();
    }
}

void Sidebar::Generate()
{
    QString filename = QFileDialog::getSaveFileName(this, "Сгенерировать", "", "Text Files (*.cpp)");
    if (filename.isEmpty()) return;


    QFile file(filename);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << "#include <iostream>\n";
        out << "#include <variant>\n\n";
        out << "namespace state {\n";
        for (auto state : nodes) {
            out << "    struct _" << (state->text()) << " {\n";
            out << "        void PrintState() const { std::cout << " << "\"State: " << state->text() << "\" << std::endl; }\n";
            out << "        using Next = _1;\n";
            out << "    };\n\n";
        }
        out << "}\n\n";
        out << "using State = std::variant<";
        for (int i = 0; i < nodes.size(); ++i) {
            out << "state::_" << (nodes[i]->text());
            if (i != nodes.size() - 1) out << ", ";
        }
        out << ">;\n\n";
        out << "auto StateReporter = [](const auto& d){ d.PrintState(); };\n";
        out << "auto nextProcessor = [](const auto& s) -> State { return typename std::decay_t<decltype(s)>::Next {}; };\n\n";

        out << "class StateMachine {\n";
        out << "private:\n";
        out << "    State state_;\n";
        out << "public:\n";
        out << "    void start();\n";
        out << "    void reportCurrentState();\n";
        out << "    void next();\n";
        out << "};\n\n";

        out << "void StateMachine::start() {\n";
        out << "    state_ = state::_" << (nodes[0]->text()) << " {};\n";
        out << "}\n\n";

        out << "void StateMachine::reportCurrentState() {\n";
        out << "    std::visit(StateReporter, state_);\n";
        out << "}\n\n";

        out << "void StateMachine::next() {\n";
        out << "    state_ = std::visit(nextProcessor, state_);\n";
        out << "}\n\n";

        out << "int main() {\n";
        out << "    StateMachine program;\n";
        out << "    program.start();\n";
        out << "    try {\n";
        out << "        program.reportCurrentState();\n";
        out << "        program.next();\n";
        out << "        program.reportCurrentState();\n";
        out << "    }\n";
        out << "    catch(std::exception& ex){\n";
        out << "        std::cout << \"Exception: \" << ex.what() << std::endl;\n";
        out << "    }\n";
        out << "}\n";
        
        // for (const auto& row : matrix) {
        //     std::vector<int> new_row;
        //     for (const auto& cell : row) {
        //         bool ok;
        //         int value = cell->text().toInt(&ok);
        //         out << (ok ? value : 0);
        //     }
        // }

        file.close();
    } else {
        QMessageBox::critical(this, "Ошибка", "Произошла ошибка при записи файла.");
    }
}

QVector<QVector<QLineEdit *>>* Sidebar::getMatrix() {
    return &matrix;
}

QVector<QLineEdit *>* Sidebar::getNodes() {
    return &nodes;
}