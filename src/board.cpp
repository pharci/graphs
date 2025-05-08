#include "board.h"
#include "node.h"
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QDebug>
#include <QVector>
#include <QLineEdit>
#include <QMessageBox>
#include <QCheckBox>
#include <QPolygon>

Board::Board(QGraphicsScene *parent) : QGraphicsScene(parent)
{
}

Board::~Board() {}

void Board::createNodes() {
    int cx = width() / 2;
    int cy = height() / 2;
    int r = qMin(cx, cy) - 50;

    clear();
    nodeObjs.clear();
    nodeObjs.resize(*size);
    for (int i = 0; i < *size; i++) {
        double angle = (2 * M_PI / *size) * i - M_PI / 2;
        int x = cx + r * cos(angle);
        int y = cy + r * sin(angle);

        Node* obj = new Node(nullptr, 25, Qt::lightGray, (*nodes)[i]->text());
        obj->setPos(x, y);
        connect(obj, &Node::positionChanged, this, &Board::createLines);
        addItem(obj);

        nodeObjs[i] = obj;
    }

    createLines();
}

void Board::createLines() {
    for (QGraphicsItem *item : this->items()) {
        if (auto *base = dynamic_cast<QGraphicsLineItem *>(item)) {
            removeItem(item);
        }
        if (auto *base = dynamic_cast<QGraphicsPathItem *>(item)) {
            removeItem(item);
        }
        if (auto *base = dynamic_cast<QGraphicsPolygonItem *>(item)) {
            removeItem(item);
        }
    }

    for (int i = 0; i < *size; i++) {
        for (int j = 0; j < *size; j++) {
            int val = (*matrix)[i][j]->text().toInt();
            int val_invers = (*matrix)[j][i]->text().toInt();

            if (val && !val_invers && i != j) {
                drawLine(i, j);
            }
            else if (val && val_invers && i != j) {
                drawDoubleLine(i, j);
            }
            else if (val && i == j) {
                drawLoop(i);
            }
        }
    }
}

void Board::drawDoubleLine(int nodeIndexFrom, int nodeIndexTo) {
    qreal dx = nodeObjs[nodeIndexTo]->pos().x() - nodeObjs[nodeIndexFrom]->pos().x();
    qreal dy = nodeObjs[nodeIndexTo]->pos().y() - nodeObjs[nodeIndexFrom]->pos().y();
    qreal length = std::hypot(dx, dy);

    QPointF alongOffset(dx / length * 25.0, dy / length * 25.0);
    QPointF sideOffset(-dy / length * 10.0, dx / length * 10.0);

    QPointF p1 = nodeObjs[nodeIndexFrom]->pos() + alongOffset + sideOffset;
    QPointF p2 = nodeObjs[nodeIndexTo]->pos() - alongOffset + sideOffset;

    addLine(QLineF(p1, p2), QPen(Qt::white, 2));
    drawArrow(p2, atan2(dy, dx));
}

void Board::drawLine(int nodeIndexFrom, int nodeIndexTo) {
    qreal dx = nodeObjs[nodeIndexTo]->pos().x() - nodeObjs[nodeIndexFrom]->pos().x();
    qreal dy = nodeObjs[nodeIndexTo]->pos().y() - nodeObjs[nodeIndexFrom]->pos().y();
    qreal length = std::hypot(dx, dy);
    
    QPointF alongOffset(dx / length * 25.0, dy / length * 25.0);

    QPointF p1 = nodeObjs[nodeIndexFrom]->pos() + alongOffset;
    QPointF p2 = nodeObjs[nodeIndexTo]->pos() - alongOffset;

    addLine(QLineF(p1, p2), QPen(Qt::white, 2));
    drawArrow(p2, atan2(dy, dx));
}

void Board::drawLoop(int nodeIndex) {
    QPointF center = nodeObjs[nodeIndex]->pos();
    qreal loopRadius = 30.0;

    QRectF arcRect(center.x() - loopRadius - 25, center.y() - loopRadius, 2 * loopRadius, 2 * loopRadius);

    QLineF line(QPointF(center.x()- 25, center.y()), QPointF(center.x(), center.y() - 25));
    qreal angleDegrees = line.angle() + 10;

    QPainterPath path;
    path.arcMoveTo(arcRect, angleDegrees);
    path.arcTo(arcRect, angleDegrees, 360 - 2 * angleDegrees);

    QGraphicsPathItem* loop = new QGraphicsPathItem(path);
    loop->setPen(QPen(Qt::darkGray, 2));
    addItem(loop);

    qreal angle = (360 - angleDegrees) * M_PI / 180.0;
    QPointF endPoint = arcRect.center() + QPointF(cos(angle) * loopRadius, sin(angle) * loopRadius);

    drawArrow(endPoint, angle + M_PI / 2);
}

void Board::drawArrow(QPointF point, qreal angle) {
    QPolygonF arrowHead;
    arrowHead << point
            << (point - QPointF(cos(angle - M_PI / 6) * 10, sin(angle - M_PI / 6) * 10))
            << (point - QPointF(cos(angle + M_PI / 6) * 10, sin(angle + M_PI / 6) * 10));

    QGraphicsPolygonItem* arrow = new QGraphicsPolygonItem(arrowHead);
    arrow->setBrush(Qt::darkGray);
    arrow->setPen(QPen(Qt::darkGray));
    addItem(arrow);
}

void Board::setParams(QVector<QVector<QLineEdit*>> *matrix, QVector<QLineEdit *> *nodes, int *size) {
    this->matrix = matrix;
    this->size = size;
    this->nodes = nodes;
    createNodes();
    update();
}