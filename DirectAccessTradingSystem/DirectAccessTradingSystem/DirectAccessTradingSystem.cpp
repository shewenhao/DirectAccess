// DirectAccessTradingSystem.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "DirectAccessTradingSystem.h"
#include "ShZdFutureMarketApi.h"
#include "ShZdFutureUserApiDataType.h"
#include "ShZdFutureUserApiStruct.h"
#include "ShZdFutureTradeApi.h"

#include <iostream>
#include <fstream>
#include <io.h>
#include <windows.h>
#include <string>
#include <cstdio>
#include <chrono>  // chrono::system_clock
#include <ctime>   // localtime
#include <sstream> // stringstream
#include <iomanip>

using namespace std;
using namespace kdb;

CSHZdMarketApi *apiHandle;
CSHZdTraderApi* apiTrade;
kdb::Connector kConnector;
//===================================================================================
#pragma region  //行情的实现
class MarketSpi :public CSHZdMarketSpi
{
	void OnFrontConnected();
	void OnFrontDisconnected(int nReason);
	void OnHeartBeatWarning(int nTimeLapse);
	void OnRspUserLogin(CTShZdRspUserLoginField *pRspUserLogin, CTShZdRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast);
	void OnRspUserLogout(CTShZdUserLogoutField *pUserLogout, CTShZdRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast);
	void OnRspError(CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnRspSubMarketData(CTShZdSpecificInstrumentField *pSpecificInstrument, CTShZdRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast);
	void OnRspUnSubMarketData(CTShZdSpecificInstrumentField *pSpecificInstrument, CTShZdRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast);
	void OnRtnDepthMarketData(CTShZdDepthMarketDataField *pDepthMarketData);
	void OnRtnFilledMarketData(CTShZdFilledDataField* pFilledMarketData);
	void OnRtnDataInsertToKDB(CTShZdDepthMarketDataField *pDepthMarketData);
	void OnkdbConnection();
	string return_current_time_and_date();

private: 
	kdb::Connector kConnector;
};

void MarketSpi::OnFrontConnected()
{
	cout << "Info connected sucessed!" << endl;
	kConnector.connect("localhost", 5001);
}

void MarketSpi::OnFrontDisconnected(int nReason)
{
	cout << "Info Disconnected!" << nReason << endl;
}

void MarketSpi::OnHeartBeatWarning(int nTimeLapse)
{

}

void MarketSpi::OnRspUserLogin(CTShZdRspUserLoginField *pRspUserLogin, CTShZdRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast)
{
	if (bIsLast == false)
		cout << "login:" << pRspUserLogin->UserID << " " << pRspUserLogin->AccountID << " " << pRspUserLogin->CurrencyNo << endl;
	else
		cout << "login over!" << endl;
}

void MarketSpi::OnRspUserLogout(CTShZdUserLogoutField *pUserLogout, CTShZdRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast)
{

}

void MarketSpi::OnRspError(CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cout << "error:" << pRspInfo->ErrorID << "===" << pRspInfo->ErrorMsg << endl;
}

void MarketSpi::OnRspSubMarketData(CTShZdSpecificInstrumentField *pSpecificInstrument, CTShZdRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast)
{

}

void MarketSpi::OnRspUnSubMarketData(CTShZdSpecificInstrumentField *pSpecificInstrument, CTShZdRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast)
{

}

void MarketSpi::OnRtnDepthMarketData(CTShZdDepthMarketDataField *pDepthMarketData)
{
	cout << "M:" << pDepthMarketData->ExchangeID << " " << pDepthMarketData->InstrumentID << " "
		<< pDepthMarketData->TradingDay << " " << pDepthMarketData->AskPrice1 << " " << pDepthMarketData->UpdateTime << endl;
	OnRtnDataInsertToKDB(pDepthMarketData);
}

string MarketSpi::return_current_time_and_date()
{
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);

	std::stringstream ss;
	ss << std::put_time(std::localtime(&in_time_t), "%Y.%m.%dD%X");
	auto duration = now.time_since_epoch();

