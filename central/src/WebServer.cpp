//
// Created by p.szwed
//

#include <iostream>
#include <ctime>
// Added for the json-example
#define BOOST_SPIRIT_THREADSAFE
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

// Added for the default_resource example
#include <algorithm>
#include <boost/filesystem.hpp>
#include <fstream>
#include <vector>

#include "WebServer.h"
#include "Sensor.h"
#include "Communicator.h"
#include "Logger.h"

using namespace std;
using namespace boost::property_tree;

void WebServer::validateSession(std::shared_ptr<HttpServer::Request> request, SessionList* sessionList)
{
    std::stringstream cookieHeader;
    SimpleWeb::CaseInsensitiveMultimap::iterator header = request->header.find("Cookie");
    if (header == request->header.end())
        throw std::invalid_argument("No session cookie!");
    cookieHeader << header->second;
    std::string cookieHeaderString;
    cookieHeader >> cookieHeaderString;

    SimpleWeb::CaseInsensitiveMultimap cookie = SimpleWeb::HttpHeader::FieldValue::SemicolonSeparatedAttributes::parse(cookieHeaderString);
    const std::string sessionIdCookieString(cookie.find("sessionId")->second);

    bool exists = sessionList->exists_session(sessionIdCookieString);
    if (exists)
        common::Logger::log("session found - " + sessionIdCookieString);
}


