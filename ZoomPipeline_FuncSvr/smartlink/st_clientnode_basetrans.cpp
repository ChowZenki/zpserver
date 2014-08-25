#include "st_clientnode_basetrans.h"
#include "st_client_table.h"
#include <assert.h>
namespace ParkinglotsSvr{
	st_clientNode_baseTrans::st_clientNode_baseTrans(st_client_table * pClientTable, QObject * pClientSock ,QObject *parent) :
		zp_plTaskBase(parent)
	{
		m_bUUIDRecieved = false;
		m_currentReadOffset = 0;
		m_currentMessageSize = 0;
		m_pClientSock = pClientSock;
		m_uuid = 0xffffffff;//Not Valid
		m_pClientTable = pClientTable;
		bTermSet = false;
		m_last_Report = QDateTime::currentDateTime();
		m_remotePort = 0;
	}
	void st_clientNode_baseTrans::setRemoteInfo(QString addr, quint16 port)
	{
		m_remotePort = port;
		m_remoteAddress = addr;
	}
	QString st_clientNode_baseTrans::peerInfo()
	{
		return m_remoteAddress + ":" + m_remotePort;
	}

	quint32 st_clientNode_baseTrans::uuid()
	{
		return m_uuid;
	}
	QObject * st_clientNode_baseTrans::sock()
	{
		return m_pClientSock;
	}
	bool st_clientNode_baseTrans::uuidValid()
	{
		return m_bUUIDRecieved;
	}
	QDateTime st_clientNode_baseTrans::lastActiveTime()
	{
		return m_last_Report;
	}
	qint32 st_clientNode_baseTrans::bytesLeft()
	{
		return m_currentHeader.DataLen + sizeof(PKLTS_Trans_Header)
				-m_currentMessageSize ;
	}
	//judge whether id is valid.
	bool st_clientNode_baseTrans::bIsValidUserId(quint32 id)
	{
		return id >=(unsigned int)0x00000002 && id <=(unsigned int)0xFFFFFFFE;
	}

	//The main functional method, will run in thread pool
	int st_clientNode_baseTrans::run()
	{
		if (bTermSet==true)
		{
			//qDebug()<<QString("%1(%2) Node Martked Deleted, return.\n").arg((unsigned int)this).arg(ref());
			return 0;
		}
		int nCurrSz = -1;
		int nMessage = m_nMessageBlockSize;
		while (--nMessage>=0 && nCurrSz!=0  )
		{
			QByteArray block;
			m_mutex_rawData.lock();
			if (m_list_RawData.size())
				block =  *m_list_RawData.begin();
			m_mutex_rawData.unlock();
			if (block.isEmpty()==false && block.isNull()==false)
			{
				m_currentReadOffset = filter_message(block,m_currentReadOffset);
				if (m_currentReadOffset >= block.size())
				{
					m_mutex_rawData.lock();
					if (m_list_RawData.empty()==false)
						m_list_RawData.pop_front();
					else
						assert(false);
					m_currentReadOffset = 0;
					m_mutex_rawData.unlock();
				}
			}
			else
			{
				m_mutex_rawData.lock();
				//pop empty cabs
				if (m_list_RawData.empty()==false)
					m_list_RawData.pop_front();
				m_mutex_rawData.unlock();
			}
			m_mutex_rawData.lock();
			nCurrSz = m_list_RawData.size();
			m_mutex_rawData.unlock();
		}
		m_mutex_rawData.lock();
		nCurrSz = m_list_RawData.size();
		m_mutex_rawData.unlock();
		if (nCurrSz==0)
			return 0;
		return -1;
	}

