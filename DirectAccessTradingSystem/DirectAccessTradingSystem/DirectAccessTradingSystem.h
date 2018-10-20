#ifndef TEST_H_
#define TEST_H_

#include <string>
#include <atlstr.h>
#include <iostream>
#include <windows.h>
#include <stdio.h>
#ifndef KXVER
#define KXVER 3
#include "kdb_function.h"
#endif

using namespace std;

string ExePath() {
	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	string::size_type pos = string(buffer).find_last_of("\\/");
	return string(buffer).substr(0, pos);
}

string replace(std::string& str, const std::string& from, const std::string& to) {
	size_t start_pos = str.find(from);
	if (start_pos == std::string::npos)
		return false;
	str.replace(start_pos, from.length(), to);
	return str;
}

string replaceAll(std::string& str, const std::string& from, const std::string& to) {
	if (from.empty())
		return str;
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}
	return str;
}

//保存读取的信息的结构体
struct ReadMessage
{
	char m_authorInfor[100];//经纪公司代码
	char m_userId[100];//用户名
	char m_passwd[100];//密码

	char m_mdFront[50];//行情服务器地址
	char m_tradeFront[50];//交易服务器地址

	string m_read_contract;//合约代码
	
	string m_strategy_strategytype;

	int m_kdbPort;

	string m_read_contract0;

	string m_read_contract1;
};



void SetMessage(ReadMessage& readMessage, string strategyAccountParampath)//要用引用
{
	string kdbDataSetPath = ExePath();
	string acountParampath = kdbDataSetPath + "/Input/" + strategyAccountParampath;
	acountParampath = replaceAll(acountParampath, "\\", "/");

	////-------------------------------读取账号模块-------------------------------
	CString read_AuthorInfor;
	GetPrivateProfileString("Account", "AuthonInfo", "AuthonInfo_error", read_AuthorInfor.GetBuffer(MAX_PATH), MAX_PATH, acountParampath.c_str());

	CString read_userId;
	GetPrivateProfileString("Account", "userId", "userId_error", read_userId.GetBuffer(MAX_PATH), MAX_PATH, acountParampath.c_str());

	CString read_passwd;
	GetPrivateProfileString("Account", "userId", "userId_error", read_userId.GetBuffer(MAX_PATH), MAX_PATH, acountParampath.c_str());

	strcpy_s(readMessage.m_authorInfor, read_AuthorInfor);
	strcpy_s(readMessage.m_userId, read_userId);
	strcpy_s(readMessage.m_passwd, read_passwd);

	//-------------------------------设置合约模块-------------------------------
	CString read_contract0;
	GetPrivateProfileString("Contract", "contract0", "contract_error", read_contract0.GetBuffer(MAX_PATH), MAX_PATH, acountParampath.c_str());
	readMessage.m_read_contract0 = (LPCTSTR)read_contract0;

	CString read_contract1;
	GetPrivateProfileString("Contract", "contract1", "contract_error", read_contract1.GetBuffer(MAX_PATH), MAX_PATH, acountParampath.c_str());
	readMessage.m_read_contract1 = (LPCTSTR)read_contract1;

	//-------------------------------读取地址模块-------------------------------
	CString read_MDAddress;
	GetPrivateProfileString("FrontAddress", "MDAddress", "MDAddress_error", read_MDAddress.GetBuffer(MAX_PATH), MAX_PATH, acountParampath.c_str());

	CString read_TDAddress;
	GetPrivateProfileString("FrontAddress", "TDAddress", "TDAddress_error", read_TDAddress.GetBuffer(MAX_PATH), MAX_PATH, acountParampath.c_str());

	strcpy_s(readMessage.m_mdFront, read_MDAddress);
	strcpy_s(readMessage.m_tradeFront, read_TDAddress);

	//-------------------------------读取kdb模块-------------------------------
	CString read_kdbPort;
	GetPrivateProfileString("DataBaseAddress", "kdbPort", "kdbPort_error", read_kdbPort.GetBuffer(MAX_PATH), MAX_PATH, acountParampath.c_str());
	readMessage.m_kdbPort = atoi(read_kdbPort);

	//-------------------------------设置策略与否-------------------------------
	CString read_StrategyType;
	GetPrivateProfileString("StrategySetting", "StrategyType", "StrategySetting_error", read_StrategyType.GetBuffer(MAX_PATH), MAX_PATH, acountParampath.c_str());

	readMessage.m_strategy_strategytype = read_StrategyType;


}




#endif