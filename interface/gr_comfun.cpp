#include "gr_comfun.h"

int GridFit(double xx)
{
    return ((int)(xx*1.0/GRID_STEP+0.5));
}

double sign(double xx)
{
    return xx>=0?1:-1;
}

double Get1cm()
{
    // 0.4 = 1/2.54
    return 0.4*dpi;
}

double FancyFloor(double x,int digits)
{
    digits=digits>0?digits-1:digits;
    long double a=pow(10,-digits);
    long double t=int(x*a);
    t=t/a;
    return t;
}

int NumOfDigits(double x)
{
    int i=0;
    if(x==0) return 0;
    while(int(x)!=0)
    {
        x/=10;
        i++;
    }
    if(i==0)
    {
        while(int(x)==0)
        {
            x*=10;
            i--;
        }
    }
    return i;
}

double GraphRound(double x)
{
    QString text=QString::number(x,'E',2);
    return text.toFloat();
}

string WinToString(window_index ind)
{
    //WI_MAIN=0,WI_WS,WI_BT,WI_AB,WI_GR
    string str;
    switch (ind)
    {
    case WI_MAIN:
        str="MainMenu";
        break;
    case WI_WS:
        str="WorkSpace";
        break;
    case WI_BT:
        str="BlockType";
        break;
    case WI_AB:
        str="AddBlock";
        break;
    case WI_GR:
        str="Graph";
        break;
    default:
        str="NONE";
        break;
    }
    return str;
}

string BoolToStrint(bool flg)
{
    return flg?"true":"false";
}




