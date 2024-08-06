#include "canvas.h"
#include <QTimer>

Canvas::Canvas() {
    this->drawSquare = false;
    this->setXLength(600);
    this->setYLength(600);
    this->setMsToRedraw(15);
    this->setSandSideLength(1);

    //Initializing the screen matrix.
    this->initializeScreenMatrix();

    //qInfo() <<"xLength: " << this->getXLength() << ", yLength: " << this->getYLength();


    //Set up timer.
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Canvas::computeScreenMatrix);
    timer->start(this->getMsToRedraw());
    this->initialized = true;

    //Test below to watch a sand fall.
    //this->drawSquare = true;
    //this->drawSand(300,100);


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
    Q_UNUSED(event);

}

void Canvas::mouseMoveEvent(QMouseEvent *event){

    if(this->drawSquare == true){
        QPoint clickPosition = event->pos(); 
        drawSand(clickPosition.x(), clickPosition.y());
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
        //TODO Convert position.x and position.y to the indices in this->screenMatrix. Probably with a for loop, make multiple cells true.
        this->screenMatrix[x][y] = true;
        //qInfo() << "Drawing to x: " <<x << ", y: " << y << "--  xLength: " << this->getXLength() << ", yLength: " <<this->getYLength();
    }
    update();
}

void Canvas::removeSand(int x, int y){
    this->clamp(&x,&y);
    //qInfo() <<"Removing from " << x  << ", " << y <<  " -- xLength: " << this->getXLength() << ", yLength: " <<this->getYLength();
    //TODO Convert position.x and position.y to the indices in this->screenMatrix. Probably with a for loop, make multiple cells true.
    this->screenMatrix[x][y] = false;
    update();
}

void Canvas::computeScreenMatrix(){
    //Make the sand pieces fall by making their current cells false and one below true.

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
                        this->screenMatrix[x][y+1] = true;
                        y++;
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

    QPainter painter(this);

    for(int y = 0; y<this->getYLength();y++){
        for(int x = 0; x< this->getXLength();x++){
            if(this->isOccupied(x,y) == true){
                 painter.setBrush(Qt::blue);
                 painter.setPen(Qt::blue);
                 //qInfo()<<"Painting blue at " << x << ", " << y;
                 painter.drawRect(x,y, this->sandSideLength, this->sandSideLength);
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


    if(this->isOccupied(x,y+1)){ //If it does not have a sand one cell below.
        return true;
    }
    else{   //If it has a sand one cell below.
        return false;
    }


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