WebServer::WebServer(SessionList* sessionList, Communicator* communicator, HttpServer* server)
{

    // HTTP-server at port 8080 using 1 thread
    server->config.port = 8080;

    // Default GET. If no other matches, this anonymous function will be called.
    // Will respond with content in the static/-directory, and its subdirectories.
    // Default file: index.html
    // Can for instance be used to retrieve an HTML 5 client that uses REST-resources on this server
    server->default_resource["GET"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
        try {
            auto web_root_path = boost::filesystem::canonical("../static");
            auto path = boost::filesystem::canonical(web_root_path / request->path);
            // Check if path is within web_root_path
            if(distance(web_root_path.begin(), web_root_path.end()) > distance(path.begin(), path.end()) ||
               !equal(web_root_path.begin(), web_root_path.end(), path.begin()))
                throw invalid_argument("path must be within root path");
            if(boost::filesystem::is_directory(path))
                path /= "index.html";

            SimpleWeb::CaseInsensitiveMultimap header;

            // Uncomment the following line to enable Cache-Control
            // header.emplace("Cache-Control", "max-age=86400");

            auto ifs = make_shared<ifstream>();
            ifs->open(path.string(), ifstream::in | ios::binary | ios::ate);

            if(*ifs) {
                auto length = ifs->tellg();
                ifs->seekg(0, ios::beg);

                header.emplace("Content-Length", to_string(length));
                response->write(header);

                // Trick to define a recursive function within this scope (for example purposes)
                class FileServer {
                public:
                    static void read_and_send(const shared_ptr<HttpServer::Response> &response, const shared_ptr<ifstream> &ifs) {
                        // Read and send 128 KB at a time
                        static vector<char> buffer(131072); // Safe when server is running on one thread
                        streamsize read_length;
                        if((read_length = ifs->read(&buffer[0], static_cast<streamsize>(buffer.size())).gcount()) > 0) {
                            response->write(&buffer[0], read_length);
                            if(read_length == static_cast<streamsize>(buffer.size())) {
                                response->send([response, ifs](const SimpleWeb::error_code &ec) {
                                    if(!ec)
                                        read_and_send(response, ifs);
                                    else
                                        cerr << "Connection interrupted" << endl;
                                });
                            }
                        }
                    }
                };
                FileServer::read_and_send(response, ifs);
            }
            else
                throw invalid_argument("could not read file");
        }
        catch(const exception &e) {
            response->write(SimpleWeb::StatusCode::client_error_bad_request, "Could not open path " + request->path + ": " + e.what());
        }
    };

    server->resource["^/RoSA/sensor$"]["GET"] = [communicator, sessionList](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {

        try {
            WebServer::validateSession(request, sessionList);
            std::string responseBody = "[";
            std::vector<std::pair<std::string, SensorList::SensorState>> sensors = communicator->get_sensor_list();
            for (unsigned i = 0; i < sensors.size(); i++)
            {
                responseBody += Sensor(sensors[i].first, sensors[i].second).toJSONString();
                if (i < sensors.size() - 1 ) responseBody += ",";
            }

            responseBody += "]";

            const std::string contentTypeHeader("Content-Type: application/json;\n");
            stringstream ss;
            ss << contentTypeHeader;

            response->write(SimpleWeb::StatusCode::success_ok, responseBody, SimpleWeb::HttpHeader::parse(ss));
            common::Logger::log("Return list of sensors");


        }
        catch (std::invalid_argument &e)
        {
            response->write(SimpleWeb::StatusCode::client_error_forbidden);
        }

    };


    server->resource["^/RoSA/sensor/refresh$"]["GET"] = [communicator, sessionList](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {

        try {
            stringstream stream;
            std::unordered_map<std::string, std::string> query;
            auto query_fields = request->parse_query_string();
            for(auto &field : query_fields)
                query.emplace(field.first, field.second);
            std::string address = query.at("address");
            communicator->ask_for_values(address);
            response->write(SimpleWeb::StatusCode::success_ok, "");
            common::Logger::log("Sensor refresh requested - " + address);

            response->write(SimpleWeb::StatusCode::success_accepted);
            common::Logger::log("Refresh status at " + address);


        }
        catch (std::logic_error &e)
        {
            response->write(SimpleWeb::StatusCode::client_error_bad_request, e.what());
        }

    };


    server->resource["^/RoSA/sensor/add$"]["POST"] = [communicator, sessionList](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {

        try {
            WebServer::validateSession(request, sessionList);
        } catch (std::exception &e) {

            response->write(SimpleWeb::StatusCode::client_error_forbidden, e.what());
        }

        try {
            stringstream stream;
            std::unordered_map<std::string, std::string> query;
            auto query_fields = request->parse_query_string();
            for(auto &field : query_fields)
                query.emplace(field.first, field.second);
            std::string address = query.at("address");
            float threshold = stof(query.at("threshold"));
            communicator->add_sensor(address, threshold);
            response->write(SimpleWeb::StatusCode::success_created, "");
            common::Logger::log("Sensor added - " + address + " with threshold: " + std::to_string(threshold));

        }
        catch (std::logic_error &e)
        {
            response->write(SimpleWeb::StatusCode::client_error_bad_request, e.what());
        }
    };

    server->resource["^/RoSA/sensor/modify$"]["POST"] = [communicator, sessionList](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {

        try {
            WebServer::validateSession(request, sessionList);
        } catch (std::exception &e) {

            response->write(SimpleWeb::StatusCode::client_error_forbidden, e.what());
        }

        try {
            stringstream stream;
            std::unordered_map<std::string, std::string> query;
            auto query_fields = request->parse_query_string();
            for(auto &field : query_fields)
                query.emplace(field.first, field.second);
            std::string address = query.at("address");
            float threshold = stof(query.at("threshold"));
            communicator->set_threshold(address, threshold);
            response->write(SimpleWeb::StatusCode::success_accepted);
            common::Logger::log("Sensor modified - " + address + " with threshold: " + std::to_string(threshold));

        }
        catch (std::logic_error &e)
        {
            response->write(SimpleWeb::StatusCode::client_error_bad_request, e.what());
        }
    };


    server->resource["^/RoSA/sensor$"]["DELETE"] = [communicator, sessionList](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {

        try {
            WebServer::validateSession(request, sessionList);
        } catch (std::exception &e) {

            response->write(SimpleWeb::StatusCode::client_error_forbidden, e.what());
        }

        try {
            stringstream stream;
            std::unordered_map<std::string, std::string> query;
            auto query_fields = request->parse_query_string();
            for(auto &field : query_fields)
                query.emplace(field.first, field.second);
            std::string address = query.at("address");
            communicator->erase_sensor(address);
            response->write(SimpleWeb::StatusCode::success_no_content);
            common::Logger::log("Sensor erased - " + address);

        }
        catch (std::logic_error &e)
        {
            response->write(SimpleWeb::StatusCode::client_error_bad_request, e.what());
        }
    };


    server->resource["^/RoSA/login$"]["POST"] = [sessionList](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {

        try {
            ptree pt;
            read_json(request->content, pt);

            std::string sessionId = sessionList->generate_session_id();

            sessionList->add_session(sessionId);

            const std::string sessionIdHeader("Set-Cookie: sessionId="+ sessionId +"; Max-Age=36000\n");
            const std::string contentTypeHeader("Content-Type: text/plain\n");
            stringstream ss;
            ss << sessionIdHeader << contentTypeHeader;

            response->write(SimpleWeb::StatusCode::success_ok, "", SimpleWeb::HttpHeader::parse(ss));
            common::Logger::log("User login - " + pt.get<string>("username") + "; session: " + sessionId);

        } catch (std::invalid_argument &e)
        {
            response->write(SimpleWeb::StatusCode::client_error_bad_request, e.what());
        }
    };

    server->resource["^/RoSA/logout$"]["POST"] = [sessionList](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {

        try {

            std::stringstream cookieHeader;
            cookieHeader << request->header.find("Cookie")->second;
            std::string cookieHeaderString;
            cookieHeader >> cookieHeaderString;

            SimpleWeb::CaseInsensitiveMultimap cookie = SimpleWeb::HttpHeader::FieldValue::SemicolonSeparatedAttributes::parse(cookieHeaderString);
            const std::string sessionIdCookieString(cookie.find("sessionId")->second);
            sessionList->erase_session(sessionIdCookieString);

            response->write(SimpleWeb::StatusCode::success_ok);

            common::Logger::log("Session logout - " + sessionIdCookieString);
        }
        catch (std::invalid_argument &e)
        {
            response->write(SimpleWeb::StatusCode::client_error_bad_request, e.what());
        }
    };

}