#include "canvas.h"
#include <QTimer>
#include <QtMath>

Canvas::Canvas() {
    this->drawSquare = false;
    this->setXLength(600);
    this->setYLength(600);
    this->setMsToRedraw(15);
    this->setSandSideLength(30);
    //this->setFallSpeed(5);

    //Initializing the screen matrix.
    this->initializeScreenMatrix();

    //Initializing the available drawing points.
    this->initAvailableDrawingPoints();

    //Set up brush color.
    this->brushColor = new QColor(Qt::gray);

    //Initialize the QPoint object that will keep track of the last position of our mouse.
    this->lastMousePos = new QPoint();

    //Set up timer.
    this->timer = new QTimer(this);
    connect(this->timer, &QTimer::timeout, this, &Canvas::computeScreenMatrix);
    this->timer->start(this->getMsToRedraw());


    this->initialized = true;

    //Test below to watch a sand fall.
    //this->drawSquare = true;
    //this->drawSand(300,100);
    srand(time(0));
}

Canvas::~Canvas(){
    delete[] this->availableDrawingPoints;

    for(int i = 0; i< this->getXLength(); i++){
        delete[] this->screenMatrix[i];
    }
    delete[] this->screenMatrix;
    delete this->timer;
    delete this->brushColor;
    delete this->lastMousePos;

}

void Canvas::initializeScreenMatrix(){
    this->screenMatrix = new bool*[this->getXLength()];
    for(int i = 0; i< this->getXLength();i++){
        this->screenMatrix[i] = new bool[this->getYLength()];
    }

    for(int i = 0; i < this->getXLength();i++){
        for (int j = 0; j< this->getYLength(); j++){
                  this->screenMatrix[i][j] = false;
            }
    }

}


void Canvas::mousePressEvent(QMouseEvent *event){
    this->drawSquare = true;
    QPoint clickPosition = event->pos();
    drawSand(clickPosition.x(), clickPosition.y());

    this->lastMousePos->setX(clickPosition.x());
    this->lastMousePos->setY(clickPosition.y());

}

void Canvas::mouseMoveEvent(QMouseEvent *event){
    if(!this->withinBoundaries(event->pos().x(),event->pos().y())) return;

    if(this->drawSquare == true){
        QPoint clickPosition = event->pos();
        drawSand(clickPosition.x(), clickPosition.y());

        this->lastMousePos->setX(clickPosition.x());
        this->lastMousePos->setY(clickPosition.y());
    }
}

void Canvas::mouseReleaseEvent(QMouseEvent *event){
    Q_UNUSED(event);
    this->drawSquare = false;
}


void Canvas::clamp(int* x, int *y){
    int xLen = this->getXLength();
    int yLen = this->getYLength();
    if((*x) >= xLen){
        (*x) = xLen-1;
    }
    else if(*x <= 0){
        (*x) = 1;
    }

    if ((*y) >= yLen){
        (*y) = yLen-1;
    }
    else if((*y) <= 0){
        (*y) = 1;
    }
}

void Canvas::drawSand(int x, int y){
    if(this->drawSquare && !this->isOccupied(x,y)){
        this->clamp(&x,&y);
        this->findClosestDrawingPoint(&x,&y);
        this->screenMatrix[x][y] = true;
        //qInfo() << "Drawing to x: " <<x << ", y: " << y << "--  xLength: " << this->getXLength() << ", yLength: " <<this->getYLength();
    }
    //update();
}

void Canvas::computeScreenMatrix(){
    //qInfo() <<"Called computeScreenMatrix";
    for(int x = 0; x< this->getXLength();x++){
        for(int y = 0; y< this->getYLength();y++){
            if(this->isOccupied(x,y)){
                if(this->touchesGround(x,y)){
                    //TODO Check if it needs to go sides.
                    //qInfo() <<"Touches ground.";
                    continue;
                }
                else{
                    //qInfo() <<"Does not touch ground. x: " << x << ", y: " <<y;
                    if(y < this->getYLength()-2){
                        this->screenMatrix[x][y] = false;
                        this->screenMatrix[x][y+this->getSandSideLength()/2] = true;
                        y+= this->getSandSideLength()+1;
                    }
                }
            }
        }
    }
    update();

}

