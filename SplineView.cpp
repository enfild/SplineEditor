#include "SplineView.h"

#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QDebug>
#include <QLabel>

SplineView::SplineView(QWidget *parent) :
    QWidget(parent)
{
    setMouseTracking(true);
}

void SplineView::undo()
{
    history.undo(spline);
    hotIndex = -1;
    deselect();
    update();
}

void SplineView::redo()
{
    history.redo(spline);
    hotIndex = -1;
    deselect();
    update();
}

const Spline & SplineView::getSpline() const
{
    return spline;
}

void SplineView::setSpline(const Spline &spline)
{
    this->spline = spline;
    history.clear();
    hotIndex = -1;
    deselect();
    update();
}

void SplineView::updateSelectedDot(Spline::dot dot)
{
    if(selectedIndex != -1)
    {
        history.push(spline);
        spline.replace(selectedIndex, dot);
        update();
    }
}

void SplineView::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.fillRect(rect(), QColor(32, 32, 32));

    painter.setPen(QColor(128, 128, 128, 64));
    painter.drawLine(rect().center().x(), 0, rect().center().x(), height());
    painter.drawLine(0, rect().center().y(), width(), rect().center().y());

    painter.translate(rect().center() + QPoint(horShift, verShift));
    painter.scale(zoomIndex, zoomIndex);
    painter.setRenderHint(QPainter::Antialiasing);

    QPen pen(QColor(182, 219, 73));
    pen.setWidth(2);
    painter.setPen(pen);
    painter.drawPolyline(spline.getCurve());

    painter.setBrush(QColor(182, 219, 73));
    for(const Spline::dot & dot : spline.getDots())
    {
        painter.drawEllipse(dot, 3, 3);
    }

    if(hotIndex != -1)
    {
        painter.setBrush(Qt::white);
        pen.setWidth(1);
        painter.setPen(pen);
        painter.drawEllipse(spline.getDot(hotIndex), 4, 4);
    }

    if(selectedIndex != -1)
    {
        painter.setBrush(Qt::white);
        pen.setWidth(1);
        painter.setPen(pen);
        painter.drawEllipse(spline.getDot(selectedIndex), 6, 6);
    }
    if(insertIndex != -1)
    {
        pen.setWidth(1);
        pen.setStyle(Qt::DashLine);
        painter.setBrush(Qt::transparent);
        painter.setPen(pen);

        if(insertIndex < spline.size())
        {
            Spline::Knot knot = spline.getKnot(insertIndex);
            painter.drawLine(knot, insertPos);

            if(insertIndex - 1 >= 0)
            {
                knot = spline.getKnot(insertIndex - 1);
                painter.drawLine(knot, insertPos);
            }
        }

        painter.drawEllipse(insertPos, 6, 6);
    }
}

void SplineView::mouseMoveEvent(QMouseEvent *event)
{
    setFocus();
    QPoint pos = (mapFromGlobal(QCursor::pos()) - QPoint(horShift, verShift) - rect().center()) / zoomIndex;
    if(movingDot && hotIndex != -1)
    {
        Spline::dot dot = spline.getDot(hotIndex);
        dot.setX(pos.x());
        dot.setY(pos.y());
        spline.replace(hotIndex, dot);
        emit dotSelected(dot);
    }
    else
    {
        qreal distance;
        hotIndex = spline.findClosest(pos, distance);
        if(distance > HOT_DOT_RADIUS)
        {
            hotIndex = -1;
        }
    }

    if(event->buttons() & Qt::LeftButton && hotIndex != -1)
    {
        if(!movingDot)
        {
            history.push(spline);
            movingDot = true;
        }
    }

    if(insertIndex != -1)
    {
        updateInsertState();
    }

    update();
}

void SplineView::wheelEvent(QWheelEvent *event)
{
    if (event->orientation() == Qt::Vertical) {
        const int angle = event->angleDelta().y();
        if(zoomIndex > 1 || angle > 0)
        {
            zoomIndex = zoomIndex + (angle / QWheelEvent::DefaultDeltasPerStep);
        }
        update();
    }
}


void SplineView::mousePressEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton && insertIndex != -1)
    {
        history.push(spline);
        spline.insert(insertIndex, Spline::dot(insertPos));
        deselect();
        update();
    }
    else
        if(selectedIndex != -1 && hotIndex == -1)
        {
            deselect();
        }
        else
            if(event->buttons() & Qt::RightButton && hotIndex != -1)
            {
                history.push(spline);
                spline.remove(hotIndex);
                hotIndex = -1;
                update();
            }
            else
                if(event->buttons() & Qt::LeftButton && hotIndex != -1)
                {
                    selectedIndex = hotIndex;
                    emit dotSelected(spline.getDot(selectedIndex));
                    update();
                }
}

void SplineView::mouseReleaseEvent(QMouseEvent *)
{
    movingDot = false;
}

void SplineView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Left:
        horShift = horShift - 10;
        update();
        break;
    case Qt::Key_Right:
        horShift = horShift + 10;
        update();
        break;
    case Qt::Key_Down:
        verShift = verShift - 10;
        update();
        break;
    case Qt::Key_Up:
        verShift = verShift + 10;
        update();
        break;
    case Qt::Key_Home:
        horShift = 0;
        verShift = 0;
        update();
        break;
    default:
        break;
    }

    if(event->key() & Qt::Key_Delete && hotIndex != -1)
    {
        history.push(spline);
        spline.remove(hotIndex);
        hotIndex = -1;
        update();
    }

    if(event->key() == Qt::Key_Escape)
    {
        deselect();
    }
    else
        if(event->key() == Qt::Key_Shift)
        {
            updateInsertState();
            update();
        }
}

void SplineView::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Shift)
    {
        insertIndex = -1;
        update();
    }
}

void SplineView::deselect()
{
    if(selectedIndex == -1)
    {
        return;
    }
    selectedIndex = -1;
    emit dotDeselected();
    update();
}

void SplineView::updateInsertState()
{
    insertPos = (mapFromGlobal(QCursor::pos()) - QPoint(horShift, verShift) - rect().center()) / zoomIndex;
    if(spline.size() == 0)
    {
        insertIndex = 0;
    }
    else
    {
        qreal distance;
        insertIndex = spline.findClosest(insertPos, distance);
    }
}
