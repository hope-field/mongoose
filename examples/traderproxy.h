#ifndef TRADERPROXY_H
#define TRADERPROXY_H

//#include <iostream>
#include <map>
//#include <hash_map>
//#include <hash_set>

#include "trader.h"

using namespace std;

class StringComparerForMap
{
public:
    bool operator()(const std::string x, const std::string y)
    {
    // Add compare logic here
		return x.compare(y);
    }
};

typedef		map<string, Trade*, StringComparerForMap>				Trades_t;
typedef		map<string, Trade*, StringComparerForMap>::iterator		Trades_it;

class traderproxy  : public map<string, Trade*>{
private:
	Trades_t	m_traders;
public:
	traderproxy();
	~traderproxy();
	
	Trade*	create_trader(const char*, const char*, const char*, const char*);
	int		remove_trader(const char*);
	Trade*	find_trader(const char*);
public:
	const char* get_account_info(const char*, const char*, const char*, const char*);
	const char* get_position_info(const char*, const char*, const char*, const char*);
	const char*	show_orders(const char *f, const char* b, const char *u,const char* p);
	const char*	show_trades(const char *f, const char* b, const char *u,const char* p);
const char*
order_insert(const char* instrument, const char* price,const char *director, const char *offset, const char *volume
	, const char* f, const char *b, const char *u, const char *p);
const char*
order_delete(const char *instrument, const char *session, const char *frontid, const char* orderref
	, const char *f, const char *b, const char *u, const char *p);
};

#endif // NGX_TICK_HANDLER_H
