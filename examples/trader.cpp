
#include	<iostream>
#include	<unistd.h>
#include	"trader.h"
#include	"cJSON.h"
#include	"stdlib.h"

//����-�޼�
int Trade::ReqOrderInsert(const char* instrument, double price, int director, int offset, int volume)
{
	CThostFtdcInputOrderField f;
	memset(&f, 0, sizeof(f));
	f.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;	//1Ͷ��
	f.ContingentCondition = THOST_FTDC_CC_Immediately;//��������
	f.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	f.IsAutoSuspend = 0;
	f.OrderPriceType = THOST_FTDC_OPT_LimitPrice;		//***�����1  �޼�2***
	f.TimeCondition =  THOST_FTDC_TC_GFD;		//***�������1  ������Ч3***
	f.VolumeCondition = THOST_FTDC_VC_AV;	//��������1  ��С����2  ȫ���ɽ�3
	f.MinVolume = 1;

	strcpy(f.InvestorID, investor);
	strcpy(f.UserID, investor);
	strcpy(f.BrokerID, broker);

	strcpy(f.InstrumentID, instrument);	//��Լ

	f.LimitPrice = price;				//***�۸�***

	if(director == 0)
		f.Direction = THOST_FTDC_D_Buy;			//��
	else
		f.Direction = THOST_FTDC_D_Sell;			//��

	if(offset == 0)
		f.CombOffsetFlag[0] = THOST_FTDC_OF_Open;//����
	else if(offset == 1)
		f.CombOffsetFlag[0] = THOST_FTDC_OF_Close;	//ƽ��
	else
		f.CombOffsetFlag[0] = THOST_FTDC_OF_CloseToday;	//ƽ��

	f.VolumeTotalOriginal = volume;//����

	sprintf(f.OrderRef, "%d", ++orderRef);//OrderRef

	return pUserApi->ReqOrderInsert(&f, iReqID++);	//����
}
//����-�м�
int Trade::ReqOrderInsert(const char* instrument, int director, int offset, int volume)
{
	CThostFtdcInputOrderField f;
	memset(&f, 0, sizeof(f));
	f.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;	//1Ͷ��
	f.ContingentCondition = THOST_FTDC_CC_Immediately;//��������
	f.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	f.IsAutoSuspend = 0;
	f.OrderPriceType = THOST_FTDC_OPT_AnyPrice;		//***�����1  �޼�2***
	f.TimeCondition =  THOST_FTDC_TC_IOC;		//***�������1  ������Ч3***
	f.VolumeCondition = THOST_FTDC_VC_AV;	//��������1  ��С����2  ȫ���ɽ�3
	f.MinVolume = 1;

	strcpy(f.InvestorID, investor);
	strcpy(f.UserID, investor);
	strcpy(f.BrokerID, broker);

	strcpy(f.InstrumentID, instrument);	//��Լ

	f.LimitPrice = 0;					//***�۸�***

	if(director == 0)
		f.Direction = THOST_FTDC_D_Buy;			//��
	else
		f.Direction = THOST_FTDC_D_Sell;			//��

	if(offset == 0)
		f.CombOffsetFlag[0] = THOST_FTDC_OF_Open;//����
	else if(offset == 1)
		f.CombOffsetFlag[0] = THOST_FTDC_OF_Close;	//ƽ��
	else
		f.CombOffsetFlag[0] = THOST_FTDC_OF_CloseToday;	//ƽ��

	f.VolumeTotalOriginal = volume;//����

	sprintf(f.OrderRef, "%d", ++orderRef);//OrderRef

	return pUserApi->ReqOrderInsert(&f, iReqID++);	//����
}
//��������
void Trade::pushOrders(int ticks, int seconds, const char* instrument, double price, int director, int offset, int volume)
{
	int slp = 1000 / ticks;
	for(int i = 0; i < ticks * seconds; ++i)
	{
		ReqOrderInsert(instrument, price, director, offset, volume);
	}
}

//����
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

//��ֲ�
int Trade::ReqQryInvestorPosition()
{
	CThostFtdcQryInvestorPositionField f;
	memset(&f, 0, sizeof(f));
	strcpy(f.BrokerID, broker);
	strcpy(f.InvestorID, investor);

	return pUserApi->ReqQryInvestorPosition(&f, ++iReqID);
}

