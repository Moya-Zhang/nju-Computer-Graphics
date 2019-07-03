#include "window.h"
#include "ui_window.h"
using namespace Qt;
#define WIN_WIDTH 600
#define WIN_HEIGHT 400
#define BACK_COLOR white
window::window(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::window)
{
    ui->setupUi(this);
    curfigure=NULL;
    pix=QPixmap(this->size());
    backup=QPixmap(this->size());
    pix.fill(BACK_COLOR);
    painter.begin(&pix);
    pen.setWidth(3);
    painter.setPen(pen);
    curcolor=black;
    setWindowFlags(windowFlags()&~Qt::WindowMaximizeButtonHint);    // 禁止最大化按钮
    setFixedSize(this->width(),this->height());                     // 禁止拖动窗口大小
}

window::~window()
{
    delete ui;
}
void window::paintEvent(QPaintEvent* event){
    QPainter painter(this);
    painter.drawPixmap(0,0,this->size().width(),this->size().height(),pix);
}
void window::showfigure(figure* curfigure){
    pen.setColor(curfigure->color);
    painter.setPen(pen);
    for(int i=0;i<curfigure->points.size();i++)
        painter.drawPoint(curfigure->points.at(i));
    update();
    pen.setColor(curcolor);
    painter.setPen(pen);
}
void window::showvertex(figure *curfigure){
    pen.setColor(blue);
    pen.setWidth(8);
    painter.setPen(pen);
    for(int i=0;i<curfigure->vertex.size();i++)
        painter.drawPoint(curfigure->vertex.at(i));
    pen.setColor(BACK_COLOR);
    pen.setWidth(4);
    painter.setPen(pen);
    for(int i=0;i<curfigure->vertex.size();i++)
        painter.drawPoint(curfigure->vertex.at(i));
    update();
    pen.setColor(curcolor);
    pen.setWidth(3);
    painter.setPen(pen);
}
void window::showcenter(figure* curfigure){
    pen.setColor(green);
    pen.setWidth(8);
    painter.setPen(pen);
    painter.drawPoint(curfigure->center);
    pen.setColor(BACK_COLOR);
    pen.setWidth(4);
    painter.setPen(pen);
    painter.drawPoint(curfigure->center);
    update();
    pen.setColor(curcolor);
    pen.setWidth(3);
    painter.setPen(pen);
}

