#include "gr_coreobj.h"

Gr_CoreObj::Gr_CoreObj()
{

}

Gr_CoreObj::Gr_CoreObj(core_obj_types t, core_obj_modes m, int Ni, int No, void (*f1)(PAR_1), double (*f2)(PAR_2), int ord,const vector<double> &p,const vector<double> &s, int p_num,vector<vector<pair<double,double>>> *d,int d_n)
{
    Gr_Logger().Get()<<"CORE OBJ. Core object contructor; type: "<<(int)t;
    type=t;
    mode=m;
    order=ord;
    par_num=p_num;
    data=d;
    data_num=d_n;

    for (int i=0;i<Ni;i++)
    {
        inputs.push_back(new double(0));
        inp_r.push_back(new int(false));
        inputs_old.push_back(false);
    }

    for (int i=0;i<No;i++)
    {
        outputs.push_back(0);
        outp_r.push_back(false);
    }


    for (int i=0;i<order;i++)
    {
        double s_t;
        if(s.size()==order)
            s_t=s[i];
        else
        {
            s_t=0;
            Gr_Logger().Get(LogError)<<"CORE OBJ. Not enough states";
        }
        states.push_back(s_t);
    }

    if(outputs.size()==states.size())
    {
        for(int i=0;i<states.size();i++)
        {
            outputs[i]=states[i];
        }
    }
    else if(outputs.size()>0)
    {
        outputs[0]=0;
    }

    for(int i=0;i<par_num;i++)
    {
        double pr;
        if(p.size()==par_num)
            pr=p[i];
        else
        {
            pr=0;
            Gr_Logger().Get(LogError)<<"CORE OBJ. Not enough pars";
        }
        pars.push_back(pr);
    }

    fun=f1;
    diff=f2;
}

void Gr_CoreObj::Reset()
{
    Gr_Logger().Get()<<"CORE OBJ. Reset; type: "<<(int)type;
    for (int i=0;i<outputs.size();i++)
    {
        outp_r[i]=false;
    }
    told=-0.001;
    //clear scope data
    for(int i=0;i<data_num;i++)
    {
        data->at(i).clear();
    }
}

void Gr_CoreObj::Link(int Ni,double *val, int *flg)
{
    Gr_Logger().Get()<<"CORE OBJ. Link; type: "<<(int)type<<" inpN: "<<Ni;
    inputs[Ni]=val;
    inp_r[Ni]=flg;
}

void Gr_CoreObj::Calc(double t, calc_meths m,bool gr_flg)
{
    //add scope data
    if(mode==COM_SCOPE)
    {
        if(type==COT_SCP && gr_flg)
        {
            data->at(0).push_back(pair<double,double>(t,*inputs[0]));
        }
        else if(type==COT_DISPLAY)
        {
            if(data->at(0).size()>0)
                data->at(0).at(0)=pair<double,double>(t,*inputs[0]);
            else
                data->at(0).push_back(pair<double,double>(t,*inputs[0]));
        }
        else if(type==COT_SCP_XY && gr_flg)
        {
            data->at(0).push_back(pair<double,double>(*inputs[0],*inputs[1]));
        }
        else if(COT_SCP_4PLOTS && gr_flg)
        {
            data->at(0).push_back(pair<double,double>(t,*inputs[0]));
            data->at(1).push_back(pair<double,double>(t,*inputs[1]));
            data->at(2).push_back(pair<double,double>(t,*inputs[2]));
            data->at(3).push_back(pair<double,double>(t,*inputs[3]));
        }
    }
    //we do that to plot first conditions
    if(t==0) return;
    //calc
    if(order==0)
    {
        if(fun!=NULL)
            fun(told,t,inputs,inputs_old,outputs,pars);
    }
    else
    {
        if(diff!=NULL)
        {
            double dt=t-told;
            double dx1=0,dx2=0,dx12=0;
            if(m==CM_SIMPSON)
            {
                dx1=diff(told,told,inputs,inputs_old,states,pars);
                dx2=diff(told,t,inputs,inputs_old,states,pars);
            }
            dx12=diff(told,told+dt/2,inputs,inputs_old,states,pars);
            for(int i=states.size()-1;i>=0;i--)
            {
                if(m==CM_EULER)
                {
                    dx12=RectMeth(told,t,states[i],dx12);
                    states[i]=dx12;
                }
                else if(m==CM_SIMPSON)
                {
                    double x_t=SimsMeth(told,t,states[i],dx1,dx2,dx12);
                    dx1=states[i];
                    dx2=x_t;
                    dx12=SimsMeth(told,told+dt/2,states[i],dx1,dx2,dx12);
                    states[i]=dx2;
                }
            }
            if(outputs.size()==states.size())
            {
                for(int i=0;i<states.size();i++)
                {
                    outputs[i]=states[i];
                }
            }
            else if(outputs.size()>0)
            {
                outputs[0]=states[0];
            }
            else
                Gr_Logger().Get(LogError)<<"CORE OBJ. Not enough outputs";
        }
    }
    for(int i=0;i<inputs.size();i++)
    {
        inputs_old[i]=*inputs[i];
    }
    told=t;
}

bool Gr_CoreObj::Check()
{
    Gr_Logger().Get()<<"CORE OBJ. Check; type: "<<(int)type;
    if(mode==COM_SOURCE || mode==COM_SCOPE)
    {
        SetOutpReady();
        return true;
    }
    if(CheckInpReady())
    {
        SetOutpReady();
        return true;
    }
    return false;
}

void Gr_CoreObj::SetOutpReady()
{
    for (int i=0;i<outp_r.size();i++)
    {
        outp_r[i]=true;
    }
}

bool Gr_CoreObj::CheckInpReady()
{
    for (int i=0;i<inp_r.size();i++)
    {
        if(*inp_r[i]==false) return false;
    }
    return true;
}

double Gr_CoreObj::RectMeth(double t1,double t2,double x1,double dx)
{
    double dt=t2-t1;
    return x1+dx*dt;
}

double Gr_CoreObj::SimsMeth(double t1,double t2,double x1,double dx1,double dx2,double dx12)
{
    double dt=t2-t1;
    return x1+dt*(dx1+4*dx12+dx2)/6;
}


