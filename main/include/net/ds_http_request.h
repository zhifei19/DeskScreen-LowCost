#ifndef _H_DS_HTTP_REQUEST_
#define _H_DS_HTTP_REQUEST_

typedef enum{
    HTTP_GET_TIME = 0,
    HTTP_GET_WEATHER,
}HTTP_REQUEST_TYPE_E;

void http_client_init(void);
void ds_http_request_type(HTTP_REQUEST_TYPE_E type);

#endif // !_H_DS_HTTP_REQUEST_
