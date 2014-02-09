#include "st_clientnode.h"
#include "st_client_table.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <assert.h>
namespace SmartLink{
//0x0001 msg, stMsg_HostRegistReq
bool st_clientNode::RegisitNewNode()
{
    SMARTLINK_MSG_APP * pAppLayer =
            (SMARTLINK_MSG_APP *)(
                ((const char *)(m_currentBlock.constData()))
                +sizeof(SMARTLINK_MSG)-1);
    //Check the database, find current equipment info
    QSqlDatabase db = m_pClientTable->dbRes()->databse(m_pClientTable->Database_UserAcct());
    if (db.isValid()==true && db.isOpen()==true )
    {
        QSqlQuery query(db);
        QString sql = QString ("select * from instruments where equip_id = '%1'");
        if (true==query.exec(sql))
        {

        }
        else
        {

        }
    }
    else
    {
        //Server db is currently not accessable, wait.
    }
    return true;
}
}
