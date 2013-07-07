// Copyright (c) 2004-2009 Sergey Lyubka
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// Unit test for the mongoose web server. Tests embedded API.


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifndef _WIN32
#include <unistd.h>
#endif

#include "mongoose.h"
#include "tick.h"

#if !defined(LISTENING_PORT)
#define LISTENING_PORT "24"
#endif

static Tick tick_server;

static const char *standard_reply = "HTTP/1.1 200 OK\r\n"
  "Content-Type: text/plain\r\n"
  "Connection: close\r\n\r\n";

static void test_get_var(struct mg_connection *conn) {
  char *var, *buf;
  size_t buf_len;
  const char *cl;
  int var_len;

  mg_printf(conn, "%s", standard_reply);
  struct mg_request_info *ri = mg_get_request_info(conn);

  buf_len = 0;
  var = buf = NULL;
  cl = mg_get_header(conn, "Content-Length");
  mg_printf(conn, "cl: %p\n", cl);
  if ((!strcmp(ri->request_method, "POST") ||
       !strcmp(ri->request_method, "PUT"))
      && cl != NULL) {
    buf_len = atoi(cl);
    buf = (char*)malloc(buf_len);
    /* Read in two pieces, to test continuation */
    if (buf_len > 2) {
      mg_read(conn, buf, 2);
      mg_read(conn, buf + 2, buf_len - 2);
    } else {
      mg_read(conn, buf, buf_len);
    }
  } else if (ri->query_string != NULL) {
    buf_len = strlen(ri->query_string);
    buf = (char*)malloc(buf_len + 1);
    strcpy(buf, ri->query_string);
  }
  var = (char*)malloc(buf_len + 1);
  var_len = mg_get_var(buf, buf_len, "my_var", var, buf_len + 1);
  mg_printf(conn, "Value: [%s]\n", var);
  mg_printf(conn, "Value size: [%d]\n", var_len);
  free(buf);
  free(var);
}

static void test_get_header(struct mg_connection *conn) {
  const char *value;
  int i;

  struct mg_request_info *ri = mg_get_request_info(conn);
  mg_printf(conn, "%s", standard_reply);
  printf("HTTP headers: %d\n", ri->num_headers);
  for (i = 0; i < ri->num_headers; i++) {
    printf("[%s]: [%s]\n", ri->http_headers[i].name, ri->http_headers[i].value);
  }

  value = mg_get_header(conn, "Host");
  if (value != NULL) {
    mg_printf(conn, "Value: [%s]", value);
  }
}

static void test_get_request_info(struct mg_connection *conn) {
  int i;

  struct mg_request_info *ri = mg_get_request_info(conn);
  mg_printf(conn, "%s", standard_reply);

  mg_printf(conn, "Method: [%s]\n", ri->request_method);
  mg_printf(conn, "URI: [%s]\n", ri->uri);
  mg_printf(conn, "HTTP version: [%s]\n", ri->http_version);

  for (i = 0; i < ri->num_headers; i++) {
    mg_printf(conn, "HTTP header [%s]: [%s]\n",
              ri->http_headers[i].name,
              ri->http_headers[i].value);
  }

  mg_printf(conn, "Query string: [%s]\n",
            ri->query_string ? ri->query_string: "");
  mg_printf(conn, "Remote IP: [%lu]\n", ri->remote_ip);
  mg_printf(conn, "Remote port: [%d]\n", ri->remote_port);
  mg_printf(conn, "Remote user: [%s]\n",
            ri->remote_user ? ri->remote_user : "");
}

static void test_error(struct mg_connection *conn) {
  struct mg_request_info *ri = mg_get_request_info(conn);
  int status = (int) ri->remote_port;
  mg_printf(conn, "HTTP/1.1 %d XX\r\n"
            "Conntection: close\r\n\r\n", status);
  mg_printf(conn, "Error: [%d]", status);
}

static void test_post(struct mg_connection *conn ) {
  const char *cl;
  char *buf;
  int len;

  struct mg_request_info *ri = mg_get_request_info(conn);
  mg_printf(conn, "%s", standard_reply);
  if (strcmp(ri->request_method, "POST") == 0 &&
      (cl = mg_get_header(conn, "Content-Length")) != NULL) {
    len = atoi(cl);
    if ((buf = (char*)malloc(len)) != NULL) {
      mg_write(conn, buf, len);
      free(buf);
    }
  }
}

static void get_account_info(struct mg_connection *conn, Trade* t) {
	t->ReqQryTradingAccount();
	cerr<<"@"<<__FUNCTION__<<endl;
}

static void get_position_info(struct mg_connection *conn, Trade *t) {
	t->ReqQryInvestorPosition();
}

static void post_order_insert(struct mg_connection *conn, Trade *t) {
	char instrument[16], _price[16], _director[16], _offset[16], _volume[16];
	double price; int director; int offset; int volume;
	char post_data[1024];int post_data_len;
	const struct mg_request_info *ri = mg_get_request_info(conn);
	
	post_data_len = mg_read(conn, post_data, sizeof(post_data));
	
	mg_get_var(post_data, post_data_len, "instrument", instrument, sizeof(instrument));
	mg_get_var(post_data, post_data_len, "price", _price, sizeof(_price));
	mg_get_var(post_data, post_data_len, "director", _director, sizeof(_director));
	mg_get_var(post_data, post_data_len, "offset", _offset, sizeof(_offset));
	mg_get_var(post_data, post_data_len, "volume", _volume, sizeof(_volume));
	
	price = atof(_price);
	director = atoi(_director);
	offset = atoi(_offset);
	volume = atoi(_volume);
	
	t->ReqOrderInsert(instrument, price, director, offset, volume);
}

