#include "figures.h"

QJsonObject Figure::toJson(int id) {
    QJsonObject obj;
    obj["id"] = id;
    obj["lu_x"] = leftup.x();
    obj["lu_y"] = leftup.y();
    obj["rd_x"] = rightdown.x();
    obj["rd_y"] = rightdown.y();
    obj["class"] = this->getClass();
    return obj;
}

void Figure::move(int d_x, int d_y){
    leftup.rx() += d_x;
    leftup.ry() += d_y;
    rightdown.rx() += d_x;
    rightdown.ry() += d_y;
    center.rx() += d_x;
    center.ry() += d_y;
};

Rectangle::Rectangle(const QPoint& _leftup, const QPoint& _rightdown) : Figure(_leftup, _rightdown), rect(QRect(_leftup, _rightdown)){
    center = QPoint((_leftup.x() + _rightdown.x())/2, (_leftup.y() + _rightdown.y())/2);
}

void Rectangle::draw(QPainter& painter){
    painter.drawRect(rect);
}

void Rectangle::move(int d_x, int d_y){
    Figure::move(d_x, d_y);
    rect.translate(d_x, d_y);
};

bool Rectangle::checkCollision(QPoint& p){
    return rect.contains(p);
}

Ellipse::Ellipse(const QPoint& _leftup, const QPoint& _rightdown) : Figure(_leftup, _rightdown), rect(QRect(_leftup, _rightdown)){
    center = QPoint((_leftup.x() + _rightdown.x())/2, (_leftup.y() + _rightdown.y())/2);
}

void Ellipse::draw(QPainter& painter){
    painter.drawEllipse(rect);
}

void Ellipse::move(int d_x, int d_y){
    Figure::move(d_x, d_y);
    rect.translate(d_x, d_y);
};

bool Ellipse::checkCollision(QPoint& p){
    if (!rect.contains(p)) return false;

    int a = rect.width() / 2, b = rect.height() / 2,
        x = p.x() - center.x(), y = p.y() - center.y();
    return (x*x)/(a*a) + (y*y)/(b*b) <= 1;
}

Triangle::Triangle(const QPoint& _leftup, const QPoint& _rightdown) : Figure(_leftup, _rightdown){
    int lu_x = _leftup.x(), lu_y = _leftup.y(),
        rd_x = _rightdown.x(), rd_y = _rightdown.y(),
        c_x = (lu_x + rd_x)/2,
        c_y = ( (rd_y - lu_y ) * 2 / 3) + lu_y;
    center = QPoint(c_x, c_y);
    polygon << _rightdown << QPoint(lu_x, rd_y) << QPoint(c_x, lu_y);
}

void Triangle::draw(QPainter& painter){
    painter.drawPolygon(polygon);
}

void Triangle::move(int d_x, int d_y){
    Figure::move(d_x, d_y);
    polygon.translate(d_x, d_y);
};

bool Triangle::checkCollision(QPoint& p){
    return polygon.containsPoint(p, Qt::OddEvenFill);
}

Figure* FigureFactory::createFigure(FigureClasses f_class, const QPoint& leftup, const QPoint& rightdown) {
    switch (f_class) {
    case RECTANGLE:
        return new Rectangle(leftup, rightdown);
    case TRIANGLE:
        return new Triangle(leftup, rightdown);
    case ELLIPSE:
        return new Ellipse(leftup, rightdown);
    default:
        return nullptr;
    }
}
