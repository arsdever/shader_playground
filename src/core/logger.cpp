#include "logger.hpp"

#include <spdlog/cfg/argv.h>
#include <spdlog/cfg/env.h>
#include <spdlog/sinks/stdout_color_sinks.h>

void configure_logger(int argc, char** argv)
{
    spdlog::cfg::load_env_levels();
    spdlog::cfg::load_argv_levels(argc, argv);
}

logger_ptr get_logger(std::string_view name)
{
    static auto sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

    std::string name_str { name };
    auto logger = spdlog::get(name_str);
    if (logger)
        return logger;

    logger = std::make_shared<spdlog::logger>(name_str);

    logger->sinks().push_back(sink);
    spdlog::register_logger(logger);

    return logger;
}
