#pragma once
#include <QGraphicsObject>
#include <QPoint>
#include <QRectF>

class Node : public QGraphicsObject 
{
    Q_OBJECT
private:
    QRectF shape;
    QColor color;
    QString name;
    double R;
public:
    explicit Node(
        QGraphicsObject *parent = nullptr, 
        double R = 25.0,
        QColor color = Qt::lightGray,
        QString name = "Name"
    );
    virtual ~Node();
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
signals:
    void positionChanged(const QPointF& newPos);
};