#include "node.h"
#include <QPainter>

Node::Node(QGraphicsObject *parent, double R, QColor color, QString name) : 
    QGraphicsObject(parent), 
    R(R),
    color(color),
    name(name)
{
    shape = QRectF(-R, -R, 2*R, 2*R);

    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
}

Node::~Node() {}

QRectF Node::boundingRect() const {
    return shape;
}

QVariant Node::itemChange(GraphicsItemChange change, const QVariant &value) {
    if (change == ItemPositionHasChanged) {
        emit positionChanged(pos());
    }
    return QGraphicsObject::itemChange(change, value);
}

void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
    painter->setBrush(color);
    painter->drawEllipse(boundingRect());

    painter->setPen(QPen(Qt::black, 2));
    painter->drawText(boundingRect(), Qt::AlignCenter, name);
}