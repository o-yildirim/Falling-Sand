#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QThread>

class Canvas : public QWidget
{
    Q_OBJECT

    public:
        Canvas();
        ~Canvas();


        void drawSand(int,int);
        void computeScreenMatrix();
        bool touchesGround(int,int);
        bool withinBoundaries(int,int);
        bool isOccupied(int,int);
        void clamp(int*,int*);
        void findClosestDrawingPoint(int*,int*);
        float euclideanDist(int,int,int,int);

        void initAvailableDrawingPoints();
        void initializeScreenMatrix();

        int getXLength();
        int getYLength();
        int getMsToRedraw();
        int getSandSideLength();
        int getFallSpeed();

        void setXLength(int);
        void setYLength(int);
        void setMsToRedraw(int);
        void setSandSideLength(int);
        void setFallSpeed(int);

    protected:
        void mousePressEvent(QMouseEvent *event) override;
        void mouseMoveEvent(QMouseEvent *event) override;
        void mouseReleaseEvent(QMouseEvent *event) override;
        void paintEvent(QPaintEvent *event) override;


    private:
        int xLength;
        int yLength;
        int sandSideLength;
        int msToRedraw;
        int sizeOfAvailableDrawingPoints;
        int fallSpeed;
        bool drawSquare;
        bool **screenMatrix;
        bool initialized = false;
        QPoint *availableDrawingPoints;
        QTimer* timer;
        QColor* brushColor;
        QPoint* lastMousePos;
};

#endif // CANVAS_H
