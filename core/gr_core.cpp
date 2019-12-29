#include "gr_core.h"

Gr_Core::Gr_Core()
{

}

Gr_Core::~Gr_Core()
{
    //delete shit
}

void Gr_Core::Add(core_obj_types t, int Ni, int No, vector<double> &p,vector<double> &s,vector<vector<pair<double,double>>> *d)
{
    Gr_Logger().Get()<<"CORE. Add block";
    Gr_CoreObj o(t,GetMode(t),Ni,No,GetFun(t),GetInteg(t),NumOfStates(t),p,s,NumOfPars(t),d,NumOfDataObjs(t));
    Elems.push_back(o);
}

void Gr_Core::Link(int obj_in, int ind_in, int obj_out, int ind_out)
{
    if(obj_in>=Elems.size() || obj_out>=Elems.size() || obj_in<0 || obj_out<0)
    {
        Gr_Logger().Get(LogError)<<"CORE. Link Index out of bound; ind: "<<(obj_in>obj_out?obj_in:obj_out);
        return;
    }
    else if(ind_in>=Elems[obj_in].inputs.size() || ind_out>=Elems[obj_out].outputs.size() || ind_in<0 || ind_out<0)
    {
        Gr_Logger().Get(LogError)<<"CORE. Input or output N out of bound: "<<ind_in<<" "<<ind_out;
        return;
    }
    Elems[obj_in].Link(ind_in,&Elems[obj_out].outputs[ind_out],&Elems[obj_out].outp_r[ind_out]);
}

void Gr_Core::Reset()
{
    Gr_Logger().Get()<<"CORE. Reset objects flags & clear data";
    for (int i=0;i<Elems.size();i++)
    {
        Elems[i].Reset();
    }
}

bool Gr_Core::SetCalcPrior()
{
    Gr_Logger().Get()<<"CORE. Set calc priority";
    Reset();
    for(int i=0;i<Elems.size();i++)
    {
        ElCalc.push_back(&Elems[i]);
    }
    int n=0,n0=0;
    do{
        n0=n;
        for (int i=n;i<ElCalc.size();i++)
        {
            if(ElCalc[i]->Check())
            {
                if(i != n)
                {
                    Gr_CoreObj *o=ElCalc[n];
                    ElCalc[n]=ElCalc[i];
                    ElCalc[i]=o;
                }
                n++;
            }
        }
    }while(n!=ElCalc.size() && n!=n0);

    if(n==n0)
    {
        Gr_Logger().Get(LogError)<<"CORE. Algebraic loop occured";
        return false;
    }
    return true;
}

void Gr_Core::Calc(double T)
{
    if(!SetCalcPrior())
    {
        Gr_Logger().Get(LogError)<<"CORE. Can't set calc priority. Stop sim thread";
        emit calc_fin();
        return;
    }
    Gr_Logger().Get()<<"CORE. Start calc; Tsim: "<<T;
    time=0;

    double gr_step=T/GRAPH_MAX_LINES;
    double gr_time=0;

    bool gr_flag;
    do
    {
        //set gr flag
        if(time>=gr_time)
        {
            gr_flag=true;
            gr_time+=gr_step;
        }
        else
            gr_flag=false;

        for (int i=0;i<ElCalc.size();i++)
        {
            ElCalc[i]->Calc(time,c_meth,gr_flag);
        }
        time=time+step;
    }while(time<T);
    Gr_Logger().Get()<<"CORE. Stop sim thread";
    emit calc_fin();
    Gr_Logger().Get()<<"CORE. End calc";
}

void Gr_Core::SetStep(double s)
{
    Gr_Logger().Get()<<"CORE. Set calc step";
    step=s;
}

void Gr_Core::SetCalcMeth(calc_meths m)
{
    Gr_Logger().Get()<<"CORE. Set calc method";
    c_meth=m;
}

int Gr_Core::NumOfStates(core_obj_types t)
{
    Gr_Logger().Get()<<"CORE. Get num of states";
    switch (t)
    {
    case COT_INTEG:
        return 1;
    case COT_INTEG2:
        return 2;
    case COT_TRANSFER:
        return 1;
    case COT_HARMONIC:
        return 2;
    }
    return 0;
}

int Gr_Core::NumOfPars(core_obj_types t)
{
    Gr_Logger().Get()<<"CORE. Get num of pars";
    switch (t)
    {
    case COT_SOURCE:
    case COT_GAIN:
    case COT_DEADZONE:
    case COT_SATURATION:
    case COT_BACKLASH:
        return 1;
    case COT_TRANSFER:
    case COT_STEP:
    case COT_HYSTER:
        return 2;
    case COT_SIN_SOURCE:
    case COT_HARMONIC:
        return 3;
    case COT_PULSE:
        return 4;
    }
    return 0;
}

