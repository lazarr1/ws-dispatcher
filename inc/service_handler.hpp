#pragma once

#include <iostream>
#include <optional>
#include <string>

class IServiceHandler {
    public:
        virtual void processMsg(const std::string msg) = 0;
        virtual std::optional<std::string> getResponse() = 0;
        virtual bool keepAlive() const = 0;

};


class ExampleServiceHandler: public IServiceHandler {
public:    
    int exampleState = 0;
    bool keep_alive = true;

    void processMsg(const std::string msg) {
        exampleState++;
        std::cout << "processing: " << msg << std::endl;
    }

    std::optional<std::string> getResponse() {
        if (exampleState >=  3){
            keep_alive = false;
            return "Finished processing";
        } else {
            keep_alive = true;
            return "";
        }
    }

    bool keepAlive() const {
        return keep_alive;
    }
};