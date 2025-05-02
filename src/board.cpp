#include "board.h"
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QDebug>
#include <QVector>
#include <QLineEdit>
#include <QMessageBox>
#include <QCheckBox>
#include <QPolygon>

Board::Board(QWidget *parent) : QWidget(parent)
{
    setMouseTracking(true);
}

Board::~Board() {}

void Board::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPen pen = QPen(Qt::lightGray, 1);
    painter.setPen(pen);
    painter.drawLine(0, 0, width(), 0);

    painter.setPen(Qt::black);

    int radius = 20;
    int cx = width() / 2;
    int cy = height() / 2;
    int r = qMin(cx, cy) - 50;

    QVector<QPointF> pos;

    for (int i = 0; i < *size; i++) {
        double angle = (2 * M_PI / *size) * i - M_PI / 2;
        int x = cx + r * cos(angle);
        int y = cy + r * sin(angle);
        pos.append(QPointF(x, y));
    }

    for (int i = 0; i < *size; i++) {
        for (int j = 0; j < *size; j++) {
            int itemValue = (*matrix)[i][j]->text().toInt();
            if (itemValue > 0) {
                painter.setPen(Qt::gray);
                if (i == j) {
                    double loopRadius = 30.0;
                    QPointF loopCenter = pos[i] + QPointF(-radius, 0);
                    QRectF loopRect(loopCenter.x() - loopRadius, loopCenter.y() - loopRadius, 2 * loopRadius, 2 * loopRadius);
                    painter.drawArc(loopRect, 45 * 16, 270 * 16);

                    if (oriented->isChecked()) {
                        double angle = (45.0) * M_PI / 180.0;
                        drawArrow(&painter, QPointF(pos[i].x() + radius, pos[i].y()), 10, angle, loopRadius);
                    }                
                } else {
                    QLineF line(pos[i], pos[j]);
                    double lambda = (line.length() - radius) / radius;
                    double x = (pos[i].x() + lambda * pos[j].x()) / (1 + lambda); 
                    double y = (pos[i].y() + lambda * pos[j].y()) / (1 + lambda); 

                    painter.drawLine(pos[i].x(), pos[i].y(), x, y);

                    if (weighted->isChecked()) { 
                        painter.setPen(Qt::gray);
                        double offset = 10;
                        painter.drawText((pos[i].x() + x) / 2 + offset, (pos[i].y() + y) / 2 + offset, QString::number(itemValue));
                    }

                    painter.setPen(Qt::gray);
                    if (oriented->isChecked()) {   
                        double angle = atan2(y - pos[i].y(), x - pos[i].x());
                        drawArrow(&painter, QPointF(x, y), 10, angle, 1);
                    }
                }
            }
        }
    }

    painter.setBrush(Qt::gray);
    painter.setPen(Qt::black);
    for (int i = 0; i < *size; i++) {
        QRectF circle(pos[i].x() - radius, pos[i].y() - radius, radius * 2, radius * 2);
        painter.drawEllipse(circle);
        painter.drawText(circle, Qt::AlignCenter, QString::number(i + 1));
    }
}

void Board::drawArrow(QPainter *painter, QPointF point, double size, double angle, double distance) {
    QPolygonF arrow;
    arrow << QPointF(0, 0) << QPointF(-size, size / 2) << QPointF(-size, -size / 2);
    QPointF top(point.x() - distance * cos(angle), point.y() - distance * sin(angle));
    painter->save();
    painter->translate(top);
    painter->rotate(angle * 180.0 / M_PI);
    painter->setBrush(Qt::black);
    painter->drawPolygon(arrow);
    painter->restore();
}

void Board::setParams(QVector<QVector<QLineEdit*>> *matrix, int *size, QCheckBox *weighted, QCheckBox *oriented) {
    this->matrix = matrix;
    this->size = size;
    this->weighted = weighted;
    this->oriented = oriented;
    update();
}