//��ֲ���ϸ
int Trade::ReqQryInvestorPositionDetail()
{
	CThostFtdcQryInvestorPositionDetailField f;
	memset(&f, 0, sizeof(f));
	strcpy(f.BrokerID, broker);
	strcpy(f.InvestorID, investor);
	return pUserApi->ReqQryInvestorPositionDetail(&f, ++iReqID);
}

//���ʽ�
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

//��ǩԼ����
int Trade::ReqQryAccountregister()
{
	CThostFtdcQryAccountregisterField f;
	memset(&f, 0, sizeof(f));
	strcpy(f.BrokerID, broker);
	strcpy(f.AccountID, investor);

	return  pUserApi->ReqQryAccountregister(&f, ++iReqID);
}

//�������ʻ�
int Trade::ReqQueryBankAccountMoneyByFuture(const char* bankID, const char* bankPWD, const char* accountPWD)
{
	CThostFtdcReqQueryAccountField f;
	memset(&f, 0, sizeof(f));
	//strcpy(f.TradeCode, "204002");

	strcpy(f.BankID, bankID);
	strcpy(f.BankBranchID, "0000");		//������
	strcpy(f.BankPassWord, bankPWD);

	strcpy(f.BrokerID, broker);
	strcpy(f.AccountID, investor);
	strcpy(f.Password, accountPWD);
	f.SecuPwdFlag = THOST_FTDC_BPWDF_BlankCheck;	//���ĺ˶�
	strcpy(f.CurrencyID, "RMB"); 					//���֣�RMB-����� USD-��Բ HKD-��Ԫ

	cout<<__FUNCTION__<<endl;
	return pUserApi->ReqQueryBankAccountMoneyByFuture(&f, ++iReqID);
}

//��ת����
int Trade::ReqTransferByFuture(const char* bankID, const char* bankPWD, const char* accountPWD, double amount, bool f2B)
{
	CThostFtdcReqTransferField f;
	memset(&f, 0, sizeof(f));

	strcpy(f.BankID, bankID);
	strcpy(f.BankBranchID, "0000");		//������
	strcpy(f.BankPassWord, bankPWD);

	strcpy(f.BrokerID, broker);
	strcpy(f.AccountID, investor);
	strcpy(f.Password, accountPWD);
	f.SecuPwdFlag = THOST_FTDC_BPWDF_BlankCheck;	//���ĺ˶�
	strcpy(f.CurrencyID, "RMB"); 					//���֣�RMB-����� USD-��Բ HKD-��Ԫ

	f.TradeAmount = amount;
	if(f2B)	//��ת��
		return pUserApi->ReqFromFutureToBankByFuture(&f, ++iReqID);
	else	//��ת��
		return pUserApi->ReqFromBankToFutureByFuture(&f, ++iReqID);
}

//����
void Trade::OnFrontConnected()
{
	ReqLogin();
	cerr<<"@"<<__FUNCTION__<<endl;
	status = 1;
//	state = state_after_connect;
}

//�Ͽ�
void Trade::OnFrontDisconnected(int nReason)
{
	cerr<<"��Ӧ|���׶Ͽ�!"<<nReason<<endl;
	status = 0;
}

//����
void Trade::OnHeartBeatWarning(int nTimeLapse)
{
}

//��֤
void Trade::OnRspAuthenticate(CThostFtdcRspAuthenticateField* pRspAuthenticateField, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
}

//��¼
void Trade::OnRspUserLogin(CThostFtdcRspUserLoginField* pRspUserLogin,
	CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	int ret;
	if(IsErrorRspInfo(pRspInfo))
	{
		cerr<<"��¼����" << pRspInfo->ErrorID<<":"<<pRspInfo->ErrorMsg<< endl;
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

//�����ȷ����Ϣ
void Trade::OnRspQrySettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField* pSettlementInfoConfirm,
	CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	int ret;
	if(this->IsErrorRspInfo(pRspInfo))
	{
		cerr<<"��ѯ����ȷ�ϴ���"<<endl;
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
//			cerr<<"���� | �������Ϣ..."<<endl;
			memset(&f, 0, sizeof(f));
			strcpy(f.BrokerID, broker);
			strcpy(f.InvestorID, investor);
			status = 3;
			ret = pUserApi->ReqQrySettlementInfo(&f, ++iReqID);
		}
	}
}

