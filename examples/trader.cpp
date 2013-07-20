
#include	<iostream>
#include	<unistd.h>
#include	"trader.h"
#include	"cJSON.h"
#include	"stdlib.h"

//报单-限价
int Trade::ReqOrderInsert(const char* instrument, double price, int director, int offset, int volume)
{
	CThostFtdcInputOrderField f;
	memset(&f, 0, sizeof(f));
	f.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;	//1投机
	f.ContingentCondition = THOST_FTDC_CC_Immediately;//立即触发
	f.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	f.IsAutoSuspend = 0;
	f.OrderPriceType = THOST_FTDC_OPT_LimitPrice;		//***任意价1  限价2***
	f.TimeCondition =  THOST_FTDC_TC_GFD;		//***立即完成1  当日有效3***
	f.VolumeCondition = THOST_FTDC_VC_AV;	//任意数量1  最小数量2  全部成交3
	f.MinVolume = 1;

	strcpy(f.InvestorID, investor);
	strcpy(f.UserID, investor);
	strcpy(f.BrokerID, broker);

	strcpy(f.InstrumentID, instrument);	//合约

	f.LimitPrice = price;				//***价格***

	if(director == 0)
		f.Direction = THOST_FTDC_D_Buy;			//买
	else
		f.Direction = THOST_FTDC_D_Sell;			//卖

	if(offset == 0)
		f.CombOffsetFlag[0] = THOST_FTDC_OF_Open;//开仓
	else if(offset == 1)
		f.CombOffsetFlag[0] = THOST_FTDC_OF_Close;	//平仓
	else
		f.CombOffsetFlag[0] = THOST_FTDC_OF_CloseToday;	//平今

	f.VolumeTotalOriginal = volume;//数量

	sprintf(f.OrderRef, "%d", ++orderRef);//OrderRef

	return pUserApi->ReqOrderInsert(&f, iReqID++);	//报单
}
//报单-市价
int Trade::ReqOrderInsert(const char* instrument, int director, int offset, int volume)
{
	CThostFtdcInputOrderField f;
	memset(&f, 0, sizeof(f));
	f.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;	//1投机
	f.ContingentCondition = THOST_FTDC_CC_Immediately;//立即触发
	f.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	f.IsAutoSuspend = 0;
	f.OrderPriceType = THOST_FTDC_OPT_AnyPrice;		//***任意价1  限价2***
	f.TimeCondition =  THOST_FTDC_TC_IOC;		//***立即完成1  当日有效3***
	f.VolumeCondition = THOST_FTDC_VC_AV;	//任意数量1  最小数量2  全部成交3
	f.MinVolume = 1;

	strcpy(f.InvestorID, investor);
	strcpy(f.UserID, investor);
	strcpy(f.BrokerID, broker);

	strcpy(f.InstrumentID, instrument);	//合约

	f.LimitPrice = 0;					//***价格***

	if(director == 0)
		f.Direction = THOST_FTDC_D_Buy;			//买
	else
		f.Direction = THOST_FTDC_D_Sell;			//卖

	if(offset == 0)
		f.CombOffsetFlag[0] = THOST_FTDC_OF_Open;//开仓
	else if(offset == 1)
		f.CombOffsetFlag[0] = THOST_FTDC_OF_Close;	//平仓
	else
		f.CombOffsetFlag[0] = THOST_FTDC_OF_CloseToday;	//平今

	f.VolumeTotalOriginal = volume;//数量

	sprintf(f.OrderRef, "%d", ++orderRef);//OrderRef

	return pUserApi->ReqOrderInsert(&f, iReqID++);	//报单
}
//连续报单
void Trade::pushOrders(int ticks, int seconds, const char* instrument, double price, int director, int offset, int volume)
{
	int slp = 1000 / ticks;
	for(int i = 0; i < ticks * seconds; ++i)
	{
		ReqOrderInsert(instrument, price, director, offset, volume);
	}
}

