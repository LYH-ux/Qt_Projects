#ifndef MYCHARTVIEW_H
#define MYCHARTVIEW_H

#include <QWidget>
#include <QGraphicsView>
#include <QtCharts/QChartGlobal>

QT_BEGIN_NAMESPACE
class QGraphicsScene;
class QMouseEvent;
class QResizeEvent;
QT_END_NAMESPACE

QT_CHARTS_BEGIN_NAMESPACE
class QChart;
QT_CHARTS_END_NAMESPACE

class MyChart;
class Callout;

QT_CHARTS_USE_NAMESPACE

class MyChartView : public QGraphicsView
{
    Q_OBJECT
public:
    MyChartView(QWidget *parent = 0);
    void updateChartSeries(QPointF chartSeriesData);
protected:
    void resizeEvent(QResizeEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    bool viewportEvent(QEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);

public slots:
    void keepCallout();
    void tooltip(QPointF point, bool state);

private:
    QGraphicsSimpleTextItem *m_coordX;
    QGraphicsSimpleTextItem *m_coordY;
    MyChart *customChart;
    Callout *m_tooltip;
    QList<Callout *> m_callouts;
    bool m_isTouching;

};

#endif // MYCHARTVIEW_H
