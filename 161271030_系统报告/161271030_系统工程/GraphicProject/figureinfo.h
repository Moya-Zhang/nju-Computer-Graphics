#ifndef FIGUREINFO_H
#define FIGUREINFO_H
#include <QColor>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QString>
#include <iostream>
#include <windows.h>
#include <QVector>
#include <cmath>
#include <cstdlib>
#include <QDebug>
#include <set>
#include <queue>
#include <qalgorithms.h>
struct tempPoint{
    int x,y;
    tempPoint(int x,int y):x(x),y(y){}
    operator <(const tempPoint& b)const{
        return this->x<b.x||(this->x==b.x&&this->y<b.y);
    }
};


enum Shape{fline,fcircle,fellipse,fpolygon,fcurve};
class figure{
public:
    Shape shape;
    QVector<QPoint> points;
    QVector<QPoint> vertex;
    QColor color;
    QPoint center;
    int editing;
    bool filled;
    figure(QColor c):filled(false),editing(-1),color(c){}
    double distance(QPoint a,QPoint b){
        double x=a.x()-b.x(),y=a.y()-b.y();
        return sqrt(x*x+y*y);
    }
    QPoint rotatePoint(double angle,QPoint p,QPoint mid){
        //返回点p绕mid旋转angle后形成的点
        double x=p.x()-mid.x(),y=p.y()-mid.y();
        double x1=x*cos(angle)-y*sin(angle);
        double y1=x*sin(angle)+y*cos(angle);
        return QPoint(mid.x()+x1,mid.y()+y1);
    }
    QPoint zoomPoint(double ratio,QPoint p,QPoint mid){
        //返回点p绕mid放大ratio倍后的点
        double x=p.x()-mid.x(),y=p.y()-mid.y();
        double x1=x*ratio,y1=y*ratio;
        return QPoint(mid.x()+x1,mid.y()+y1);
    }

    virtual void add(QPoint v){}
    virtual int finish(){}
    virtual bool cut(const QPoint leftdown,int width,int height){}

    virtual void rotate(double angle)=0; //旋转
    virtual void zoom(double ratio)=0; //缩放
    virtual void update()=0;
    virtual void updateMid()=0;
    virtual void fill(){}

    void onVertex(QPoint p){
        for(int i=0;i<vertex.size();i++)
            if(qAbs(p.x()-vertex[i].x())<5&&qAbs(p.y()-vertex[i].y())<5){
                editing=i;break;
            }
    }
    void edit(QPoint p){
        if(editing==-1)return;
        vertex[editing]=p;
        update();
        updateMid();
    }
    void endEdit(){
        editing=-1;
    }

    virtual void move(QPoint offset)=0;
    void Bresenham(QPoint a,QPoint b){
        int x1=a.x()<b.x()?a.x():b.x();
        int y1=a.x()<b.x()?a.y():b.y();
        int x2=a.x()<b.x()?b.x():a.x();
        int y2=a.x()<b.x()?b.y():a.y();
        int dx=x2-x1,dy=y2-y1;
        int signk=dy>0?1:-1;
        dy=qAbs(dy);
        int p;
        if(dx>dy){
            p=2*dy-dx;
            for(;x1<=x2;x1++){
                points.append(QPoint(x1,y1));
                if(p>=0){
                    y1+=signk;
                    p+=2*(dy-dx);
                }
                else{
                    p+=2*dy;
                }
            }
        }
        else{
            if(y1>y2){
                qSwap(y1,y2);
                qSwap(x1,x2);
            }
            p=2*dx-dy;
            for(;y1<=y2;y1++){
                points.append(QPoint(x1,y1));
                if(p>=0){
                    x1+=signk;
                    p+=2*(dx-dy);
                }
                else p+=2*dx;
            }
        }
    }
};
class line:public figure{
public:
    line(QPoint p1,QPoint p2,QColor c):figure(c){
        shape=fline;
        vertex.push_back(p1);
        vertex.push_back(p2);
        center=QPoint((p1.x()+p2.x())/2,(p1.y()+p2.y())/2);
        Bresenham(vertex[0],vertex[1]);
    }
    void update(){
        points.clear();
        Bresenham(vertex[0],vertex[1]);
    }
    void updateMid(){
        QPoint& beg=vertex[0];
        QPoint& end=vertex[1];
        center=QPoint((beg.x()+end.x())/2,(beg.y()+end.y())/2);
    }

