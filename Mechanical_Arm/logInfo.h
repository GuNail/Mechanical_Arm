#ifndef LOGINFO_H
#define LOGINFO_H
#include <QDateTime>
#include <QThread>

#define LOG                                                                                                            \
    qDebug() << __FILE__ << __LINE__ << QThread::currentThread()                                                       \
             << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << " "

#endif // LOGINFO_H
