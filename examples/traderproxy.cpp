
/**
 *	$id$;
 */

#include	"traderproxy.h"
#include	"cJSON.h"
#include	"slre.h"
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
	char	ukey[32];
    int u_len = get_post_var(conn, "user", ukey, sizeof(ukey));
	
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
    char post_data[1024], user[sizeof(post_data)], password[sizeof(post_data)];
    int post_data_len;
    char	*f = NULL, *b = NULL, *u = NULL, *p = NULL;
//    char	*f = "tcp://27.17.62.149:40205", *b = "1035";
//    char	*u = "00000072", *p = "123456";
    cJSON *root = NULL;
    const struct mg_request_info *request_info = mg_get_request_info(conn);
    // User has submitted a form, show submitted data and a variable value
    const char* ct = mg_get_header(conn, "Content-Type");
    const char* user_data = (const char*)request_info->user_data;
    post_data_len = mg_read(conn, post_data, sizeof(post_data));

    if (!slre_match((slre_option)0, ct, "Application/json", strlen(ct))) {
    	root = cJSON_Parse(post_data);
	if(root) {
		f = cJSON_GetObjectItem(root, "host")->valuestring;
		b = cJSON_GetObjectItem(root, "broker")->valuestring;
		u = cJSON_GetObjectItem(root, "user")->valuestring;
		p = cJSON_GetObjectItem(root, "password")->valuestring;
	}
    }
    cerr<<u<<"@1"<<post_data<<endl; 
    if (!u) {	
	Trades_it it = m_traders.find(u);
	
	if(it != m_traders.end()) t = it->second;
    }

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
	char	*f = NULL, *b = NULL, *u = NULL, *p = NULL;
//    char	*f = "tcp://27.17.62.149:40205", *b = "1035";
//    char	*u = "00000072", *p = "123456";
    cJSON *root = NULL;
    const struct mg_request_info *request_info = mg_get_request_info(conn);
    // User has submitted a form, show submitted data and a variable value
    const char* ct = mg_get_header(conn, "Content-Type");
    post_data_len = mg_read(conn, post_data, sizeof(post_data));
  
    if (!slre_match((slre_option)0, ct, "Application/json", strlen(ct))) {
    	root = cJSON_Parse(post_data);
		f = cJSON_GetObjectItem(root, "host")->valuestring;
		b = cJSON_GetObjectItem(root, "broker")->valuestring;
		u = cJSON_GetObjectItem(root, "user")->valuestring;
		p = cJSON_GetObjectItem(root, "password")->valuestring;
    }
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