	typedef std::chrono::duration<int, std::ratio_multiply<std::chrono::hours::period, std::ratio<8>
	>::type> Days; /* UTC: +8:00 */
	Days days = std::chrono::duration_cast<Days>(duration);
	duration -= days;
	auto hours = std::chrono::duration_cast<std::chrono::hours>(duration);
	duration -= hours;
	auto minutes = std::chrono::duration_cast<std::chrono::minutes>(duration);
	duration -= minutes;
	auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
	duration -= seconds;
	auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
	duration -= milliseconds;
	auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(duration);
	duration -= microseconds;
	auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);
	cout << (ss.str()).append(".").append(to_string(milliseconds.count())) << endl;
	return (ss.str()).append(".").append(to_string(milliseconds.count()));
}

void MarketSpi::OnRtnDataInsertToKDB(CTShZdDepthMarketDataField *pDepthMarketData)
{
	string insertstring;
	string date = pDepthMarketData->TradingDay;
	string datesplit = "D";
	string time = pDepthMarketData->UpdateTime;
	string receivedate = return_current_time_and_date();
	string symbol = pDepthMarketData->InstrumentID;
	double bidprice1 = pDepthMarketData->BidPrice1;
	int    bidvol1 = 1;
	double askprice1 = pDepthMarketData->AskPrice1;
	int    askvol1 = 1;

	insertstring.append("`Quote insert (");
	insertstring.append(date.substr(0, 4) + ".");
	insertstring.append(date.substr(4, 2) + ".");
	insertstring.append(date.substr(6, 2));
	insertstring.append(datesplit);
	insertstring.append(time.substr(1,8));
	insertstring.append(";");
	insertstring.append(receivedate);
	insertstring.append(";");
	insertstring.append("`");
	insertstring.append(symbol);
	insertstring.append(";");
	insertstring.append(to_string(bidprice1));
	insertstring.append(";");
	insertstring.append(to_string(bidvol1));
	insertstring.append(";");
	insertstring.append(to_string(askprice1));
	insertstring.append(";");
	insertstring.append(to_string(askvol1));
	insertstring.append(")");
	if (symbol == "L-ZS3M")
	{
		insertstring.replace(insertstring.find("L-"), 1, "");
		insertstring.replace(insertstring.find("-"), 1, "");
	}
	
	

	kConnector.sync(insertstring.c_str());
}

