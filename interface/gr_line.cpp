#include "gr_line.h"

Gr_Line::Gr_Line()
{
    //flg_pi=false;
    //flg_po=false;
}

/*void Gr_Line::SetPi(int *xx,int xx_s,int *yy,int yy_s, int ind,int num)
{
    xi=xx;
    yi=yy;
    xi_s=xx_s;
    yi_s=yy_s;

    flg_pi=true;
    indIn=ind;
    numIn=num;
    Link();
}

void Gr_Line::SetPo(int *xx,int xx_s,int *yy,int yy_s, int ind,int num)
{
    xo=xx;
    yo=yy;
    xo_s=xx_s;
    yo_s=yy_s;

    flg_po=true;
    indOut=ind;
    numOut=num;
    Link();
}*/

void Gr_Line::SetPi(Gr_DrawCore *o, int num)
{
    Gr_Logger().Get()<<"Line. Set Input par, CalcID: "<<o->calcInd<<" Input num: "<<num;
    //flg_pi=true;
    in_obj=o;
    numIn=num;
}

void Gr_Line::SetPo(Gr_DrawCore *o, int num)
{
    Gr_Logger().Get()<<"Line. Set Output par, CalcID: "<<o->calcInd<<" Input num: "<<num;
    //flg_po=true;
    out_obj=o;
    numOut=num;
}

bool Gr_Line::Clicked(int x,int y)
{
    //check a shit case
    if(in_obj==NULL || out_obj==NULL)
        return false;

    //get Input & Output coordinates
    QPoint p1=in_obj->GetInpCoor(numIn);
    QPoint p2=out_obj->GetOutCoor(numOut);

    //find min & max for In/Out-puts
    int xmin=p1.x()<p2.x()?p1.x():p2.x();
    int xmax=p1.x()>p2.x()?p1.x():p2.x();
    int ymin=p1.y()<p2.y()?p1.y():p2.y();
    int ymax=p1.y()>p2.y()?p1.y():p2.y();

    //if line is vertical
    if(xmin==xmax)
    {
        if(y>=ymin && y<=ymax && abs(x-xmin)<=GRID_STEP*1.0/4)
        {
            return true;
        }
    }
    //otherwise
    else
    {
        //calc contour error
        double k=(p2.y()-p1.y())*1.0/(p2.x()-p1.x());
        double b=p1.y()-k*p1.x();
        double ek=(y-k*x-b)/sqrt(1+k*k);

        if(abs(ek)<=GRID_STEP*1.0/4 && x>=xmin && x<=xmax)
        {
            return true;
        }
    }
    return false;
}

void Gr_Line::ToggleHL()
{
    Gr_Logger().Get()<<"Toggle line HL";
    isHL=!isHL;
}

void Gr_Line::Draw(QPainter *p, QPaintEvent *e)
{
    if(in_obj==NULL || out_obj==NULL)
        return;
    p->save();
    if(isHL)
    {
        QPen pt=p->pen();
        pt.setWidth(pt.width()*2);
        pt.setColor(Qt::red);
        p->setPen(pt);
    }
    QPoint p_in=in_obj->GetInpCoor(numIn);
    p->drawLine(p_in,out_obj->GetOutCoor(numOut));
    p->restore();
}

/*void Gr_Line::Link()
{
    lines.clear();
    if(flg_pi && flg_po)
    {
        QLine* l1=new QLine(*xi+xi_s,*yi+yi_s,*xo+xo_s,*yo+yo_s);
        lines.push_back(l1);
    }
}*/

/*void Gr_Line::Draw(QPainter *p, QPaintEvent *e)
{
    Link();
    for (int i=0;i<lines.size();i++)
    {
        QLine lt((lines[i]->p1()*GRID_STEP+QPoint(GRID_OFFSET_X,GRID_OFFSET_Y)),lines[i]->p2()*GRID_STEP+QPoint(GRID_OFFSET_X,GRID_OFFSET_Y));
        p->drawLine(lt);
    }
    //draw arrow
    QPoint pa0=lines[0]->p1()*GRID_STEP+QPoint(GRID_OFFSET_X,GRID_OFFSET_Y);
    pa0.rx()=pa0.x()+GRID_STEP;
    QPoint pa1,pa2;
    pa1.rx()=pa0.x()-GRID_STEP/2;
    pa1.ry()=pa0.y()-GRID_STEP/3;
    pa2.rx()=pa0.x()-GRID_STEP/2;
    pa2.ry()=pa0.y()+GRID_STEP/3;
    p->drawLine(QLine(pa0,pa1));
    p->drawLine(QLine(pa0,pa2));
}*/
