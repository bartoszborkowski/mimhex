#include <QGraphicsRectItem>
#include <QBrush>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QGraphicsSvgItem>
#include <QDebug>

#include "Field.h"

const QString Field::s_blackStoneFilename(":/images/gogui-black.svg");
const QString Field::s_whiteStoneFilename(":/images/gogui-white.svg");

const qreal Field::s_stoneAspectRatio = 1.0;
const qreal Field::s_shapeAspectRatio = 0.5;

const QPen Field::s_shapePen(Qt::red, 3.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

const qreal Field::s_fieldZValue = 1.0;
const qreal Field::s_stoneZValue = 1.0;
const qreal Field::s_backgroundZValue = 0.0;
const qreal Field::s_shapeZValue = 2.0;
const qreal Field::s_labelZValue = 3.0;

Field::Field(const QPainterPath & path, QGraphicsItem *parent) :
  QGraphicsItemGroup(parent), m_background(new QGraphicsPathItem(path, this)), m_stone(NULL),
  m_mark(NULL), m_circle(NULL), m_square(NULL), m_triangle(NULL), m_label(NULL) {
  setAcceptHoverEvents(true);
  setZValue(s_fieldZValue);

  QPen pen;
  pen.setStyle(Qt::NoPen);
  m_background->setPen(pen);
  m_background->setZValue(s_backgroundZValue);
  addToGroup(m_background);
}

void Field::clearField() {
    removeItem(m_stone);
    removeItem(m_mark);
    removeItem(m_circle);
    removeItem(m_square);
    removeItem(m_triangle);
    removeItem(m_label);
    removeBGMark();
    update();
} 

void Field::setMousePressHandler(const callback_type& handler) {
  m_mousePressHandler = handler;
}

void Field::mousePressEvent ( QGraphicsSceneMouseEvent * event ) {
  if (m_mousePressHandler)
    m_mousePressHandler(event->buttons());
}

///* debugging functions
void Field::hoverEnterEvent(QGraphicsSceneHoverEvent * event) {
  Q_UNUSED(event);
  m_background->setBrush(QColor(64, 64, 64, 64));
  update();
}

void Field::hoverLeaveEvent(QGraphicsSceneHoverEvent * event) {
  Q_UNUSED(event);
  if (bgMark.get())
    m_background->setBrush(*bgMark);
  else
    m_background->setBrush(Qt::NoBrush);
  update();
}
//*/

void Field::removeItem(QGraphicsItem*& item) {
  if (item) {
    scene()->removeItem(item);
    removeFromGroup(item);
    delete item;
    item = NULL;
  }
}

QGraphicsItem* Field::addStone(EStoneColor stoneColor) {
  removeStone();
  switch (stoneColor) {
  case StoneBlack:
    m_stone = new QGraphicsSvgItem(s_blackStoneFilename, this);
    break;
  case StoneWhite:
    m_stone = new QGraphicsSvgItem(s_whiteStoneFilename, this);
    break;
  default:
    break;
  }

  if (m_stone) {
    int min = (boundingRect().width() < boundingRect().height()) ? boundingRect().width() : boundingRect().height();
    qreal scaleRatio = s_stoneAspectRatio * min / m_stone->boundingRect().width();

    addToGroup(m_stone);

    m_stone->setZValue(s_stoneZValue);
    m_stone->scale(scaleRatio, scaleRatio);
    m_stone->translate(-m_stone->boundingRect().width() / 2, -m_stone->boundingRect().height() / 2);
  }

  return m_stone;
}

void Field::removeStone() {
  removeItem(m_stone);
}

class Mark: public QGraphicsRectItem
{
public:
  Mark(QGraphicsItem * parent = 0) :
    QGraphicsRectItem(parent) {
  }
  Mark(const QRectF & rect, QGraphicsItem * parent = 0) :
    QGraphicsRectItem(rect, parent) {
  }
  ;
  Mark(qreal x, qreal y, qreal width, qreal height, QGraphicsItem * parent = 0) :
    QGraphicsRectItem(x, y, width, height, parent) {
  }
  ~Mark() {
  }

  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
    painter->setPen(pen());
    painter->drawLine(rect().bottomLeft(), rect().topRight());
    painter->drawLine(rect().topLeft(), rect().bottomRight());
  }
};

QGraphicsItem* Field::addMark() {
  Mark* mark;
  removeMark();
  qreal width = s_shapeAspectRatio * ((boundingRect().width() > boundingRect().height()) ? boundingRect().width() : boundingRect().height());
  m_mark = mark = new Mark(-width / 2, -width / 2, width, width, this);
  mark->setPen(s_shapePen);
  mark->setZValue(s_shapeZValue);
  addToGroup(m_mark);
  return m_mark;
}

void Field::removeMark() {
  removeItem(m_mark);
}

QGraphicsItem* Field::addCircle() {
  QGraphicsEllipseItem *circle;
  removeCircle();
  qreal width = s_shapeAspectRatio * ((boundingRect().width() > boundingRect().height()) ? boundingRect().width() : boundingRect().height());
  m_circle = circle = new QGraphicsEllipseItem(- width / 2, - width / 2, width, width, this);

  circle->setPen(s_shapePen);
  circle->setZValue(s_shapeZValue);

  addToGroup(m_circle);
  return m_circle;
}

void Field::removeCircle() {
  removeItem(m_circle);
}

QGraphicsItem* Field::addSquare() {
  QGraphicsRectItem *square;
  removeSquare();
  qreal width = s_shapeAspectRatio * ((boundingRect().width() > boundingRect().height()) ? boundingRect().width() : boundingRect().height());
  m_square = square = new QGraphicsRectItem(- width / 2, - width / 2, width, width, this);
  square->setPen(s_shapePen);
  square->setZValue(s_shapeZValue);
  addToGroup(m_square);
  return m_square;
}

void Field::removeSquare() {
  removeItem(m_square);
}

QGraphicsItem* Field::addTriangle() {
  QGraphicsPolygonItem *triangle;
  removeTriangle();
  qreal width = s_shapeAspectRatio * ((boundingRect().width() > boundingRect().height()) ? boundingRect().width() : boundingRect().height());
  QPolygonF polygon(3);
  polygon[0] = QPointF(-width / 2, width / 2);
  polygon[1] = QPointF(width / 2, width / 2);
  polygon[2] = QPointF(0.0, -width / 2);
  m_triangle = triangle = new QGraphicsPolygonItem(polygon, this);
  triangle->setPen(s_shapePen);
  triangle->setZValue(s_shapeZValue);
  addToGroup(m_triangle);
  return m_triangle;
}

void Field::removeTriangle() {
  removeItem(m_triangle);
}

QGraphicsItem* Field::addLabel(const QString& labelString) {
  QGraphicsTextItem *label;
  removeLabel();
  m_label = label = new QGraphicsTextItem(labelString, this);
  label->setFont(QFont("Arial", 20));
  addToGroup(m_label);
  label->setZValue(s_labelZValue);
  qreal scaleRatio = s_shapeAspectRatio * boundingRect().height() / m_label->boundingRect().height();
  m_label->scale(scaleRatio, scaleRatio);
  m_label->translate(-m_label->boundingRect().width() / 2, -m_label->boundingRect().height() / 2);
  return m_label;
}

void Field::removeLabel() {
  removeItem(m_label);
}

void Field::addBGMark(const QColor& c) {
  m_background->setBrush(c);
  bgMark.reset(new QColor(c));
  update();
}

void Field::removeBGMark() {
  m_background->setBrush(Qt::NoBrush);
  bgMark.reset();
}


