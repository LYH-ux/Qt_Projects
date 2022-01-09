#include "mychart.h"
#include <QSplineSeries>
#include <QString>
#include <QStringList>
#include <QValueAxis>
#include <QLegendMarker>
#include <QDebug>
#include <QList>
#include <QtMath>
#include <QGraphicsSimpleTextItem>
#include <QtWidgets/QGesture>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>


MyChart::MyChart(QGraphicsItem *parent, Qt::WindowFlags wFlags):QChart(QChart::ChartTypeCartesian, parent, wFlags)
{

    axisX = new QValueAxis;
    m_series = new QSplineSeries(this);

    axisX->setRange(0, 1000);
    axisX->setLabelFormat("%d");
    axisX->setGridLineVisible(true);
    axisX->setTickCount(6);
    axisX->setTitleText("X");

    axisY = new QValueAxis;
    axisY->setRange(0,1000);
    axisY->setLabelFormat("%d");
    axisY->setGridLineVisible(true);
    axisY->setTickCount(11);
    axisY->setMinorTickCount(5);
    axisY->setTitleText("Y");

    this->addAxis(axisX, Qt::AlignBottom);
    this->addAxis(axisY, Qt::AlignLeft);

    m_series->setPen(QPen(Qt::red, 1, Qt::SolidLine));
    m_series->append(0, 0);
    m_series->append(2,1);


    this->legend()->setVisible(true);
    this->setAcceptHoverEvents(true);

    addDataSeries(m_series);

    // Seems that QGraphicsView (QChartView) does not grab gestures.
    // They can only be grabbed here in the QGraphicsWidget (QChart).
    grabGesture(Qt::PanGesture);
    grabGesture(Qt::PinchGesture);
    setTheme(QChart::ChartThemeDark);

}

MyChart::~MyChart()
{
    delete m_series;
    delete axisX;
    delete axisY;
}

void MyChart::addDataSeries(QList<QPointF> &data)
{

        QSplineSeries *series = new QSplineSeries();
        m_serieslist.append(series);
        series->setName(QString("line " + QString::number(m_serieslist.count())));
        series->append(data);
        series ->attachAxis(axisX);
        series ->attachAxis(axisY);
        addSeries(series);
        connectMarkers();

}

void MyChart::addDataSeries(QAbstractSeries *series)
{
    m_serieslist.append(series);
    series->setName(QString("line " + QString::number(m_serieslist.count())));
    addSeries(series);
    series ->attachAxis(axisX);  //must attach with axis
    series ->attachAxis(axisY);
    connectMarkers();

}

void MyChart::removeDataSeries()
{
    // Remove last series from chart
    if (m_serieslist.count() > 0) {
        QAbstractSeries *series = m_serieslist.last();
        removeSeries(series);
        m_serieslist.removeLast();
        delete series;
    }
}

void MyChart::connectMarkers()
{
    // Connect all markers to handler
        foreach(QLegendMarker* marker, this->legend()->markers()) {
            // Disconnect possible existing connection to avoid multiple connections
            QObject::disconnect(marker, &QLegendMarker::clicked, this, &MyChart::handleMarkerClicked);
            QObject::connect(marker, &QLegendMarker::clicked, this, &MyChart::handleMarkerClicked);
        }

}

void MyChart::disconnectMarkers()
{
    foreach(QLegendMarker* marker, this->legend()->markers()) {
            QObject::disconnect(marker, &QLegendMarker::clicked, this, &MyChart::handleMarkerClicked);
        }
}

void MyChart::handleMarkerClicked()
{

    QLegendMarker* marker = qobject_cast<QLegendMarker*> (sender());
        Q_ASSERT(marker);
        switch (marker->type()) {
        case QLegendMarker::LegendMarkerTypeXY:
        {
            // Toggle visibility of series
            marker->series()->setVisible(!marker->series()->isVisible());
            // Turn legend marker back to visible, since hiding series also hides the marker
            // and we don't want it to happen now.
            marker->setVisible(true);   
            // Dim the marker, if series is not visible
            qreal alpha = 1.0;

            if (!marker->series()->isVisible()) {
                alpha = 0.5;
            }

            QColor color;
            QBrush brush = marker->labelBrush();
            color = brush.color();
            color.setAlphaF(alpha);
            brush.setColor(color);
            marker->setLabelBrush(brush);

            brush = marker->brush();
            color = brush.color();
            color.setAlphaF(alpha);
            brush.setColor(color);
            marker->setBrush(brush);

            QPen pen = marker->pen();
            color = pen.color();
            color.setAlphaF(alpha);
            pen.setColor(color);
            marker->setPen(pen);
            break;
        }
        default:
        {
            qDebug() << "Unknown marker type";
            break;
        }
        }
}

void MyChart::updateSeries(QPointF pointData)
{

     if(pointData.x()>axisX->max()){
         axisX->setMax(axisX->max()*1.25);
     }
     m_series->append(pointData);
}

bool MyChart::sceneEvent(QEvent *event)
{
    if (event->type() == QEvent::Gesture){
        return gestureEvent(static_cast<QGestureEvent *>(event));
    }
    return QChart::event(event);
}

bool MyChart::gestureEvent(QGestureEvent *event)
{
    if (QGesture *gesture = event->gesture(Qt::PanGesture)){
        QPanGesture *pan = static_cast<QPanGesture *>(gesture);
        QChart::scroll(-(pan->delta().x()), pan->delta().y());
    }

    if (QGesture *gesture = event->gesture(Qt::PinchGesture)){
        QPinchGesture *pinch = static_cast<QPinchGesture *>(gesture);
        if (pinch->changeFlags() & QPinchGesture::ScaleFactorChanged)
            QChart::zoom(pinch->scaleFactor());
    }
    return true;
}
