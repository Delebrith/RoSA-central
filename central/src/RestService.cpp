//
// Created by pszwed on 26.04.18.
//

#include "RestService.h"

using namespace std;
using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;

const std::string RestService::base_uri = "/RoSA";

RestService::RestService(utility::string_t url) : m_listener(url)
{
    m_listener.support(methods::GET, std::bind(&RestService::handle_get, this, std::placeholders::_1));
    m_listener.support(methods::POST, std::bind(&RestService::handle_post, this, std::placeholders::_1));
    m_listener.support(methods::DEL, std::bind(&RestService::handle_delete, this, std::placeholders::_1));
    m_listener.support(methods::OPTIONS, std::bind(&RestService::handle_options, this, std::placeholders::_1));
}

void RestService::handle_options(http_request request)
{
    http_response response(status_codes::OK);
    response.headers().add(U("Allow"), U("POST, GET, DELETE, OPTIONS"));

    response.headers().add(U("Access-Control-Allow-Origin"), U("*"));
    response.headers().add(U("Access-Control-Allow-Methods"), U("POST, GET, DELETE, OPTIONS"));
    response.headers().add(U("Access-Control-Allow-Headers"), U("Content-Type"));
    request.reply(response);
}

void RestService::handle_get(http_request request)
{
    ucout << request.to_string() << endl;

    if (request.request_uri().path() != RestService::base_uri + "/sensor")
    {
        request.reply(status_codes::NotFound);
    } else
    {
        get_sensor(request);
    }
}

void RestService::handle_post(http_request request)
{
    ucout << request.to_string() << endl;

    std::string path = request.request_uri().path();

    if (path == RestService::base_uri + "/sensor")
    {
        post_sensor(request);
    }
    else if (path == RestService::base_uri + "/login")
    {
        post_login(request);
    }
    else if (path == RestService::base_uri + "/logout")
    {
        post_logout(request);
    }
    else
    {
        request.reply(status_codes::NotFound);
    }
}

void RestService::handle_delete(http_request request)
{
    ucout << request.to_string() << endl;

    if (request.request_uri().path() != RestService::base_uri + "/sensor")
    {
        request.reply(status_codes::NotFound);
    } else
    {
      delete_sensor(request);
    }

}

void RestService::respond(const http_request& request, const status_code& status, const json::value& response) {
    json::value body;
    body[U("status")] = json::value::number(status);
    body[U("response")] = response;

    http_response http_response(status);
    http_response.headers().add(U("Allow"), U("POST, GET, DELETE, OPTIONS"));

    http_response.headers().add(U("Access-Control-Allow-Origin"), U("*"));
    http_response.headers().add(U("Access-Control-Allow-Methods"), U("POST, GET, DELETE, OPTIONS"));
    http_response.headers().add(U("Access-Control-Allow-Headers"), U("Content-Type"));
    http_response.set_body(body);
    request.reply(http_response);
}

void RestService::get_sensor(http_request request) {
    //TODO prepare vector of sensors
    respond(request, status_codes::OK, json::value::string(U("list of sensors")));
}

void RestService::post_sensor(http_request request) {

    auto http_request_params = uri::split_query(request.request_uri().query());

    auto found_value = http_request_params.find(U("address"));

    if (found_value == end(http_request_params)) {
        auto err = U("Request received with get var \"address\" omitted from query.");
        cout << err << endl;
        respond(request, status_codes::BadRequest, json::value::string(err));
        return;
    }

    auto sensor_addr = found_value->second;
    cout << U("Received request: ") << sensor_addr << endl;

    //TODO insert new sensor

    respond(request, status_codes::Created, json::value::string(U("Added new sensor, address: ") + sensor_addr));
}

void RestService::delete_sensor(http_request request) {

    auto http_request_params = uri::split_query(request.request_uri().query());

    auto found_value = http_request_params.find(U("address"));

    if (found_value == end(http_request_params)) {
        auto err = U("Request received with get var \"address\" omitted from query.");
        cout << err << endl;
        respond(request, status_codes::BadRequest, json::value::string(err));
        return;
    }

    auto sensor_id = found_value->second;
    cout << U("Received request: ") << sensor_id << endl;
    respond(request, status_codes::ResetContent, json::value::string(U("Deleted sensor, address: ") + sensor_id));
}

void RestService::post_login(http_request request) {
    //TODO login operation
    respond(request, status_codes::OK, json::value::string(U("logged in")));

}

void RestService::post_logout(http_request request) {
    //TODO logout operation
    respond(request, status_codes::OK, json::value::string(U("logged out")));

}
