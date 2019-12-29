#include "gr_logger.h"

Gr_Logger::Gr_Logger()
{

}

Gr_Logger::~Gr_Logger()
{
#ifdef CAN_LOG
    os<<endl;
    fprintf(stderr, "%s", os.str().c_str());
    fflush(stderr);
    QString dir=QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QFile f(dir+file_name);
    if(!f.open(QIODevice::WriteOnly|QIODevice::Append|QIODevice::Text)) return;
    QTextStream out(&f);
    out<<os.str().c_str();
    f.close();
#endif
}

std::ostringstream& Gr_Logger::Get(LogType lvl)
{
    os  << "- "
        <<QTime::currentTime().toString().toStdString()
        <<" "<<ToString(lvl)<<" ";
    return os;
}

string Gr_Logger::ToString(LogType l)
{
    string s;
    switch (l)
    {
    case LogInfo:
        s="INFO ";
        break;
    case LogWarn:
        s="WARN ";
        break;
    case LogError:
        s="ERROR";
        break;
    case LogDebug:
        s="DEBUG";
        break;
    default:
        s="NONE ";
        break;
    }
    return s;
}

