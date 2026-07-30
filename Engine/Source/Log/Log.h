#pragma once
#include <string>
#include <string_view>
#include <memory>
#include <source_location>
#include <concepts>
#include <format>
#include "Core/Utility.h"

namespace VulkanEngine
{
enum class LogVerbosity : uint8_t
{
    NoLogging = 0,
    Display,
    Warning,
    Error,
    Log,
    Critical,
};

struct LogCategory
{
    explicit LogCategory(const std::string& name) : m_name(name) {}
    std::string name() const { return m_name; };

private:
    const std::string m_name{};
};

class Log final : public NonCopyable
{
public:
    static Log& getInstance()
    {
        static Log instance;
        return instance;
    }

    void log(const LogCategory& category,  //
        LogVerbosity verbosity,            //
        const std::string& message,        //
        bool showLocation = false,         //
        const std::source_location location = std::source_location::current()) const;

private:
    Log();
    ~Log();

    class Impl;
    std::unique_ptr<Impl> m_pImpl;
};

constexpr LogVerbosity c_minVerbosity = LogVerbosity::Display;
constexpr LogVerbosity c_maxVerbosity = LogVerbosity::Critical;

// concepts
template <typename T>
concept ValidLogCategory = std::constructible_from<LogCategory, T>;

template <typename T>
concept LoggableMessage = std::convertible_to<T, std::string> || std::convertible_to<T, std::string_view>;

template <LogVerbosity V>
concept ValidVerbosityLevel = V == LogVerbosity::NoLogging ||  //
                              V == LogVerbosity::Display ||    //
                              V == LogVerbosity::Warning ||    //
                              V == LogVerbosity::Error ||      //
                              V == LogVerbosity::Log ||        //
                              V == LogVerbosity::Critical;

}  // namespace VulkanEngine

#define DEFINE_LOG_CATEGORY_STATIC(logName)            \
    namespace                                          \
    {                                                  \
    const VulkanEngine::LogCategory logName(#logName); \
    }

#define VE_LOG_IMPL(categoryName, verbosity, showLocation, formatStr, ...)                                                             \
    do                                                                                                                                 \
    {                                                                                                                                  \
        if constexpr (VulkanEngine::LogVerbosity::verbosity >= VulkanEngine::c_minVerbosity &&                                         \
                      VulkanEngine::LogVerbosity::verbosity <= VulkanEngine::c_maxVerbosity)                                           \
        {                                                                                                                              \
            static_assert(VulkanEngine::ValidVerbosityLevel<VulkanEngine::LogVerbosity::verbosity>,                                    \
                "Verbosity should be one of Display, Warning, Error, Log, Critical");                                                  \
            static_assert(VulkanEngine::ValidLogCategory<decltype(categoryName)>, "Category must be of a type LogCategory");           \
            static_assert(VulkanEngine::LoggableMessage<decltype(formatStr)>, "Log message should be convertable to string");          \
            VulkanEngine::Log::getInstance().log(                                                                                      \
                categoryName, VulkanEngine::LogVerbosity::verbosity, std::format(formatStr __VA_OPT__(, ) __VA_ARGS__), showLocation); \
        }                                                                                                                              \
    } while (0)

#define VE_LOG(categoryName, verbosity, formatStr, ...) VE_LOG_IMPL(categoryName, verbosity, false, formatStr, __VA_ARGS__)

#define VE_LOG_DEBUG(categoryName, verbosity, formatStr, ...) VE_LOG_IMPL(categoryName, verbosity, true, formatStr, __VA_ARGS__)