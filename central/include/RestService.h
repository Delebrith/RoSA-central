//
// Created by pszwed on 26.04.18.
//

#ifndef REST_RESTSERVER_H
#define REST_RESTSERVER_H

#include <cpprest/json.h>
#include <cpprest/http_listener.h>
#include <cpprest/uri.h>
#include <cpprest/asyncrt_utils.h>

using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;

class RestService {

public:
    static const std::string base_uri;
    RestService();
    RestService(utility::string_t url);
    pplx::task<void> open() {return m_listener.open(); };
    pplx::task<void> close() {return m_listener.close(); }

private:
    void handle_options(http_request request);
    void handle_get(http_request request);
    void handle_post(http_request request);
    void handle_delete(http_request request);
    void respond(const http_request& request, const status_code& status, const json::value& response);
    void get_sensor(http_request request);
    void add_sensor(http_request request);
    void modify_sensor(http_request request);
    void delete_sensor(http_request request);
    void post_login(http_request request);
    void post_logout(http_request request);
    http_listener m_listener;

};


#endif //REST_RESTSERVER_H
