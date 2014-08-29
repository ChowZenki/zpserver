#include "maindialog.h"
#include "ui_maindialog.h"
#include "../ZoomPipeline_FuncSvr/smartlink/st_message.h"
#include <QSettings>
#include <time.h>
#include <QMessageBox>
#include <string.h>
#include <QVector>
using namespace ParkinglotsSvr;
MainDialog::MainDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::MainDialog)
{
	ui->setupUi(this);
	m_bLogedIn = false;
	m_bUUIDGot = false;
	nTimer = startTimer(100);
	ui->listView_msg->setModel(&model);
	client = new QGHTcpClient (this);
	connect(client, SIGNAL(readyRead()),this, SLOT(new_data_recieved()));
	connect(client, SIGNAL(connected()),this, SLOT(on_client_connected()));
	connect(client, SIGNAL(disconnected()),this,SLOT(on_client_disconnected()));
	connect(client, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(displayError(QAbstractSocket::SocketError)));
	connect(client, SIGNAL(bytesWritten(qint64)), this, SLOT(on_client_trasferred(qint64)));

	m_currentReadOffset = 0;
	m_currentMessageSize = 0;


	QSettings settings("goldenhawking club","FunctionalClientTest",this);
	ui->lineEdit_ip->setText(settings.value("settings/ip","localhost").toString());
	ui->lineEdit_Port->setText(settings.value("settings/port","23456").toString());
	ui->lineEdit_serial_num->setText(settings.value("settings/client2svr_serialnum","TESTMACHINE").toString());
	ui->lineEdit_user_id->setText(settings.value("settings/client2svr_user_id","0").toString());

	m_model_devlist.insertColumns(0,3);
	m_model_devlist.setHeaderData(0,Qt::Horizontal,tr("Device Name"));
	m_model_devlist.setHeaderData(1,Qt::Horizontal,tr("Device No"));
	m_model_devlist.setHeaderData(2,Qt::Horizontal,tr("Device ID(24BIN,48HEXASC)"));
	ui->tableView_deviceList->setModel(&m_model_devlist);

	Qt::WindowFlags flg = this->windowFlags();
	flg |= Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint;
	this->setWindowFlags(flg);
}

MainDialog::~MainDialog()
{
	delete ui;
}
void MainDialog::on_client_trasferred(qint64 dtw)
{
	QGHTcpClient * pSock = qobject_cast<QGHTcpClient*>(sender());
	if (pSock)
	{
		displayMessage(QString("client %1 Transferrd %2 bytes.").arg((quintptr)pSock).arg(dtw));
	}

}
void MainDialog::on_client_connected()
{
	QGHTcpClient * pSock = qobject_cast<QGHTcpClient*>(sender());
	if (pSock)
	{
		displayMessage(QString("client %1 connected.").arg((quintptr)pSock));
		ui->pushButton_connect->setEnabled(false);
		ui->pushButton_clientRegisit->setEnabled(true);
	}

}
void MainDialog::on_client_disconnected()
{
	m_bLogedIn = m_bUUIDGot = false;
	client = new QGHTcpClient (this);
	connect(client, SIGNAL(readyRead()),this, SLOT(new_data_recieved()));
	connect(client, SIGNAL(connected()),this, SLOT(on_client_connected()));
	connect(client, SIGNAL(disconnected()),this,SLOT(on_client_disconnected()));
	connect(client, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(displayError(QAbstractSocket::SocketError)));
	connect(client, SIGNAL(bytesWritten(qint64)), this, SLOT(on_client_trasferred(qint64)));

	QGHTcpClient * pSock = qobject_cast<QGHTcpClient*>(sender());
	if (pSock)
	{
		displayMessage(QString("client %1 disconnected.").arg((quintptr)pSock));
		ui->pushButton_connect->setEnabled(true);
		pSock->abort();
		pSock->deleteLater();
	}
	ui->pushButton_clientRegisit->setEnabled(false);
	ui->pushButton_clientLogin->setEnabled(false);
	ui->pushButton_devlist_upload->setEnabled(false);
	ui->pushButton_UMI_Upload->setEnabled(false);
}
void MainDialog::displayError(QAbstractSocket::SocketError /*err*/)
{
	QGHTcpClient * sock = qobject_cast<QGHTcpClient *> (sender());
	if (sock)
		displayMessage(QString("client %1 error msg:").arg((quintptr)sock)+sock->errorString());
}
void MainDialog::new_data_recieved()
{
	QTcpSocket * pSock = qobject_cast<QTcpSocket*>(sender());
	if (pSock)
	{
		QByteArray block =pSock->readAll();
		if (block.isEmpty()==false && block.size()>0)
		{

			do
			{
				m_currentReadOffset = filter_message(block,m_currentReadOffset);
			}
			while (m_currentReadOffset < block.size());
			m_currentReadOffset = 0;
		}
	}
}
void MainDialog::displayMessage(QString str)
{
	model.insertRow(0,new QStandardItem(str));
	while (model.rowCount()>=256)
		model.removeRow(model.rowCount()-1);
}
void MainDialog::saveIni()
{
	QSettings settings("goldenhawking club","FunctionalClientTest",this);
	settings.setValue("settings/ip", ui->lineEdit_ip->text());
	settings.setValue("settings/port", ui->lineEdit_Port->text());
	settings.setValue("settings/client2svr_serialnum", ui->lineEdit_serial_num->text());
	settings.setValue("settings/client2svr_user_id", ui->lineEdit_user_id->text());
}

