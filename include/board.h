#pragma once
#include <QWidget>
#include <QVector>
#include <QLineEdit>
#include <QCheckBox>

class Board : public QWidget
{
    Q_OBJECT
public:
    explicit Board(QWidget *parent = nullptr);
    ~Board();
    void setParams(QVector<QVector<QLineEdit *>> *matrix, QVector<QLineEdit *> *nodes, int *size);

protected:
    QVector<QVector<QLineEdit *>> *matrix;
    QVector<QLineEdit *> *nodes;
    int *size;

    void paintEvent(QPaintEvent *event) override;
    void drawArrow(QPainter *painter, QPointF point, double size, double angle, double distance);
};