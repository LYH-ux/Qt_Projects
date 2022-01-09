#include "mychartview.h"
#include <QtGui/QResizeEvent>
#include <QtWidgets/QGraphicsScene>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <QtWidgets/QGraphicsTextItem>
#include <QtGui/QMouseEvent>
#include <mychart.h>
#include <QPalette>
#include "callout.h"

MyChartView::MyChartView(QWidget *parent)
    : QGraphicsView(new QGraphicsScene, parent),
      m_coordX(0),
      m_coordY(0),
      customChart(0),
      m_isTouching(false)
{
    //setDragMode(QGraphicsView::NoDrag);
    setDragMode(QGraphicsView::RubberBandDrag);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setRenderHint(QPainter::Antialiasing);
    setMouseTracking(true);
    setRubberBandSelectionMode(Qt::IntersectsItemShape);

    customChart = new MyChart;
    customChart->setAnimationOptions(QChart::AllAnimations);
    customChart->setMinimumWidth(720);
    customChart->setMaximumHeight(520);
    customChart -> setAcceptHoverEvents(true);

    scene() -> addItem(customChart);
    m_coordX = new QGraphicsSimpleTextItem(customChart);
    m_coordX->setPos(customChart->size().width()/2 - 50, customChart->size().height()-20);
    m_coordX->setText("X: ");
    m_coordY = new QGraphicsSimpleTextItem(customChart);
    m_coordY->setPos(customChart->size().width()/2 + 50, customChart->size().height()-20);
    m_coordY->setText("Y: ");

    //connect(customChart->m_series, &QSplineSeries::clicked,this, &MyChartView::keepCallout);
    connect(customChart->m_series, &QSplineSeries::hovered, this, &MyChartView::tooltip);

    this->setMouseTracking(true);
}

void MyChartView::updateChartSeries(QPointF chartSeriesData)
{
    customChart -> updateSeries(chartSeriesData);
}

void MyChartView::resizeEvent(QResizeEvent *event)
{
    if (scene())
    {
         scene()->setSceneRect(QRect(QPoint(0, 0), event->size()));
         customChart->resize(event->size());
         m_coordX->setPos(customChart->size().width()/2 - 50, customChart->size().height());
         m_coordY->setPos(customChart->size().width()/2 + 50, customChart->size().height());
         const auto callouts = m_callouts;
         for (Callout *callout : callouts)
             callout->updateGeometry();
    }
    QGraphicsView::resizeEvent(event);
}

void MyChartView::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isTouching)
        return;
    m_coordX->setText(QString("X: %1").arg(customChart->mapToValue(event->pos()).x()));
    m_coordY->setText(QString("Y: %1").arg(customChart->mapToValue(event->pos()).y()));
    QGraphicsView::mouseMoveEvent(event);
}

void MyChartView::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_isTouching)
        m_isTouching = false;

    // Because we disabled animations when touch event was detected
    // we must put them back on.
    customChart->setAnimationOptions(QChart::SeriesAnimations);
    QGraphicsView::mouseReleaseEvent(event);
}


bool MyChartView::viewportEvent(QEvent *event)
{
    if (event->type() == QEvent::TouchBegin) {
        // By default touch events are converted to mouse events. So
        // after this event we will get a mouse event also but we want
        // to handle touch events as gestures only. So we need this safeguard
        // to block mouse events that are actually generated from touch.
        m_isTouching = true;

        // Turn off animations when handling gestures they
        // will only slow us down.
        customChart->setAnimationOptions(QChart::NoAnimation);
    }
    return QGraphicsView::viewportEvent(event);
}

void MyChartView::mousePressEvent(QMouseEvent *event)
{
    if (m_isTouching)
        return;
    QGraphicsView::mousePressEvent(event);
}



void MyChartView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Plus:
        customChart->zoomIn();
        break;
    case Qt::Key_Minus:
        customChart->zoomOut();
        break;
    case Qt::Key_Left:
        customChart->scroll(-10, 0);
        break;
    case Qt::Key_Right:
        customChart->scroll(10, 0);
        break;
    case Qt::Key_Up:
        customChart->scroll(0, 10);
        break;
    case Qt::Key_Down:
        customChart->scroll(0, -10);
        break;
    default:
        QGraphicsView::keyPressEvent(event);
        break;
    }
}


void MyChartView::keepCallout()
{
    m_callouts.append(m_tooltip);
    m_tooltip = new Callout(customChart);
}

void MyChartView::tooltip(QPointF point, bool state)
{
    if (m_tooltip == 0)
        m_tooltip = new Callout(customChart);

    if (state) {
        m_tooltip->setText(QString("X: %1 \nY: %2 ").arg(point.x()).arg(point.y()));
        m_tooltip->setAnchor(point);
        m_tooltip->setZValue(11);
        m_tooltip->updateGeometry();
        m_tooltip->show();
    } else {
        m_tooltip->hide();
    }
}
