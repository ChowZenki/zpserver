#include "zp_plworkingthread.h"
#include <assert.h>
#include "zp_pipeline.h"
namespace ZPTaskEngine{
zp_plWorkingThread::zp_plWorkingThread(QObject *parent) :
    QThread(parent)
{
    m_bRuning = true;
    m_pipeline = qobject_cast<zp_pipeline *>(parent);
    assert(m_pipeline != nullptr);
}

void zp_plWorkingThread::setStopMark()
{
    m_bRuning = false;
}

void zp_plWorkingThread::run()
{
    while (m_bRuning)
    {
        bool bValid = false;
        zptaskfunc funcobj = m_pipeline->popTask(&bValid);
        if (bValid)
        {
            int res = funcobj();
            if (res!=0)
                  m_pipeline->pushTask(funcobj);
        }
        else
            this->msleep(500);
    }
    m_pipeline->m_mutex_protect.lock();
    m_pipeline->m_nExistingThreads--;
    m_pipeline->m_mutex_protect.unlock();
    this->deleteLater();

}

}
