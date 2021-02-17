#ifndef HISTORY_H
#define HISTORY_H

#include "spline.h"
#include <QStack>

class History
{
public:
    void push(const Spline &spline)
    {
        backStack.push(spline);
        forwardStack.clear();
    };
    void redo(Spline &spline)
    {
        if(forwardStack.size() > 0)
        {
            backStack.push(spline);
            spline = forwardStack.pop();
        }
    };
    void undo(Spline & spline)
    {
        if(backStack.size() > 0)
        {
            forwardStack.push(spline);
            spline = backStack.pop();
        }
    }
    void clear(void)
    {
        backStack.clear();
        forwardStack.clear();
    }

private:
    QStack<Spline> backStack;
    QStack<Spline> forwardStack;
};

#endif // HISTORY_H
