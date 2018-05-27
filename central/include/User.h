//
// Created by p.szwed on 27.05.18.
//

#ifndef ROSA_CENTRAL_USER_H
#define ROSA_CENTRAL_USER_H

#include <cpprest/json.h>
#include <string>

struct User
{
    User(std::string username, std::string password)
    {
        this->username = username;
        this->password = password;
    }

    User(web::json::value json)
    {
        std::string extracted_username = (std::string) json.at("username").as_string();
        std::string extracted_password = (std::string) json.at("password").as_string();
        this->username = extracted_username;
        this->password = extracted_password;
    }

    std::string username;
    std::string password;

    web::json::value toJSON()
    {
        json::value json;
        json[U("username")] = json::value::string(this->username);
        return json;
    }
};

#endif //ROSA_CENTRAL_USER_H