void MainDialog::timerEvent(QTimerEvent * evt)
{
	static int nCount = 0;
	if (evt->timerId()==nTimer)
	{
		nCount++;
		if (nCount % 200 == 0 && client->isOpen()==true)
		{
			//send heart-beating
			QByteArray array(sizeof(PKLTS_Heartbeating),0);
			char * ptr = array.data();
			PKLTS_Heartbeating * pMsg = (PKLTS_Heartbeating *)ptr;
			pMsg->Mark = 0xBEBE;
			pMsg->tmStamp = time(0)&0x00ffff;
			//pMsg->source_id = 0;
			//3/10 possibility to send a data block to server
			client->SendData(array);

		}
		if (nCount % 250 == 0 && client->isOpen()==true && this->m_bLogedIn==true)
		{

			quint16 nMsgLen = sizeof(PKLTS_App_Header);
					/*+sizeof(stMsg_HostTimeCorrectReq)*/ ;
			QByteArray array(sizeof(PKLTS_Trans_Header) + nMsgLen,0);
			char * ptr = array.data();
			PKLTS_Message * pMsg = (PKLTS_Message *)ptr;
			pMsg->trans_header.Mark = 0x55AA;
			pMsg->trans_header.SrcID = (quint32)((quint64)(ui->lineEdit_user_id->text().toUInt()) & 0xffffffff );;
			pMsg->trans_header.DstID = (quint32)((quint64)(0x00000001) & 0xffffffff );;
			pMsg->trans_header.DataLen = nMsgLen;
			pMsg->trans_payload.app_layer.app_header.MsgType = 0x1002;
			//3/10 possibility to send a data block to server
			client->SendData(array);
		}
	}
}
void MainDialog::on_pushButton_connect_clicked()
{
	saveIni();
	client->connectToHost(ui->lineEdit_ip->text(),ui->lineEdit_Port->text().toUShort());
}

//Regisit
void  MainDialog::on_pushButton_clientRegisit_clicked()
{
	if (client->isOpen()==false)
	{
		QMessageBox::warning(this,"Please connect first!","Connect to server and test funcs");
		return;
	}
	if (m_bLogedIn==true)
	{
		QMessageBox::warning(this,"Already log in!","uuid has been revieved and login.");
		return;
	}
	saveIni();
	//Get the serial Num
	QString serialNum = ui->lineEdit_serial_num->text();
	std::string str_serialNum = serialNum.toStdString();
	const char * pSrcSerialNum = str_serialNum.c_str();
	int nMaxLenSerialNum = str_serialNum.length();

	quint16 nMsgLen =sizeof(PKLTS_App_Header)
			+sizeof(stMsg_HostRegistReq) + nMaxLenSerialNum;
	QByteArray array(sizeof(PKLTS_Trans_Header) + nMsgLen,0);
	char * ptr = array.data();
	PKLTS_Message * pMsg = (PKLTS_Message *)ptr;

	pMsg->trans_header.Mark = 0x55AA;
	pMsg->trans_header.SrcID = (quint32)((quint64)(0xffffffff) & 0xffffffff );;
	pMsg->trans_header.DstID = (quint32)((quint64)(0x00000001) & 0xffffffff );;
	pMsg->trans_header.DataLen = nMsgLen;
	pMsg->trans_payload.app_layer.app_header.MsgType = 0x1000;

	for (int i=0;i<=nMaxLenSerialNum;i++)
		pMsg->trans_payload.app_layer.app_data.msg_HostRegistReq.HostSerialNum[i] =
				i<nMaxLenSerialNum?pSrcSerialNum[i]:0;

	//3/10 possibility to send a data block to server
	client->SendData(array);
}

