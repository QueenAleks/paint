#ifndef CANVAS_H
#define CANVAS_H

#include "figures.h"
#include "connection.h"

#include <QWidget>
#include <QPainter>
//#include <QPaintEvent>
#include <QMouseEvent>
#include <QJsonArray>
#include <QJsonDocument>

using std::vector;

enum Modes { NONE, DRAW_TRIANGLE, DRAW_ELLIPSE, DRAW_RECTANGLE, CONNECT, MOVE, ERASE };

class Canvas : public QWidget {
    Q_OBJECT

    QList<Figure*> figuresBuff = QList<Figure*>();
    QList<Connection*> connectionsBuff = QList<Connection*>();
    Modes currentMode = NONE;

    bool drawing = false;
    QPoint startPoint;
    QPoint tempPoint;

    Figure* firstConnection = nullptr;

    Figure* movingFigure = nullptr;
public:
    explicit Canvas(QWidget* parent = nullptr);
    ~Canvas() override;

    void setMode(Modes mode);
    void save(const QString& filename);
    void load(const QString& filename);
protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void paintEvent(QPaintEvent*) override;
private:
    void abortCurrentAction();
    Figure* checkCollisions(QPoint& point);
    void addConnection(Figure*, Figure*);
    void eraseFigure(Figure*);
};

#endif // CANVAS_H