void Canvas::paintEvent(QPaintEvent *event){
    Q_UNUSED(event);
    //qInfo()<< "Paint event called";

    //TODO. Fix below. It does not keep creating sand as I hold mouse button in a specific position.
    if(this->drawSquare){
            int* x = new int;
            int* y = new int;

            *x = this->lastMousePos->x();
            *y = this->lastMousePos->y();
            this->findClosestDrawingPoint(x,y);

            this->clamp(x,y);
            this->findClosestDrawingPoint(x,y);
            this->screenMatrix[*x][*y] = true;

            delete x;
            delete y;
    }

    QPainter painter(this);

    for(int y = 0; y<this->getYLength();y++){
        for(int x = 0; x< this->getXLength();x++){
            if(this->isOccupied(x,y) == true){
                 painter.setBrush(QColor(this->brushColor->red(), this->brushColor->green(), this->brushColor->blue(),this->brushColor->alpha()));
                 painter.setPen(QColor(this->brushColor->red(), this->brushColor->green(), this->brushColor->blue(),this->brushColor->alpha()));
                 //qInfo()<<"Painting blue at " << x << ", " << y;
                 painter.drawRect(x,y, this->sandSideLength, this->sandSideLength);

                 //TODO below. Store every sand and their colors in an array.
                 //delete this->brushColor;
                 //this->brushColor = new QColor(rand()%255, rand()%255, rand()%255, rand()%255);
            }

        }
    }
}

bool Canvas::withinBoundaries(int x, int y){
    if((x > 0 && x < this->getXLength()-1) && (y > 0 && y < this->getYLength()-1)){
        return true;
    }
    return false;
}

bool Canvas::touchesGround(int x, int y){
    if(y == this->getYLength()-2)//On the bottom of the screen.
    {
        return true;
    }
    if(this->isOccupied(x,y+this->getSandSideLength())){ //If it does not have a sand one cell below.
        return true;
    }
    else{   //If it has a sand one cell below.
        return false;
    }


}

void Canvas::initAvailableDrawingPoints(){
    if(this->getSandSideLength() == 1) return; //Every pixel is a sand piece, further computation is not needed.

    int sandSideLength = this->getSandSideLength();
    this->sizeOfAvailableDrawingPoints = (this->getXLength() * this->getYLength()) / (sandSideLength * sandSideLength);
    this->availableDrawingPoints = new QPoint[this->sizeOfAvailableDrawingPoints];

    int currentIndex = 0;
    for(int x = 0; x < this->getXLength(); x+= sandSideLength){
        for(int y = 0; y < this->getYLength(); y+= sandSideLength){
            QPoint* point = new QPoint(x,y);
            this->availableDrawingPoints[currentIndex] = *point;
            currentIndex++;
            delete point;
        }
    }
    //Printing to test below.
    //for(int i =0; i<pointCount;i++){
    //    qInfo() << i << ", point: " << this->availableDrawingPoints[i];
    //}
}


float Canvas::euclideanDist(int x1,int x2, int y1, int y2){
    return qSqrt(qPow((x1 - x2), 2) + qPow((y1 - y2), 2));
}

void Canvas::findClosestDrawingPoint(int* x, int* y){
    if(this->getSandSideLength() == 1) return; //No need for further computation.

    QPoint closest;
    float closestDist = this->euclideanDist(0, this->getXLength(), 0, this->getYLength()); //Initialized as the longest distance possible.
    for(int i = 0; i < this->sizeOfAvailableDrawingPoints; i++){
        QPoint curPoint = this->availableDrawingPoints[i];
        float dist = this->euclideanDist(*x, curPoint.x(), *y, curPoint.y());
        if(dist < closestDist){
            closestDist = dist;
            closest = curPoint;
        }
    }
    *x = closest.x();
    *y = closest.y();
}

bool Canvas::isOccupied(int x, int y){
    return this->screenMatrix[x][y];
}

void Canvas::setXLength(int newXLength){
    if(this->initialized) return;
    this->xLength = newXLength;
}

int Canvas::getXLength(){
    return this->xLength;
}

void Canvas::setYLength(int newYLength){
    if(this->initialized) return;
    this->yLength = newYLength;

}
int Canvas::getYLength(){
    return this->yLength;
}

void Canvas::setMsToRedraw(int ms){
    this->msToRedraw = ms;
}

int Canvas::getMsToRedraw(){
    return this->msToRedraw;
}

void Canvas::setSandSideLength(int length){
    this->sandSideLength = length;
}

int Canvas::getSandSideLength(){
    return this->sandSideLength;
}

// void Canvas::setFallSpeed(int speed){
//     this->fallSpeed = speed;
// }

// int Canvas::getFallSpeed(){
//     return this->fallSpeed;
// }




