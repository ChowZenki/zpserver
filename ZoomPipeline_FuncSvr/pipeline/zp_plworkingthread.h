#ifndef ZP_PLWORKINGTHREAD_H
#define ZP_PLWORKINGTHREAD_H

#include <QThread>
namespace ZPTaskEngine{

class zp_pipeline;
//Working thread, reading functions from queue,
//running tasks
class zp_plWorkingThread : public QThread
{
    Q_OBJECT
public:
    explicit zp_plWorkingThread(QObject *parent = 0);

protected:
    zp_pipeline * m_pipeline;

    bool m_bRuning;

    virtual void run();

public:
    void setStopMark();




};
}
#endif // ZP_PLWORKINGTHREAD_H
