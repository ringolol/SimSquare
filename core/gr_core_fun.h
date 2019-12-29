#ifndef GR_CORE_FUN_H
#define GR_CORE_FUN_H

#include <vector>
#include "gr_core_const.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "./logger/gr_logger.h"

using namespace std;

//

double FloatMod(double a, double b);

//block functions

void FunSource1(PAR_1);

void FunSource0(PAR_1);

void FunLogAND(PAR_1);

void FunLine(PAR_1);

void FunLogOR(PAR_1);

void FunLogXOR(PAR_1);

void FunLogNOT(PAR_1);

void FunADD(PAR_1);

void FunSPLIT(PAR_1);

void FunGAIN(PAR_1);

void FunSource(PAR_1);

void FunDiff(PAR_1);

void FunStep(PAR_1);

void FunPuls(PAR_1);

void FunSinSource(PAR_1);

void FunTime(PAR_1);

void FunSub(PAR_1);

void FunMult(PAR_1);

void FunDiv(PAR_1);

void FunSign(PAR_1);

void FunAbs(PAR_1);

void FunSqrt(PAR_1);

void FunSqr(PAR_1);

void FunPow(PAR_1);

void FunSin(PAR_1);

void FunCos(PAR_1);

void FunTan(PAR_1);

void FunAsin(PAR_1);

void FunAcos(PAR_1);

void FunAtan(PAR_1);

void FunAtan2(PAR_1);

void FunExp(PAR_1);

void FunLog10(PAR_1);

void FunLn(PAR_1);

void FunDeadZone(PAR_1);

void FunSaturation(PAR_1);

void FunHysteresis(PAR_1);

void FunBackLash(PAR_1);

void FunRSTrigger(PAR_1);

void FunCompare(PAR_1);

void FunSwitch(PAR_1);

//block itegrals

double IntINTEG(PAR_2);

double IntTRANSFER(PAR_2);

double IntHARMONIC(PAR_2);

#endif // GR_CORE_FUN_H
