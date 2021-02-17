#include "Spline.h"
#include <QLinkedList>

Spline::Spline()
{
}

void Spline::add(const Spline::dot &dot)
{
    dots.push_back(dot);
    dirty = true;
}

void Spline::insert(int i, const Spline::dot &dot)
{
    dots.insert(i, dot);
    dirty = true;
}

void Spline::replace(int i, const Spline::dot &dot)
{
    if(i >= dots.size()){
        return;}
    dots[i] = dot;
    dirty = true;
}

void Spline::remove(int i)
{
    dots.erase(dots.begin() + i);
    dirty = true;
}

int Spline::size() const
{
    return dots.size();
}

const Spline::dot &Spline::getDot(int i) const
{
    return dots[i];
}

const QVector<Spline::dot> &Spline::getDots() const
{
    return dots;
}

int Spline::findClosest(QPointF point, qreal & distance) const
{
    if(dots.isEmpty())
    {
        return -1;
    }

    int closestIndex = 0;

    QPointF diff = point - dots[0];
    distance = qAbs(diff.x())+qAbs(diff.y());

    for(int i = 1; i < dots.size(); ++i)
    {
        const dot & dot = dots.at(i);
        diff = point - dot;
        qreal newDistance = qAbs(diff.x())+qAbs(diff.y());
        if(newDistance < distance)
        {
            distance = newDistance;
            closestIndex = i;
        }
    }

    return closestIndex;
}

QVector<QPointF> Spline::getCurve()
{
    if(dirty)
    {
        interpolate(curve, dots);
        dirty = false;
    }

    return curve;
}

void Spline::interpolate(QVector<QPointF> &curve, const QVector<Spline::dot> &dots)
{
    curve.clear();

    if(dots.size() < 2)
    {
        return;
    }

    const int subdivs = 30;
    const qreal uStep = 1.0 / subdivs;

    curve.reserve((dots.size()-1)*subdivs + 1);

    for(int k = 0; k < dots.size() - 1; ++k)
    {
        const dot& prev = k == 0 ? dots[k] : dots[k-1];
        const dot& cur = dots[k];
        const dot& next = dots[k+1];
        const dot& next2 = k + 2 >= dots.size() ? dots[k+1] : dots[k+2];

        float tension = cur.tension;
        float bias = cur.bias;
        float continuity = cur.continuity;
        QPointF d0 = 0.5*(1-tension)*((1+bias)*(1-continuity)*(cur - prev) + (1-bias)*(1+continuity)*(next - cur));
        tension = next.tension;
        bias = next.bias;
        continuity = next.continuity;
        QPointF d1 = 0.5*(1-tension)*((1+bias)*(1+continuity)*(next - cur) + (1-bias)*(1-continuity)*(next2 - next));

        qreal u = 0.0;
        for(int subdiv = 0; subdiv < subdivs; ++subdiv)
        {
            qreal u2 = u*u;
            qreal u3 = u*u*u;
            curve.push_back((2*u3 - 3*u2 + 1) * cur
                    + (-2*u3 + 3*u2) * next
                    + (u3 - 2*u2 + u) * d0
                    + (u3 - u2) * d1);

            u += uStep;
        }
    }

    curve.push_back(dots.last());
}