	//push new binary data into queue
	int st_clientNode_baseTrans::push_new_data(const  QByteArray &  dtarray)
	{
		int res = 0;
		m_mutex_rawData.lock();

		m_list_RawData.push_back(dtarray);
		res = m_list_RawData.size();
		m_mutex_rawData.unlock();
		m_last_Report = QDateTime::currentDateTime();
		return res;
	}
	//!deal one message, affect m_currentRedOffset,m_currentMessageSize,m_currentHeader
	//!return bytes Used.
	int st_clientNode_baseTrans::filter_message(QByteArray  block, int offset)
	{
		const int blocklen = block.length();
		while (blocklen>offset)
		{
			const char * dataptr = block.constData();

			//Recieve First 2 byte
			while (m_currentMessageSize<2 && blocklen>offset )
			{
				m_currentBlock.push_back(dataptr[offset++]);
				m_currentMessageSize++;
			}
			if (m_currentMessageSize < 2) //First 2 byte not complete
				continue;

			if (m_currentMessageSize==2)
			{
				const char * headerptr = m_currentBlock.constData();
				memcpy((void *)&m_currentHeader,headerptr,2);
			}


			//Heart Beating
			if (m_currentHeader.Mark == 0xBEBE)
			{
				//while (m_currentMessageSize< sizeof(PKLTS_HEARTBEATING) && blocklen>offset )
				//{
				//	m_currentBlock.push_back(dataptr[offset++]);
				//	m_currentMessageSize++;
				//}
				if (m_currentMessageSize< sizeof(PKLTS_Heartbeating) && blocklen>offset )
				{
					int nCpy = offset - blocklen;
					if (nCpy > sizeof(PKLTS_Heartbeating) - m_currentMessageSize)
						nCpy =  sizeof(PKLTS_Heartbeating) - m_currentMessageSize;
					m_currentBlock.push_back(QByteArray(dataptr+offset,nCpy));
					offset += nCpy;
					m_currentMessageSize+=nCpy;
				}
				if (m_currentMessageSize < sizeof(PKLTS_Heartbeating)) //Header not completed.
					continue;

				//Send back
				emit evt_SendDataToClient(this->sock(),m_currentBlock);
				//Try to Get UUID Immediately
				if (m_bUUIDRecieved==false)
				{
					PKLTS_Heartbeating * pHbMsg = (PKLTS_Heartbeating *)( m_currentBlock.constData());
					if (bIsValidUserId(pHbMsg->source_id))
					{
						m_bUUIDRecieved = true;
						m_uuid =  pHbMsg->source_id;
						//regisit client node to hash-table;
						m_pClientTable->regisitClientUUID(this);
					}
				}

				//This Message is Over. Start a new one.
				m_currentMessageSize = 0;
				m_currentBlock = QByteArray();
				continue;
			}
			else if (m_currentHeader.Mark == 0x55AA)
				//Trans Message
			{
				//while (m_currentMessageSize< sizeof(PKLTS_TRANS_HEADER) && blocklen>offset)
				//{
				//	m_currentBlock.push_back(dataptr[offset++]);
				//	m_currentMessageSize++;
				//}
				if (m_currentMessageSize< sizeof(PKLTS_Trans_Header) && blocklen>offset)
				{
					int nCpy =  blocklen - offset;
					if (nCpy > sizeof(PKLTS_Trans_Header) - m_currentMessageSize)
						nCpy =  sizeof(PKLTS_Trans_Header) - m_currentMessageSize;
					m_currentBlock.push_back(QByteArray(dataptr+offset,nCpy));
					offset += nCpy;
					m_currentMessageSize+=nCpy;
				}
				if (m_currentMessageSize < sizeof(PKLTS_Trans_Header)) //Header not completed.
					continue;
				else if (m_currentMessageSize == sizeof(PKLTS_Trans_Header))//Header just  completed.
				{
					const char * headerptr = m_currentBlock.constData();
					memcpy((void *)&m_currentHeader,headerptr,sizeof(PKLTS_Trans_Header));

					//continue reading if there is data left behind
					if (block.length()>offset)
					{
						qint32 bitLeft = m_currentHeader.DataLen + sizeof(PKLTS_Trans_Header)
								-m_currentMessageSize ;
						//while (bitLeft>0 && blocklen>offset)
						//{
						//	m_currentBlock.push_back(dataptr[offset++]);
						//	m_currentMessageSize++;
						//	bitLeft--;
						//}
						if (bitLeft>0 && blocklen>offset)
						{
							int nCpy =  blocklen - offset;
							if (nCpy > bitLeft)
								nCpy =  bitLeft;
							m_currentBlock.push_back(QByteArray(dataptr+offset,nCpy));
							offset += nCpy;
							m_currentMessageSize+=nCpy;
							bitLeft -= nCpy;
						}
						//deal block, may be send data as soon as possible;
						deal_current_message_block();
						if (bitLeft>0)
							continue;
						//This Message is Over. Start a new one.
						m_currentMessageSize = 0;
						m_currentBlock = QByteArray();
						continue;
					}
				}
				else
				{
					if (block.length()>offset)
					{
						qint32 bitLeft = m_currentHeader.DataLen + sizeof(PKLTS_Trans_Header)
								-m_currentMessageSize ;
						//while (bitLeft>0 && blocklen>offset)
						//{
						//	m_currentBlock.push_back(dataptr[offset++]);
						//	m_currentMessageSize++;
						//	bitLeft--;
						//}
						if (bitLeft>0 && blocklen>offset)
						{
							int nCpy =  blocklen - offset;
							if (nCpy > bitLeft)
								nCpy =  bitLeft;
							m_currentBlock.push_back(QByteArray(dataptr+offset,nCpy));
							offset += nCpy;
							m_currentMessageSize+=nCpy;
							bitLeft -= nCpy;
						}
						//deal block, may be processed as soon as possible;
						deal_current_message_block();
						if (bitLeft>0)
							continue;
						//This Message is Over. Start a new one.
						m_currentMessageSize = 0;
						m_currentBlock = QByteArray();
						continue;
					}
				} // end if there is more bytes to append
			} //end deal trans message
			else
			{
				const char * ptrCurrData =  m_currentBlock.constData();
				emit evt_Message(this,tr("Client Send a unknown start Header %1 %2. Close client immediately.")
								 .arg((int)(ptrCurrData[0])).arg((int)(ptrCurrData[1])));
				m_currentMessageSize = 0;
				m_currentBlock = QByteArray();
				offset = blocklen;
				emit evt_close_client(this->sock());
			}
		} // end while block len > offset

		return offset;
	}
	//in Trans-Level, do nothing.
	int st_clientNode_baseTrans::deal_current_message_block()
	{
		//First, get uuid as soon as possible
		if (m_bUUIDRecieved==false)
		{
			if (bIsValidUserId( m_currentHeader.SrcID) )
			{
				m_bUUIDRecieved = true;
				m_uuid =  m_currentHeader.SrcID;
				//regisit client node to hash-table;
				m_pClientTable->regisitClientUUID(this);
			}
			else if (m_currentHeader.SrcID==0xffffffff)
			{
				//New clients
			}
			else //Invalid
			{
				emit evt_Message(this,tr("Client ID is invalid! Close client immediatly."));
				m_currentBlock = QByteArray();
				emit evt_close_client(this->sock());
			}
		}
		else
		{
			if (!( bIsValidUserId(m_currentHeader.SrcID)
				  ||
				  (m_currentHeader.SrcID==0xffffffff)
				  )
					)
			{
				emit evt_Message(this,tr("Client ID is invalid! Close client immediatly."));
				m_currentBlock = QByteArray();
				emit evt_close_client(this->sock());
			}
			if (bIsValidUserId(m_currentHeader.SrcID)==true &&
					m_uuid != m_currentHeader.SrcID)
			{
				emit evt_Message(this,tr("Client ID Changed in Runtime! Close client immediatly, %1->%2.").arg(m_uuid).arg(m_currentHeader.SrcID));
				m_currentBlock = QByteArray();
				emit evt_close_client(this->sock());
			}


		}

		return 0;
	}
	void st_clientNode_baseTrans::CheckHeartBeating()
	{
		QDateTime dtm = QDateTime::currentDateTime();
		qint64 usc = this->m_last_Report.secsTo(dtm);
		if (usc >=m_pClientTable->heartBeatingThrd())
		{
			emit evt_Message(this,tr("Client ") + QString("%1").arg((unsigned int)((quint64)this)) + tr(" is dead, kick out."));
			emit evt_close_client(this->sock());
		}
	}
}
