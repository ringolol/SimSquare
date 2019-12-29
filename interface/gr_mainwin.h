#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGLWidget>
#include <QtOpenGL>
#include <vector>
#include <fstream>
#include <sys/stat.h>

#include "gr_const.h"
#include "gr_drawable.h"
#include "gr_line.h"
#include "./logger/gr_logger.h"

#include "./core/gr_core.h"

#include "vibrate.h"

#include <QGuiApplication>


#if defined(Q_OS_ANDROID)
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtAndroid>
#endif

#include <QInputMethod>

using namespace std;


class MainWindow : public QOpenGLWidget//QOpenGLWindow
{
    Q_OBJECT

public:
    MainWindow();
    MainWindow(QApplication *a);
    ~MainWindow();

private:
    //Events
    //mouse events
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    //gesture and tap events
    bool event(QEvent *event) override;
    //draw events
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent (QKeyEvent *event) override;


    //Draw everything
    void Anima();

    //Load windows/panels/buttons/blocks
    void LoadContent();

    //Init functions
    //void SetWindow();
    void SetDrawInstr();
    void SetPainters();
    void SetGrid();
    void SetVibrator();
    //set up timers
    void SetMainTimer();
    void SetDblClkTimer();
    void SetTapHoldTimer();
    void SetGraphTimer();

    //Mouse/tap custom handlers
    //mouse
    void MouPressEv(int px,int py);
    void MouMoveEv(int px,int py);
    void MouReleaseEv(int px,int py);
    void ZoomEv(int px,int py,double sc);
    void TapAndHoldEv();
    void NotDoubleClk();
    //clicks
    //button was clicked
    void ButtonClick(QMouseEvent *event, Gr_Drawable *o);
    //modify object: rotate/set parameters/link ...
    void ModifyObject(QMouseEvent *event, int i, int force_CT=-1);
    //click was on workspace itself
    void WorkSpaceClick();
    //old shit which i don't use
    //void CopyByDragging(QMouseEvent *event, int i);
    //click was on workspace object
    void WorkspaceObjClick(QMouseEvent *event);
    //check if the body of the object wa clicked
    bool isBodyClick(int ind);
    //reset main mouse flags/timers
    void ResetFlags();
    //resetdouble click flags/timer
    void ResetDblClk();

    //Draw functions
    void Draw(QPaintEvent *event);
    void DrawWorkSpace(QPaintEvent *event);
    void DrawInf(QPaintEvent *event);
    void DrawObjects(QPaintEvent *event);
    void DrawGrid(QPaintEvent *event);
    void DrawLines(QPaintEvent *event);
    void DrawInpHL(QPaintEvent *event);
    void UpdSizeInf();

    //Core operations
    //convert from interface to core
    void Convert();
    //calc core
    void Calc();
    //get new calc index
    int GetCalcInd();
    //start new thread and calc in it
    void StartCalc();

    //Pop-up menus
    //call menu to set up parameters and states for the object
    bool SetSAndP(Gr_DrawCore* o);
    //get sim time and sim step from the user
    bool GetSimData();

    //Work with files
    bool Save(bool PopFlg=false);
    bool Load();

    //Workspace fun
    Gr_DrawCore* AddBlock(core_obj_types ct);
    void LoadImg(Gr_DrawCore *obj);
    void UpdGrid();
    void Clear();
    void PrepareGra();

    //hightlight
    void ClearHL();
    void DeleteObjs();
    void ToggleHL(int ind);
    void ClearPutsHL();
    void SetPutsHL(int ind, int in_out,int num);

    //
    void ChangeWindow(window_index w);
    void SetIsHold(bool flg);
    void StopTapHoldTimer();
    void SetIndPressed(int ind);
    void ResetIndPressed();
    bool DoesIndExist(int ind);

public slots:
    //Save on exit or minimize
    void AppStateEv(Qt::ApplicationState state);
    void AboutToQuit();

    //update after thread death
    void StopThread();

signals:
    //is used to create calc thread
    void calc_sign(double t);

private:

    //path
    QString AppDir;
    QString  file_name="autosave.data";
    QString  dir_file;
    //android
    #if defined(Q_OS_ANDROID)
    QAndroidJniObject activity;
    QAndroidJniObject context;
    #endif

private:
    QApplication *ThisApp;
    QThread CoreThread;

    //window sizes
    int w_h=100,w_w=100;

    //cursor type
    //0 - link; 1 - rotate; 2 - magic; 3 - drag simple.
    int cursor_type=0;

    //Active window index
    window_index ActiveWin=WI_MAIN;

    //Windows
    map<window_index,Gr_Window*> Windows;
    //Graph object
    Gr_Graph *Graph=NULL;

    //Elements
    vector<Gr_DrawCore*> Drawables;
    //Lines
    vector<Gr_Line*> Lines;

    //TIMERS
    //main timer
    QTimer *mainTimer=NULL;
    //drag timer
    QTimer *dragTimer=NULL;
    //double clk timer
    QTimer *doubleClkTimer=NULL;
    //tap and hold timer
    QTimer *tapHoldTimer=NULL;
    //graph upd timer
    QTimer *graphUpd=NULL;

    //vibrator to vibrate
    Vibrator vibrator;

    //DRAW
    //pinter, brushes ,fonts and pens
    QPainter painter;
    QBrush bgBrush;
    QFont mainFont;
    QPen mainPen;
    QPen gridPen;
    QPen linePen;

    //Overlay flag (if true than show it)
    bool InfOverlay=false;
    int testCheckpoint=-1;
    QString testStr="";

    //MOUSE
    //used to check click on a panel
    //first - panelID, second - buttonID.
    pair<int,int> clk;

    //data to link two objects
    bool DotInExist=false, DotOutExist=false;
    int line_indIn,line_indOut;
    int line_numIn,line_numOut;

    //main mouse coordinates
    int mX=0, mY=0;
    //original mouse coordinates
    int mX0=0, mY0=0;

    //scroll
    bool scroll=false;
    bool may_scroll=false;
    int Yscr=0;

    //double click rotate
    bool DoubleClkFlg=false;
    bool DblRotate=false;
    int RotateInd=-1;

    //tap and hold
    bool isHold=false;

    //index of pressed object
    //used on tap/mouse release
    int ind_pressed=-1;

    //drag object
    //index of the dragging element
    int drag_ind=-1;
    //draging offset
    int drag_offset_X=0;
    int drag_offset_Y=0;
    //type of drag (0 - no drag, 1 - drag from toolbox (not existing), 2 - drag on workspace)
    int isDrag=0;

    //drag workspace
    //coordinates for offseting the grid
    int Xoff1=0, Yoff1=0;
    //offset flag
    bool isOffset=false;

    //used to create smooth grid zoom
    double grid_step_smooth=GRID_STEP0;
    double grid_offset_smoth_X=0, grid_offset_smoth_Y=0;

    //smoth grid grab
    //...

private:
    //CORE
    //core object
    Gr_Core *core=NULL;
    //core index for a new element
    int calcInd=0;
    //sim time and sim step
    double Tsim=1.0;
    double Ssim=0.001;
};

#endif // MAINWINDOW_H