    bool cut(const QPoint leftdown,int width,int height){
        int minx=leftdown.x(),miny=leftdown.y();
        int maxx=leftdown.x()+width,maxy=leftdown.y()+height;
        int x1 = vertex[0].x(), y1 = vertex[0].y();
        int x2 = vertex[1].x(), y2 = vertex[1].y();
        int dx = x2-x1, dy = y2-y1;
        int p[4] = {-dx, dx, -dy, dy}; //指示方向：内->外or外->内
        int q[4] = {x1-minx, maxx-x1, y1-miny, maxy-y1}; //指示位置：在内侧or在外侧
        double u1 = 0, u2 = 1; //记录两侧裁剪边界的u值
        for(int i=0;i<4;i++)
        {
            if(p[i]==0) //平行于此边界
            {
                if(q[i]<0) //在此边界的外部，直接丢弃；在内部，则本边界对此直线没有裁剪点，不做更改
                    return false;
            }
            else //与此边界的直线有交点，裁剪
            {
                double r = (double)q[i]/(double)p[i];
                if(p[i]<0) //外->内
                    u1 = u1>r?u1:r;
                else //内->外
                    u2 = u2<r?u2:r;
            }
        }

        if(u1>u2) //裁剪的左侧在右侧的右边，舍弃
            return false;
        vertex[0].setX(x1+int(u1*dx+0.5));vertex[0].setY(y1+int(u1*dy+0.5));
        vertex[1].setX(x1+int(u2*dx+0.5));vertex[1].setY(y1+int(u2*dy+0.5));
        center=QPoint((vertex[0].x()+vertex[1].x())/2,(vertex[0].y()+vertex[1].y())/2);
        update();
        return true;
    }

    virtual void move(QPoint offset){
        vertex[0]+=offset;vertex[1]+=offset;center+=offset;
        for(int i=0;i<points.size();i++)
            points[i]+=offset;
    }

    virtual void rotate(double angle){ //旋转
        vertex[0]=rotatePoint(angle,vertex[0],center);
        vertex[1].setX(2*center.x()-vertex[0].x());
        vertex[1].setY(2*center.y()-vertex[0].y());
//        vertex[1]=rotatePoint(angle,vertex[1],center);
        update();
    }
    virtual void zoom(double ratio){ //缩放
        QPoint offset=vertex[1]-vertex[0];
        int d=offset.x()*offset.x()+offset.y()*offset.y();
        if(d<1600&&ratio<1)return;
        vertex[0]=zoomPoint(ratio,vertex[0],center);
        vertex[1].setX(2*center.x()-vertex[0].x());
        vertex[1].setY(2*center.y()-vertex[0].y());
//        vertex[1]=zoomPoint(ratio,vertex[1],center);
        update();
    }

};
class curve:public figure{
public:
    curve(QPoint p,QColor c):figure(c){
        shape=fcurve;
        vertex.push_back(p);
    }
    void add(QPoint p){
        vertex.push_back(p);
        if(vertex.size()==4){
            producePoints();
            updateMid();
        }
    }

