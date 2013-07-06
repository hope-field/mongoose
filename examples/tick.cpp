<<<<<<< HEAD
/**
 *	$id$;
 */

#include "tick.h"

Trade*	Tick::create_trader(char* f, char* b, char* u, char* p )
{
	Trade*		t = NULL;
	Trades_it	it = m_traders.find(u);
	
	if(m_traders.end() == it) {
		t = new Trade();
		if (NULL != t)
			t->ReqConnect(f, b, u, p);
		m_traders.insert(pair<string, Trade*>(u, t));
	} else {
		t = it->second;
	}
	return t;
}

int	Tick::remove_trader(string ukey)
{
	int ret = 0;
	Trades_it it = m_traders.find(ukey);
	if(it != m_traders.end()) {
		m_traders.erase(ukey);

		ret = 1;
	}

	return ret;
}

void Tick::show_traders ()
{
	for(Trades_it it = m_traders.begin(); it != m_traders.end(); it++ ) {
	}
}

Trade*	Tick::find_trader(string ukey)
{
	Trade	*t = NULL;
	Trades_it it = m_traders.find(ukey);
	
	if(it != m_traders.end()) t = it->second;
/*	for(Trades_it	it = m_traders.begin(); it != m_traders.end(); ++it) {
		if(!strcmp(id, it->first)) {
			t = it->second;
			break;
		}
	}
*/
	return t;
}	

Tick::Tick()
{
}

Tick::~Tick()
{
	for(Trades_it	it = m_traders.begin(); it != m_traders.end(); ++it) {
		Trade*	t = it->second;
		m_traders.erase(it->first);
		delete	t;
	}
}
