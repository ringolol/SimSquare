#ifndef GR_COREOBJ_H
#define GR_COREOBJ_H

#include "gr_core_const.h"
#include "gr_core_fun.h"
#include <vector>
#include <map>
#include "./logger/gr_logger.h"

using namespace std;

class Gr_CoreObj
{
public:
    Gr_CoreObj();
    Gr_CoreObj(core_obj_types t,core_obj_modes m, int Ni, int No, void (*f1)(PAR_1), double (*f2)(PAR_2)=NULL, int ord=0, const vector<double> &p=vector<double>(),const vector<double> &s=vector<double>(), int p_num=-1,vector<vector<pair<double,double>>> *d=NULL,int d_n=0);
    //~Gr_CoreObj();
    void Calc(double t, calc_meths m=CM_EULER, bool gr_flg=false);
    //reset flags
    void Reset();
    //link an input to an output
    void Link(int Ni,double *val, int *flg);
    //unlink an input
    //void UnLink(int Ind);
    //void SetPar(double *p);
    //check can it be calculated now or not
    bool Check();
    //set all outputs ready
    void SetOutpReady();
    //check are the inputs ready or not
    bool CheckInpReady();
    //numerical integration method
    static double RectMeth(double t1,double t2,double x1,double dx);
    static double SimsMeth(double t1,double t2,double x1,double dx1,double dx2,double dx12);
    //
    //vector<pair<double,double>>* GetScoreData();
private:
    //none integral fun
    void (*fun)(PAR_1)=NULL;
    //integral fun
    double (*diff)(PAR_2)=NULL;
public:
    //inputs
    vector <double*> inputs;
    //inputs old
    vector <double> inputs_old;
    //outputs
    vector <double> outputs;
    //input ready flags
    vector <int*> inp_r;
    //output ready flags
    vector <int> outp_r;
    //parameters
    vector <double> pars;
    //states - differentials
    vector <double> states;
    //block type
    core_obj_types type;
    //block mode
    core_obj_modes mode;
    //order of integral equasion, if 0 than none integral fun is used
    int order=0;
    int par_num=0;
    //time from the previouse step
    double told=0;
    //data for scope
    vector<vector<pair<double,double>>> *data;
    int data_num=0;
};

#endif // GR_COREOBJ_H
