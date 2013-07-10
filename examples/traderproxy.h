#ifndef NGX_TICK_HANDLER_H
#define NGX_TICK_HANDLER_H

#include <iostream>
#include <map>
#include <hash_map>
//#include <hash_set>

#include "mongoose.h"
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

class traderproxy  : public map<struct mg_connection*, Trade*>{
private:
	Trades_t	m_traders;
public:
	Tick();
	~Tick();
	
	string	get_user_from_conn(struct mg_connection*);
	Trade*	create_trader(struct mg_connection*);
	int		remove_trader(struct mg_connection*);
	void	show_traders();
	Trade*	find_trader(struct mg_connection*);
};

#endif // NGX_TICK_HANDLER_H
