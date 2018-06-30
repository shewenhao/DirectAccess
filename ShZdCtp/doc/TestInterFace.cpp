// TestInterFace.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "..\\..\\Include\\ShZdFutureMarketApi.h"
#include "..\\..\\Include\\ShZdFutureUserApiDataType.h"
#include "..\\..\\Include\\ShZdFutureUserApiStruct.h"

#include "..\\..\\Include\\ShZdFutureTradeApi.h"

#include <iostream>
#include <fstream>
#include <io.h>
#include <windows.h>
#include <string>//iostream
using namespace std;

CSHZdMarketApi *apiHandle;
CSHZdTraderApi* apiTrade;
//
//DWORD WINAPI reConnected(LPVOID lpParameter)
//{
//	Sleep(2000);
//	char *ppInstrumentID[2];
//	string temp="CME,6B1612;CME,GC1612;CME,6J1612";
//	ppInstrumentID[0]=(char*)temp.c_str();	
//	string temp1="HKEX,HSI1612;HKEX,HSI1611";
//	ppInstrumentID[1]=(char*)temp1.c_str();	
//	apiHandle->RegisterFront("protocol://222.73.105.170:9002");//222.73.105.170:9002
//	apiHandle->SubscribeMarketData(ppInstrumentID,2); 
//	return 0;
//}

#pragma region  //行情的实现
class MarketSpi:public CSHZdMarketSpi
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
};

void MarketSpi::OnFrontConnected()
{
	cout <<"Info connected sucessed!" << endl;
}

void MarketSpi::OnFrontDisconnected(int nReason)
{
	cout <<"Info Disconnected!" << nReason<< endl;
}

void MarketSpi::OnHeartBeatWarning(int nTimeLapse)
{

}
void MarketSpi::OnRspUserLogin(CTShZdRspUserLoginField *pRspUserLogin, CTShZdRspInfoField *pRspInfo, 
	int nRequestID, bool bIsLast)
{
	if(bIsLast==false)
		cout<<"login:"<<pRspUserLogin->UserID<<" "<<pRspUserLogin->AccountID<<" "<<pRspUserLogin->CurrencyNo <<endl;
	else
		cout<<"login over!"<<endl;
}

void MarketSpi::OnRspUserLogout(CTShZdUserLogoutField *pUserLogout, CTShZdRspInfoField *pRspInfo, 
	int nRequestID, bool bIsLast)
{

}

void MarketSpi::OnRspError(CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cout <<"error:"<<pRspInfo->ErrorID<<"==="<<pRspInfo->ErrorMsg << endl;	
}
void MarketSpi::OnRspSubMarketData(CTShZdSpecificInstrumentField *pSpecificInstrument,CTShZdRspInfoField *pRspInfo, 
	int nRequestID, bool bIsLast)
{

}
void MarketSpi::OnRspUnSubMarketData(CTShZdSpecificInstrumentField *pSpecificInstrument, CTShZdRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast)
{

}
void MarketSpi::OnRtnDepthMarketData(CTShZdDepthMarketDataField *pDepthMarketData)
{
	cout <<"M:"<<pDepthMarketData->ExchangeID<<" "<<pDepthMarketData->InstrumentID<<" "
		<<pDepthMarketData->TradingDay<<" "<<pDepthMarketData->AskPrice1<<" "<<pDepthMarketData->UpdateTime<<endl; 
}

void MarketSpi::OnRtnFilledMarketData(CTShZdFilledDataField* pFilledMarketData)
{
	cout<<"F:"<<pFilledMarketData->ExchangeID<<" "<<pFilledMarketData->InstrumentID<<" "
		<<pFilledMarketData->Volume<<" "<<pFilledMarketData->LastPrice<<" "<<pFilledMarketData->FilledVolume
		<<" "<<pFilledMarketData->UpdateTime<<endl; 
}
#pragma endregion
//===================================================================================

