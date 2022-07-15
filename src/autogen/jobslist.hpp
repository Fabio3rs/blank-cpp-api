#pragma once
#include "../jobs/HelloWorldHerokuJob.hpp"

namespace autogen {
static void registerJobs(job::JobsHandler &handler) {
    handler.register_job_handler<job::HelloWorldHerokuJob>();
}
}; // namespace autogen
