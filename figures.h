#ifndef FIGURES_H
#define FIGURES_H

#include <QPainter>
#include <QPoint>
#include <QJsonObject>

enum FigureClasses{ ELLIPSE, RECTANGLE, TRIANGLE };

class Figure{
protected:
    QPoint leftup;
    QPoint rightdown;
    QPoint center;
public:
    Figure(const QPoint& _leftup, const QPoint& _rightdown) : leftup(_leftup), rightdown(_rightdown){}
    QPoint getCenter() const { return center; }
    virtual FigureClasses getClass() = 0;
    virtual void draw(QPainter& painter) = 0;
    virtual void move(int d_x, int d_y);
    virtual bool checkCollision(QPoint&) = 0;
    QJsonObject toJson(int id);

    virtual ~Figure(){}
};

class Rectangle : public Figure{
    QRect rect;
public:
    FigureClasses getClass() override {return RECTANGLE;}
    Rectangle(const QPoint& _leftup, const QPoint&  _rightdown);
    void draw(QPainter& painter) override;
    void move(int d_x, int d_y) override;
    bool checkCollision(QPoint&) override;
};

class Ellipse : public Figure{
    QRect rect;
public:
    FigureClasses getClass() override {return ELLIPSE;}
    Ellipse(const QPoint& _leftup, const QPoint& _rightdown);
    void draw(QPainter& painter) override;
    void move(int d_x, int d_y) override;
    bool checkCollision(QPoint&) override;
};

class Triangle : public Figure{
    QPolygon polygon;
public:
    FigureClasses getClass() override {return TRIANGLE;}
    Triangle(const QPoint& _leftup, const QPoint&  _rightdown);
    void draw(QPainter& painter) override;
    void move(int d_x, int d_y) override;
    bool checkCollision(QPoint&) override;
};

class FigureFactory {
public:
    static Figure* createFigure(FigureClasses f_class, const QPoint& leftup, const QPoint& rightdown);
};

#endif
