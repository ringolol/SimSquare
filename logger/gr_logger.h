#ifndef GR_LOGGER_H
#define GR_LOGGER_H

#include <string>
#include <sstream>
#include <iostream>
#include <QTime>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QStandardPaths>

//#define CAN_LOG

using namespace std;

enum LogType{LogInfo=0,LogWarn,LogError,LogDebug};

class Gr_Logger
{
public:
    Gr_Logger();
    ~Gr_Logger();
    ostringstream& Get(LogType lvl=LogInfo);
    //LogType LogLvl=LogInfo;
  protected:
    ostringstream os;
  private:
    Gr_Logger(const Gr_Logger&);
    Gr_Logger& operator =(const Gr_Logger&);
    string ToString(LogType l);
    QString file_name="//log.txt";
};

#endif // GR_LOGGER_H
