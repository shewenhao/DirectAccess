#ifndef TEST_H_
#define TEST_H_

#include <string>
#include <iostream>
#ifndef KXVER
#define KXVER 3
#include "kdb_function.h"
#endif

using namespace std;

//�����ȡ����Ϣ�Ľṹ��
struct ReadMessage
{
	//TThostFtdcBrokerIDType	m_appId;//���͹�˾����
	//TThostFtdcUserIDType	m_userId;//�û���
	//TThostFtdcPasswordType	m_passwd;//����

	char m_mdFront[50];//�����������ַ
	char m_tradeFront[50];//���׷�������ַ

	string m_read_contract;//��Լ����
};



void SetMessage(ReadMessage& readMessage)//Ҫ������
{
	////-------------------------------��ȡ�˺�ģ��-------------------------------
	//CString read_brokerID;
	//GetPrivateProfileString("Account", "brokerID", "brokerID_error", read_brokerID.GetBuffer(MAX_PATH), MAX_PATH, "./input/AccountParam.ini");

	//CString read_userId;
	//GetPrivateProfileString("Account", "userId", "userId_error", read_userId.GetBuffer(MAX_PATH), MAX_PATH, "./input/AccountParam.ini");

	//CString read_passwd;
	//GetPrivateProfileString("Account", "passwd", "passwd_error", read_passwd.GetBuffer(MAX_PATH), MAX_PATH, "./input/AccountParam.ini");

	//strcpy_s(readMessage.m_appId, read_brokerID);
	//strcpy_s(readMessage.m_userId, read_userId);
	//strcpy_s(readMessage.m_passwd, read_passwd);



	////-------------------------------��ȡ��ַģ��-------------------------------
	//CString read_MDAddress;
	//GetPrivateProfileString("FrontAddress", "MDAddress", "MDAddress_error", read_MDAddress.GetBuffer(MAX_PATH), MAX_PATH, "./input/AccountParam.ini");

	//CString read_TDAddress;
	//GetPrivateProfileString("FrontAddress", "TDAddress", "TDAddress_error", read_TDAddress.GetBuffer(MAX_PATH), MAX_PATH, "./input/AccountParam.ini");

	//strcpy_s(readMessage.m_mdFront, read_MDAddress);
	//strcpy_s(readMessage.m_tradeFront, read_TDAddress);


	////-------------------------------���ú�Լģ��-------------------------------
	//CString read_contract;
	//GetPrivateProfileString("Contract", "contract", "contract_error", read_contract.GetBuffer(MAX_PATH), MAX_PATH, "./input/AccountParam.ini");

	//readMessage.m_read_contract = (LPCTSTR)read_contract;

}



#endif