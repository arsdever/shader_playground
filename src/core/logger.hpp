#pragma once

#include <spdlog/spdlog.h>

using logger_ptr = std::shared_ptr<spdlog::logger>;

void configure_logger(int argc, char** argv);

logger_ptr get_logger(std::string_view name);