void MainDialog::on_pushButton_clientLogin_clicked()
{
	if (client->isOpen()==false)
	{
		QMessageBox::warning(this,"Please connect first!","Connect to server and test funcs");
		return;
	}
	saveIni();
	quint32 userID = ui->lineEdit_user_id->text().toUInt();
	QString strSerialNum = ui->lineEdit_serial_num->text();
	std::string strStdSerialNum = strSerialNum.toStdString();
	const char * pSrcSerialNum = strStdSerialNum.c_str();
	int nMaxLenSerialNum = strStdSerialNum.length();

	quint16 nMsgLen = sizeof(PKLTS_App_Header)
			+sizeof(stMsg_HostLogonReq)+nMaxLenSerialNum;
	QByteArray array(sizeof(PKLTS_Trans_Header) + nMsgLen,0);
	char * ptr = array.data();
	PKLTS_Message * pMsg = (PKLTS_Message *)ptr;
	pMsg->trans_header.Mark = 0x55AA;
	pMsg->trans_header.SrcID = (quint32)((quint64)(userID) & 0xffffffff );
	pMsg->trans_header.DstID = (quint32)((quint64)(0x00000001) & 0xffffffff );;
	pMsg->trans_header.DataLen = nMsgLen;
	pMsg->trans_payload.app_layer.app_header.MsgType = 0x1001;
	pMsg->trans_payload.app_layer.app_data.msg_HostLogonReq.ID = userID;
	for (int i=0;i<=nMaxLenSerialNum;i++)
		pMsg->trans_payload.app_layer.app_data.msg_HostLogonReq.HostSerialNum[i] =
				i<nMaxLenSerialNum?pSrcSerialNum[i]:0;

	//3/10 possibility to send a data block to server
	client->SendData(array);
}