    curve(QPoint p1,QPoint p2,QPoint p3,QPoint p4,QColor c):figure(c){
        vertex.push_back(p1);
        vertex.push_back(p2);
        vertex.push_back(p3);
        vertex.push_back(p4);
        //根据vertex计算曲线各点
    }
    virtual void move(QPoint offset){
        center+=offset;
        for(int i=0;i<vertex.size();i++)
            vertex[i]+=offset;
        for(int i=0;i<points.size();i++){
            points[i]+=offset;
        }
    }
    void producePoints(){
        for(double u=0;u<=1;u+=0.001)
        {
            double a = pow(1-u,3);
            double b = 3*u*pow(1-u,2);
            double c = 3*pow(u,2)*(1-u);
            double d = pow(u,3);
            points.push_back(QPoint(a*vertex[0].x() + b*vertex[1].x() + c*vertex[2].x() + d*vertex[3].x() + 0.5,
                                       a*vertex[0].y() + b*vertex[1].y() + c*vertex[2].y() + d*vertex[3].y() + 0.5));
        }
    }
    void update(){
        points.clear();
        producePoints();
    }
    void updateMid(){
        int minX = vertex[0].x();
        int maxX = vertex[0].x();
        int minY = vertex[0].y();
        int maxY = vertex[0].y();
        for(QPoint v:vertex){
            if(v.x()<minX) minX=v.x();
            if(v.x()>maxX) maxX=v.x();
            if(v.y()<minY) minY=v.y();
            if(v.y()>maxY) maxY=v.y();
        }
        center.setX((minX+maxX)/2);center.setY((minY+maxY)/2);
    }

    virtual void rotate(double angle){ //旋转
        for(int i=0;i<vertex.size();i++)
            vertex[i]=rotatePoint(angle,vertex[i],center);
        update();
    }
    virtual void zoom(double ratio){ //缩放
        int minX = vertex[0].x();
        int maxX = vertex[0].x();
        int minY = vertex[0].y();
        int maxY = vertex[0].y();
        for(QPoint v:vertex){
            if(v.x()<minX) minX=v.x();
            if(v.x()>maxX) maxX=v.x();
            if(v.y()<minY) minY=v.y();
            if(v.y()>maxY) maxY=v.y();
        }
        QPoint offset(maxX-minX,maxY-minY);
        int d=offset.x()*offset.x()+offset.y()*offset.y();
        if(d<1600&ratio<1)return;
        for(int i=0;i<vertex.size();i++)
            vertex[i]=zoomPoint(ratio,vertex[i],center);
        update();
    }
};

class circle:public figure{
public:
    circle(QPoint p1,QPoint p2,QColor c):figure(c){
        shape=fcircle;
        vertex.push_back(p1);
        vertex.push_back(p2);
        center=vertex[0];
        QPoint& end=vertex[1];
        producePoints(center,end);
    }
    virtual void move(QPoint offset){
        vertex[0]+=offset;
        vertex[1]+=offset;
        center=vertex[0];
        for(int i=0;i<points.size();i++){
            points[i]+=offset;
        }
    }
    void fill(){
        QPoint offset=vertex[1]-vertex[0];
        int r2=offset.x()*offset.x()+offset.y()*offset.y();
        int r=sqrt(r2);
        int miny=vertex[0].y()-r,maxy=vertex[0].y()+r;
        int i,j;
        for(int y = miny;y<=maxy;y++){
            int d2=(y-vertex[0].y())*(y-vertex[0].y());
            int s2=r2-d2;
            int s=sqrt(s2);
            i=vertex[0].x()-s;
            j=vertex[0].x()+s;
            for(int x=i;x<=j;x++)
                points.append(QPoint(x,y));
        }
        filled=true;
    }
    void update(){
        points.clear();
        center=vertex[0];
        QPoint& end=vertex[1];
        producePoints(center,end);
        if(filled) fill();
    }
    void updateMid(){
        center=vertex[0];
    }