//撤单
int Trade::ReqOrderAction(const char* instrument, int session, int frontid, const char* orderref)
{
	CThostFtdcInputOrderActionField f;
	memset(&f, 0, sizeof(f));
	f.ActionFlag = THOST_FTDC_AF_Delete; 	//THOST_FTDC_AF_Modify
	strcpy(f.BrokerID, broker);
	strcpy(f.InvestorID, investor);

	strcpy(f.InstrumentID, instrument);
	f.SessionID = session;
	f.FrontID = frontid;
	strcpy(f.OrderRef, orderref);
	cout<<__FUNCTION__<<endl;

	return pUserApi->ReqOrderAction(&f, ++iReqID);
}

//查持仓
int Trade::ReqQryInvestorPosition()
{
	CThostFtdcQryInvestorPositionField f;
	memset(&f, 0, sizeof(f));
	strcpy(f.BrokerID, broker);
	strcpy(f.InvestorID, investor);

	return pUserApi->ReqQryInvestorPosition(&f, ++iReqID);
}

//查持仓明细
int Trade::ReqQryInvestorPositionDetail()
{
	CThostFtdcQryInvestorPositionDetailField f;
	memset(&f, 0, sizeof(f));
	strcpy(f.BrokerID, broker);
	strcpy(f.InvestorID, investor);
	return pUserApi->ReqQryInvestorPositionDetail(&f, ++iReqID);
}

//查资金
int Trade::ReqQryTradingAccount()
{
	int ret = 0;
	CThostFtdcQryTradingAccountField f;
	strcpy(f.BrokerID, broker);
	strcpy(f.InvestorID, investor);
	do {
		ret =  pUserApi->ReqQryTradingAccount(&f, ++iReqID);
		//sleep (500);		
	} while (3 == ret);

	return ret;
}

//查签约银行
int Trade::ReqQryAccountregister()
{
	CThostFtdcQryAccountregisterField f;
	memset(&f, 0, sizeof(f));
	strcpy(f.BrokerID, broker);
	strcpy(f.AccountID, investor);

	return  pUserApi->ReqQryAccountregister(&f, ++iReqID);
}

//查银行帐户
int Trade::ReqQueryBankAccountMoneyByFuture(const char* bankID, const char* bankPWD, const char* accountPWD)
{
	CThostFtdcReqQueryAccountField f;
	memset(&f, 0, sizeof(f));
	//strcpy(f.TradeCode, "204002");

	strcpy(f.BankID, bankID);
	strcpy(f.BankBranchID, "0000");		//必须有
	strcpy(f.BankPassWord, bankPWD);

	strcpy(f.BrokerID, broker);
	strcpy(f.AccountID, investor);
	strcpy(f.Password, accountPWD);
	f.SecuPwdFlag = THOST_FTDC_BPWDF_BlankCheck;	//明文核对
	strcpy(f.CurrencyID, "RMB"); 					//币种：RMB-人民币 USD-美圆 HKD-港元

	cout<<__FUNCTION__<<endl;
	return pUserApi->ReqQueryBankAccountMoneyByFuture(&f, ++iReqID);
}

//银转功能
int Trade::ReqTransferByFuture(const char* bankID, const char* bankPWD, const char* accountPWD, double amount, bool f2B)
{
	CThostFtdcReqTransferField f;
	memset(&f, 0, sizeof(f));

	strcpy(f.BankID, bankID);
	strcpy(f.BankBranchID, "0000");		//必须有
	strcpy(f.BankPassWord, bankPWD);

	strcpy(f.BrokerID, broker);
	strcpy(f.AccountID, investor);
	strcpy(f.Password, accountPWD);
	f.SecuPwdFlag = THOST_FTDC_BPWDF_BlankCheck;	//明文核对
	strcpy(f.CurrencyID, "RMB"); 					//币种：RMB-人民币 USD-美圆 HKD-港元

	f.TradeAmount = amount;
	if(f2B)	//期转银
		return pUserApi->ReqFromFutureToBankByFuture(&f, ++iReqID);
	else	//银转期
		return pUserApi->ReqFromBankToFutureByFuture(&f, ++iReqID);
}

