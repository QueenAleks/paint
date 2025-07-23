#include "connection.h"

bool Connection::containsFigure(Figure* f) const {
    return f == first || f == second;
}

void Connection::draw(QPainter& painter) const {
    painter.drawLine(first->getCenter(), second->getCenter());
}