    virtual void rotate(double angle){ //旋转
        return;
    }
    virtual void zoom(double ratio){ //缩放
        center=vertex[0];
        QPoint& end=vertex[1];
        QPoint offset=end-center;
        int r=offset.x()*offset.x()+offset.y()*offset.y();
        r<<=2;
        if(r<1600&&ratio<1)return;
        end=zoomPoint(ratio,end,center);
        update();
    }
    void producePoints(QPoint a,QPoint b){
        int dx=qAbs(b.x()-a.x()),dy=qAbs(b.y()-a.y());
        int r=sqrt(dx*dx+dy*dy);
        int x=0,y=r;
        int d=1-r;
        int deltax=3;
        int deltay=5-r-r;
        while(x<y)
        {
            if(d<0){
                d+=deltax;
                deltax+=2;
                deltay+=2;
                x++;
            }
            else{
                d+=deltay;
                deltax+=2;
                deltay+=4;
                x++;
                y--;
            }
            //逆时针画点
            points.append(QPoint(a.x()+x,a.y()+y));
            points.append(QPoint(a.x()+y,a.y()+x));
            points.append(QPoint(a.x()+y,a.y()-x));
            points.append(QPoint(a.x()+x,a.y()-y));
            points.append(QPoint(a.x()-x,a.y()-y));
            points.append(QPoint(a.x()-y,a.y()-x));
            points.append(QPoint(a.x()-y,a.y()+x));
            points.append(QPoint(a.x()-x,a.y()+y));
        }
    }
};

class ellipse:public figure{
public:
    ellipse(QPoint p1,QPoint p2,QColor c):figure(c){
        shape=fellipse;
        vertex.push_back(p1);vertex.push_back(p2);
        QPoint& beg=vertex[0];
        QPoint& end=vertex[1];
        beg=p1,end=p2;
        center=QPoint((p1.x()+p2.x())/2,(p1.y()+p2.y())/2);
        producePoints(beg,end);
    }
    virtual void move(QPoint offset){
        QPoint& beg=vertex[0];
        QPoint& end=vertex[1];
        beg+=offset;end+=offset;center+=offset;
        for(int i=0;i<points.size();i++)
            points[i]+=offset;
    }
    void fill(){
        QPoint offset=vertex[1]-vertex[0];
        //b2*x2+a2*y2=a2*b2=r2
        int a=qAbs(offset.x())>>1;
        int b=qAbs(offset.y())>>1;
        int a2=a*a,b2=b*b;
        int r2=a2*b2;
        int miny=center.y()-b,maxy=center.y()+b;
        int i,j;
        for(int y = miny;y<=maxy;y++){
            int y2=(y-center.y())*(y-center.y());
            int x2=(r2-a2*y2)/b2;
            int d=sqrt(x2);
            int i=center.x()-d,j=center.x()+d;
            for(int x=i;x<=j;x++)
                points.append(QPoint(x,y));
        }
        filled=true;
    }

    void update(){
        points.clear();
        QPoint& beg=vertex[0];
        QPoint& end=vertex[1];
        producePoints(beg,end);
        if(filled)fill();
    }
    void updateMid(){
        QPoint& beg=vertex[0];
        QPoint& end=vertex[1];
        center=QPoint((beg.x()+end.x())/2,(beg.y()+end.y())/2);
    }