//连接
void Trade::OnFrontConnected()
{
	ReqLogin();
	cerr<<"@"<<__FUNCTION__<<endl;
	status = 1;
//	state = state_after_connect;
}

//断开
void Trade::OnFrontDisconnected(int nReason)
{
	cerr<<"响应|交易断开!"<<nReason<<endl;
	status = 0;
}

//心跳
void Trade::OnHeartBeatWarning(int nTimeLapse)
{
}

//认证
void Trade::OnRspAuthenticate(CThostFtdcRspAuthenticateField* pRspAuthenticateField, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
}

//登录
void Trade::OnRspUserLogin(CThostFtdcRspUserLoginField* pRspUserLogin,
	CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	int ret;
	if(IsErrorRspInfo(pRspInfo))
	{
		cerr<<"登录错误" << pRspInfo->ErrorID<<":"<<pRspInfo->ErrorMsg<< endl;
		pUserApi->Release();
	}
	else
	{
		CThostFtdcQrySettlementInfoConfirmField req;
		memset(&req, 0, sizeof(req));
		strcpy(req.BrokerID, broker);
		strcpy(req.InvestorID, investor);
	        cerr<<__FUNCTION__<<endl;
		status = 3;
//		int ret = pUserApi->ReqQrySettlementInfoConfirm(&req, ++iReqID);
	}
}

//查结算确认信息
void Trade::OnRspQrySettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField* pSettlementInfoConfirm,
	CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	int ret;
	if(this->IsErrorRspInfo(pRspInfo))
	{
		cerr<<"查询结算确认错误"<<endl;
	}
	else
	{  
		strcpy(tradingDay, pUserApi->GetTradingDay());
		if((pSettlementInfoConfirm) && strcmp(pSettlementInfoConfirm->ConfirmDate, tradingDay) == 0)
		{
			CThostFtdcSettlementInfoConfirmField f;
			memset(&f, 0, sizeof(f));
			strcpy(f.BrokerID, broker);
			strcpy(f.InvestorID, investor);
			ret = pUserApi->ReqSettlementInfoConfirm(&f, ++iReqID);
		}
		else
		{
			CThostFtdcQrySettlementInfoField f;
//			cerr<<"请求 | 查结算信息..."<<endl;
			memset(&f, 0, sizeof(f));
			strcpy(f.BrokerID, broker);
			strcpy(f.InvestorID, investor);
			status = 3;
			ret = pUserApi->ReqQrySettlementInfo(&f, ++iReqID);
		}
	}
}

//查结算信息
void Trade::OnRspQrySettlementInfo(CThostFtdcSettlementInfoField* pSettlementInfo,
	CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	if(pSettlementInfo)
		this->settleInfo += pSettlementInfo->Content;
	if(bIsLast)
	{
		//settleInfo, 1);		//用户确认
		CThostFtdcSettlementInfoConfirmField f;

		memset(&f, 0, sizeof(f));
		strcpy(f.BrokerID, broker);
		strcpy(f.InvestorID, investor);
		pUserApi->ReqSettlementInfoConfirm(&f, ++iReqID);
	}
}

//确认结算
void Trade::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField* pSettlementInfoConfirm,
	CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	status = 3;
//	main_handler(req, res);
}

//查合约
void Trade::OnRspQryInstrument(CThostFtdcInstrumentField* pInstrument,
	CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	if(!IsErrorRspInfo(pRspInfo))
	{
		char* tmp = new char[strlen(pInstrument->InstrumentID) + 1];
		strcpy(tmp, pInstrument->InstrumentID);
		//       dicInstrument[string(tmp)] = *pInstrument;	//合约名称标识
	}
	if(bIsLast)
	{
		if(this->addrID == -1)	//连接
		{
			char* tmp = new char[256];
			strcpy(tmp, string(this->frontAddr).substr(0, strlen(this->frontAddr)-2).append("13").c_str());
			//           quote->ReqConnect(tmp, this->broker);
		}
		else
			;//            ;quote->ReqConnect(addrID);
	}
}

