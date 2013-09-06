
/**
 *	$id$;
 */
#include	<stdlib.h>

#include	"traderproxy.h"
//#include	"ticker.h"

static	traderproxy	g_ticker;

Trade*	traderproxy::create_trader(const char* f, const char *b, const char *u, const char* p)
{
	Trade*		t = NULL;

	if(!t) {
		t = new Trade();
		if (NULL != t)
			t->ReqConnect(f, b, u, p);
		insert(pair<string, Trade*>(u, t));
	}
	return t;
}

int	traderproxy::remove_trader(const char* u)
{
	return 1;
	//return earse(u);
}

Trade*	traderproxy::find_trader(const char *u)
{
	Trade*		t = NULL;
	traderproxy::iterator	iter;

	iter = find(u);
	if(end() != iter)
		t = iter->second;
    return t;
//    return find(u);
}	

traderproxy::traderproxy()
{
}

traderproxy::~traderproxy()
{
/*	for(Trades_it	it = m_traders.begin(); it != m_traders.end(); ++it) {
		m_traders.erase(it->first);
			delete	it->second;
	}
*/
}

const char*
traderproxy::get_account_info(const char* f, const char* b, const char* u, const char *p)
{
	Trade*	t = find_trader(u);

	if (!t) {
		t = create_trader(f, b, u, p);
		if(!t) return NULL;
	}

	t->isdone = 0;
	memset(t->buffer, 0, sizeof(t->buffer));

	while(t->status < 2) {}

	int ret = t->ReqQryTradingAccount();

	while (!t->isdone) {}

	return t->buffer;
}

const char*
traderproxy::get_position_info(const char* f, const char* b, const char* u, const char *p)
{
	Trade*	t = find_trader(u);

	if (!t) {
		t = create_trader(f, b, u, p);
		if(!t) return NULL;
	}

	t->isdone = 0;
	memset(t->buffer, 0, sizeof(t->buffer));

	while(t->status < 2) {}

	int ret = t->ReqQryInvestorPosition();
	fprintf(stderr, "@%s\n", __FUNCTION__);

	while (!t->isdone) {}

	return t->buffer;
}

const char*
traderproxy::order_insert(const char* instrument, const char* price, const char* director, const char* offset,
	const char* volume, const char* f, const char* b, const char* u, const char *p)
{
	Trade*	t = find_trader(u);

	if (!t) {
		t = create_trader(f, b, u, p);
		if(!t) return NULL;
	}

	t->isdone = 0;
	memset(t->buffer, 0, sizeof(t->buffer));

	while(t->status < 2) {}

	int ret = t->ReqOrderInsert(instrument, atof(price), atoi(director), atoi(offset), atoi(volume));

	while (!t->isdone) {}

	return t->buffer;
}

const char*
traderproxy::order_delete(const char *instrument, const char *session, const char * frontid,
	const char *orderref, const char* f, const char* b, const char* u, const char *p)
{
	Trade*	t = find_trader(u);

	if (!t) {
		t = create_trader(f, b, u, p);
		if(!t) return NULL;
	}

	t->isdone = 0;
	memset(t->buffer, 0, sizeof(t->buffer));

	while(t->status < 2) {}

	t->ReqOrderAction(instrument, atoi(session), atoi(frontid), orderref);

	while (!t->isdone) {}

	return t->buffer;
}

const char*
traderproxy::show_orders(const char *f, const char* b, const char *u,const char* p)
{
	Trade*	t = find_trader(u);

	if (!t) {
		t = create_trader(f, b, u, p);
		if(!t) return NULL;
	}

	t->isdone = 0;
	memset(t->buffer, 0, sizeof(t->buffer));

	while(t->status < 2) {}

	int ret = t->ShowOrders();
	fprintf(stderr, "@%s\n", __FUNCTION__);

	return t->buffer;
}

const char*
traderproxy::show_trades(const char *f, const char* b, const char *u,const char* p)
{
	Trade*	t = find_trader(u);

	if (!t) {
		t = create_trader(f, b, u, p);
		if(!t) return NULL;
	}

	t->isdone = 0;
	memset(t->buffer, 0, sizeof(t->buffer));

	while(t->status < 2) {}

	int ret = t->ShowTrades();
	fprintf(stderr, "@%s\n", __FUNCTION__);

	return t->buffer;
}

const char*
show_orders(const char *f, const char *b, const char *u, const char* p)
{
	return g_ticker.show_orders(f, b, u, p);
}

const char*
show_trades(const char *f, const char *b, const char *u, const char* p)
{
	return g_ticker.show_trades(f, b, u, p);
}

const char*
get_account_info(const char *f, const char *b, const char *u, const char* p)
{
	return g_ticker.get_account_info(f, b, u, p);
}

const char*
get_position_info(const char *f, const char *b, const char *u, const char *p)
{
	return g_ticker.get_position_info(f, b, u, p);
}

const char *
order_insert(const char *instrument, const char *price, const char* director, const char * offset
	,const char *volume, const char *f, const char *b, const char *u, const char *p)
{
	return g_ticker.order_insert(instrument, price, director, offset, volume, f, b, u, p);
}

const char *
order_delete(const char *instrument, const char *session, const char* frontid, const char *orderref 
	, const char *f, const char *b, const char *u, const char *p)
{
	return g_ticker.order_delete(instrument, session, frontid, orderref, f, b, u, p);
}
