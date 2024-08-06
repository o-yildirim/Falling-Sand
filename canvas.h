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
        int getXLength();
        int getYLength();
        int getMsToRedraw();
        int getSandSideLength();

        void drawSand(int,int);
        void removeSand(int,int);
        void computeScreenMatrix();
        bool touchesGround(int,int);
        bool withinBoundaries(int,int);
        bool isOccupied(int,int);

        void setXLength(int);
        void setYLength(int);
        void setMsToRedraw(int);
        void setSandSideLength(int);
        void initializeScreenMatrix();
        void clamp(int*,int*);


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
        bool drawSquare;
        bool **screenMatrix;
        bool initialized = false;
};

#endif // CANVAS_H
