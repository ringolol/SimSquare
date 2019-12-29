#ifndef GR_CORE_H
#define GR_CORE_H

#include <vector>
#include "gr_coreobj.h"
#include "gr_core_const.h"
#include <iostream>
#include <QWidget>
#include "./logger/gr_logger.h"
#include <string>

using namespace std;

class Gr_Core:public QObject
{
    Q_OBJECT
public:
    Gr_Core();
    ~Gr_Core();
    //add new element
    void Add(core_obj_types t, int Ni, int No,vector<double> &p,vector<double> &s,vector<vector<pair<double,double>>> *d);
    //link an input and an output
    void Link(int obj_in, int ind_in, int obj_out, int ind_out);
    //unlink an input and an output
    //void UnLink(int obj, int ind);
    //simulate
public slots:
    void Calc(double T);
signals:
    void calc_fin();
public:
    //reset elements flags
    void Reset();
    //find the right way to calculate and detect arithmetic loops
    bool SetCalcPrior();
    //set calculation step
    void SetStep(double s);
    //set calculate method
    void SetCalcMeth(calc_meths m);

    //new
    static int NumOfStates(core_obj_types t);
    static int NumOfPars(core_obj_types t);
    static int NumOfInputs(core_obj_types t);
    static int NumOfOutputs(core_obj_types t);
    static void (*GetFun(core_obj_types t))(PAR_1);
    static double (*GetInteg(core_obj_types t))(PAR_2);
    static core_obj_modes GetMode(core_obj_types t);
    static int NumOfDataObjs(core_obj_types t);
    static vector<string> GetParNames(core_obj_types t);
    static vector<double> GetInitCond(core_obj_types t);
public:
    //element massive
    vector <Gr_CoreObj> Elems;
    //element massive with the right prioritet order
    vector <Gr_CoreObj*> ElCalc;
    //simulation time
    double time=0;
    //simulation step
    double step=0.001;
    //calc meth
    calc_meths c_meth=CM_EULER;
};

#endif // GR_CORE_H
