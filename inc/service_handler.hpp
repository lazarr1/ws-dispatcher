#pragma once

#include <iostream>
#include <vector>

enum class SessionAction {
    Continue,
    Close,
    Error
};

struct ServiceResult {
    std::vector<std::string> outgoing_msgs;
    SessionAction action = SessionAction::Continue;
};


class IServiceHandler {
public:
    virtual ServiceResult onMessage(std::string msg) = 0;
};

class ExampleServiceHandler : public IServiceHandler {
public:
    int exampleState = 0;

    ServiceResult onMessage(std::string msg) override {
        ServiceResult result;

        exampleState++;
        std::cout << "processing: " << msg << std::endl;

        if (exampleState >= 3) {
            exampleState = 0;
            result.outgoing_msgs.push_back("Finished processing");
            result.action = SessionAction::Close;
        }

        return result;
    }
};
