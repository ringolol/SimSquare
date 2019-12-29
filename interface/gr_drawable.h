#ifndef GR_DRAWABLE_H
#define GR_DRAWABLE_H

#include <QtOpenGL>
#include "gr_const.h"
#include "gr_comfun.h"
#include <vector>

#include "./core/gr_core_const.h"
#include "./core/gr_core.h"

using namespace std;

//The father
class Gr_Drawable
{
public:
    Gr_Drawable();
    Gr_Drawable(int xx, int yy, int ww, int hh, drawable_types tt=DRWABLE_STATIC, drawable_events ev=DE_NONE);
    void Draw(QPainter *painter, QPaintEvent *event, int dx=0, int dy=0, int sx=1, int sy=1);
    //class interface
    int GetX();
    int GetY();
    int GetWidth();
    int GetHeight();
    void SetX(int a);
    void SetY(int a);
    void SetSize(int ww,int hh);
    void SetPos(int xx,int yy);

    drawable_types GetType();
    void SetType(drawable_types a);
    //was it clicked?
    bool Clicked(int xx, int yy);
    //does it has a linked image
    bool HasImg();
    //get an image
    QPixmap* GetImg();
    //set an image
    void SetImg(QPixmap * timg);

    //
    void Show(bool flg=true);

    //load an image from a file
    void LoadImg(const QString str);

public:
    //left top corner coordinates
    int x,y;
    //original
    int x0,y0,h0,w0;
    //width and height of the object
    int width,height;
    bool isDrawn=true;
    bool DrawBorder=false;
    //hightlight
    bool wasClicked=false;

public:
    //type of the object
    drawable_types type;
    //image linked to the object
    QPixmap *img;
    //image to draw
    QPixmap *imgDraw;
    //if there is an image linked to the object, the flag is true
    bool ImgFlag=false;

public:
    //btn event
    drawable_events event;
    drawable_class class_type;
};

//=================================================================================================================================================
//Class for workspace elements
class Gr_DrawCore:public Gr_Drawable
{
public:
    vector<vector<pair<double,double>>> data;
    //number of inputs and outpust accordingly
    int inpN,outpN;
    //img width
    int width_img;
    vector<double> pars;
    vector<double> states;
    core_obj_types core_type=COT_NONE;
    bool isStatic=false;
    int calcInd=-1;
    bool isRotated=false;
    double panel_grid_step=GRID_STEP0;
    double panel_grid_off_x=0;
    double panel_grid_off_y=0;
    int panel_dx=0,panel_dy=0;

    int putWasClicked=0;
    int clickedPutInd=-1;

    //class interface
    int GetX();
    int GetY();
    int GetWidth();
    int GetWidthImg();
    int GetHeight();
    void SetX(int a);
    void SetY(int a);
    int GetNi();
    int GetNo();
    QPoint GetInpCoor(int ind);
    QPoint GetOutCoor(int ind);
    int InputClicked(int xx, int yy);
    int OutputClicked(int xx, int yy);
    void Rotate();
    void SetPos(int xx,int yy);
    void SetSize(int ww,int hh);

    //was it clicked?
    bool Clicked(int xx, int yy);
    //load an image from a file
    void LoadImg(const QString str);
    void SetHL(int p,int ind);
    void ResetHL();

    Gr_DrawCore(int xx, int yy, int Ni, int No, drawable_types tt,core_obj_types ct=COT_NONE,bool stflg=false,drawable_events ev=DE_NONE);
    Gr_DrawCore(Gr_DrawCore* drawt);
    void Draw(QPainter *painter, QPaintEvent *event,int xx=0, int yy=0);
};

//=================================================================================================================================================
//Panels
class Gr_Panel:public Gr_Drawable
{
public:
    bool static_w=true,static_h=true;
    bool resize_cont=false;
    bool KeepRatio=true;

    bool top=false;
    bool bottom=false;
    bool left=false;
    bool right=false;
    bool mid_horiz=false;
    bool mid_vert=false;



    vector<Gr_Drawable*> Objects;
    Gr_Panel(int xx=0, int yy=0, int ww=1, int hh=1, drawable_types tt=DRWABLE_STATIC);
    void Scale(double sx,double sy);
    void Move(double ww,double ww0, double wh,double wh0);
    void AddObj(Gr_Drawable* o);
    void Clear();
    void Draw(QPainter *painter, QPaintEvent *event);
    void Draw(QPainter *painter, QPaintEvent *event,int xx, int yy);
    //bool ObjClicked(int ind,int xx,int yy);
    int ObjClicked(int xx,int yy);
    void SetScalable();
};

//=================================================================================================================================================
//Windows
class Gr_Window:public Gr_Drawable
{
public:
    Gr_Window();
    Gr_Window(int xx, int yy, int ww, int hh, drawable_types tt);
    vector<Gr_Panel*> Panels;
    void Draw(QPainter *painter, QPaintEvent *event);
    pair<int,int> ObjClicked(int xx,int yy);
    void Upd(int ww,int hh);
};

//=================================================================================================================================================
//Graph window
class Gr_Graph:public Gr_Window
{
public:
    bool isPrepared=false;
    vector<vector<pair<double,double>>> *data_all=NULL;
    vector<pair<double,double>> *data=NULL;
    vector<pair<int,int>> plot;
    vector<vector<pair<int,int>>> plot2;
    double xmin,xmax,ymin,ymax;
    double h_x,h_y;
    int nod_x,nod_y;
    int x0,y0;
    int gra_w,gra_h,gra_x,gra_y;
    int NumXLab=5;
    int NumYLab=5;

    Gr_Graph();
    Gr_Graph(int xx, int yy, int ww, int hh, drawable_types tt);
    void SetData(vector<vector<pair<double,double>>> *d);
    void Prepare(bool newflg=true);
    void Draw(QPainter *painter, QPaintEvent *event);
};

#endif // GR_DRAWABLE_H
