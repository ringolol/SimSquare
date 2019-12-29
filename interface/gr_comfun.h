#ifndef GR_COMFUN_H
#define GR_COMFUN_H

#include "gr_const.h"
#include <math.h>
#include <qstring.h>
#include <string.h>

using namespace std;

int GridFit(double xx);

double sign(double xx);

double Get1cm();

//ceiling to the chosen digit
double FancyFloor(double x,int digits=0);

//get the number of digits
int NumOfDigits(double x);

//round to the highest digit
//ex: GraphRound(1400)=1000; GraphRound(2677)=3000
double GraphRound(double x);

string WinToString(window_index);

string BoolToStrint(bool flg);

#endif // GR_COMFUN_H
