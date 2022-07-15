#include "HelloWorldHerokuJob.hpp"
#include <iostream>

namespace job {
HelloWorldHerokuJob::HelloWorldHerokuJob() : QueueableJob() {}

HelloWorldHerokuJob::HelloWorldHerokuJob(
    const Poco::JSON::Object::Ptr &inputdata)
    : QueueableJob(), data(inputdata) {}

void HelloWorldHerokuJob::handle() {
    if (data) {
        data->stringify(std::cout, 5);
        std::cout << std::endl;
    } else {
        std::cout << "HelloWorldHerokuJob data is null" << std::endl;
    }
}
} // namespace job