//查持仓响应
void Trade::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField* pInvestorPosition,
	CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	if(pInvestorPosition)
	{
	}

	if(bIsLast)
	{
		isdone = 1;
	}
}

//查持仓明细
void Trade::OnRspQryInvestorPositionDetail(CThostFtdcInvestorPositionDetailField* pInvestorPositionDetail,
	CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	if (bIsLast) {
		isdone = 1;
	}
}

//查资金
void Trade::OnRspQryTradingAccount(CThostFtdcTradingAccountField* pTradingAccount,
	CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	if(bIsLast)
	{
		if (!IsErrorRspInfo(pRspInfo) &&  pTradingAccount){
			cJSON	*root, *fmt; char* out;
			root = cJSON_CreateObject();
			cJSON_AddItemToObject(root, "return", cJSON_CreateString("sucess"));
			cJSON_AddItemToObject(root, "format", fmt = cJSON_CreateObject());
			cJSON_AddNumberToObject(fmt, "balance", pTradingAccount->Balance);
			cJSON_AddNumberToObject(fmt, "available", pTradingAccount->Available);
			cJSON_AddNumberToObject(fmt, "currmargin", pTradingAccount->CurrMargin);
			cJSON_AddNumberToObject(fmt, "closeprofit", pTradingAccount->CloseProfit);
			cJSON_AddNumberToObject(fmt, "positionprofit", pTradingAccount->PositionProfit);
			cJSON_AddNumberToObject(fmt, "commission", pTradingAccount->Commission);
			cJSON_AddNumberToObject(fmt, "frozenmargin", pTradingAccount->FrozenMargin);
			cJSON_AddNumberToObject(fmt, "frozencommission", pTradingAccount->FrozenCommission);
			
			out = cJSON_Print(root);
			memcpy(buffer, out, strlen(out));
			cJSON_Delete(root);
			free(out);
		}
		isdone = 1;
	}
}

//报单响应
void Trade::OnRtnOrder(CThostFtdcOrderField* pOrder)
{
	cJSON	*root;
	char	*out;
	root = cJSON_CreateObject();
	cJSON_AddItemToObject(root, "return", cJSON_CreateString("sucess"));
	
	out = cJSON_Print(root);
	memcpy(buffer, out, strlen(out));
	cJSON_Delete(root);
	free(out);
	isdone = 1;
}

//成交响应
void Trade::OnRtnTrade(CThostFtdcTradeField* pTrade)
{
	cJSON	*root;
	char	*out;
	root = cJSON_CreateObject();
	cJSON_AddItemToObject(root, "return", cJSON_CreateString("sucess"));
	
	out = cJSON_Print(root);
	memcpy(buffer, out, strlen(out));
	cJSON_Delete(root);
	free(out);
	isdone = 1;
	//show(str(//boost::format("成交编号:%1%, 平台编号%2%, 成交时间%3%")%pTradeID %pBrokerOrderSeq %pTradeTime), 2);
}

//报单错误
void Trade::OnRspOrderInsert(CThostFtdcInputOrderField* pInputOrder, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	if (bIsLast) {
		isdone = 1;
	}
	//show(pRspInfo->ErrorMsg, 2);
}

//报单错误
void Trade::OnErrRtnOrderInsert(CThostFtdcInputOrderField* pInputOrder, CThostFtdcRspInfoField* pRspInfo)
{
	//show(pRspInfo->ErrorMsg, 2);
	isdone = 1;
}

//撤单错误
void Trade::OnRspOrderAction(CThostFtdcInputOrderActionField* pInputOrderAction, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	//show(pRspInfo->ErrorMsg, 2);
	isdone = 1;
}