static void del_order_action(struct mg_connection *conn, Trade* t) {
	char instrument[16], _session[16], _frontid[16], orderref[16];
	int session; int frontid;
	char post_data[1024];int post_data_len;
	const struct mg_request_info *ri = mg_get_request_info(conn);
	
	post_data_len = mg_read(conn, post_data, sizeof(post_data));
	
	mg_get_var(post_data, post_data_len, "instrument", instrument, sizeof(instrument));
	mg_get_var(post_data, post_data_len, "session", _session, sizeof(_session));
	mg_get_var(post_data, post_data_len, "frontid", _frontid, sizeof(_frontid));
	mg_get_var(post_data, post_data_len, "orderref", orderref, sizeof(orderref));
	
	session = atoi(_session);
	frontid = atoi(_frontid);
	
	t->ReqOrderAction(instrument, session, frontid, orderref);
}

static const struct ticker_config {
  const char *method;
  const char *uri;
  void (*func)(struct mg_connection * , Trade*);
} ticker_config[] = {
  {"GET", "/get_account", &get_account_info},
  {"GET", "/get_position", &get_position_info},
  {"POST", "/order_insert", &post_order_insert},
  {"DELETE", "/del_order_action", &del_order_action},
  {NULL, NULL, NULL}
};

static int ticker_request_handler(struct mg_connection *conn) {
  char post_data[1024], user[sizeof(post_data)], password[sizeof(post_data)];
  int post_data_len;
  int i;
  Trade* trader = NULL;
  char	*f = "tcp://27.17.62.149:40205", *b = "1035";
  char	*u = "00000072", *p = "123456";
  const struct mg_request_info *request_info = mg_get_request_info(conn);
  // User has submitted a form, show submitted data and a variable value
  post_data_len = mg_read(conn, post_data, sizeof(post_data));

  // Parse form data. input1 and input2 are guaranteed to be NUL-terminated
  mg_get_var(post_data, post_data_len, "user", user, sizeof(user));
  mg_get_var(post_data, post_data_len, "password", password, sizeof(password));
  
  trader = tick_server.create_trader(f, b, u, p);
  
  if(!trader) return 0;
  
  trader->isdone = 0;
  memset(trader->buffer, 0, sizeof(trader->buffer));
  
  while(trader->status < 2) {}
 
  {
	  for (i = 0; ticker_config[i].uri != NULL; i++) {
	    if (!strcmp(request_info->request_method, ticker_config[i].method) &&
	         !strcmp(request_info->uri, ticker_config[i].uri)) {
	      ticker_config[i].func(conn, trader);
	      break;
	    }
	  }
	 cerr<<"@1"<<endl; 
	  while (!trader->isdone){}
	 cerr<<"@2"<<endl; 
	  
	  mg_write(conn, trader->buffer, strlen(trader->buffer));
  }

  return 1;
}

static void ticker_wb_ready_handler(struct mg_connection *conn) {
  unsigned char buf[40];
  buf[0] = 0x81;
  buf[1] = snprintf((char *) buf + 2, sizeof(buf) - 2, "%s", "server ready");
  mg_write(conn, buf, 2 + buf[1]);
}

// Arguments:
//   flags: first byte of websocket frame, see websocket RFC,
//          http://tools.ietf.org/html/rfc6455, section 5.2
//   data, data_len: payload data. Mask, if any, is already applied.
static int ticker_wb_data_handler(struct mg_connection *conn, int flags,
                                  char *data, size_t data_len) {
  unsigned char reply[200];
  size_t i;

  (void) flags;

  printf("rcv: [%.*s]\n", (int) data_len, data);

  // Truncate echoed message, to simplify output code.
  if (data_len > 125) {
    data_len = 125;
  }

  // Prepare frame
  reply[0] = 0x81;  // text, FIN set
  reply[1] = data_len;

  // Copy message from request to reply, applying the mask if required.
  for (i = 0; i < data_len; i++) {
    reply[i + 2] = data[i];
  }

  // Echo the message back to the client
  mg_write(conn, reply, 2 + data_len);

  // Returning zero means stoping websocket conversation.
  // Close the conversation if client has sent us "exit" string.
  return memcmp(reply + 2, "exit", 4);
}

int main(void) {
  struct mg_context *ctx;
  struct mg_callbacks callbacks;

  const char *options[] = {
	  "listening_ports", LISTENING_PORT, NULL};

  memset(&callbacks, 0, sizeof(callbacks));
  callbacks.begin_request = ticker_request_handler;
  callbacks.websocket_ready = ticker_wb_ready_handler;
  callbacks.websocket_data = ticker_wb_data_handler;
  
  ctx = mg_start(&callbacks, NULL, options);
  pause();
  return 0;
}
