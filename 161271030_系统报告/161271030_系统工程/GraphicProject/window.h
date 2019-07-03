#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QPixmap>
#include <QMouseEvent>
#include <QString>
#include <QColorDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <figureinfo.h>
#include <set>
#include <queue>
#include <QFileDialog>
#include <gl3dwidget.h>
namespace Ui {
class window;
}
enum MODE{myline,mycircle,myellipse,mypolygon,mycurve,myfill,myrotate,myzoom,mymove,myedit,mycut};

class window : public QMainWindow
{
    Q_OBJECT

public:
    explicit window(QWidget *parent = 0);
    ~window();

    void paintEvent(QPaintEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);

private slots:
    void on_actionLine_triggered();

    void on_actionCircle_triggered();

    void on_actionEllipse_triggered();

    void on_actionFill_triggered();

    void on_actionPolygon_triggered();

    void on_actionZoom_triggered();

    void on_actionRotate_triggered();

    void on_actionDelete_triggered();

    void on_actionMove_triggered();

    void on_actionEdit_triggered();

    void on_actionCut_triggered();

    void on_actionCurve_triggered();

    void on_actionSave_triggered();

    void on_action3d_triggered();

    void on_actionColor_triggered();

private:
    Ui::window *ui;
    QPixmap pix;
    QPixmap backup;
    QPainter painter;
    QPen pen;
    QPoint beg,end,cur;
    QColor curcolor;
    MODE mode;
    QVector<figure*> figures;
    figure* curfigure;
    void showfigure(figure* curfigure);
    void showvertex(figure* curfigure);
    void showcenter(figure* curfigure);
    void fill(QPoint p);
    gl3dwidget* w;
};

#endif // WINDOW_H
