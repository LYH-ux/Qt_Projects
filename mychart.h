#ifndef MYCHART_H
#define MYCHART_H

#include<QObject>
#include<QtCharts/QChart>
#include<QGestureEvent>

QT_CHARTS_BEGIN_NAMESPACE
class QSplineSeries;
class QValueAxis;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE


class MyChart : public QChart
{
    Q_OBJECT
public:
    MyChart(QGraphicsItem *parent = 0, Qt::WindowFlags wFlags = 0);
    ~MyChart();

public:
    QSplineSeries *m_series;

public slots:
    void addDataSeries(QList<QPointF> &data);
    void addDataSeries(QAbstractSeries *series);
    void removeDataSeries();
    void connectMarkers();
    void disconnectMarkers();
    void handleMarkerClicked();
    void updateSeries(QPointF pointData);

protected:
    bool sceneEvent(QEvent *event);

private:
    bool gestureEvent(QGestureEvent *event);


private:

    QList<QAbstractSeries *> m_serieslist;
    QValueAxis *axisX;
    QValueAxis *axisY;
    QVector<QPointF> pointData;

};

#endif // MYCHART_H