#pragma region  //交易的实现定义
class TradeSpi:public CSHZdTraderSpi  
{
	public:
	void OnFrontConnected();
	void OnFrontDisconnected(int nReason);
	void OnHeartBeatWarning(int nTimeLapse);	
	void OnRspUserLogin(CTShZdRspUserLoginField *pRspUserLogin,CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnRspUserLogout(CTShZdUserLogoutField *pUserLogout,CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnRspUserPasswordUpdate(CTShZdUserPasswordUpdateField *pUserPasswordUpdate,CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnRspOrderInsert(CTShZdInputOrderField *pInputOrder,CTShZdRspInfoField *pRspInfo,int nRequestID, bool bIsLast);
	void OnRspParkedOrderInsert(CTShZdParkedOrderField *pParkedOrder,CTShZdRspInfoField *pRspInfo,int nRequestID, bool bIsLast);
	void OnRspParkedOrderAction(CTShZdParkedOrderActionField *pParkedOrderAction,CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnRspOrderAction(CTShZdInputOrderActionField *pInputOrderAction,CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);;
	void OnRspRemoveParkedOrder(CTShZdRemoveParkedOrderField *pRemoveParkedOrder,CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnRspRemoveParkedOrderAction(CTShZdRemoveParkedOrderActionField *pRemoveParkedOrderAction,CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnRspQryOrder(CTShZdOrderField *pOrder,CTShZdRspInfoField *pRspInfo,int nRequestID, bool bIsLast);
	void OnRspQryTrade(CTShZdTradeField *pTrade, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnRtnTrade(CTShZdTradeField *pTrade);
	void OnRspQryInvestorPosition(CTShZdInvestorPositionField *pInvestorPosition,CTShZdRspInfoField *pRspInfo,int nRequestID, bool bIsLast);
	void OnRspQryTradingAccount(CTShZdTradingAccountField *pTradingAccount,CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnRspQryExchange(CTShZdExchangeField *pExchange,CTShZdRspInfoField *pRspInfo,int nRequestID, bool bIsLast);
	void OnRspQryInstrument(CTShZdInstrumentField *pInstrument,CTShZdRspInfoField *pRspInfo,int nRequestID, bool bIsLast);	
	void OnRspQryInvestorPositionDetail(CTShZdInvestorPositionDetailField *pInvestorPositionDetail,CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);//14
	void OnRspQryTransferSerial(CTShZdTransferSerialField *pTransferSerial,CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnRspError(CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void OnRtnOrder(CTShZdOrderField *pOrder);
	void OnRtnTradeMoney(CTShZdTradeMoneyField *pTradeMoney);
	void OnErrRtnOrderInsert(CTShZdInputOrderField *pInputOrder,CTShZdRspInfoField *pRspInfo);
	void OnRtnErrorConditionalOrder(CTShZdErrorConditionalOrderField *pErrorConditionalOrder);
	void OnRspQryParkedOrder(CTShZdParkedOrderField *pParkedOrder,CTShZdRspInfoField *pRspInfo,int nRequestID, bool bIsLast);
	void OnRspQryParkedOrderAction(CTShZdParkedOrderActionField *pParkedOrderAction,CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast);	
	void OnRtnOpenCloseTime(CTShZdOpenCloseTimeField *openCloseTime,int nRequestID,bool bIsLast);
	void OnRtnMarketOpenCloseTime(CTShZdMarketOpenCloseTimeField *openCloseTime,int nRequestID,bool bIsLast); 
	void OnRtnCommonOpenCloseTime(CTShZdCommonOpenCloseTimeField *commonTime,int nRequestID,bool bIsLast);
	void OnRspMoneyRatio(CTShZdMoneyRatioField * rspRation,int nRequestID,bool bIsLast);
};

#pragma endregion  //交易的实现定义

#pragma region  //交易的实现
void TradeSpi::OnFrontConnected()
{
	cout <<"Info trade connected sucessed!" << endl;
}
void TradeSpi::OnFrontDisconnected(int nReason)
{
	cout <<"Info trade dis connected sucessed!" << endl;
}
void TradeSpi::OnHeartBeatWarning(int nTimeLapse)
{
}

void TradeSpi::OnRspUserLogin(CTShZdRspUserLoginField *pRspUserLogin, 
	CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if(!bIsLast)
	cout <<"login:"<<pRspUserLogin->UserID <<" "<<pRspUserLogin->AccountID <<" " <<pRspUserLogin->CurrencyNo<<" "
	<<pRspUserLogin->UserName<<" "<<nRequestID<<endl;  
}
void TradeSpi::OnRspUserLogout(CTShZdUserLogoutField *pUserLogout, 
	CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}
void TradeSpi::OnRspUserPasswordUpdate(CTShZdUserPasswordUpdateField *pUserPasswordUpdate, 
	CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cout << "pwdUpdate:" << pUserPasswordUpdate->NewPassword << " "<<
		nRequestID<<endl;
}

void TradeSpi::OnRspOrderInsert(CTShZdInputOrderField *pInputOrder, CTShZdRspInfoField *pRspInfo, 
	int nRequestID, bool bIsLast)
{
	cout << "sysNo:" << pInputOrder->OrderRef <<" "<< pRspInfo->ErrorMsg <<" "<< nRequestID <<endl;
}
void TradeSpi::OnRspParkedOrderInsert(CTShZdParkedOrderField *pParkedOrder,CTShZdRspInfoField *pRspInfo, 
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
	if(pInputOrderAction->ActionFlag=='0')
	{
		cout << "cancel:" << pInputOrderAction->OrderRef <<" "<<pInputOrderAction->OrderSysID<<" "<< pRspInfo->ErrorMsg <<endl;
	}
	else
	{
		cout << "modify:" << pInputOrderAction->OrderRef <<" "<<pInputOrderAction->OrderSysID<<" "<< pRspInfo->ErrorMsg <<endl;
		cout<<"改单后数量："<<pInputOrderAction->VolumeChange<<" "<<"改单后的价格："<<pInputOrderAction->LimitPrice<<endl;
	}
}
void TradeSpi::OnRspRemoveParkedOrder(CTShZdRemoveParkedOrderField *pRemoveParkedOrder,CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}
void TradeSpi::OnRspRemoveParkedOrderAction(CTShZdRemoveParkedOrderActionField *pRemoveParkedOrderAction,CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}
void TradeSpi::OnRspQryOrder(CTShZdOrderField *pOrder, CTShZdRspInfoField *pRspInfo, 
	int nRequestID, bool bIsLast)
{
	cout<<"OrderList:"<<pOrder->UserID<<" "<<pOrder->InstrumentID<<" "<<pOrder->OrderRef<<" "
		<<nRequestID<<endl;
	if(bIsLast)
		cout<<"OrderList: over"<<endl;
}
void TradeSpi::OnRspQryTrade(CTShZdTradeField *pTrade, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cout<<"FilledList:"<<pTrade->UserID<<" "<<pTrade->ExchangeID<<" "<<pTrade->InstrumentID<<" "
		<<nRequestID<<endl;
	if(bIsLast)
		cout<<"FilledList: over"<<endl;
}
void TradeSpi::OnRspQryInvestorPosition(CTShZdInvestorPositionField *pInvestorPosition,CTShZdRspInfoField *pRspInfo,int nRequestID, bool bIsLast)
{ //持仓汇总
	cout<<"Position:"<<pInvestorPosition->InstrumentID<<" "<<pInvestorPosition->PositionProfit<<" "
		<<nRequestID<<endl;
	if(bIsLast)
		cout<<"Position: over"<<endl;
}
void TradeSpi::OnRspQryTradingAccount(CTShZdTradingAccountField *pTradingAccount,CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cout<<"Account:"<<pTradingAccount->AccountID<<" "<<string(pTradingAccount->CurrencyNo).substr(0,8).c_str()<<" "
		<<pTradingAccount->CurrUse<<" "<<nRequestID<<endl;
	if(bIsLast)
		cout<<"Account:over"<<endl;
}

void TradeSpi::OnRspQryExchange(CTShZdExchangeField *pExchange, CTShZdRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast)
{
	cout <<"Exchange:"<<pExchange->ExchangeID <<"  "<<pExchange->ExchangeName<<" "
		<<nRequestID<<endl;
	if(bIsLast)
		cout<<"Exchange: over!"<<endl;
}

void TradeSpi::OnRspQryInstrument(CTShZdInstrumentField *pInstrument, CTShZdRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast)
{
	cout <<"Instrument:"<<pInstrument->ExchangeID<<" "<<pInstrument->InstrumentID<<" "
		<<nRequestID<<endl;
	if(bIsLast)
		cout<<"Instrument: over!"<<endl;
}

void TradeSpi::OnRspQryInvestorPositionDetail(CTShZdInvestorPositionDetailField *pInvestorPositionDetail, 
	CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{ //持仓明细
	cout<<"PositionDetail:"<<pInvestorPositionDetail->ExchangeID<<"  "<<pInvestorPositionDetail->Direction<<" "
		<<pInvestorPositionDetail->Volume<<" "<<pInvestorPositionDetail->InstrumentID<<" "
		<<nRequestID<<endl;
	if(bIsLast)
		cout<<"PositionDetail:over"<<endl;
}

void TradeSpi::OnRspQryTransferSerial(CTShZdTransferSerialField *pTransferSerial,
	CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cout<<"InOutMoney:"<<pTransferSerial->UserID<<" "<<pTransferSerial->Amount<<" "<<pTransferSerial->InOutMedth<<
		" "<< nRequestID<<endl;
	if(bIsLast)
		cout<<"InOutMoney:over"<<endl;
}

void TradeSpi::OnRspError(CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cout<<"ErrorInfo:"<<pRspInfo->ErrorID<<" "<<pRspInfo->ErrorMsg<<endl;
	if(bIsLast)
		cout<<"ErrorInfo:over"<<endl;
}

void TradeSpi::OnRtnOrder(CTShZdOrderField *pOrder)
{
	cout << "orderback:" << pOrder->OrderRef <<" "<<pOrder->OrderSubmitStatus <<" "<<pOrder->OrderSysID <<" "<<pOrder->LimitPrice<<endl;
}
void TradeSpi::OnRtnTrade(CTShZdTradeField *pTrade)
{
	cout << "tradeback:" << pTrade->OrderSysID <<" "<<pTrade->TradeID <<" "<<pTrade->OrderRef <<" "<<pTrade->Price<<" "<<pTrade->Volume<<endl;
}
void TradeSpi::OnErrRtnOrderInsert(CTShZdInputOrderField *pInputOrder, CTShZdRspInfoField *pRspInfo)
{
	cout<<"Order Error"<<pInputOrder->OrderLocalID <<" "<<pInputOrder->OrderRef <<" "<<pRspInfo->ErrorID<<" "<<pRspInfo->ErrorMsg <<endl;
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

 void TradeSpi::OnRtnOpenCloseTime(CTShZdOpenCloseTimeField *openCloseTime,int nRequestID,bool bIsLast)
 {
	 cout<<"OpenCloseTime:"<<openCloseTime->TradeDate<<" "<<openCloseTime->OpenTime<<" "<<openCloseTime->CloseTime<<endl;
 }

 void TradeSpi::OnRtnMarketOpenCloseTime(CTShZdMarketOpenCloseTimeField *openCloseTime,int nRequestID,bool bIsLast)
 {
	 cout<<"OpenCloseSumWinTime:"<<openCloseTime->CurrYear<<" "<<openCloseTime->ExchangeID<<" "<<openCloseTime->SummeyBeginTime
		 <<" "<<openCloseTime->WinterBeginTime<<" "<< nRequestID<<endl;
 }

 ///直达支持产品的开收盘时间
 void TradeSpi::OnRtnCommonOpenCloseTime(CTShZdCommonOpenCloseTimeField *commonTime,int nRequestID,bool bIsLast)
 {
	 cout<<"OpenCloseSumWin:"<<commonTime->ProductID<<" "<<commonTime->NomalSummerWinter<<" "<<commonTime->OpenTime
		 <<" "<<commonTime->CloseTime<<" "<< nRequestID<<endl;
 }

 void TradeSpi::OnRtnTradeMoney(CTShZdTradeMoneyField *pTradeMoney)
 {
	 cout<<"OnRtnTradeMoney:"<<pTradeMoney->UserID<<" "<<pTradeMoney->AccountID<<" "<<pTradeMoney->FrozenDeposit
		 <<" "<<pTradeMoney->Commission<<endl;
 }

 void TradeSpi::OnRspMoneyRatio(CTShZdMoneyRatioField * rspRation,int nRequestID,bool bIsLast)
 {
	 cout<<"Ratio:"<<rspRation->CurrencyID <<" "<<rspRation->Ratio<<" "<<nRequestID <<endl;
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
	string selectchar = "";
	selectchar=getchar();
	int index=0;
	if(selectchar.compare("m") == 0)
	{
		#pragma region //行情测试
		selectchar = getchar();
		selectchar = getchar();
		
		while (selectchar.compare("e") != 0) {
			if(selectchar.compare("a") == 0)
			{				
				apiHandle = CSHZdMarketApi::CreateSHZdMarketApi("..\\marketLog",false);
				apiHandle->RegisterSpi(new MarketSpi);
				apiHandle->Init();
				apiHandle->AuthonInfo("55822DC39D9316D5111D9EED00C1CED81B6F0DCEA8D97DDEBD350D939CF8A9D304E3C73A742CFB80");				
				apiHandle->RegisterLoginFront("protocol://222.73.119.230:7003");
				
				Sleep(2000);
				CTShZdReqUserLoginField field;
				memset(&field,0,sizeof(CTShZdReqUserLoginField));
				memcpy(field.UserID,"demo000601",16);
				memcpy(field.Password,"888888",41);
				apiHandle->ReqUserLogin(&field,1);	
				Sleep(5000);
				apiHandle->GetTradingDay();
				apiHandle->RegisterFront("protocol://222.73.105.170:9002");//

				char *ppInstrumentID[8];			
				string temp1="HKEX,HHI1702;HKEX,HHI1703;HKEX,HHI1704;HKEX,HHI1705;HKEX,HHI1706";
				string temp2="HKEX,HHI1707;HKEX,HHI1708;HKEX,HHI1709;HKEX,HHI1710";	
				string temp3="HKEX,HSI1702;HKEX,HSI1703;HKEX,HSI1704;HKEX,HSI1705;HKEX,HSI1706";
				string temp4="HKEX,HSI1707;HKEX,HSI1708;HKEX,HSI1709;HKEX,HSI1710";
				string temp5="ICE,BRN1704;ICE,BRN1705;ICE,BRN1706;ICE,BRN1707;ICE,BRN1708";
				string temp6="ICE,BRN1709;ICE,BRN1710;ICE,BRN1711;ICE,BRN1712";
				string temp7="LME,AA3M;LME,AH3M";
				string temp8="LME,CA3M";
				ppInstrumentID[0]=(char*)temp1.c_str();		
				ppInstrumentID[1]=(char*)temp2.c_str();
				ppInstrumentID[2]=(char*)temp3.c_str();		
				ppInstrumentID[3]=(char*)temp4.c_str();
				ppInstrumentID[4]=(char*)temp5.c_str();		
				ppInstrumentID[5]=(char*)temp6.c_str();
				ppInstrumentID[6]=(char*)temp7.c_str();		
				ppInstrumentID[7]=(char*)temp8.c_str();

				apiHandle->SubscribeMarketData(ppInstrumentID,8); 
			}
			else if(selectchar.compare("r") == 0)
			{
				apiHandle->Release();
			}
			else if(selectchar.compare("k") == 0)
			{
				char *ppInstrumentID[2];
				string temp="CME,6B1702;CME,GC1701;CME,6J1702;CME,GCK1702";
				ppInstrumentID[0]=(char*)temp.c_str();	
				string temp1="HKEX,HSI1703;HKEX,HSI1703";
				ppInstrumentID[1]=(char*)temp1.c_str();
				apiHandle->SubscribeMarketData(ppInstrumentID,2);
			}
			selectchar = getchar();
		}
		#pragma endregion
	}
	else if(selectchar.compare("t") == 0)
	{
		selectchar = getchar();
		selectchar = getchar();
		while (selectchar.compare("e") != 0) {
			if(selectchar.compare("a") == 0)
			{
				apiTrade = CSHZdTraderApi::CreateSHZdTraderApi("..\\tradeLog",false);
				apiTrade->RegisterSpi(new TradeSpi);
				apiTrade->Init();
				apiTrade->AuthonInfo("55822DC39D9316D5111D9EED00C1CED81B6F0DCEA8D97DDEBD350D939CF8A9D304E3C73A742CFB80");				
				apiTrade->RegisterFront("protocol://222.73.119.230:7003");// 222.73.119.230:7003      
			}
			else if(selectchar.compare("l") == 0)
			{ //登陆
				CTShZdReqUserLoginField field;
				memset(&field,0,sizeof(CTShZdReqUserLoginField));
				memcpy(field.UserID,"MN000301",16);//f000202
				memcpy(field.Password,"888888",41);
				apiTrade->ReqUserLogin(&field,100);	
				Sleep(500);
				apiTrade->GetTradingDay();
			}
			else if(selectchar.compare("u") == 0)
			{//用户登出
				CTShZdUserLogoutField pLogout;
				memset(&pLogout,0,sizeof(CTShZdUserLogoutField));
				memcpy(pLogout.UserID,"MN000301",16); 
				apiTrade->ReqUserLogout(&pLogout,0);
			}
			else if(selectchar.compare("r") == 0)
			{ //退出dll
				apiTrade->Release();
			}
			else if(selectchar.compare("p") == 0)
			{//修改密码
				CTShZdUserPasswordUpdateField pPwdUpdate;
				memset(&pPwdUpdate,0,sizeof(CTShZdUserPasswordUpdateField));				
				memcpy(&pPwdUpdate.UserID,"MN000301",16);
				memcpy(&pPwdUpdate.OldPassword,"666666",41);
				memcpy(&pPwdUpdate.NewPassword,"888888",41);
				apiTrade->ReqUserPasswordUpdate(&pPwdUpdate,0);
			}		
			else if(selectchar.compare("b") == 0)
			{
				CTShZdInputOrderField pOrder;
				memset(&pOrder,0,sizeof(CTShZdInputOrderField));

				memcpy(pOrder.OrderLocalID,"20161128141700",14);
				memcpy(pOrder.ExchangeID,"CME",11);//交易所
				memcpy(pOrder.InvestorID,"MN00000903",13);//资金帐号
				
				memcpy(pOrder.InstrumentID,"CL1703",31);
				memcpy(pOrder.UserID,"MN000301",16);
				pOrder.Direction='1';
				pOrder.VolumeTotalOriginal = 3;
				pOrder.LimitPrice = atof("45");
				pOrder.OrderPriceType='2';
				pOrder.OrderType ='P';
				apiTrade->ReqOrderInsert(&pOrder,0);
			}
			else if(selectchar.compare("s") == 0)
			{
				CTShZdInputOrderField pOrder;
				memset(&pOrder,0,sizeof(CTShZdInputOrderField));
				memcpy(pOrder.OrderLocalID,"20161128141700",14);
				memcpy(pOrder.ExchangeID,"CME",11);//交易所
				memcpy(pOrder.InvestorID,"MN00000903",13);//资金帐号				
				memcpy(pOrder.InstrumentID,"CL1703",31);
				memcpy(pOrder.UserID,"TJ000771",16);
				pOrder.Direction='2';
				pOrder.VolumeTotalOriginal = 3;
				pOrder.LimitPrice = atof("60");
				pOrder.OrderPriceType='1';
				pOrder.OrderType ='P';
				apiTrade->ReqOrderInsert(&pOrder,0);

			}			
			else if(selectchar.compare("c") == 0)
			{//撤单
				CTShZdOrderActionField pCancel;
				memset(&pCancel,0,sizeof(CTShZdInputOrderActionField));
				
				printf("依次输入系统号，订单号(空格分开)\n");
				scanf("%s%s",pCancel.OrderRef,pCancel.OrderSysID);
				pCancel.ActionFlag = '0';//0是撤单
				pCancel.OrderType ='P';
				apiTrade->ReqOrderAction(&pCancel,0);
			}
			else if(selectchar.compare("m") == 0)
			{//改单
				CTShZdOrderActionField pModify;
				memset(&pModify,0,sizeof(CTShZdInputOrderActionField));
				
				printf("依次输入订单号，系统号（空格分开）\n");
				scanf("%s%s",pModify.OrderSysID,pModify.OrderRef);
				pModify.VolumeChange = 4;//改单后的数量
				pModify.LimitPrice = 46.00;//改单价格
				//pModify.ModifyTriggerPrice = atof("0.00");
				pModify.ActionFlag = '3';//3是改单
				pModify.OrderType ='P';
				apiTrade->ReqOrderAction(&pModify,0);
			}
			else if(selectchar.compare("5") == 0)
			{//委托查询
				CTShZdQryOrderField pQryOrder;
				memset(&pQryOrder,0,sizeof(CTShZdQryOrderField));
				memcpy(pQryOrder.UserID,"MN000301",13);
				apiTrade->ReqQryOrder(&pQryOrder,5);
			}
			else if(selectchar.compare("6") == 0)
			{//成交查询
				CTShZdQryTradeField pQryTrade;
				memset(&pQryTrade,0,sizeof(CTShZdQryTradeField));
				memcpy(pQryTrade.UserID,"MN000301",13);
				apiTrade->ReqQryTrade(&pQryTrade,6);
			}
			else if(selectchar.compare("7") == 0)
			{//资金查询
				CTShZdQryTradingAccountField pQryTradingAccount;
				memset(&pQryTradingAccount,0,sizeof(CTShZdQryTradingAccountField));
				memcpy(pQryTradingAccount.UserID,"MN000301",13);
				apiTrade->ReqQryTradingAccount(&pQryTradingAccount,7);
			}
			else if(selectchar.compare("8") == 0)
			{//交易所查询
				CTShZdQryExchangeField pQryExchange;
				memset(&pQryExchange,0,sizeof(CTShZdQryExchangeField));
				memcpy(pQryExchange.ExchangeID,"",9);
				apiTrade->ReqQryExchange(&pQryExchange, 8);
			}
			else if(selectchar.compare("9") == 0)
			{//合约查询
				index++;
				CTShZdQryInstrumentField pQryInstrument;
				memset(&pQryInstrument,0,sizeof(CTShZdQryInstrumentField));
				memcpy(pQryInstrument.ExchangeID,"",9);
				memcpy(pQryInstrument.InsertTimeStart,"20170101",10);
				memcpy(pQryInstrument.ProductID,"",9);
				pQryInstrument.Index=index*500; //
				apiTrade->ReqQryInstrument(&pQryInstrument,9);				
			}
			else if(selectchar.compare("z") == 0)
			{//持仓明细查询
				CTShZdQryInvestorPositionDetailField pQryInvestorPositionDetail;
				memset(&pQryInvestorPositionDetail,0,sizeof(CTShZdQryInvestorPositionDetailField));
				memcpy(pQryInvestorPositionDetail.UserID,"MN000301",13);
				apiTrade->ReqQryInvestorPositionDetail(&pQryInvestorPositionDetail,11);
			}
			else if(selectchar.compare("y") == 0)
			{//持仓汇总查询
				CTShZdQryInvestorPositionField pQryInvestorPosition;
				memset(&pQryInvestorPosition,0,sizeof(CTShZdQryInvestorPositionField));
				memcpy(pQryInvestorPosition.UserID,"MN000301",13);
				memcpy(pQryInvestorPosition.InvestorID,"MN00000903",18);
				apiTrade->ReqQryInvestorPosition(&pQryInvestorPosition,12);
			}
			else if(selectchar.compare("x") == 0)
			{//直达开收盘时间
				CTShZdReqOpenCloseTimeField OpenCloseTime;
				memcpy(OpenCloseTime.UserID,"MN000301",13);
				apiTrade->ReqQueryOpenCloseTime(&OpenCloseTime,13);
			}
			else if(selectchar.compare("w") == 0)
			{ //出入金
				CTShZdQryTransferSerialField pQryTransferSerial;
				memset(&pQryTransferSerial,0,sizeof(CTShZdQryTransferSerialField));
				memcpy(pQryTransferSerial.UserID,"MN000301",13);
				pQryTransferSerial.InOutMedth='K';
				pQryTransferSerial.InOutMoneyType='I';
				pQryTransferSerial.Amount=6000;
				pQryTransferSerial.InOutMoneyUsing='F';
				memcpy(pQryTransferSerial.CurrencyID,"USD",5);
				apiTrade->ReqQryTransferSerial(&pQryTransferSerial,14);
			}
			else if(selectchar.compare("j") == 0)
			{//直达支持的交易所的冬夏令时
				CTShZdReqMarketOpenCloseTimeField OpenCloseTime;
				memset(&OpenCloseTime,0,sizeof(CTShZdReqMarketOpenCloseTimeField));
				memcpy(OpenCloseTime.UserID,"demo000177",16);
				apiTrade->ReqQueryMarketOpenCloseTime(&OpenCloseTime,15);
			}
			else if(selectchar.compare("k") == 0)
			{ //产品的开收盘时间
				CTShZdReqCommonOpenCloseTimeField comTime;
				memset(&comTime,0,sizeof(CTShZdReqCommonOpenCloseTimeField));
				memcpy(comTime.UserID,"demo000177",16);
				memcpy(comTime.ExchangeID,"CME",9);
				apiTrade->ReqQueryCommonOpenCloseTime(&comTime,15);
			}
			else if(selectchar.compare("f")==0)
			{//循环报单
				int i=0;
				while(i<50)
				{
					i++;
					int j=i%2;
					CTShZdInputOrderField pOrder;
					memset(&pOrder,0,sizeof(CTShZdInputOrderField));
					string tem="2016112814000"+IntToStr(i);
					memcpy(pOrder.OrderLocalID,tem.c_str(),14);
					memcpy(pOrder.ExchangeID,"CME",11);//交易所
					memcpy(pOrder.InvestorID,"MN00000901",13);//资金帐号				
					memcpy(pOrder.InstrumentID,"CL1703",31);
					memcpy(pOrder.UserID,"MN000301",16);					
					pOrder.VolumeTotalOriginal = 3;
					pOrder.LimitPrice = atof("60");
					pOrder.OrderPriceType='2';				
					if(j==0)
					{
						pOrder.Direction='2';
					}
					else
					{
						pOrder.Direction='1';
					}
					apiTrade->ReqOrderInsert(&pOrder,500+i);
					Sleep(200);
				}
			}
			else  if(selectchar.compare("q")==0)
			{//汇率
				CTShZdReqMoneyRatioField reqRation;
				memset(&reqRation,0,sizeof(CTShZdReqMoneyRatioField));
				memcpy(reqRation.UserID,"MN000301",9);
				apiTrade->ReqQueryMoneyRatio(&reqRation,20);
			}
			selectchar = getchar();
		}	
		selectchar = getchar();
	}
	return 0;
}

