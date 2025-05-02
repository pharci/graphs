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
    void setParams(QVector<QVector<QLineEdit *>> *matrix, int *size, QCheckBox *weighted, QCheckBox *oriented);

protected:
    QVector<QVector<QLineEdit *>> *matrix;
    int *size;
    QCheckBox *weighted;
    QCheckBox *oriented;

    void paintEvent(QPaintEvent *event) override;
    void drawArrow(QPainter *painter, QPointF point, double size, double angle, double distance);
};