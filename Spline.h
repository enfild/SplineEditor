#ifndef SPLINE_H
#define SPLINE_H

#include <QPointF>
#include <QVector>

class Spline
{
public:
    struct dot : public QPointF
    {
        dot(QPointF pos) : QPointF(pos)
        {

        }

        dot(qreal x = 0.0f, qreal y = 0.0f) :QPointF(x, y)
        {

        }

        dot(qreal x, qreal y, float tension, float bias, float continuity) :
            QPointF(x, y),
            tension(tension),
            bias(bias),
            continuity(continuity)
        {

        }

        float tension = 0.0f;
        float bias = 0.0f;
        float continuity = 0.0f;
    };

    Spline();

    void add(const dot &dot);
    void insert(int i, const dot &dot);
    void replace(int i, const dot &dot);
    void remove(int i);

    int size() const;

    const dot & getDot(int index) const;
    const QVector<dot> & getDots() const;

    int findClosest(QPointF pos, qreal & distance) const;

    QVector<QPointF> getCurve();

    static void interpolate(QVector<QPointF> & curve, const QVector<dot> & dots);

private:
    bool dirty = false;
    QVector<dot> dots;
    QVector<QPointF> curve;

};
#endif
