#ifndef GR_LINE_H
#define GR_LINE_H

#include <QLine>
#include <vector>
#include "gr_comfun.h"
#include <QPainter>
#include <QPaintEvent>
#include "gr_drawable.h"
#include "./logger/gr_logger.h"

using namespace std;

class Gr_Line
{
public:
    Gr_Line();

    //draw lines
    void Draw(QPainter *p, QPaintEvent *e);

    //set input coordinates and data
    //void SetPi(int *xx,int xx_s,int *yy,int yy_s, int ind, int num);

    //set output coordinates and data
    //void SetPo(int *xx,int xx_s,int *yy,int yy_s, int ind, int num);

    void SetPi(Gr_DrawCore *o, int num);
    void SetPo(Gr_DrawCore *o, int num);

    bool Clicked(int x,int y);

    void ToggleHL();

public:
    Gr_DrawCore *in_obj=NULL,*out_obj=NULL;
    //bool flg_pi=false,flg_po=false;

    //lines (now there is only ine line)
    vector <QLine*> lines;

    //data for interface
    //x's, y's and offsets
    int *xi,*yi,*xo,*yo;
    int xi_s,yi_s,xo_s,yo_s;

    //data for core (indexes,puts numbers)
    int numIn=-1;
    int numOut=-1;

    //hightlight
    bool isHL=false;
};

#endif // GR_LINE_H
