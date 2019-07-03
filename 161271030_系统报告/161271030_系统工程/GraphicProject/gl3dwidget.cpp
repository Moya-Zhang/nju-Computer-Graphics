#include "gl3dwidget.h"

gl3dwidget::gl3dwidget(QWidget *parent)
    : QGLWidget(parent),rotationX(0),rotationY(0),rotationZ(0)
{
    QString filename;
    filename = QFileDialog::getOpenFileName(this,tr("Open File"),tr(""),tr("OFF File (*.off)"));
    if(filename.isEmpty())return;
    QFile file(filename);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in(&file);
    QString format;
    in>>format;
//读入顶点数、面数
    int discard;
    in >> numV;in >> numF;in >> discard;
//初始化点集和索引点集
    vertex=new GLfloat[numV*3];
    vertexIndex=new GLfloat*[numV];
    for(int i=0;i<numV;i++){
        vertexIndex[i]=new GLfloat[3];
    }
//读入点集和索引点集
    for(int i=0;i<numV;i++)
        for(int j=0;j<3;j++){
            in>>vertex[i*3+j];
            vertexIndex[i][j]=vertex[i*3+j];
        }
//读入边集
    in >> FType;
    faces = new GLushort[numF*FType];
    for (int i = 0;i<numF;i++) {
        for (int j = 0;j<FType;j++) {
            unsigned short temp;
            in >> temp;
            faces[i*FType + j]=(GLushort)temp;
        }
        in >> discard;
    }
    file.close();
}

void gl3dwidget::initializeGL(){
    //设置GL3dwidget的坐标和尺寸
    //设置清除时颜色
    setGeometry(300, 150, 640, 480);//设置窗口初始位置和大小
    glShadeModel(GL_SMOOTH);//设置阴影平滑模式
    glClearColor(0.0, 0.0, 0.0, 0);//改变窗口的背景颜色
    glClearDepth(1.0);//设置深度缓存
    glEnable(GL_DEPTH_TEST);//允许深度测试
    glDepthFunc(GL_LEQUAL);//设置深度测试类型
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);//进行透视校正
}

void gl3dwidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();//重置模型观察矩阵
    glTranslatef(0.0, 0.0, 0.0);//移动绘制平面到屏幕正中心

    glRotatef(rotationX, 0.0, 1.0, 0.0);
    glRotatef(rotationY, 1.0, 0.0, 0.0);
    glRotatef(rotationZ, 0.0, 0.0, 1.0);

    //*打开光源
    GLfloat light_position[] = { 1000.0, 1000.0, 1000.0, 1000.0 };//坐标
    GLfloat light_ambient [] = { 0.0, 0.0, 1.0, 1.0 };//环境
    GLfloat light_diffuse [] = { 0.0, 0.75, 1.0, 1.0 };//漫射
    GLfloat light_specular[] = { 1.0, 0.5, 0.4, 1.0 };//镜面
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT , light_ambient );
    glLightfv(GL_LIGHT0, GL_DIFFUSE , light_diffuse );
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glEnableClientState(GL_VERTEX_ARRAY); //启用顶点数组
    glVertexPointer(3, GL_FLOAT, 0, vertex); //注册顶点数组
    glColor3f(0.45, 0.33, 0.07);
    glDrawElements(GL_TRIANGLES, numF*FType, GL_UNSIGNED_SHORT, faces);
    glColor3f(1.0, 1.0, 1.0);
    glFlush();
}

void gl3dwidget::resizeGL(int w, int h)
{
    glMatrixMode(GL_MODELVIEW);        // 设置模型矩阵
    glViewport(0, 0, (GLint)w, (GLint)h);//重置当前窗口
    glMatrixMode(GL_PROJECTION);//选择投影矩阵
    glLoadIdentity();//重置投影矩阵
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
void gl3dwidget::mousePressEvent(QMouseEvent *event)
{
    lastPos= event->pos();
}

void gl3dwidget::mouseMoveEvent(QMouseEvent *event)
{
    GLfloat dx = GLfloat(event->x() - lastPos.x()) / width();
    GLfloat dy = GLfloat(event->y() - lastPos.y()) / height();
    if(event->buttons() & Qt::LeftButton){
        rotationX += 180 * dy;
        rotationY += 180 * dx;
        updateGL();
    }
    else if(event->buttons() & Qt::RightButton){
        rotationX += 180 * dy;
        rotationZ += 180 * dx;
        updateGL();
    }
    lastPos = event->pos();
}
void gl3dwidget::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_Left)
        rotationX++;
    else if(event->key()==Qt::Key_Right)
        rotationX--;
    else if(event->key()==Qt::Key_Up)
        rotationY++;
    else if(event->key()==Qt::Key_Down)
        rotationY--;
    updateGL();
}
void gl3dwidget::wheelEvent(QWheelEvent* event){
    rotationZ+=event->delta()>0?10:-10;
    updateGL();
}