void window::mousePressEvent(QMouseEvent* event){
    if(event->pos().y()>WIN_HEIGHT-20||event->pos().y()<45)
        return;
    backup=pix;
    if(event->button()==Qt::RightButton){//按下右键，结束多边形的绘制
        if(mode!=mypolygon)return;
        if(!curfigure->finish()) return;
        figures.append(curfigure);
        showfigure(curfigure);
        curfigure=NULL;
        return;
    }
    //否则根据模式选择对应操作
    switch(mode){
    case mycut:{
        beg=event->pos();
        pen.setColor(blue);
        pen.setWidth(1);
        painter.setPen(pen);
        break;
    }
    case myline:{
        beg=event->pos();
        update();
        break;//line
    }
    case mycircle:{
        beg=event->pos();
        break;
    }
    case myellipse:{
        beg=event->pos();break;
    }
    case mypolygon:{
        cur=event->pos();
        if(curfigure==NULL)
            curfigure=new polygon(curcolor);
        curfigure->add(cur);
        showfigure(curfigure);
        break;
    }
    case mymove:{
        cur=event->pos();
        QCursor cursor;
        cursor.setShape(Qt::ClosedHandCursor);
        setCursor(cursor);
        break;
    }
    case myedit:{
        cur=event->pos();
        curfigure->onVertex(cur);
        break;
    }
    case mycurve:{
        cur=event->pos();
        if(curfigure==NULL){
            curfigure=new curve(cur,curcolor);
            painter.drawPoint(cur);
            update();
        }
        else{
            curfigure->add(cur);
            painter.drawPoint(cur);
            update();
            if(curfigure->vertex.size()==4){
                figures.append(curfigure);
                pix.fill(BACK_COLOR);
                for(int i=0;i<figures.size();i++)
                    showfigure(figures.at(i));
                curfigure=NULL;
            }
        }
        break;
    }
    default:{
        break;
    }
    }
}
void window::wheelEvent(QWheelEvent*event){
    if(figures.size()==0)return;
    switch(mode){
    case myrotate:{
        int angle;
        angle=event->delta()/3;
        figure* temp=figures.at(figures.size()-1);
        temp->rotate(angle*3.14/180);//旋转
        pix.fill(BACK_COLOR);
        for(int i=0;i<figures.size();i++){
            showfigure(figures.at(i));//输出所有图形
        }
        showcenter(curfigure);
        break;
    }
    case myzoom:{
        double ratio;
        ratio=event->delta()>0?120:80;
        figure* temp=figures.at(figures.size()-1);
        figures.pop_back();//取最后一个图形
        temp->zoom(ratio/100);//缩放
        figures.append(temp);//添回去
        pix.fill(BACK_COLOR);
        for(int i=0;i<figures.size();i++){
            showfigure(figures.at(i));//输出所有图形
        }
        showcenter(curfigure);
        break;
    }
    default:break;
    }
}
void window::mouseMoveEvent(QMouseEvent* event){
    if(event->pos().y()>WIN_HEIGHT-20||event->pos().y()<45)
        return;
    if(!event->buttons()&Qt::LeftButton)
        return;
    switch(mode){
    case mycut:{
        cur=event->pos();
        painter.end();
        pix=backup;
        painter.begin(&pix);
        painter.setPen(pen);
        int minx=beg.x()<cur.x()?beg.x():cur.x();
        int maxx=beg.x()>cur.x()?beg.x():cur.x();
        int miny=beg.y()<cur.y()?beg.y():cur.y();
        int maxy=beg.y()>cur.y()?beg.y():cur.y();
        QPoint topleft(minx,maxy),bottomright(maxx,miny);
        QRect rect(topleft,bottomright);
        painter.drawRect(rect);
        update();
        break;
    }
    case myline:{
        cur=event->pos();
        curfigure=new line(beg,cur,curcolor);
        painter.end();
        pix=backup;
        painter.begin(&pix);
        painter.setPen(pen);
        showfigure(curfigure);
        break;
    }
    case mycircle:{
        cur=event->pos();
        curfigure=new circle(beg,cur,curcolor);
        painter.end();
        pix=backup;
        painter.begin(&pix);
        painter.setPen(pen);
        showfigure(curfigure);
        break;
    }
    case myellipse:{
        cur=event->pos();
        curfigure=new ellipse(beg,cur,curcolor);
        painter.end();
        pix=backup;
        painter.begin(&pix);
        painter.setPen(pen);
        showfigure(curfigure);
        break;
    }
    case mymove:{
        QPoint offset=event->pos()-cur;
        cur=event->pos();
        curfigure=figures.at(figures.size()-1);
        curfigure->move(offset);
        pix.fill(BACK_COLOR);
        for(int i=0;i<figures.size();i++){
            showfigure(figures.at(i));//输出所有图形
        }
        break;
    }
    case myedit:{
        cur=event->pos();
        curfigure->edit(cur);
        pix.fill(BACK_COLOR);
        for(int i=0;i<figures.size();i++){
            showfigure(figures.at(i));//输出所有图形
        }
        break;
    }
    default:break;
    }
}

void window::mouseReleaseEvent(QMouseEvent* event){
    if(event->pos().y()>WIN_HEIGHT-20||event->pos().y()<45)
        return;
    if(event->button()!=Qt::LeftButton)return;
    switch(mode){
    case mycut:{
        cur=event->pos();
        pen.setWidth(3);
        pen.setColor(curcolor);
        painter.setPen(pen);
        int minx=beg.x()<cur.x()?beg.x():cur.x();
        int maxx=beg.x()>cur.x()?beg.x():cur.x();
        int miny=beg.y()<cur.y()?beg.y():cur.y();
        int maxy=beg.y()>cur.y()?beg.y():cur.y();
        QPoint bottomleft(minx,miny),bottomright(maxx,maxy);
        curfigure->cut(bottomleft,maxx-minx,maxy-miny);
        pix.fill(BACK_COLOR);
        for(int i=0;i<figures.size();i++){
            showfigure(figures.at(i));//输出所有图形
        }
        update();
        break;
    }
    case myline:{
        end=event->pos();
        figures.append(curfigure);
        curfigure=NULL;break;
    }
    case mycircle:{
        end=event->pos();
        figures.append(curfigure);
        curfigure=NULL;break;
    }
    case myellipse:{
        end=event->pos();
        figures.append(curfigure);
        curfigure=NULL;break;
    }
    case mymove:{
        QCursor cursor;
        cursor.setShape(Qt::ArrowCursor);
        setCursor(cursor);break;
    }
    case myedit:{
        curfigure->endEdit();
        pix.fill(BACK_COLOR);
        for(int i=0;i<figures.size();i++){
            showfigure(figures.at(i));//输出所有图形
        }
        showvertex(curfigure);
        break;
    }
    default:{
        break;
    }
    }
}