//签约银行
void Trade::OnRspQryAccountregister(CThostFtdcAccountregisterField* pAccountregister,
	CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	if(IsErrorRspInfo(pRspInfo))
		;//show(str(//boost::format("查签约银行错误:%1%") %pRspInfo->ErrorMsg));
	else if(pAccountregister)
	{
		string bank = "";
		if(pAccountregister->BankID[0] == THOST_FTDC_BF_ABC)
			bank = "农业银行";
		else if(pAccountregister->BankID[0] == THOST_FTDC_BF_BC)
			bank = "中国银行";
		else if(pAccountregister->BankID[0] == THOST_FTDC_BF_BOC)
			bank = "交通银行";
		else if(pAccountregister->BankID[0] == THOST_FTDC_BF_CBC)
			bank = "建设银行";
		else if(pAccountregister->BankID[0] == THOST_FTDC_BF_ICBC)
			bank = "工商银行";
		else if(pAccountregister->BankID[0] == THOST_FTDC_BF_Other)
			bank = "其他银行";

		string bankID = string(pAccountregister->BankAccount);
		bankID = bankID.substr(strlen(pAccountregister->BankAccount)-4, 4);
	}
	else
		;//show("无签约银行!", 1);
}

//查银行帐户响应
void Trade::OnRtnQueryBankBalanceByFuture(CThostFtdcNotifyQueryAccountField* pNotifyQueryAccount)
{
	//show(str(//boost::format("可用余额:%1%, 可取余额:%2%")%pNotifyQueryAccount->BankUseAmount
	//             %pNotifyQueryAccount->BankFetchAmount), 1);
}

//查银行帐户错误
void Trade::OnRspQueryBankAccountMoneyByFuture(CThostFtdcReqQueryAccountField* pReqQueryAccount, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	if(IsErrorRspInfo(pRspInfo))
		cerr<<"查询银行帐号错误:%1%"<<endl;
	else
		;//show(pReqQueryAccount->Digest, 1);
}

//查银行帐户错误
void Trade::OnErrRtnQueryBankBalanceByFuture(CThostFtdcReqQueryAccountField* pReqQueryAccount, CThostFtdcRspInfoField* pRspInfo)
{
	cerr<<"查银行余额错误:%1%"<<endl;
}

//期->银
void Trade::OnRtnFromFutureToBankByFuture(CThostFtdcRspTransferField* pRspTransfer)
{
	if(pRspTransfer->ErrorID == 0)
		cerr<<"期货转银行成功!"<<endl;
	else
		cerr<<"期货转银行失败:%1%"<<endl;
}

//期->银错误
void Trade::OnErrRtnFutureToBankByFuture(CThostFtdcReqTransferField* pReqTransfer, CThostFtdcRspInfoField* pRspInfo)
{
	//show(str(//boost::format("期货转银行失败:%1%") %pRspInfo->ErrorMsg), 1);
}

//期->银错误
void Trade::OnRspFromFutureToBankByFuture(CThostFtdcReqTransferField* pReqTransfer, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	//show(str(//boost::format("期货转银行失败:%1%") %pRspInfo->ErrorMsg), 1);
}

//银->期
void Trade::OnRtnFromBankToFutureByFuture(CThostFtdcRspTransferField* pRspTransfer)
{
	if(pRspTransfer->ErrorID == 0)
		;//show("银行转期货成功!", 1);
	else
		;//show(str(//boost::format("银行转期货失败:%1%") %pRspTransfer->ErrorMsg), 1);
}

//银->期错误
void Trade::OnErrRtnBankToFutureByFuture(CThostFtdcReqTransferField* pReqTransfer, CThostFtdcRspInfoField* pRspInfo)
{
	//show(str(//boost::format("银行转期货失败:%1%") %pRspInfo->ErrorMsg), 1);
}

//银->期错误
void Trade::OnRspFromBankToFutureByFuture(CThostFtdcReqTransferField* pReqTransfer, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	//show(str(//boost::format("银行转期货失败:%1%") %pRspInfo->ErrorMsg), 1);
}

//(*交易响应事件
void Trade::OnRspUserLogout(CThostFtdcUserLogoutField* pUserLogout, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	cerr<<"logout+"<<pUserLogout->UserID<<endl;
	status = 1;
}

void Trade::OnRspUserPasswordUpdate(CThostFtdcUserPasswordUpdateField* pUserPasswordUpdate, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
}

