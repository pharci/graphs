#pragma once
#include "node.h"
#include <QWidget>
#include <QVector>
#include <QLineEdit>
#include <QCheckBox>
#include <QGraphicsScene>

class Board : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit Board(QGraphicsScene *parent = nullptr);
    ~Board();
    void setParams(QVector<QVector<QLineEdit *>> *matrix, QVector<QLineEdit *> *nodes, int *size);
    void createNodes();
    void createLines();
protected:
    QVector<QVector<QLineEdit *>> *matrix;
    QVector<QLineEdit *> *nodes;
    int *size;
    QVector<Node *> nodeObjs;
    void drawArrow(QPointF point, qreal angle);
    void drawLoop(int nodeIndex);
    void drawLine(int nodeIndexFrom, int nodeIndexTo);
    void drawDoubleLine(int nodeIndexFrom, int nodeIndexTo);

};