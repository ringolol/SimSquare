#include "gr_core_fun.h"

using namespace std;

double FloatMod(double a, double b)
{
    return (a/b-(int)(a/b))*b;
}

bool FromDoubleToBool(double a)
{
    return a>=0.5?1.0:0.0;
}

//=================================

void FunSource1(PAR_1)
{
    out[0]=1.0;
}

void FunSource0(PAR_1)
{
    out[0]=0.0;
}

void FunLogAND(PAR_1)
{
    out[0]=FromDoubleToBool(*in[0]) & FromDoubleToBool(*in[1]);
}

void FunLine(PAR_1)
{
    out[0]=*in[0];
}

void FunLogOR(PAR_1)
{
    out[0]=FromDoubleToBool(*in[0]) | FromDoubleToBool(*in[1]);
}

void FunLogXOR(PAR_1)
{
    out[0]=FromDoubleToBool(*in[0]) ^ FromDoubleToBool(*in[1]);
}

void FunLogNOT(PAR_1)
{
    out[0]=!FromDoubleToBool(*in[0]);
}

void FunADD(PAR_1)
{
    double sum=0;
    for(int i=0;i<in.size();i++)
    {
        sum+=*in[i];
    }
    out[0]=sum;
}

void FunSPLIT(PAR_1)
{
    for(int i=0;i<out.size();i++)
    {
        out[i]=*in[0];
    }
}

void FunGAIN(PAR_1)
{
    if(p.size()>0)
        out[0]=(p[0])*(*in[0]);
    else
        Gr_Logger().Get(LogError)<<"FUN. GAIN doesn't have a parameter"<<endl;
}

void FunSource(PAR_1)
{
    if(p.size()>0)
        out[0]=(p[0]);
    else
        Gr_Logger().Get(LogError)<<"FUN. SOURCE doesn't have a parameter"<<endl;
}

void FunDiff(PAR_1)
{
    if(t==told)
        out[0]=0;
    else
        out[0]=(*in[0]-in_old[0])/(t-told);
}

void FunStep(PAR_1)
{
    // 0 - A
    // 1 - dt
    if(t<p[1])
        out[0]=0;
    else
        out[0]=p[0];
}

void FunPuls(PAR_1)
{
    // 0 - A
    // 1 - T
    // 2 - alf
    // 3 - dt
    double v=FloatMod(t-p[3],p[1]);
    if(v/(p[1])>=(1-p[2]))
        out[0]=p[0];
    else
        out[0]=0;
}

void FunSinSource(PAR_1)
{
    // 0 - A
    // 1 - f
    // 2 - fi0
    double w=2*M_PI*(p[1]);
    out[0]=p[0]*sin(w*t+p[2]);
}

void FunTime(PAR_1)
{
    out[0]=t;
}

void FunSub(PAR_1)
{
    out[0]=*in[0]-*in[1];
}

void FunMult(PAR_1)
{
    double mul=1.0;
    for(int i=0;i<in.size();i++)
    {
        mul*=*in[i];
    }
    out[0]=mul;
}

void FunDiv(PAR_1)
{
    // *out[1] - error flag
    if(*in[1]!=0)
        out[0]=*in[0]/(*in[1]);
    else
        out[0]=0;
}

void FunSign(PAR_1)
{
    if(*in[0]==0)
        out[0]=0.5;
    else if(*in[0]>0)
    {
        out[0]=1;
    }
    else
    {
        out[0]=-1;
    }
}

void FunAbs(PAR_1)
{
    out[0]=abs(*in[0]);
}

void FunSqrt(PAR_1)
{
    if(*in[0]>=0)
        out[0]=sqrt(*in[0]);
    else
        out[0]=0;
}

void FunSqr(PAR_1)
{
    out[0]=*in[0]*(*in[0]);
}

void FunPow(PAR_1)
{
    out[0]=pow(*in[0],*in[1]);
}

void FunSin(PAR_1)
{
    out[0]=sin(*in[0]);
}

void FunCos(PAR_1)
{
    out[0]=cos(*in[0]);
}

void FunTan(PAR_1)
{
    out[0]=tan(*in[0]);
}

void FunAsin(PAR_1)
{
    out[0]=asin(*in[0]);
}

void FunAcos(PAR_1)
{
    out[0]=acos(*in[0]);
}

void FunAtan(PAR_1)
{
    out[0]=atan(*in[0]);
}

void FunAtan2(PAR_1)
{
    out[0]=atan2(*in[0],*in[1]);
}

void FunExp(PAR_1)
{
    out[0]=exp(*in[0]);
}

void FunLog10(PAR_1)
{
    out[0]=log10(*in[0]);
}

void FunLn(PAR_1)
{
    out[0]=log(*in[0]);
}

void FunDeadZone(PAR_1)
{
    if(*in[0]>p[0])
        out[0]=*in[0]-p[0];
    else if(*in[0]<-p[0])
        out[0]=*in[0]+p[0];
    else
        out[0]=0;
}

void FunSaturation(PAR_1)
{
    if(*in[0]>p[0])
        out[0]=p[0];
    else if(*in[0]<-p[0])
        out[0]=-p[0];
    else
        out[0]=*in[0];
}

void FunHysteresis(PAR_1)
{
    if(out[0]==p[0])
    {
        if(*in[0]<=-p[1])
        {
            out[0]=-p[0];
        }
    }
    else if(out[0]==-p[0])
    {
        if(*in[0]>=p[1])
        {
            out[0]=p[0];
        }
    }
    else
    {
        if(*in[0]>0) out[0]=p[0];
        else out[0]=-p[0];
    }
}

void FunBackLash(PAR_1)
{
    if(out[0]<*in[0]-p[0])
        out[0]=*in[0]-p[0];
    else if(out[0]>*in[0]+p[0])
        out[0]=*in[0]+p[0];
}

void FunRSTrigger(PAR_1)
{
    bool set_in=FromDoubleToBool(*in[0]);
    bool reset_in=FromDoubleToBool(*in[1]);

    if(set_in && !reset_in)
        out[0]=1;
    else if(!set_in && reset_in)
        out[0]=0;
    else if(set_in && reset_in)
        out[0]=1;

    out[1]=!FromDoubleToBool(out[0]);
}

void FunCompare(PAR_1)
{
    if(*in[0]>*in[1]) out[0]=1;
    else out[0]=0;
}

void FunSwitch(PAR_1)
{
    if(FromDoubleToBool(*in[1])) out[0]=*in[0];
    else out[0]=*in[2];
}

//================================================================================================

double IntINTEG(PAR_2)
{
    return *in[0];
}

double IntTRANSFER(PAR_2)
{
    //p0 - k
    //p1 - T
    return ((*in[0])*(p[0])-(st[0]))/(p[1]);
}

double IntHARMONIC(PAR_2)
{
    //p0 - k
    //p1 - T
    //p2 - teta ( e )
    return ((*in[0])*(p[0])-2*(p[1])*(p[2])*(st[1])-(st[0]))/(p[1]);
}