    virtual void rotate(double angle){ //旋转
        QPoint& beg=vertex[0];
        QPoint& end=vertex[1];
        QPoint offset;
        if(angle>0){
            offset=beg-center;
            beg.setX(center.x()-offset.y());beg.setY(center.y()+offset.x());
            offset=end-center;
            end.setX(center.x()-offset.y());end.setY(center.y()+offset.x());
        }
        else{
            offset=beg-center;
            beg.setX(center.x()+offset.y());beg.setY(center.y()-offset.x());
            offset=end-center;
            end.setX(center.x()+offset.y());end.setY(center.y()-offset.x());
        }
        update();
    }
    virtual void zoom(double ratio){ //缩放
        QPoint& beg=vertex[0];
        QPoint& end=vertex[1];
        QPoint offset=end-beg;
        int d=offset.x()*offset.x()+offset.y()*offset.y();
        if(d<1600&&ratio<1)return;
        beg=zoomPoint(ratio,beg,center);
        vertex[1].setX(2*center.x()-vertex[0].x());
        vertex[1].setY(2*center.y()-vertex[0].y());
//        end=zoomPoint(ratio,end,center);
        update();
    }
    void producePoints(QPoint a,QPoint b){
        int dx=b.x()-a.x(),dy=b.y()-a.y();
        int rx=dx/2,ry=dy/2;
        QPoint mid(a.x()+rx,a.y()+ry);
        rx=qAbs(rx);
        ry=qAbs(ry);
        int rx2=rx*rx,ry2=ry*ry;
        int tworx2=2*rx2,twory2=2*ry2;
        int p=ry2-rx2*ry+rx2/4;
        int x=0,y=ry;
        int px=0,py=tworx2*y;
        while(px<py){
            x++;
            px+=twory2;
            if(p<0)
                p+=ry2+px;
            else{
                y--;
                py-=tworx2;
                p+=ry2+px-py;
            }
            points.append(QPoint(mid.x()+x,mid.y()+y));
            points.append(QPoint(mid.x()+x,mid.y()-y));
            points.append(QPoint(mid.x()-x,mid.y()-y));
            points.append(QPoint(mid.x()-x,mid.y()+y));
        }
        p=ry2*(x+0.5)*(x+0.5)+rx2*(y-1)*(y-1)-rx2*ry2;
        while(y>0){
            y--;
            py-=tworx2;
            if(p>0)
                p+=rx2-py;
            else{
                x++;
                px+=twory2;
                p+=rx2-py+px;
            }
            points.append(QPoint(mid.x()+x,mid.y()+y));
            points.append(QPoint(mid.x()+x,mid.y()-y));
            points.append(QPoint(mid.x()-x,mid.y()-y));
            points.append(QPoint(mid.x()-x,mid.y()+y));
        }
    }
};

class polygon:public figure{
public:
    polygon(QColor c):figure(c){
        shape=fpolygon;
    }
    void fill(){
        int miny=points[0].y(),maxy=points[0].y();
        for(QPoint v:vertex){
            if(v.y()<miny)miny=v.y();
            if(v.y()>maxy)maxy=v.y();
        }
        int sx,sy,tx,ty,x;
        QVector<int> index;
        int size=vertex.size();
        int *done = new int[size];//用done存放已经做过记录的顶点的下标
        memset(done,0,sizeof(int)*size);
        for(int y = miny;y<=maxy;y++)
        {
            //记录扫描线与边线的交点,扫描每两个顶点(第i个和第j个)连成的边
            for(int i = 0,j = size-1; i < size; j = i, i++) {
                sx = vertex[i].x();
                sy = vertex[i].y();
                tx = vertex[j].x();
                ty = vertex[j].y();
                int lowy,heighty;
                lowy = (sy<ty)?sy:ty;
                heighty = (sy>ty)?sy:ty;
                //水平线
                if(ty == sy){
                    if(y == ty){
                        int xmax,xmin;
                        xmax = (sx>tx)?sx:tx;
                        xmin = (sx<tx)?sx:tx;
                        for(int xx = xmin;xx<=xmax;xx++){//把水平线上所有点加入points中
                            QPoint p(xx,i);
                            points.append(p);
                        }
                    }
                    continue;
                }
                //没有交点
                if(y<lowy||y>heighty)
                    continue;
                x = sx + (y - sy) * (tx - sx) / (ty - sy);
                //判断交点(x,y)是不是顶点v[i]
                if((x == vertex[i].x()&&y == vertex[i].y())){
                    if(done[i]) continue;//第i个顶点已经做了记录
                    //判断顶点是不是极值点
                    //即判断与交点相关联的两条线的另外两个顶点是不是在交点的同一侧
                    int i1=(i+1)%size,i2=(i+size-1)%size;
                    index.push_back(x);
                    //同号、极值点、多记录一次
                    if((vertex[i1].y()-y)*(vertex[i2].y()-y)>0){
                        index.push_back(x);
                    }
                    done[i] = 1;//第i个顶点已经做了记录
                    continue;
                }
                //交点(x,y)是不是顶点v[j]
                else if((x == vertex[j].x()&&y == vertex[j].y())){
                    if(done[j]) continue;//第j个顶点已经做了记录
                    //判断顶点是不是极值点
                    //即判断与交点相关联的两条线的另外两个顶点是不是在交点的同一侧
                    int j1=(j+1)%size,j2=(j+size-1)%size;
                    index.push_back(x);
                    //同号、极值点、多记录一次
                    if((vertex[j1].y()-y)*(vertex[j2].y()-y)>0){
                        index.push_back(x);
                    }
                    done[j] = 1;
                    continue;
                }
                //交点不是顶点
                else index.push_back(x);
            }

            //将index排序
            qSort(index.begin(),index.end());

            //填充多边形
            for(int n=0,m=n+1;m<index.size();n+=2,m=n+1)
            {
                for(int xx = index[n];xx<=index[m];xx++)
                {
                    QPoint p(xx,y);
                    points.append(p);
                }
            }
            index.clear();
        }
        filled=true;
    }

