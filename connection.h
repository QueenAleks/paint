#ifndef CONNECTION_H
#define CONNECTION_H

#include "figures.h"

class Connection
{
    Figure* first;
    Figure* second;
public:
    Connection(Figure* _f, Figure* _s) : first(_f), second(_s){}
    bool containsFigure(Figure*) const;
    void draw(QPainter& painter) const;
    Figure* getFirst() const {return first;}
    Figure* getSecond() const {return second;}
};

#endif // CONNECTION_H
