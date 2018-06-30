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

private: 
	kdb::Connector kConnector;
};


void MarketSpi::OnFrontConnected()
{
	cout << "Info connected sucessed!" << endl;
	kConnector.connect("localhost", 5000);
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

void MarketSpi::OnRtnDataInsertToKDB(CTShZdDepthMarketDataField *pDepthMarketData)
{
	string insertstring;
	string date = pDepthMarketData->TradingDay;
	string datesplit = "D";
	string time = pDepthMarketData->UpdateTime;
	string symbol = pDepthMarketData->InstrumentID;
	double buy1price = pDepthMarketData->BidPrice1;
	double sell1price = pDepthMarketData->AskPrice1;

	insertstring.append("`DAQuote insert (");
	insertstring.append(date.substr(0, 4) + ".");
	insertstring.append(date.substr(4, 2) + ".");
	insertstring.append(date.substr(6, 2));
	insertstring.append(datesplit);
	insertstring.append(time.substr(1,8));
	insertstring.append(";");
	insertstring.append("`");
	insertstring.append(symbol);
	insertstring.append(";");
	insertstring.append(to_string(buy1price));
	insertstring.append(";");
	insertstring.append(to_string(sell1price));
	insertstring.append(";");
	insertstring.append(to_string(buy1price));
	insertstring.append(";");
	insertstring.append(to_string(sell1price));
	insertstring.append(")");

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
	apiHandle = CSHZdMarketApi::CreateSHZdMarketApi("..\\MDflow\\", false);
	apiHandle->RegisterSpi(new MarketSpi);
	apiHandle->Init();
	apiHandle->AuthonInfo("55822DC39D9316D5111D9EED00C1CED81B6F0DCEA8D97DDEBD350D939CF8A9D304E3C73A742CFB80");
	apiHandle->RegisterLoginFront("protocol://222.73.119.230:7003");

	Sleep(2000);
	CTShZdReqUserLoginField field;
	memset(&field, 0, sizeof(CTShZdReqUserLoginField));
	memcpy(field.UserID, "demo000601", 16);
	memcpy(field.Password, "123456", 41);
	apiHandle->ReqUserLogin(&field, 1);
	Sleep(5000);
	apiHandle->GetTradingDay();
	apiHandle->RegisterFront("protocol://222.73.105.170:9002");

	Sleep(5000);
	char *ppInstrumentID[1];
	string subLMEContracts = "LME,NI3M";
	ppInstrumentID[0] = (char*)subLMEContracts.c_str();
	apiHandle->SubscribeMarketData(ppInstrumentID, 1);


	if (!kConnector.connect("localhost", 5000))
		return -1;
	kdb::Result res = kConnector.sync("DAQuote:([] Date:();`symbol$Symbol:(); `float$Leg1Bid1:(); `float$Leg1Ask1:(); `float$Leg2Bid1:();`float$Leg2Ask1:())");
	//kconnector.sync("`DAQuote insert (2018.08.08D08:08:08.888;1.0;1.2;2.0;2.2)");

	while (true)
	{

	}

	return 0;
}