void Trade::OnRspTradingAccountPasswordUpdate(CThostFtdcTradingAccountPasswordUpdateField* pTradingAccountPasswordUpdate, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
}

void Trade::OnRspParkedOrderInsert(CThostFtdcParkedOrderField* pParkedOrder, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
}

void Trade::OnRspParkedOrderAction(CThostFtdcParkedOrderActionField* pParkedOrderAction, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
}

void Trade::OnRspQueryMaxOrderVolume(CThostFtdcQueryMaxOrderVolumeField* pQueryMaxOrderVolume, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
}

void Trade::OnRspRemoveParkedOrder(CThostFtdcRemoveParkedOrderField* pRemoveParkedOrder, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
}

void Trade::OnRspRemoveParkedOrderAction(CThostFtdcRemoveParkedOrderActionField* pRemoveParkedOrderAction, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
}

void Trade::OnRspQryOrder(CThostFtdcOrderField* pOrder, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	if (bIsLast) {
		isdone = 1;	
	}
}

void Trade::OnRspQryTrade(CThostFtdcTradeField* pTrade, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	if (bIsLast) {
		isdone = 1;	
	}
}

void Trade::OnRspQryInvestor(CThostFtdcInvestorField* pInvestor, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	if (bIsLast) {
		isdone = 1;	
	}
}

void Trade::OnRspQryTradingCode(CThostFtdcTradingCodeField* pTradingCode, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
}

void Trade::OnRspQryInstrumentMarginRate(CThostFtdcInstrumentMarginRateField* pInstrumentMarginRate, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
}

void Trade::OnRspQryInstrumentCommissionRate(CThostFtdcInstrumentCommissionRateField* pInstrumentCommissionRate, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
}

void Trade::OnRspQryExchange(CThostFtdcExchangeField* pExchange, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
}

void Trade::OnRspQryDepthMarketData(CThostFtdcDepthMarketDataField* pDepthMarketData, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
}

void Trade::OnRspQryTransferBank(CThostFtdcTransferBankField* pTransferBank, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
}

void Trade::OnRspQryNotice(CThostFtdcNoticeField* pNotice, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
}

void Trade::OnRspQryInvestorPositionCombineDetail(CThostFtdcInvestorPositionCombineDetailField* pInvestorPositionCombineDetail, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
}

void Trade::OnRspQryCFMMCTradingAccountKey(CThostFtdcCFMMCTradingAccountKeyField* pCFMMCTradingAccountKey, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
}

void Trade::OnRspQryEWarrantOffset(CThostFtdcEWarrantOffsetField* pEWarrantOffset, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
}

void Trade::OnRspQryTransferSerial(CThostFtdcTransferSerialField* pTransferSerial, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
}

void Trade::OnRspError(CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
}

void Trade::OnErrRtnOrderAction(CThostFtdcOrderActionField* pOrderAction, CThostFtdcRspInfoField* pRspInfo)
{
}

void Trade::OnRtnInstrumentStatus(CThostFtdcInstrumentStatusField* pInstrumentStatus)
{
}

void Trade::OnRtnTradingNotice(CThostFtdcTradingNoticeInfoField* pTradingNoticeInfo)
{
}

void Trade::OnRtnErrorConditionalOrder(CThostFtdcErrorConditionalOrderField* pErrorConditionalOrder)
{
}

void Trade::OnRspQryContractBank(CThostFtdcContractBankField* pContractBank, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
}

void Trade::OnRspQryParkedOrder(CThostFtdcParkedOrderField* pParkedOrder, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
}

void Trade::OnRspQryParkedOrderAction(CThostFtdcParkedOrderActionField* pParkedOrderAction, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
}

void Trade::OnRspQryTradingNotice(CThostFtdcTradingNoticeField* pTradingNotice, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
}

void Trade::OnRspQryBrokerTradingParams(CThostFtdcBrokerTradingParamsField* pBrokerTradingParams, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
}

void Trade::OnRspQryBrokerTradingAlgos(CThostFtdcBrokerTradingAlgosField* pBrokerTradingAlgos, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
}

