//
// Created by pszwed on 26.04.18.
//

#include "RestService.h"
#include "Sensor.h"

using namespace std;
using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;

const std::string RestService::base_uri = "/RoSA";

RestService::RestService(utility::string_t url, SensorList* sensorList) : m_listener(url)
{
    this->sensorList = sensorList;
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
    response.headers().add(U("Access-Control-Allow-Headers"), U("Content-Type, access-control-allow-headers, access-control-allow-origin, access-control-allow-methods"));
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

    if (path == RestService::base_uri + "/sensor/add")
    {
        add_sensor(request);
    }
    else if (path == RestService::base_uri + "/sensor/modify")
    {
        modify_sensor(request);
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
    http_response.headers().add(U("Access-Control-Allow-Origin"), U("*"));
    http_response.set_body(body);
    request.reply(http_response);
}

void RestService::get_sensor(http_request request) {
    std::vector<json::value> sensor_vector;
    std::vector<std::pair<std::string, SensorList::SensorState>> sensors = sensorList->get_sensors();
    for (unsigned i = 0; i < sensors.size(); i++)
    {
        sensor_vector.push_back(Sensor(sensors[i].first, sensors[i].second).toJSON());
    }
    respond(request, status_codes::OK, json::value::array(sensor_vector));
}



void RestService::add_sensor(http_request request) {

    auto http_request_params = uri::split_query(request.request_uri().query());

    auto found_address = http_request_params.find(U("address"));

    if (found_address == end(http_request_params)) {
        auto err = U("Request received with get var \"address\" omitted from query.");
        cout << err << endl;
        respond(request, status_codes::BadRequest, json::value::string(err));
        return;
    }

    auto found_threshold = http_request_params.find(U("threshold"));

    if (found_threshold == end(http_request_params)) {
        auto err = U("Request received with get var \"threshold\" omitted from query.");
        cout << err << endl;
        respond(request, status_codes::BadRequest, json::value::string(err));
        return;
    }

    auto sensor_addr = found_address->second;
    auto sensor_threshold = found_threshold->second;

    cout << U("Received request for sensor: ") << sensor_addr << " with threshold: " << sensor_threshold << endl;

    //insert new sensor
    try {
        sensorList->add_sensor(sensor_addr, stof(sensor_threshold));
        respond(request, status_codes::Created, json::value::string(
                U("Added new sensor, address: ") + sensor_addr + U(" with threshold: ") + sensor_threshold));
    } catch (std::exception e) {
        respond(request, status_codes::BadRequest, json::value::string(
                U("Failed to create sensor, address: ") + sensor_addr + U(" with threshold: ") + sensor_threshold ));

    }
}

void RestService::modify_sensor(http_request request) {

    auto http_request_params = uri::split_query(request.request_uri().query());

    auto found_address = http_request_params.find(U("address"));

    if (found_address == end(http_request_params)) {
        auto err = U("Request received with get var \"address\" omitted from query.");
        cout << err << endl;
        respond(request, status_codes::BadRequest, json::value::string(err));
        return;
    }

    auto found_threshold = http_request_params.find(U("threshold"));

    if (found_threshold == end(http_request_params)) {
        auto err = U("Request received with get var \"threshold\" omitted from query.");
        cout << err << endl;
        respond(request, status_codes::BadRequest, json::value::string(err));
        return;
    }

    auto sensor_addr = found_address->second;
    auto sensor_threshold = found_threshold->second;

    cout << U("Received request for sensor: ") << sensor_addr << " with threshold: " << sensor_threshold << endl;

    //modify sensor
    try {
        sensorList->set_threshold(sensor_addr, stof(sensor_threshold));
        respond(request, status_codes::Created, json::value::string(
                U("Modified sensor, address: ") + sensor_addr + U(" with threshold: ") + sensor_threshold));
    } catch (std::exception e) {
        respond(request, status_codes::BadRequest, json::value::string(
                U("Failed to modify sensor, address: ") + sensor_addr + U(" with threshold: ") + sensor_threshold ));

    }
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
    //insert new sensor
    try {
        sensorList->erase_sensor(sensor_id);
        respond(request, status_codes::Created, json::value::string(
                U("deleted sensor, address: ") + sensor_id));
    } catch (std::exception e) {
        respond(request, status_codes::BadRequest, json::value::string(
                U("Failed to delete sensor, address: ") + sensor_id ));

    }
}

void RestService::post_login(http_request request) {
    //TODO login operation
    respond(request, status_codes::OK, json::value::string(U("logged in")));

}

void RestService::post_logout(http_request request) {
    //TODO logout operation
    respond(request, status_codes::OK, json::value::string(U("logged out")));

}
