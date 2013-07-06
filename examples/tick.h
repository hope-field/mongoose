#ifndef NGX_TICK_HANDLER_H
#define NGX_TICK_HANDLER_H

#include <iostream>
#include <map>
#include <hash_map>
//#include <hash_set>

#include "trade.h"

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

class Tick {
private:
	Trades_t	m_traders;

public:
	Tick();
	~Tick();

	Trade*	create_trader(char*, char*, char*, char*);
	int	remove_trader(string);
	void	show_traders();
	Trade*	find_trader(string);
public:
};


#endif // NGX_TICK_HANDLER_H