//!deal one message, affect m_currentRedOffset,m_currentMessageSize,m_currentHeader
//!return bytes Used.
int MainDialog::filter_message(QByteArray  block, int offset)
{
	const int blocklen = block.length();
	const char * dataptr = block.constData();
	while (blocklen>offset)
	{
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
			while (m_currentMessageSize< sizeof(PKLTS_Heartbeating) && blocklen>offset )
			{
				m_currentBlock.push_back(dataptr[offset++]);
				m_currentMessageSize++;
			}
			if (m_currentMessageSize < sizeof(PKLTS_Heartbeating)) //Header not completed.
				continue;
			const PKLTS_Heartbeating * ptr = (const PKLTS_Heartbeating *)m_currentBlock.constData();
			displayMessage(tr("Recieved Heart-beating msg sended %1 sec(s) ago.").
						   arg((time(0)&0x00ffff)-(ptr->tmStamp)));
			//This Message is Over. Start a new one.
			m_currentMessageSize = 0;
			m_currentBlock = QByteArray();

			continue;
		}
		else if (m_currentHeader.Mark == 0x55AA)
			//Trans Message
		{
			while (m_currentMessageSize< sizeof(PKLTS_Trans_Header) && blocklen>offset)
			{

				m_currentBlock.push_back(dataptr[offset++]);
				m_currentMessageSize++;
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
					while (bitLeft>0 && blocklen>offset)
					{
						m_currentBlock.push_back(dataptr[offset++]);
						m_currentMessageSize++;
						bitLeft--;
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
					while (bitLeft>0 && blocklen>offset)
					{
						m_currentBlock.push_back(dataptr[offset++]);
						m_currentMessageSize++;
						bitLeft--;
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
			const char * ptrCurrData = m_currentBlock.constData();
			displayMessage(tr("Client Send a unknown start Header %1 %2. Close client immediately.")
						   .arg((int)(ptrCurrData[0])).arg((int)(ptrCurrData[1])));
			m_currentMessageSize = 0;
			m_currentBlock = QByteArray();
			offset = blocklen;

			this->client->abort();
		}
	} // end while block len > offset

	return offset;
}
//!deal current message
int MainDialog::deal_current_message_block()
{
	//The bytes left to recieve.
	qint32 bytesLeft = m_currentHeader.DataLen + sizeof(PKLTS_Trans_Header)
			-m_currentMessageSize ;
	if (bytesLeft)
		return 0;


	const char * ptr = m_currentBlock.constData();

	PKLTS_Message * pMsg = (PKLTS_Message *)((unsigned char *)ptr);
	PKLTS_Message::uni_trans_payload::tag_pklts_app_layer * pApp = &pMsg->trans_payload.app_layer;
	if (pApp->app_header.MsgType==0x1800)
	{
		if (pApp->app_data.msg_HostRegistRsp.DoneCode<2 && pApp->app_data.msg_HostRegistRsp.DoneCode>=0)
		{
			m_bLogedIn = true;
			displayMessage(tr("Regisit Succeed, Res = %1")
					   .arg(pApp->app_data.msg_HostRegistRsp.DoneCode)
					   );
			ui->pushButton_clientLogin->setEnabled(true);
			ui->pushButton_clientRegisit->setEnabled(false);
			ui->lineEdit_user_id->setText(QString("%1").arg(pApp->app_data.msg_HostRegistRsp.ID));
		}
		else
			displayMessage(tr("Regisit Failed, Res = %1")
					   .arg(pApp->app_data.msg_HostRegistRsp.DoneCode)
					   );


	}
	else if (pApp->app_header.MsgType==0x1801)
	{
		if (pApp->app_data.msg_HostLogonRsp.DoneCode==0)
		{
			m_bLogedIn = true;
			displayMessage(tr("Login Succeed, Res = %1")
					   .arg(pApp->app_data.msg_HostLogonRsp.DoneCode)
					   );
			ui->pushButton_clientLogin->setEnabled(false);
			ui->pushButton_devlist_upload->setEnabled(true);
			ui->pushButton_UMI_Upload->setEnabled(true);
		}
		else if (pApp->app_data.msg_HostLogonRsp.DoneCode==1)
		{
			displayMessage(tr("Login Failed, Res = %1")
					   .arg(pApp->app_data.msg_HostLogonRsp.DoneCode)
					   );
		}
		else
			displayMessage(tr("Login Failed,, Res = %1")
					   .arg(pApp->app_data.msg_HostLogonRsp.DoneCode)
					   );
	}
	else if (pApp->app_header.MsgType==0x1802)
	{
		if (pApp->app_data.msg_HostTimeCorrectRsp.DoneCode==0)
		{
			m_bLogedIn = true;
			displayMessage(tr("Host Time is %1-%2-%3 %4:%5:%6.")
					   .arg(pApp->app_data.msg_HostTimeCorrectRsp.DateTime.Year)
							.arg(pApp->app_data.msg_HostTimeCorrectRsp.DateTime.Month)
							.arg(pApp->app_data.msg_HostTimeCorrectRsp.DateTime.Day)
							.arg(pApp->app_data.msg_HostTimeCorrectRsp.DateTime.Hour)
							.arg(pApp->app_data.msg_HostTimeCorrectRsp.DateTime.Minute)
							.arg(pApp->app_data.msg_HostTimeCorrectRsp.DateTime.Second)
					   );
		}
		else
			displayMessage(tr("Time Crooecting Failed,, Res = %1")
					   .arg(pApp->app_data.msg_HostTimeCorrectRsp.DoneCode)
					   );
	}
	else
	{
		QString str;
		int nLen =  m_currentHeader.DataLen;
		for (int i=0;i<nLen;i++)
		{
			str += pMsg->trans_payload.data[i];
		}
		displayMessage(str);

	}
	m_currentBlock = QByteArray();


	return 0;
}

void MainDialog::on_pushButton_devlist_add_clicked()
{
	int nRow = m_model_devlist.rowCount();
	m_model_devlist.appendRow(new QStandardItem(ui->lineEdit_dev_name->text()));
	m_model_devlist.setData(m_model_devlist.index(nRow,1),ui->lineEdit_dev_no->text());
	m_model_devlist.setData(m_model_devlist.index(nRow,2),ui->lineEdit_dev_id->text());
}

void MainDialog::on_pushButton_devlist_del_clicked()
{
	QModelIndexList lst = ui->tableView_deviceList->selectionModel()->selectedIndexes();
	int nRow = lst.first().row();
	m_model_devlist.removeRow(nRow);
}

void MainDialog::on_pushButton_devlist_upload_clicked()
{
	if (m_bLogedIn==false)
	{
		QMessageBox::warning(this,"Need log in!","uuid has not been revieved and login.");
		return;
	}
	saveIni();

	//Howmany rows in list
	int nTotalItems = m_model_devlist.rowCount();
	if (nTotalItems <=0)
		return;

	QByteArray array_data;
	for (int i=0;i<nTotalItems;++i)
	{
		QString devName = m_model_devlist.data(m_model_devlist.index(i,0)).toString();
		array_data.append(devName);
		array_data.append('\0');
	}
	for (int i=0;i<nTotalItems;++i)
	{
		QString devName = m_model_devlist.data(m_model_devlist.index(i,1)).toString();
		array_data.append(devName);
		array_data.append('\0');
	}
	for (int i=0;i<nTotalItems;++i)
	{
		QString devName = m_model_devlist.data(m_model_devlist.index(i,2)).toString().toUpper();
		int nLength = devName.length();
		for (int j = 0;j<24;++j)
		{
			QChar c1 = (j*2<nLength)?devName[j*2]:QChar('0');
			QChar c2 = (j*2+1<nLength)?devName[j*2+1]:QChar('0');
			char cUp = c1.toLatin1();
			char cLow = c2.toLatin1();
			quint8 u8val = 0;
			if (cUp>='0' && cUp<='9') u8val += cUp-'0';
			if (cUp>='A' && cUp<='F') u8val += cUp-'A' + 10;
			u8val *=16;
			if (cLow>='0' && cLow<='9') u8val += cLow-'0';
			if (cLow>='A' && cLow<='F') u8val += cLow-'A' + 10;
			array_data.append(u8val);
		}
	}
	//Get the serial Num
	quint16 nMsgLen =sizeof(PKLTS_App_Header)
			+sizeof(stMsg_SendDeviceListReq) - 1;
	QByteArray array(sizeof(PKLTS_Trans_Header) + nMsgLen,0);
	char * ptr = array.data();
	PKLTS_Message * pMsg = (PKLTS_Message *)ptr;

	quint32 userID = ui->lineEdit_user_id->text().toUInt();

	pMsg->trans_header.Mark = 0x55AA;
	pMsg->trans_header.SrcID = (quint32)((quint64)(userID) & 0xffffffff );;
	pMsg->trans_header.DstID = (quint32)((quint64)(0x00000001) & 0xffffffff );;
	pMsg->trans_header.DataLen = nMsgLen + array_data.size();
	pMsg->trans_payload.app_layer.app_header.MsgType = 0x100B;
	pMsg->trans_payload.app_layer.app_data.msg_SendDeviceListReq.DeviceNums = nTotalItems;

	array.append(array_data);
	//3/10 possibility to send a data block to server
	client->SendData(array);
}
void MainDialog::on_pushButton_UMI_Upload_clicked()
{
	if (m_bLogedIn==false)
	{
		QMessageBox::warning(this,"Need log in!","uuid has not been revieved and login.");
		return;
	}
	saveIni();
	QByteArray array_data;
	quint16 firmwareVersion = ui->lineEdit_UMI_FirmV->text().toUInt();
	array_data.append((char)(firmwareVersion & 0x0F));
	array_data.append((char)((firmwareVersion>>4) & 0x0F));

	QString HostName = ui->lineEdit_UMI_Hostname->text();
	array_data.append(HostName);
	array_data.append('\0');

	QString HostInfo = ui->lineEdit_UMI_HostInfo->text();
	array_data.append(HostInfo);
	array_data.append('\0');

	char ConnTp = 4;
	array_data.append(ConnTp);

	int rndstart = rand() % 256;
	for (int i=0;i<8;++i)
	{
		char st = (rndstart + i) % 256;
		array_data.append(st);
	}
	char Ava = rand() %2;
	array_data.append(Ava);

	rndstart = rand() % 256;
	for (int i=0;i<2;++i)
	{
		char st = (rndstart + i) % 256;
		array_data.append(st);
	}
	Ava = rand() %2;
	array_data.append(Ava);

	rndstart = rand() % 256;
	for (int i=0;i<8;++i)
	{
		char st = (rndstart + i) % 256;
		array_data.append(st);
	}
	Ava = rand() %2;
	array_data.append(Ava);

	rndstart =( rand() % 265) *( rand() % 256);
	for (int i=0;i<4;++i)
	{
		quint16 st = (rndstart + i) % 65536;
		array_data.append((char)(st & 0x0F));
		array_data.append((char)((st>>4) & 0x0F));
	}

	//Get the serial Num
	quint16 nMsgLen =sizeof(PKLTS_App_Header);
	QByteArray array(sizeof(PKLTS_Trans_Header) + nMsgLen,0);
	char * ptr = array.data();
	PKLTS_Message * pMsg = (PKLTS_Message *)ptr;

	quint32 userID = ui->lineEdit_user_id->text().toUInt();

	pMsg->trans_header.Mark = 0x55AA;
	pMsg->trans_header.SrcID = (quint32)((quint64)(userID) & 0xffffffff );;
	pMsg->trans_header.DstID = (quint32)((quint64)(0x00000001) & 0xffffffff );;
	pMsg->trans_header.DataLen = nMsgLen + array_data.size();
	pMsg->trans_payload.app_layer.app_header.MsgType = 0x100C;

	array.append(array_data);
	//3/10 possibility to send a data block to server
	client->SendData(array);
}
