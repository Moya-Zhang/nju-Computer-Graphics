#ifndef GL3DWIDGET_H
#define GL3DWIDGET_H

#include <qt_windows.h>
#include <QGLWidget>
#include <GL/gl.h>
#include <GL/glu.h>
#include <QtOpenGL/QtOpenGL>
#include <QKeyEvent>
#include <QVector3D>
#include <QVector4D>

class gl3dwidget:public QGLWidget
{
    Q_OBJECT
public:
    explicit gl3dwidget(QWidget *parent = nullptr);
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent* event);
private:
    GLfloat rotationX;//旋转角度
    GLfloat rotationY;
    GLfloat rotationZ;
    int numV;//顶点的个数
    int numF;//面的个数
    int FType;//面的类型

    GLfloat *vertex;//点集
    GLfloat **vertexIndex;//索引点集
    GLushort *faces;//面集
    QPoint lastPos;
};








#endif // GL3DWIDGET_H
