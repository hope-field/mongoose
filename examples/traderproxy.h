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
	traderproxy();
	~traderproxy();
	
	int	get_post_var(struct mg_connection*, const char*, char*, size_t);
	Trade*	create_trader(struct mg_connection*);
	int		remove_trader(struct mg_connection*);
	void	show_traders();
	Trade*	find_trader(struct mg_connection*);
};

#endif // NGX_TICK_HANDLER_H