int Gr_Core::NumOfInputs(core_obj_types t)
{
    Gr_Logger().Get()<<"CORE. Get num of inputs";
    switch (t)
    {
    case COT_NOT:
    case COT_GAIN:
    case COT_DELAY:
    case COT_DIFF:
    case COT_INTEG:
    case COT_INTEG2:
    case COT_TRANSFER:
    case COT_HARMONIC:
    case COT_SCP:
    case COT_SIGN:
    case COT_ABS:
    case COT_SQRT:
    case COT_SQR:
    case COT_SIN:
    case COT_COS:
    case COT_TAN:
    case COT_ASIN:
    case COT_ACOS:
    case COT_ATAN:
    case COT_EXP:
    case COT_LOG10:
    case COT_LN:
    case COT_DEADZONE:
    case COT_SATURATION:
    case COT_HYSTER:
    case COT_BACKLASH:
    case COT_DISPLAY:
        return 1;
    case COT_AND:
    case COT_OR:
    case COT_XOR:
    case COT_ADD:
    case COT_SUB:
    case COT_DIV:
    case COT_MULT:
    case COT_POW:
    case COT_ATAN2:
    case COT_RSTRIGGER:
    case COT_COMPARE:
    case COT_SCP_XY:
        return 2;
    case COT_SWITCH:
    case COT_ADD3:
    case COT_MULT3:
        return 3;
    case COT_SCP_4PLOTS:
    case COT_ADD4:
        return 4;
    }
    return 0;
}
int Gr_Core::NumOfOutputs(core_obj_types t)
{
    Gr_Logger().Get()<<"CORE. Get num of outputs";
    switch (t)
    {
    case COT_S0:
    case COT_S1:
    case COT_SOURCE:
    case COT_NOT:
    case COT_GAIN:
    case COT_DELAY:
    case COT_DIFF:
    case COT_INTEG:
    case COT_AND:
    case COT_OR:
    case COT_XOR:
    case COT_ADD:
    case COT_TRANSFER:
    case COT_PULSE:
    case COT_SIN_SOURCE:
    case COT_STEP:
    case COT_TIME:
    case COT_SUB:
    case COT_DIV:
    case COT_MULT:
    case COT_SIGN:
    case COT_ABS:
    case COT_SQRT:
    case COT_SQR:
    case COT_POW:
    case COT_SIN:
    case COT_COS:
    case COT_TAN:
    case COT_ASIN:
    case COT_ACOS:
    case COT_ATAN:
    case COT_ATAN2:
    case COT_EXP:
    case COT_LOG10:
    case COT_LN:
    case COT_DEADZONE:
    case COT_SATURATION:
    case COT_HYSTER:
    case COT_BACKLASH:
    case COT_COMPARE:
    case COT_SWITCH:
    case COT_ADD3:
    case COT_ADD4:
    case COT_MULT3:
        return 1;
    case COT_INTEG2:
    case COT_HARMONIC:
    case COT_RSTRIGGER:
        return 2;
    }
    return 0;
}

void (*Gr_Core::GetFun(core_obj_types t))(PAR_1)
{
    Gr_Logger().Get()<<"CORE. Get function";
    switch (t)
    {
    case COT_S0:
        return FunSource0;
    case COT_S1:
        return FunSource1;
    case COT_SOURCE:
        return FunSource;
    case COT_AND:
        return FunLogAND;
    case COT_NOT:
        return FunLogNOT;
    case COT_OR:
        return FunLogOR;
    case COT_XOR:
        return FunLogXOR;
    case COT_ADD:
    case COT_ADD3:
    case COT_ADD4:
        return FunADD;
    case COT_GAIN:
        return FunGAIN;
    case COT_DELAY:
        return FunLine;
    case COT_DIFF:
        return FunDiff;
    case COT_STEP:
        return FunStep;
    case COT_PULSE:
        return FunPuls;
    case COT_SIN_SOURCE:
        return  FunSinSource;
    case COT_TIME:
        return  FunTime;
    case COT_SUB:
        return  FunSub;
    case COT_DIV:
        return  FunDiv;
    case COT_MULT:
    case COT_MULT3:
        return  FunMult;
    case COT_SIGN:
        return  FunSign;
    case COT_ABS:
        return  FunAbs;
    case COT_SQRT:
        return  FunSqrt;
    case COT_SQR:
        return  FunSqr;
    case COT_POW:
        return  FunPow;
    case COT_SIN:
        return  FunSin;
    case COT_COS:
        return  FunCos;
    case COT_TAN:
        return  FunTan;
    case COT_ASIN:
        return  FunAsin;
    case COT_ACOS:
        return FunAcos;
    case COT_ATAN:
        return  FunAtan;
    case COT_ATAN2:
        return  FunAtan2;
    case COT_EXP:
        return  FunExp;
    case COT_LOG10:
        return  FunLog10;
    case COT_LN:
        return  FunLn;
    case COT_DEADZONE:
        return  FunDeadZone;
    case COT_SATURATION:
        return  FunSaturation;
    case COT_HYSTER:
        return  FunHysteresis;
    case COT_BACKLASH:
        return  FunBackLash;
    case COT_RSTRIGGER:
        return FunRSTrigger;
    case COT_SWITCH:
        return FunSwitch;
    case COT_COMPARE:
        return FunCompare;
    }
    return NULL;
}

