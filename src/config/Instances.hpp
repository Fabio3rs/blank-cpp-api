#pragma once
#include "../stdafx.hpp"

class Instances {
  private:
    /* data */
  public:
    std::shared_ptr<GenericQueue> queueSystem;
    std::shared_ptr<job::QueueWorker> queueWorker;
    std::string queue_name;

    template <class T>
    auto dispatch_job(const T &job,
                      std::chrono::system_clock::time_point scheduledAt = {}) {
        return queueWorker->push(queue_name, job, scheduledAt);
    }

    static auto singleton() -> Instances &;

    auto operator=(const Instances &) -> Instances & = default;
    auto operator=(Instances &&) -> Instances & = default;
    Instances(const Instances &) = default;
    Instances(Instances &&) = default;

    Instances(/* args */);
    ~Instances();
};