void window::on_actionLine_triggered()
{
    mode=myline;
    ui->statusBar->showMessage("直线模式");
    pix.fill(BACK_COLOR);
    for(int i=0;i<figures.size();i++){
        showfigure(figures.at(i));//输出所有图形
    }
}



void window::on_actionCircle_triggered()
{
    mode=mycircle;
    ui->statusBar->showMessage("画圆模式");
    pix.fill(BACK_COLOR);
    for(int i=0;i<figures.size();i++){
        showfigure(figures.at(i));//输出所有图形
    }
}

void window::on_actionEllipse_triggered()
{
    mode=myellipse;
    ui->statusBar->showMessage("椭圆模式");
    pix.fill(BACK_COLOR);
    for(int i=0;i<figures.size();i++){
        showfigure(figures.at(i));//输出所有图形
    }
}
void window::on_actionCurve_triggered()
{
    mode=mycurve;
    ui->statusBar->showMessage("曲线模式");
    curfigure=NULL;
/*    pix.fill(BACK_COLOR);
    for(int i=0;i<figures.size();i++){
        showfigure(figures.at(i));//输出所有图形
    }*/
}


void window::on_actionPolygon_triggered()
{
    mode=mypolygon;
    ui->statusBar->showMessage("多边形模式");
    curfigure=NULL;
}
void window::on_actionFill_triggered()
{
    if(figures.isEmpty())return;
    curfigure=figures[figures.size()-1];
    if(curfigure->shape==fline)return;
    curfigure->fill();
    showfigure(curfigure);
}
void window::on_actionZoom_triggered()
{
    if(figures.isEmpty())return;
    curfigure=figures[figures.size()-1];
    mode=myzoom;
    ui->statusBar->showMessage("缩放模式");
    pix.fill(BACK_COLOR);
    for(int i=0;i<figures.size();i++){
        showfigure(figures.at(i));//输出所有图形
    }
}

void window::on_actionRotate_triggered()
{
    if(figures.isEmpty())return;
    curfigure=figures[figures.size()-1];
    mode=myrotate;
    ui->statusBar->showMessage("旋转模式");
    pix.fill(BACK_COLOR);
    for(int i=0;i<figures.size();i++){
        showfigure(figures.at(i));//输出所有图形
    }
}


void window::on_actionDelete_triggered()
{
    if(figures.isEmpty()) return;
    figures.pop_back();
    pix.fill(BACK_COLOR);
    for(int i=0;i<figures.size();i++){
        showfigure(figures.at(i));//输出所有图形
    }
    update();
}


void window::on_actionMove_triggered()
{
    if(figures.isEmpty())return;
    curfigure=figures[figures.size()-1];
    mode=mymove;
    ui->statusBar->showMessage("平移模式");
    pix.fill(BACK_COLOR);
    for(int i=0;i<figures.size();i++){
        showfigure(figures.at(i));//输出所有图形
    }
}

void window::on_actionEdit_triggered()
{
    if(figures.isEmpty())return;
    curfigure=figures[figures.size()-1];
    showvertex(curfigure);
    mode=myedit;
    ui->statusBar->showMessage("编辑模式");
}

void window::on_actionCut_triggered()
{
    if(figures.isEmpty())return;
    curfigure=figures[figures.size()-1];
    if(curfigure->shape!=fline)return;
    mode=mycut;
    ui->statusBar->showMessage("裁剪模式");
}


void window::on_actionSave_triggered()
{
    QString name = QFileDialog::getSaveFileName(this,tr("Open Config"),"",tr("Images (*.jpg)"));
    if (!name.isNull())
    {
        pix.save(name,"JPG",100);
        QMessageBox::information(this,"提示","保存成功！");
    }
    else
    {
        QMessageBox::information(this,"提示","您已取消保存！");
    }
}

void window::on_action3d_triggered()
{

    w=new gl3dwidget();
    w->show();
}

void window::on_actionColor_triggered()
{
    curcolor=QColorDialog::getColor();
}













