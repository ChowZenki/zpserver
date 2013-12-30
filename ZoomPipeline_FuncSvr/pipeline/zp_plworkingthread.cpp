#include "zp_plworkingthread.h"
#include <assert.h>
#include "zp_pipeline.h"
namespace ZPTaskEngine{
zp_plWorkingThread::zp_plWorkingThread(zp_pipeline * pipl,QObject *parent) :
    QObject(parent)
{
    m_bRuning = true;
    m_pipeline = pipl;
    assert(m_pipeline != nullptr);
    m_bBusy = false;
}

void zp_plWorkingThread::setStopMark(zp_plWorkingThread * obj)
{
    if (obj != this)
        return;
    m_bRuning = false;
    m_pipeline->m_mutex_protect.lock();
    m_pipeline->m_nExistingThreads--;
    m_pipeline->m_mutex_protect.unlock();
    this->deleteLater();
    QThread::currentThread()->quit();
}

void zp_plWorkingThread::FetchNewTask(zp_plWorkingThread * obj,zp_plTaskBase * ptr)
{

    if (obj != this)
        return;
    if (m_bRuning)
    {
        m_bBusy = true;
        int res = ptr->run();
        m_bBusy = false;
        emit taskFinished(this,ptr,res);

    }

}

}