//�������Ϣ
void Trade::OnRspQrySettlementInfo(CThostFtdcSettlementInfoField* pSettlementInfo,
	CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	if(pSettlementInfo)
		this->settleInfo += pSettlementInfo->Content;
	if(bIsLast)
	{
		//settleInfo, 1);		//�û�ȷ��
		CThostFtdcSettlementInfoConfirmField f;

		memset(&f, 0, sizeof(f));
		strcpy(f.BrokerID, broker);
		strcpy(f.InvestorID, investor);
		pUserApi->ReqSettlementInfoConfirm(&f, ++iReqID);
	}
}

//ȷ�Ͻ���
void Trade::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField* pSettlementInfoConfirm,
	CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	status = 3;
//	main_handler(req, res);
}

//���Լ
void Trade::OnRspQryInstrument(CThostFtdcInstrumentField* pInstrument,
	CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	if(!IsErrorRspInfo(pRspInfo))
	{
		char* tmp = new char[strlen(pInstrument->InstrumentID) + 1];
		strcpy(tmp, pInstrument->InstrumentID);
		//       dicInstrument[string(tmp)] = *pInstrument;	//��Լ���Ʊ�ʶ
	}
	if(bIsLast)
	{
		if(this->addrID == -1)	//����
		{
			char* tmp = new char[256];
			strcpy(tmp, string(this->frontAddr).substr(0, strlen(this->frontAddr)-2).append("13").c_str());
			//           quote->ReqConnect(tmp, this->broker);
		}
		else
			;//            ;quote->ReqConnect(addrID);
	}
}

//��ֲ���Ӧ
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

//��ֲ���ϸ
void Trade::OnRspQryInvestorPositionDetail(CThostFtdcInvestorPositionDetailField* pInvestorPositionDetail,
	CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	if (bIsLast) {
		isdone = 1;
	}
}

//���ʽ�
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

//������Ӧ
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

//�ɽ���Ӧ
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
	//show(str(//boost::format("�ɽ����:%1%, ƽ̨���%2%, �ɽ�ʱ��%3%")%pTradeID %pBrokerOrderSeq %pTradeTime), 2);
}

//��������
void Trade::OnRspOrderInsert(CThostFtdcInputOrderField* pInputOrder, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	if (bIsLast) {
		isdone = 1;
	}
	//show(pRspInfo->ErrorMsg, 2);
}

//��������
void Trade::OnErrRtnOrderInsert(CThostFtdcInputOrderField* pInputOrder, CThostFtdcRspInfoField* pRspInfo)
{
	//show(pRspInfo->ErrorMsg, 2);
	isdone = 1;
}

//��������
void Trade::OnRspOrderAction(CThostFtdcInputOrderActionField* pInputOrderAction, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	//show(pRspInfo->ErrorMsg, 2);
	isdone = 1;
}

//ǩԼ����
void Trade::OnRspQryAccountregister(CThostFtdcAccountregisterField* pAccountregister,
	CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	if(IsErrorRspInfo(pRspInfo))
		;//show(str(//boost::format("��ǩԼ���д���:%1%") %pRspInfo->ErrorMsg));
	else if(pAccountregister)
	{
		string bank = "";
		if(pAccountregister->BankID[0] == THOST_FTDC_BF_ABC)
			bank = "ũҵ����";
		else if(pAccountregister->BankID[0] == THOST_FTDC_BF_BC)
			bank = "�й�����";
		else if(pAccountregister->BankID[0] == THOST_FTDC_BF_BOC)
			bank = "��ͨ����";
		else if(pAccountregister->BankID[0] == THOST_FTDC_BF_CBC)
			bank = "��������";
		else if(pAccountregister->BankID[0] == THOST_FTDC_BF_ICBC)
			bank = "��������";
		else if(pAccountregister->BankID[0] == THOST_FTDC_BF_Other)
			bank = "��������";

		string bankID = string(pAccountregister->BankAccount);
		bankID = bankID.substr(strlen(pAccountregister->BankAccount)-4, 4);
	}
	else
		;//show("��ǩԼ����!", 1);
}