    void add(QPoint v){
        vertex.append(v);
        points.append(v);
        int s=vertex.size();
        if(s>1) Bresenham(vertex.at(s-2),vertex.at(s-1));
    }
    void update(){
        points.clear();
        int i=0,j=1;
        for(;j<vertex.size();j++,i++)
            Bresenham(vertex.at(i),vertex.at(j));
        Bresenham(vertex.at(i),vertex.at(0));
        if(filled)fill();
    }
    void updateMid(){
        int minx=vertex.at(0).x(),miny=vertex.at(0).y();
        int maxx=vertex.at(0).x(),maxy=vertex.at(0).y();
        for(int i=1;i<vertex.size();i++){
            if(vertex.at(i).x()<minx)
                minx=vertex.at(i).x();
            if(vertex.at(i).x()>maxx)
                maxx=vertex.at(i).x();
            if(vertex.at(i).y()<miny)
                miny=vertex.at(i).y();
            if(vertex.at(i).y()>maxy)
                maxy=vertex.at(i).y();
        }
        center=QPoint((minx+maxx)/2,(miny+maxy)/2);
    }

    virtual void move(QPoint offset){
        center+=offset;
        for(int i=0;i<vertex.size();i++)
            vertex[i]+=offset;
        for(int i=0;i<points.size();i++)
            points[i]+=offset;
    }

    int finish(){
        if(vertex.size()<3)return 0;
        Bresenham(vertex.at(vertex.size()-1),vertex.at(0));
        updateMid();
        return 1;
    }
    virtual void rotate(double angle){ //旋转
        for(int i=0;i<vertex.size();i++)
            vertex[i]=rotatePoint(angle,vertex[i],center);
        update();
    }
    virtual void zoom(double ratio){ //缩放
        int minx=vertex.at(0).x(),miny=vertex.at(0).y();
        int maxx=vertex.at(0).x(),maxy=vertex.at(0).y();
        for(int i=1;i<vertex.size();i++){
            if(vertex.at(i).x()<minx)
                minx=vertex.at(i).x();
            if(vertex.at(i).x()>maxx)
                maxx=vertex.at(i).x();
            if(vertex.at(i).y()<miny)
                miny=vertex.at(i).y();
            if(vertex.at(i).y()>maxy)
                maxy=vertex.at(i).y();
        }
        QPoint offset(maxx-minx,maxy-miny);
        int d=offset.x()*offset.x()+offset.y()*offset.y();
        if(d<1600&&ratio<1)return;
        for(int i=0;i<vertex.size();i++)
            vertex[i]=zoomPoint(ratio,vertex[i],center);
        update();
    }

};

#endif // FIGUREINFO_H