void Trade::OnRtnFromBankToFutureByBank(CThostFtdcRspTransferField* pRspTransfer)
{
}

void Trade::OnRtnFromFutureToBankByBank(CThostFtdcRspTransferField* pRspTransfer)
{
}

void Trade::OnRtnRepealFromBankToFutureByBank(CThostFtdcRspRepealField* pRspRepeal)
{
}

void Trade::OnRtnRepealFromFutureToBankByBank(CThostFtdcRspRepealField* pRspRepeal)
{
}

void Trade::OnRtnRepealFromBankToFutureByFutureManual(CThostFtdcRspRepealField* pRspRepeal)
{
}

void Trade::OnRtnRepealFromFutureToBankByFutureManual(CThostFtdcRspRepealField* pRspRepeal)
{
}

void Trade::OnErrRtnRepealBankToFutureByFutureManual(CThostFtdcReqRepealField* pReqRepeal, CThostFtdcRspInfoField* pRspInfo)
{
}

void Trade::OnErrRtnRepealFutureToBankByFutureManual(CThostFtdcReqRepealField* pReqRepeal, CThostFtdcRspInfoField* pRspInfo)
{
}

void Trade::OnRtnRepealFromBankToFutureByFuture(CThostFtdcRspRepealField* pRspRepeal)
{
}

void Trade::OnRtnRepealFromFutureToBankByFuture(CThostFtdcRspRepealField* pRspRepeal)
{
}

void Trade::OnRtnOpenAccountByBank(CThostFtdcOpenAccountField* pOpenAccount)
{
}

void Trade::OnRtnCancelAccountByBank(CThostFtdcCancelAccountField* pCancelAccount)
{
}

void Trade::OnRtnChangeAccountByBank(CThostFtdcChangeAccountField* pChangeAccount)
{
}
//*)

int	Trade::show_commands()
{
  if(true){
    cerr<<"-----------------------------------------------"<<endl;
    cerr<<" [1] ReqUserLogin              -- 登录"<<endl;
    cerr<<" [2] ReqSettlementInfoConfirm  -- 结算单确认"<<endl;
    cerr<<" [3] ReqQryInstrument          -- 查询合约"<<endl;
    cerr<<" [4] ReqQryTradingAccount      -- 查询资金"<<endl;
    cerr<<" [5] ReqQryInvestorPosition    -- 查询持仓"<<endl;
    cerr<<" [6] ReqOrderInsert            -- 报单"<<endl;
    cerr<<" [7] ReqOrderAction            -- 撤单"<<endl;
    cerr<<" [8] PrintOrders               -- 显示报单"<<endl;
    cerr<<" [9] PrintTraders              -- 显示成交"<<endl;
    cerr<<" [0] Exit                      -- 退出"<<endl;
    cerr<<"----------------------------------------------"<<endl;
  }
  return 0;
}

void Trade::ReqLogin( )
{
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, broker);
	strcpy(req.UserID, investor);
	strcpy(req.Password, password);

	pUserApi->ReqUserLogin(&req, ++iReqID);
}

void Trade::ReqConnect(char* f, const char* b, const char* u, char* p)
{
	pUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi(u);
	pUserApi->RegisterSpi((CThostFtdcTraderSpi*)this);
	pUserApi->SubscribePublicTopic(THOST_TERT_RESTART);
	pUserApi->SubscribePrivateTopic(THOST_TERT_RESTART);

	//this->addrID = frontID;
	//char* addr1 = new char[256];
	strcpy(broker, b);
	strcpy(investor, u);
	strcpy(password, p);

	pUserApi->RegisterFront(f);
	pUserApi->Init();
	fprintf(stderr, "@%s\n", __FUNCTION__);
}

bool Trade::IsErrorRspInfo(CThostFtdcRspInfoField* pRspInfo)
	{
		// 如果ErrorID != 0, 说明收到了错误的响应
		bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
		if (bResult)
		cerr<< "--->>> ErrorID="
			<< ", ErrorMsg=" << pRspInfo->ErrorMsg;
		return bResult;
	}