double (*Gr_Core::GetInteg(core_obj_types t))(PAR_2)
{
    Gr_Logger().Get()<<"CORE. Get integral function";
    switch (t)
    {
    case COT_INTEG:
        return IntINTEG;
    case COT_INTEG2:
        return IntINTEG;
    case COT_TRANSFER:
        return IntTRANSFER;
    case COT_HARMONIC:
        return IntHARMONIC;
    }
    return NULL;
}


core_obj_modes Gr_Core::GetMode(core_obj_types t)
{
    Gr_Logger().Get()<<"CORE. Get mode";
    switch (t)
    {
    case COT_S0:
    case COT_S1:
    case COT_SOURCE:
    case COT_DELAY:
    case COT_STEP:
    case COT_SIN_SOURCE:
    case COT_PULSE:
    case COT_TIME:
        return COM_SOURCE;
    case COT_SCP:
    case COT_SCP_XY:
    case COT_DISPLAY:
    case COT_SCP_4PLOTS:
        return COM_SCOPE;
    }
    return COM_NONE;
}

int Gr_Core::NumOfDataObjs(core_obj_types t)
{
    Gr_Logger().Get()<<"CORE. Get num of data vectors";
    switch (t)
    {
    case COT_SCP:
    case COT_SCP_XY:
    case COT_DISPLAY:
        return 1;
    case COT_SCP_4PLOTS:
        return 4;
    }
    return 0;
}

vector<string>  Gr_Core::GetParNames(core_obj_types t)
{
    vector<string> vec;
    string str("");
    switch (t)
    {
    case COT_SOURCE:
        str="Amp";
        vec.push_back(str);
        break;
    case COT_GAIN:
        str="Gain";
        vec.push_back(str);
        break;
    case COT_DEADZONE:
    case COT_BACKLASH:
        str="Interval";
        vec.push_back(str);
        break;
    case COT_SATURATION:
        str="Limit";
        vec.push_back(str);
        break;
    case COT_TRANSFER:
        str="Amp";
        vec.push_back(str);
        str="Time const";
        vec.push_back(str);
        break;
    case COT_STEP:
        str="Amp";
        vec.push_back(str);
        str="Start time";
        vec.push_back(str);
        break;
    case COT_HYSTER:
        str="Amp";
        vec.push_back(str);
        str="Interval";
        vec.push_back(str);
        break;
    case COT_SIN_SOURCE:
        str="Amp";
        vec.push_back(str);
        str="Period";
        vec.push_back(str);
        str="Phase shift";
        vec.push_back(str);
        break;
    case COT_HARMONIC:
        str="Amp";
        vec.push_back(str);
        str="Time const";
        vec.push_back(str);
        str="Damping ratio";
        vec.push_back(str);
        break;
    case COT_PULSE:
        str="Amp";
        vec.push_back(str);
        str="Period";
        vec.push_back(str);
        str="On/off ratio";
        vec.push_back(str);
        str="Start time";
        vec.push_back(str);
        break;
    }
    return vec;
}

vector<double> Gr_Core::GetInitCond(core_obj_types t)
{
    vector<double> vec;
    double c;
    switch (t)
    {
    case COT_SOURCE:
        c=1;
        vec.push_back(c);
        break;
    case COT_GAIN:
        c=1;
        vec.push_back(c);
        break;
    case COT_DEADZONE:
    case COT_BACKLASH:
        c=1;
        vec.push_back(c);
        break;
    case COT_SATURATION:
        c=1;
        vec.push_back(1);
        break;
    case COT_TRANSFER:
        c=1;
        vec.push_back(c);
        c=1;
        vec.push_back(c);
        break;
    case COT_STEP:
        c=1;
        vec.push_back(c);
        c=1;
        vec.push_back(c);
        break;
    case COT_HYSTER:
        c=1;
        vec.push_back(c);
        c=1;
        vec.push_back(c);
        break;
    case COT_SIN_SOURCE:
        c=1;
        vec.push_back(c);
        c=1;
        vec.push_back(c);
        c=0;
        vec.push_back(c);
        break;
    case COT_HARMONIC:
        c=1;
        vec.push_back(c);
        c=1;
        vec.push_back(c);
        c=0.5;
        vec.push_back(c);
        break;
    case COT_PULSE:
        c=1;
        vec.push_back(c);
        c=1;
        vec.push_back(c);
        c=0.5;
        vec.push_back(c);
        c=0;
        vec.push_back(c);
        break;
    }
    return vec;
}
