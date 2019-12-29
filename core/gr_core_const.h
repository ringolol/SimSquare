#ifndef GR_CORE_CONST_H
#define GR_CORE_CONST_H

enum core_obj_types{//system
                    COT_NONE=0,
                    //source
                    COT_SOURCE=100,COT_STEP,COT_PULSE,COT_SIN_SOURCE,COT_TIME,
                    //scope
                    COT_SCP=200,COT_DISPLAY,COT_SCP_XY,COT_SCP_4PLOTS,
                    //logic
                    COT_S0=300,COT_S1,COT_AND,COT_OR,COT_NOT,COT_XOR,COT_COMPARE,COT_SWITCH,
                    COT_RSTRIGGER,
                    //math
                    COT_ADD=400,COT_ADD3,COT_ADD4,COT_SUB,COT_MULT,COT_MULT3,COT_DIV,
                    COT_GAIN,COT_DELAY,COT_SQRT,COT_SQR,COT_DIFF,COT_SIGN,COT_ABS,
                    COT_POW,COT_SIN,COT_COS,COT_TAN,COT_ASIN,COT_ACOS,COT_ATAN,
                    COT_ATAN2,COT_EXP,COT_LOG10,COT_LN,
                    //linear
                    COT_INTEG=500,COT_INTEG2,COT_TRANSFER,COT_HARMONIC,
                    //nonlinear
                    COT_BACKLASH=600,COT_SATURATION,COT_HYSTER,COT_DEADZONE};

enum core_obj_modes {COM_NONE=0,COM_SOURCE,COM_SCOPE};
enum calc_meths {CM_EULER=0, CM_SIMPSON, CM_RKF};

//a shorter way to write parameters
#define PAR_1 double told, double t, vector <double*> &in, vector <double> &in_old, vector <double> &out, vector <double> &p
#define PAR_2 double told, double t, vector <double*> &in, vector <double> &in_old, vector <double> &st, vector <double> &p

//limit data array
#define GRAPH_MAX_LINES 1000

#endif // GR_CORE_CONST_H