void MarketSpi::OnRtnFilledMarketData(CTShZdFilledDataField* pFilledMarketData)
{
	cout << "F:" << pFilledMarketData->ExchangeID << " " << pFilledMarketData->InstrumentID << " "
		<< pFilledMarketData->Volume << " " << pFilledMarketData->LastPrice << " " << pFilledMarketData->FilledVolume
		<< " " << pFilledMarketData->UpdateTime << endl;
}
#pragma endregion
//===================================================================================
#pragma region  //交易的实现定义
class TradeSpi :public CSHZdTraderSpi
{
public:
	void OnFrontConnected();
	void OnFrontDisconnected(int nReason);
	void OnHeartBeatWarning(int nTimeLapse);
	void OnRspUserLogin(CTShZdRspUserLoginField *pRspUserLogin, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnRspUserLogout(CTShZdUserLogoutField *pUserLogout, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnRspUserPasswordUpdate(CTShZdUserPasswordUpdateField *pUserPasswordUpdate, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnRspOrderInsert(CTShZdInputOrderField *pInputOrder, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnRspParkedOrderInsert(CTShZdParkedOrderField *pParkedOrder, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnRspParkedOrderAction(CTShZdParkedOrderActionField *pParkedOrderAction, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnRspOrderAction(CTShZdInputOrderActionField *pInputOrderAction, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);;
	void OnRspRemoveParkedOrder(CTShZdRemoveParkedOrderField *pRemoveParkedOrder, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnRspRemoveParkedOrderAction(CTShZdRemoveParkedOrderActionField *pRemoveParkedOrderAction, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnRspQryOrder(CTShZdOrderField *pOrder, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnRspQryTrade(CTShZdTradeField *pTrade, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnRtnTrade(CTShZdTradeField *pTrade);
	void OnRspQryInvestorPosition(CTShZdInvestorPositionField *pInvestorPosition, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnRspQryTradingAccount(CTShZdTradingAccountField *pTradingAccount, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnRspQryExchange(CTShZdExchangeField *pExchange, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnRspQryInstrument(CTShZdInstrumentField *pInstrument, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnRspQryInvestorPositionDetail(CTShZdInvestorPositionDetailField *pInvestorPositionDetail, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);//14
	void OnRspQryTransferSerial(CTShZdTransferSerialField *pTransferSerial, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnRspError(CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnRtnOrder(CTShZdOrderField *pOrder);
	void OnRtnTradeMoney(CTShZdTradeMoneyField *pTradeMoney);
	void OnErrRtnOrderInsert(CTShZdInputOrderField *pInputOrder, CTShZdRspInfoField *pRspInfo);
	void OnRtnErrorConditionalOrder(CTShZdErrorConditionalOrderField *pErrorConditionalOrder);
	void OnRspQryParkedOrder(CTShZdParkedOrderField *pParkedOrder, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnRspQryParkedOrderAction(CTShZdParkedOrderActionField *pParkedOrderAction, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnRtnOpenCloseTime(CTShZdOpenCloseTimeField *openCloseTime, int nRequestID, bool bIsLast);
	void OnRtnMarketOpenCloseTime(CTShZdMarketOpenCloseTimeField *openCloseTime, int nRequestID, bool bIsLast);
	void OnRtnCommonOpenCloseTime(CTShZdCommonOpenCloseTimeField *commonTime, int nRequestID, bool bIsLast);
	void OnRspMoneyRatio(CTShZdMoneyRatioField * rspRation, int nRequestID, bool bIsLast);
};

#pragma endregion  //交易的实现定义

#pragma region  //交易的实现
void TradeSpi::OnFrontConnected()
{
	cout << "Info trade connected sucessed!" << endl;
}
void TradeSpi::OnFrontDisconnected(int nReason)
{
	cout << "Info trade dis connected sucessed!" << endl;
}
void TradeSpi::OnHeartBeatWarning(int nTimeLapse)
{
}

void TradeSpi::OnRspUserLogin(CTShZdRspUserLoginField *pRspUserLogin,
	CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (!bIsLast)
		cout << "login:" << pRspUserLogin->UserID << " " << pRspUserLogin->AccountID << " " << pRspUserLogin->CurrencyNo << " "
		<< pRspUserLogin->UserName << " " << nRequestID << endl;
}
void TradeSpi::OnRspUserLogout(CTShZdUserLogoutField *pUserLogout,
	CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}
void TradeSpi::OnRspUserPasswordUpdate(CTShZdUserPasswordUpdateField *pUserPasswordUpdate,
	CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cout << "pwdUpdate:" << pUserPasswordUpdate->NewPassword << " " <<
		nRequestID << endl;
}

void TradeSpi::OnRspOrderInsert(CTShZdInputOrderField *pInputOrder, CTShZdRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast)
{
	cout << "sysNo:" << pInputOrder->OrderSysID << " " << pRspInfo->ErrorMsg << " " << nRequestID << endl;
}
void TradeSpi::OnRspParkedOrderInsert(CTShZdParkedOrderField *pParkedOrder, CTShZdRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast)
{
}
void TradeSpi::OnRspParkedOrderAction(CTShZdParkedOrderActionField *pParkedOrderAction,
	CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}
void TradeSpi::OnRspOrderAction(CTShZdInputOrderActionField *pInputOrderAction,
	CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (pInputOrderAction->ActionFlag == '0')
	{
		cout << "cancel:" << pInputOrderAction->OrderRef << " " << pInputOrderAction->OrderSysID << " " << pRspInfo->ErrorMsg << endl;
	}
	else
	{
		cout << "modify:" << pInputOrderAction->OrderRef << " " << pInputOrderAction->OrderSysID << " " << pRspInfo->ErrorMsg << endl;
		cout << "改单后数量：" << pInputOrderAction->VolumeChange << " " << "改单后的价格：" << pInputOrderAction->LimitPrice << endl;
	}
}
void TradeSpi::OnRspRemoveParkedOrder(CTShZdRemoveParkedOrderField *pRemoveParkedOrder, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}
void TradeSpi::OnRspRemoveParkedOrderAction(CTShZdRemoveParkedOrderActionField *pRemoveParkedOrderAction, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}
void TradeSpi::OnRspQryOrder(CTShZdOrderField *pOrder, CTShZdRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast)
{
	cout << "OrderList:" << pOrder->UserID << " " << pOrder->InstrumentID << " " << pOrder->OrderRef << " "
		<< nRequestID << endl;
	if (bIsLast)
		cout << "OrderList: over" << endl;
}
void TradeSpi::OnRspQryTrade(CTShZdTradeField *pTrade, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cout << "FilledList:" << pTrade->UserID << " " << pTrade->ExchangeID << " " << pTrade->InstrumentID << " "
		<< nRequestID << endl;
	if (bIsLast)
		cout << "FilledList: over" << endl;
}
void TradeSpi::OnRspQryInvestorPosition(CTShZdInvestorPositionField *pInvestorPosition, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{ //持仓汇总
	cout << "Position:" << pInvestorPosition->InstrumentID << " " << pInvestorPosition->PositionProfit << " "
		<< nRequestID << endl;
	if (bIsLast)
		cout << "Position: over" << endl;
}
void TradeSpi::OnRspQryTradingAccount(CTShZdTradingAccountField *pTradingAccount, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cout << "Account:" << pTradingAccount->AccountID << " " << string(pTradingAccount->CurrencyNo).substr(0, 8).c_str() << " "
		<< pTradingAccount->CurrUse << " " << nRequestID << endl;
	if (bIsLast)
		cout << "Account:over" << endl;
}

void TradeSpi::OnRspQryExchange(CTShZdExchangeField *pExchange, CTShZdRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast)
{
	cout << "Exchange:" << pExchange->ExchangeID << "  " << pExchange->ExchangeName << " "
		<< nRequestID << endl;
	if (bIsLast)
		cout << "Exchange: over!" << endl;
}

void TradeSpi::OnRspQryInstrument(CTShZdInstrumentField *pInstrument, CTShZdRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast)
{
	cout << "Instrument:" << pInstrument->ExchangeID << " " << pInstrument->InstrumentID << " "
		<< nRequestID << endl;
	if (bIsLast)
		cout << "Instrument: over!" << endl;
}

void TradeSpi::OnRspQryInvestorPositionDetail(CTShZdInvestorPositionDetailField *pInvestorPositionDetail,
	CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{ //持仓明细
	cout << "PositionDetail:" << pInvestorPositionDetail->ExchangeID << "  " << pInvestorPositionDetail->Direction << " "
		<< pInvestorPositionDetail->Volume << " " << pInvestorPositionDetail->InstrumentID << " "
		<< nRequestID << endl;
	if (bIsLast)
		cout << "PositionDetail:over" << endl;
}

void TradeSpi::OnRspQryTransferSerial(CTShZdTransferSerialField *pTransferSerial,
	CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cout << "InOutMoney:" << pTransferSerial->UserID << " " << pTransferSerial->Amount << " " << pTransferSerial->InOutMedth <<
		" " << nRequestID << endl;
	if (bIsLast)
		cout << "InOutMoney:over" << endl;
}

void TradeSpi::OnRspError(CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cout << "ErrorInfo:" << pRspInfo->ErrorID << " " << pRspInfo->ErrorMsg << endl;
	if (bIsLast)
		cout << "ErrorInfo:over" << endl;
}

void TradeSpi::OnRtnOrder(CTShZdOrderField *pOrder)
{
	cout << "orderback:" << pOrder->OrderRef << " " << pOrder->OrderSubmitStatus << " " << pOrder->OrderSysID << " " << pOrder->LimitPrice << endl;
}
void TradeSpi::OnRtnTrade(CTShZdTradeField *pTrade)
{
	cout << "tradeback:" << pTrade->OrderSysID << " " << pTrade->TradeID << " " << pTrade->OrderRef << " " << pTrade->Price << " " << pTrade->Volume << endl;
}
void TradeSpi::OnErrRtnOrderInsert(CTShZdInputOrderField *pInputOrder, CTShZdRspInfoField *pRspInfo)
{
	cout << "Order Error" << pInputOrder->OrderLocalID << " " << pInputOrder->OrderSysID << " " << pRspInfo->ErrorID << " " << pRspInfo->ErrorMsg << endl;
}

void TradeSpi::OnRtnErrorConditionalOrder(CTShZdErrorConditionalOrderField *pErrorConditionalOrder)
{
}

void TradeSpi::OnRspQryParkedOrder(CTShZdParkedOrderField *pParkedOrder, CTShZdRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast)
{
}
void TradeSpi::OnRspQryParkedOrderAction(CTShZdParkedOrderActionField *pParkedOrderAction,
	CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}

void TradeSpi::OnRtnOpenCloseTime(CTShZdOpenCloseTimeField *openCloseTime, int nRequestID, bool bIsLast)
{
	cout << "OpenCloseTime:" << openCloseTime->TradeDate << " " << openCloseTime->OpenTime << " " << openCloseTime->CloseTime << endl;
}

void TradeSpi::OnRtnMarketOpenCloseTime(CTShZdMarketOpenCloseTimeField *openCloseTime, int nRequestID, bool bIsLast)
{
	cout << "OpenCloseSumWinTime:" << openCloseTime->CurrYear << " " << openCloseTime->ExchangeID << " " << openCloseTime->SummeyBeginTime
		<< " " << openCloseTime->WinterBeginTime << " " << nRequestID << endl;
}

///直达支持产品的开收盘时间
void TradeSpi::OnRtnCommonOpenCloseTime(CTShZdCommonOpenCloseTimeField *commonTime, int nRequestID, bool bIsLast)
{
	cout << "OpenCloseSumWin:" << commonTime->ProductID << " " << commonTime->NomalSummerWinter << " " << commonTime->OpenTime
		<< " " << commonTime->CloseTime << " " << nRequestID << endl;
}

void TradeSpi::OnRtnTradeMoney(CTShZdTradeMoneyField *pTradeMoney)
{
	cout << "OnRtnTradeMoney:" << pTradeMoney->UserID << " " << pTradeMoney->AccountID << " " << pTradeMoney->FrozenDeposit
		<< " " << pTradeMoney->Commission << endl;
}

void TradeSpi::OnRspMoneyRatio(CTShZdMoneyRatioField * rspRation, int nRequestID, bool bIsLast)
{
	cout << "Ratio:" << rspRation->CurrencyID << " " << rspRation->Ratio << " " << nRequestID << endl;
}
#pragma endregion
//===================================================================================
string IntToStr(int index)
{
	char temp[64];
	string str;
	sprintf(temp, "%d", index);
	str = string(temp);
	return str;
}

int _tmain(int argc, _TCHAR* argv[])
{
	cout << "DirectAcces期货交易系统_旗舰版" << endl;
	//string strategyAccountParampath = argv[1];
	string strategyAccountParampath = "5001_SHANGHAI_ZHIDA_CONFIG.ini";
	apiHandle = CSHZdMarketApi::CreateSHZdMarketApi("..\\MDflow\\", false);
	ReadMessage readMessage;
	memset(&readMessage, 0, sizeof(readMessage));
	SetMessage(readMessage, strategyAccountParampath);
	apiHandle->RegisterSpi(new MarketSpi);
	apiHandle->Init();
	//apiHandle->AuthonInfo("55822DC39D9316D5111D9EED00C1CED81B6F0DCEA8D97DDEBD350D939CF8A9D304E3C73A742CFB80");
	//apiHandle->RegisterLoginFront("protocol://222.73.119.230:7003");
	apiHandle->AuthonInfo(readMessage.m_authorInfor);
	apiHandle->RegisterLoginFront(readMessage.m_mdFront);

	Sleep(2000);
	CTShZdReqUserLoginField field;
	memset(&field, 0, sizeof(CTShZdReqUserLoginField));
	//MN003853 - 3857， 密码88888
	//string m_Account = "MN003853";
	//string m_Password = "888888";
	string m_Account = readMessage.m_userId;
	string m_Password = readMessage.m_passwd;
	memcpy(field.UserID, m_Account.c_str(), 100);
	memcpy(field.Password, m_Password.c_str(), 41);
	apiHandle->ReqUserLogin(&field, 1);
	Sleep(5000);
	apiHandle->GetTradingDay();
	apiHandle->RegisterFront(readMessage.m_tradeFront);

	Sleep(5000);
	char *ppContract[2];
	//string subLMEContracts = "LME,CA3M;LME,AH3M;LME,L-ZS3M;LME,PB3M;LME,NI3M;LME,SN3M";
	string subLMEContracts = readMessage.m_read_contract0;
	ppContract[0] = (char*)subLMEContracts.c_str();
	//string subSGXContracts = "SGX,CN1810";
	string subSGXContracts = readMessage.m_read_contract1;
	ppContract[1] = (char*)subSGXContracts.c_str();
	apiHandle->SubscribeMarketData(ppContract, 2);


	if (!kConnector.connect("localhost", readMessage.m_kdbPort))
		return -1;

	while (true)
	{

	}

	return 0;
}

