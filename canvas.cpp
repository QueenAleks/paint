#include "canvas.h"

Canvas::Canvas(QWidget* parent) : QWidget(parent){
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::white);
    setPalette(pal);
    setAutoFillBackground(true);
}

void Canvas::setMode(Modes mode){
    abortCurrentAction();
    currentMode = mode;
}

void Canvas::save(const QString& filename){
    abortCurrentAction();
    QJsonObject buff;

    QJsonArray figuresArray;
    for (int i = 0; i < figuresBuff.size(); i++) {
        figuresArray.append(figuresBuff[i]->toJson(i));
    }
    buff["figures"] = figuresArray;

    QJsonArray connectionsArray;
    for (auto c : connectionsBuff) {
        QJsonObject obj;
        obj["first"] = figuresBuff.indexOf(c->getFirst());
        obj["second"] = figuresBuff.indexOf(c->getSecond());
        connectionsArray.append(obj);
    }
    buff["connections"] = connectionsArray;

    QJsonDocument doc(buff);
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
    }
}

void Canvas::load(const QString& filename){
    abortCurrentAction();
    qDeleteAll(figuresBuff);
    qDeleteAll(connectionsBuff);
    figuresBuff.clear();
    connectionsBuff.clear();

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) throw 0;

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) return;

    QJsonObject obj = doc.object();

    QMap<int, Figure*> figuresWithIDs;
    QJsonArray figuresArray = obj["figures"].toArray();
    for (auto v : figuresArray) {
        QJsonObject ob = v.toObject();

        FigureClasses f_class = static_cast<FigureClasses>(ob["class"].toInt());
        int id = ob["id"].toInt(),
            lu_x = ob["lu_x"].toInt(),
            lu_y = ob["lu_y"].toInt(),
            rd_x = ob["rd_x"].toInt(),
            rd_y = ob["rd_y"].toInt();

        QPoint lu = QPoint(lu_x, lu_y),
               rd = QPoint(rd_x, rd_y);
        Figure* f = FigureFactory::createFigure(f_class, lu, rd);
        if (f){
            figuresBuff.append(f);
            figuresWithIDs[id] = f;
        }
    }

    QJsonArray connectionsArray = obj["connections"].toArray();
    for (auto v : connectionsArray) {
        QJsonObject ob = v.toObject();
        Figure* first = figuresWithIDs[ob["first"].toInt()];
        Figure* second = figuresWithIDs[ob["second"].toInt()];
        connectionsBuff.append(new Connection(first, second));
    }

    update();
}

void Canvas::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::RightButton) {
        abortCurrentAction();
        return;
    }

    if (event->button() != Qt::LeftButton) return;

    QPoint pos = event->pos();

    switch (currentMode) {
    case DRAW_RECTANGLE:
    case DRAW_ELLIPSE:
    case DRAW_TRIANGLE:
        startPoint = pos;
        tempPoint = pos;
        drawing = true;
        break;
    case CONNECT:
        if (Figure* f = checkCollisions(pos)){
            if(!firstConnection){
                tempPoint = pos;
                firstConnection = f;
            } else if (firstConnection != f){
                addConnection(firstConnection, f);
                firstConnection = nullptr;
            } else {
                firstConnection = nullptr;
            }
        } else {
            firstConnection = nullptr;
        }
        update();
        break;
    case MOVE:
        if (Figure* f = checkCollisions(pos)) {
            movingFigure = f;
            startPoint = pos;
            setCursor(Qt::ClosedHandCursor);
        }
        break;
    case ERASE:
        if (Figure* f = checkCollisions(pos)){
            eraseFigure(f);
        }
        update();
        break;
    default:
        break;
    }
}

void Canvas::mouseMoveEvent(QMouseEvent* event){
    QPoint pos = event->pos();

    switch (currentMode) {
    case DRAW_ELLIPSE:
    case DRAW_TRIANGLE:
    case DRAW_RECTANGLE:
        if (drawing){
            tempPoint = pos;
            update();
        }
        break;
    case CONNECT:
        if (firstConnection){
            tempPoint = pos;
            update();
        }
        break;
    case MOVE:
        if (movingFigure && (event->buttons() & Qt::LeftButton)) {
            int d_x = pos.x() - startPoint.x(),
                d_y = pos.y() - startPoint.y();
            movingFigure->move(d_x, d_y);
            startPoint = pos;
            update();
        }
        break;
    default:
        break;
    }
}

void Canvas::mouseReleaseEvent(QMouseEvent* event){
    if (event->button() != Qt::LeftButton) return;

    if (drawing) {
        switch (currentMode) {
        case DRAW_RECTANGLE:
            figuresBuff.append(new Rectangle(startPoint, tempPoint));
            break;
        case DRAW_ELLIPSE:
            figuresBuff.append(new Ellipse(startPoint, tempPoint));
            break;
        case DRAW_TRIANGLE:
            figuresBuff.append(new Triangle(startPoint, tempPoint));
            break;
        default:
            break;
        }
        drawing = false;
        update();
    }

    if (movingFigure && currentMode == MOVE) {
        movingFigure = nullptr;
        setCursor(Qt::ArrowCursor);
    }
}

void Canvas::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Escape) {
        abortCurrentAction();
    }
}

void Canvas::paintEvent(QPaintEvent*) {
    QPainter painter(this);

    for (auto f : figuresBuff){
        f->draw(painter);
    }

    for (auto c : connectionsBuff) {
        c->draw(painter);
    }

    if (drawing){
        switch (currentMode) {
        case DRAW_RECTANGLE:
            painter.drawRect(QRect(startPoint, tempPoint));
            break;
        case DRAW_ELLIPSE:
            painter.drawEllipse(QRect(startPoint, tempPoint));
            break;
        case DRAW_TRIANGLE:
            Triangle(startPoint, tempPoint).draw(painter);
            break;
        default:
            break;
        }
    }

    if (firstConnection) {
        painter.drawLine(firstConnection->getCenter(), tempPoint);
    }
}

void Canvas::abortCurrentAction(){
    drawing = false;
    firstConnection = nullptr;
    movingFigure = nullptr;
    setCursor(Qt::ArrowCursor);
    update();
}

Figure* Canvas::checkCollisions(QPoint& point){
    for (int i = figuresBuff.size()-1; i >= 0; --i){
        if (figuresBuff[i]->checkCollision(point))
            return figuresBuff[i];
    }
    return nullptr;
}

void Canvas::addConnection(Figure* f, Figure* s){
    for (auto c : connectionsBuff){
        if (c->containsFigure(f) && c->containsFigure(s)){
            return;
        }
    }

    connectionsBuff.append(new Connection(f, s));
}

void Canvas::eraseFigure(Figure* f){
    for (int i = connectionsBuff.size()-1; i >= 0; --i){
        if (connectionsBuff[i]->containsFigure(f)){
            delete connectionsBuff.takeAt(i);
        }
    }
    delete figuresBuff.takeAt(figuresBuff.indexOf(f));
}

Canvas::~Canvas(){
    qDeleteAll(figuresBuff);
    qDeleteAll(connectionsBuff);
}
