
/**
 *	$id$;
 */

#include "traderproxy.h"

/*
Trade*	traderproxy::create_trader(struct mg_connection* conn)
{
	Trade*		t = NULL;
	string &u = get_user_from_conn(conn);
	
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
*/

int	traderproxy::remove_trader(struct mg_connection* conn)
{
	int ret = 0;
	string &ukey = get_user_from_conn(conn);
	
	Trades_it it = m_traders.find(ukey);
	
	if(it != m_traders.end()) {
		m_traders.erase(ukey);

		ret = 1;
	}

	return ret;
}

void traderproxy::show_traders ()
{
	Trades_it	it;
	
	for(it = m_traders.begin(); it != m_traders.end(); it++ ) {
	}
}

Trade*	traderproxy::find_trader(struct mg_connection *conn)
{
	Trade	*t = NULL;
	string	&ukey = get_user_from_conn(conn);
	
	Trades_it it = m_traders.find(ukey);
	
	if(it != m_traders.end()) t = it->second;

	return t;
}	

int	traderproxy::get_post_var(struct mg_connection* conn, const char *key, char *value, size_t data_len) {
    char post_data[1024], user[sizeof(post_data)], password[sizeof(post_data)];
    int post_data_len;
    Trade* trader = NULL;
    char	*f = "tcp://27.17.62.149:40205", *b = "1035";
    char	*u = "00000072", *p = "123456";
    const struct mg_request_info *request_info = mg_get_request_info(conn);
    // User has submitted a form, show submitted data and a variable value
    post_data_len = mg_read(conn, post_data, sizeof(post_data));

    // Parse form data. input1 and input2 are guaranteed to be NUL-terminated
    int len = mg_get_var(post_data, post_data_len, key, value, data_len);
//    mg_get_var(post_data, post_data_len, "password", password, sizeof(password));
  
//	string ukey = u;
  	return len;
}

traderproxy::traderproxy()
{
}

traderproxy::~traderproxy()
{
	for(Trades_it	it = m_traders.begin(); it != m_traders.end(); ++it) {
		m_traders.erase(it->first);
			delete	it->second;
	}
}


Trade* traderproxy::create_trader(struct mg_connection* conn) {
    char post_data[1024], user[sizeof(post_data)], password[sizeof(post_data)];
    int post_data_len;
    Trade* trader = NULL;
    char	*f = "tcp://27.17.62.149:40205", *b = "1035";
    char	*u = "00000072", *p = "123456";
    const struct mg_request_info *request_info = mg_get_request_info(conn);
    // User has submitted a form, show submitted data and a variable value
    post_data_len = mg_read(conn, post_data, sizeof(post_data));

    // Parse form data. input1 and input2 are guaranteed to be NUL-terminated
    mg_get_var(post_data, post_data_len, "user", user, sizeof(user));
    mg_get_var(post_data, post_data_len, "password", password, sizeof(password));
  
    trader = new Trade();
	if (!trader) return NULL;
	
	trader->ReqConnect(f, b, u, p);
	
	m_traders.insert(pair<string, Trade*>(u, trader));
//	insert(pair<struct mg_connection*, Trades_t*>(conn, it));
	
	return trader;
  //  trader = tick_server.create_trader(f, b, u, p);
}
