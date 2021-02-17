#ifndef SPLINEVIEW_H
#define SPLINEVIEW_H

#include "spline.h"
#include "history.h"
#include <qmath.h>
#include <QWidget>
#include <QColor>
#include <QMouseEvent>

// interface
class SplineView : public QWidget
{
    Q_OBJECT
public:
    explicit SplineView(QWidget *parent = 0);

public slots:
    void undo();
    void redo();

    const Spline& getSpline() const;
    void setSpline(const Spline& spline);

    void updateSelectedDot(Spline::dot dot);

signals:
    void dotSelected(Spline::dot dot);
    void dotDeselected();

protected:
    void paintEvent(QPaintEvent*) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

private:
    void deselect();
    void updateInsertState();

private:
    Spline spline;
    History history;

    bool movingDot = false;
    int hotIndex = -1;
    int selectedIndex = -1;
    int insertIndex = -1;
    double zoomIndex = 1;
    int horShift = 0;
    int verShift = 0;
    QPointF insertPos;

    const int HOT_DOT_RADIUS = 10;
};
#endif
