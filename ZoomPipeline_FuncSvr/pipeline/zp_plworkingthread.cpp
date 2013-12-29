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
}

void zp_plWorkingThread::setStopMark()
{
    m_bRuning = false;
}

void zp_plWorkingThread::FetchNewTask(zp_plWorkingThread * obj,zp_plTaskBase * ptr)
{
    if (obj != this)
        return;
    if (m_bRuning)
    {
        if (!ptr)
        {
            QThread::currentThread()->msleep(500);
            emit taskFinished(this);
        }
        else
        {
            int res = ptr->run();
            if (res!=0)
                m_pipeline->pushTask(ptr);
            ptr->moveToThread(m_pipeline->thread());
            emit taskFinished(this);
        }
    }

    else
    {
        m_pipeline->m_mutex_protect.lock();
        m_pipeline->m_nExistingThreads--;
        m_pipeline->m_mutex_protect.unlock();
        this->deleteLater();
        QThread::currentThread()->quit();
    }


}

}
