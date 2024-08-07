#include "canvas.h"
#include <QTimer>
#include <QtMath>
#include <QRandomGenerator>

Canvas::Canvas() {
    this->drawSquare = false;
    this->setXLength(600);
    this->setYLength(600);
    this->setMsToRedraw(15);
    this->setSandSideLength(6);
    //this->setFallSpeed(5);

    //Initializing the screen matrix.
    this->initializeScreenMatrix();

    //Initializing the available drawing points.
    this->initAvailableDrawingPoints();

    //Initializing color matrix.
    this->initializeColorMatrix();

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
}

Canvas::~Canvas(){
    delete[] this->availableDrawingPoints;

    for(int i = 0; i< this->getXLength(); i++){
        delete[] this->screenMatrix[i];
    }
    delete[] this->screenMatrix;

    for(int i = 0; i< this->getXLength(); i++){
        delete[] this->colorMatrix[i];
    }
    delete[] this->colorMatrix;

    delete this->timer;
    delete this->brushColor;
    delete this->lastMousePos;

}

void Canvas::initializeColorMatrix(){
    //Initializing a square matrix.
    this->colorMatrix = new QColor*[this->getXLength()];
    for(int i = 0; i<this->sizeOfAvailableDrawingPoints; i++){
        this->colorMatrix[i] = new QColor[this->getYLength()];
    }
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
        this->findClosestDrawingPoint(&x,&y);
        this->screenMatrix[x][y] = true;
        this->colorMatrix[x][y] = QColor(this->brushColor->red(),
                                                    this->brushColor->green(),
                                                    this->brushColor->blue(),
                                                    this->brushColor->alpha()
                                            );
    }
}

void Canvas::fallLeft(int x, int y){
    this->screenMatrix[x][y] = false;
    this->screenMatrix[x-this->getSandSideLength()][y+this->getSandSideLength()] = true;
    this->colorMatrix[x-this->getSandSideLength()][y+this->getSandSideLength()] = this->colorMatrix[x][y];
}

void Canvas::fallRight(int x, int y){
    this->screenMatrix[x][y] = false;
    this->screenMatrix[x+this->getSandSideLength()][y+this->getSandSideLength()] = true;
    this->colorMatrix[x+this->getSandSideLength()][y+this->getSandSideLength()] = this->colorMatrix[x][y];
}

void Canvas::fallDown(int x, int y){
    this->screenMatrix[x][y] = false;
    this->screenMatrix[x][y+this->getSandSideLength()] = true;
    this->colorMatrix[x][y+this->getSandSideLength()] = this->colorMatrix[x][y];
}

void Canvas::computeScreenMatrix(){
    //qInfo() <<"Called computeScreenMatrix";

    for(int x = 0; x< this->getXLength();x++){
        for(int y = 0; y< this->getYLength();y++){
            if(this->isOccupied(x,y)){
                //qInfo() << y;

                if(this->touchesGround(x,y)){
                    if(this->canFallRight(x,y) && this->canFallLeft(x,y)){
                        //If both sides are suitable, randomly decide which side to fall.
                        QRandomGenerator *generator = QRandomGenerator::global();
                        int randSide = generator->bounded(2); // 0 or 1
                        if(randSide == 0){//Fall left
                            this->fallLeft(x,y);
                        }
                        else{//Fall right.
                            this->fallRight(x,y);
                            x+= this->getSandSideLength()+1;
                        }
                        continue;
                    }

                    if(this->canFallRight(x,y)){
                        this->fallRight(x,y);
                        x+= this->getSandSideLength()+1;
                    }
                    else if(this->canFallLeft(x,y)){
                        this->fallLeft(x,y);
                    }
                }
                else{
                        this->fallDown(x,y);
                        y+= this->getSandSideLength()+1;
                }
            }
        }
    }
    update();

}

bool Canvas::canFallRight(int x,int y){

   return (this->withinBoundaries(x+this->getSandSideLength(),y)) &&(!this->isOccupied(x+this->getSandSideLength(),y+this->getSandSideLength()) &&
            this->isOccupied(x+this->getSandSideLength(),y+ (2*this->getSandSideLength())));
}
bool Canvas::canFallLeft(int x,int y){
    return (this->withinBoundaries(x-this->getSandSideLength()/2,y)) && (!this->isOccupied(x-this->getSandSideLength(),y+this->getSandSideLength()) &&
            this->isOccupied(x-this->getSandSideLength(),y+ (2*this->getSandSideLength())));
}

void Canvas::paintEvent(QPaintEvent *event){
    Q_UNUSED(event);

    // if(this->drawSquare){
    //         int* x = new int;
    //         int* y = new int;

    //         *x = this->lastMousePos->x();
    //         *y = this->lastMousePos->y();
    //         this->findClosestDrawingPoint(x,y);

    //         this->clamp(x,y);
    //         this->findClosestDrawingPoint(x,y);
    //         this->screenMatrix[*x][*y] = true;

    //         delete x;
    //         delete y;
    // }

    QPainter painter(this);

    for(int y = 0; y<this->getYLength();y++){
        for(int x = 0; x< this->getXLength();x++){
            if(this->isOccupied(x,y) == true){
                 painter.setBrush(QColor(this->colorMatrix[x][y].red(), this->colorMatrix[x][y].green(), this->colorMatrix[x][y].blue(),this->colorMatrix[x][y].alpha()));
                 painter.setPen(QColor(this->colorMatrix[x][y].red(), this->colorMatrix[x][y].green(), this->colorMatrix[x][y].blue(),this->colorMatrix[x][y].alpha()));
                 painter.drawRect(x,y, this->sandSideLength, this->sandSideLength);

                 //TODO below. Store every sand and their colors in an array.
                 //delete this->brushColor;
                 //this->brushColor = new QColor(rand()%255, rand()%255, rand()%255, rand()%255);
            }

        }
    }
}

bool Canvas::withinBoundaries(int x, int y){
    if((x > 0 && x < this->getXLength()) && (y > 0 && y < this->getYLength())){
        return true;
    }
    return false;
}

bool Canvas::touchesGround(int x, int y){
    if(y >= this->getYLength() - this->getSandSideLength())//On the bottom of the screen.
    {
        return true;
    }
    else if(this->isOccupied(x,y+this->getSandSideLength())){ //If it has a sand one cell below.
        return true;
    }
    else{ //If it does not have a sand one cell below.
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

void Canvas::keyPressEvent(QKeyEvent *event){
    if (event->key() == Qt::Key_Space) {
        qInfo() << "Spacebar pressed";
        QRandomGenerator *generator = QRandomGenerator::global();
        for(int i = 0; i< 4; i++){
            int randSide = generator->bounded(255); // 0,1,2,3
            if(i == 0){
                this->brushColor->setRed(randSide);
            }
            else if(i == 1){
                this->brushColor->setGreen(randSide);

            }
            else if(i==2){
                this->brushColor->setBlue(randSide);
            }
            else{
               this->brushColor->setAlpha(randSide);
            }
        }
    }
}



