#ifndef TOOLS_H
#define TOOLS_H

// clang-format off
#include "spdlog/spdlog.h"
// clang-format on
#include "spdlog/sinks/sink.h"
#include "spdlog/sinks/stdout_sinks.h"
//#include <spdlog>

extern std::shared_ptr<spdlog::logger> H_Logger;

class Tools
{
 public:
  Tools();
  ~Tools() {}
};

#endif // TOOLS_H