//�������ʻ���Ӧ
void Trade::OnRtnQueryBankBalanceByFuture(CThostFtdcNotifyQueryAccountField* pNotifyQueryAccount)
{
	//show(str(//boost::format("�������:%1%, ��ȡ���:%2%")%pNotifyQueryAccount->BankUseAmount
	//             %pNotifyQueryAccount->BankFetchAmount), 1);
}

//�������ʻ�����
void Trade::OnRspQueryBankAccountMoneyByFuture(CThostFtdcReqQueryAccountField* pReqQueryAccount, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	if(IsErrorRspInfo(pRspInfo))
		cerr<<"��ѯ�����ʺŴ���:%1%"<<endl;
	else
		;//show(pReqQueryAccount->Digest, 1);
}

//�������ʻ�����
void Trade::OnErrRtnQueryBankBalanceByFuture(CThostFtdcReqQueryAccountField* pReqQueryAccount, CThostFtdcRspInfoField* pRspInfo)
{
	cerr<<"������������:%1%"<<endl;
}

//��->��
void Trade::OnRtnFromFutureToBankByFuture(CThostFtdcRspTransferField* pRspTransfer)
{
	if(pRspTransfer->ErrorID == 0)
		cerr<<"�ڻ�ת���гɹ�!"<<endl;
	else
		cerr<<"�ڻ�ת����ʧ��:%1%"<<endl;
}

//��->������
void Trade::OnErrRtnFutureToBankByFuture(CThostFtdcReqTransferField* pReqTransfer, CThostFtdcRspInfoField* pRspInfo)
{
	//show(str(//boost::format("�ڻ�ת����ʧ��:%1%") %pRspInfo->ErrorMsg), 1);
}

//��->������
void Trade::OnRspFromFutureToBankByFuture(CThostFtdcReqTransferField* pReqTransfer, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	//show(str(//boost::format("�ڻ�ת����ʧ��:%1%") %pRspInfo->ErrorMsg), 1);
}

//��->��
void Trade::OnRtnFromBankToFutureByFuture(CThostFtdcRspTransferField* pRspTransfer)
{
	if(pRspTransfer->ErrorID == 0)
		;//show("����ת�ڻ��ɹ�!", 1);
	else
		;//show(str(//boost::format("����ת�ڻ�ʧ��:%1%") %pRspTransfer->ErrorMsg), 1);
}

//��->�ڴ���
void Trade::OnErrRtnBankToFutureByFuture(CThostFtdcReqTransferField* pReqTransfer, CThostFtdcRspInfoField* pRspInfo)
{
	//show(str(//boost::format("����ת�ڻ�ʧ��:%1%") %pRspInfo->ErrorMsg), 1);
}

//��->�ڴ���
void Trade::OnRspFromBankToFutureByFuture(CThostFtdcReqTransferField* pReqTransfer, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	//show(str(//boost::format("����ת�ڻ�ʧ��:%1%") %pRspInfo->ErrorMsg), 1);
}

//(*������Ӧ�¼�
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
    cerr<<" [1] ReqUserLogin              -- ��¼"<<endl;
    cerr<<" [2] ReqSettlementInfoConfirm  -- ���㵥ȷ��"<<endl;
    cerr<<" [3] ReqQryInstrument          -- ��ѯ��Լ"<<endl;
    cerr<<" [4] ReqQryTradingAccount      -- ��ѯ�ʽ�"<<endl;
    cerr<<" [5] ReqQryInvestorPosition    -- ��ѯ�ֲ�"<<endl;
    cerr<<" [6] ReqOrderInsert            -- ����"<<endl;
    cerr<<" [7] ReqOrderAction            -- ����"<<endl;
    cerr<<" [8] PrintOrders               -- ��ʾ����"<<endl;
    cerr<<" [9] PrintTraders              -- ��ʾ�ɽ�"<<endl;
    cerr<<" [0] Exit                      -- �˳�"<<endl;
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
		// ���ErrorID != 0, ˵���յ��˴������Ӧ
		bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
		if (bResult)
		cerr<< "--->>> ErrorID="
			<< ", ErrorMsg=" << pRspInfo->ErrorMsg;
		return bResult;
	}
