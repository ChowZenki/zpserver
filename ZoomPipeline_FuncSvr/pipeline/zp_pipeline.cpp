#include "zp_pipeline.h"
namespace ZPTaskEngine{
zp_pipeline::zp_pipeline(QObject *parent) :
    QObject(parent)
{
}

int zp_pipeline::addThreads(int nThreads)
{
    if (nThreads>=1 && nThreads <=128)
    {
        for (int i=0;i<nThreads;i++)
        {
            zp_plWorkingThread * thread = new zp_plWorkingThread(this);
            m_vec_workingThreads.push_back(thread);
            thread->start();
        }
    }
     return m_vec_workingThreads.size();
}

//remove n threads and kill them.nthreads=-1 means kill all.
int zp_pipeline::removeThreads(int nThreads)
{
    int nsz =  m_vec_workingThreads.size();
    if (nThreads<0 || nThreads>nsz)
        nThreads = nsz;

    for (int i=0;i<nThreads;i++ )
    {
        m_vec_workingThreads.last()->setStopMark();
        m_vec_workingThreads.pop_back();
    }
    return m_vec_workingThreads.size();
}

//Threads call this function to get next task, task will be popped from list.
zptaskfunc zp_pipeline::popTask( bool * bValid)
{
    *bValid = false;
    zptaskfunc funcres;
    m_mutex_protect.lock();
    if (m_list_tasks.empty()==false)
    {
        funcres =* m_list_tasks.begin();
        m_list_tasks.pop_front();
        *bValid = true;
    }
     m_mutex_protect.unlock();
    return funcres;
}

//Call this function to insert func
void zp_pipeline::pushTask(zptaskfunc task)
{
    m_mutex_protect.lock();
    m_list_tasks.push_back(task);
    m_mutex_protect.unlock();

}

int zp_pipeline::threadsCount()
{
    return m_vec_workingThreads.size();
}

int  zp_pipeline::payload()
{
    int res = 0;
    m_mutex_protect.lock();
    res = m_list_tasks.size();
    m_mutex_protect.unlock();

    return res;
}

}
