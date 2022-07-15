#pragma once

#include "../stdafx.hpp"

namespace job {
class HelloWorldHerokuJob : public QueueableJob {
    Poco::JSON::Object::Ptr data;

  public:
    QUEUEABLE_SERIALIZE(data)

    auto getName() const -> std::string override {
        return getTypeNameByInst(*this);
    }

    void handle() override;

    HelloWorldHerokuJob();
    HelloWorldHerokuJob(const Poco::JSON::Object::Ptr &inputdata);
};
} // namespace job
