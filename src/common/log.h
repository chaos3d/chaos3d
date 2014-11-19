#ifndef _CHAOS3D_COMMON_LOG_H
#define _CHAOS3D_COMMON_LOG_H

#include <typeinfo>
#include <log4cxx/logger.h>
#include <log4cxx/helpers/objectptr.h>

/// customizable logger by explicitly specializing the template
/// the default logger is "common"
/// How-to:
///     1. in header: IMPORT_LOGGER(class_name),
///             this may not be needed if linker can resolve
///     2. in source: DEFINE_LOGGER(class_name, logger_name)
///     3. LOG_XXX or LOG_XXX(void, ...)
template <typename C>
log4cxx::Logger* get_logger() {
    static log4cxx::LoggerPtr _logger = log4cxx::Logger::getLogger("common");
    return _logger;
}

/// helper trait to re-use the logger
///     INHERIT_LOGGER(clz, parent)
/// TODO: default to void to avoid template explosion
template <typename T> struct logger_trait { typedef T logger_t; };

#define IMPORT_LOGGER(clz) \
    extern template log4cxx::Logger* get_logger<clz>();

#define DEFINE_LOGGER(clz, logger_name) \
    template<> log4cxx::Logger* get_logger<clz>() { \
        static log4cxx::LoggerPtr _logger = log4cxx::Logger::getLogger(logger_name); \
        return _logger; \
    }

// To simplify, only use one level of inheritence
#define INHERIT_LOGGER(clz, parent) \
    template <> struct logger_trait<clz> { typedef parent logger_t; };

#define LOGGER_TYPE(clz) \
    typename logger_trait< \
        typename std::remove_cv< \
            typename std::remove_reference<clz>::type \
        >::type \
    >::logger_t

#define LOG_DEBUG1(msg)         LOG_DEBUG2(decltype(*this), msg)
#define LOG_TRACE1(msg)         LOG_TRACE2(decltype(*this), msg)
#define LOG_INFO1(msg)          LOG_INFO2(decltype(*this), msg)
#define LOG_WARN1(msg)          LOG_WARN2(decltype(*this), msg)
#define LOG_ERROR1(msg)         LOG_ERROR2(decltype(*this), msg)
#define LOG_FATAL1(msg)         LOG_FATAL2(decltype(*this), msg)

#define LOG_DEBUG2(clz, msg)    LOG4CXX_DEBUG(get_logger<LOGGER_TYPE(clz)>(), msg)
#define LOG_TRACE2(clz, msg)    LOG4CXX_TRACE(get_logger<LOGGER_TYPE(clz)>(), msg)
#define LOG_INFO2(clz, msg)     LOG4CXX_INFO(get_logger<LOGGER_TYPE(clz)>(), msg)
#define LOG_WARN2(clz, msg)     LOG4CXX_WARN(get_logger<LOGGER_TYPE(clz)>(), msg)
#define LOG_ERROR2(clz, msg)    LOG4CXX_ERROR(get_logger<LOGGER_TYPE(clz)>(), msg)
#define LOG_FATAL2(clz, msg)    LOG4CXX_FATAL(get_logger<LOGGER_TYPE(clz)>(), msg)

#define GET_LOG_DEF(_1,_2,NAME,...) NAME
#define LOG_DEBUG(...)          GET_LOG_DEF(__VA_ARGS__, LOG_DEBUG2, LOG_DEBUG1) (__VA_ARGS__)
#define LOG_TRACE(...)          GET_LOG_DEF(__VA_ARGS__, LOG_TRACE2, LOG_TRACE1) (__VA_ARGS__)
#define LOG_INFO(...)           GET_LOG_DEF(__VA_ARGS__, LOG_INFO2, LOG_INFO1) (__VA_ARGS__)
#define LOG_WARN(...)           GET_LOG_DEF(__VA_ARGS__, LOG_WARN2, LOG_WARN1) (__VA_ARGS__)
#define LOG_ERROR(...)          GET_LOG_DEF(__VA_ARGS__, LOG_EROR2, LOG_ERROR1) (__VA_ARGS__)
#define LOG_FATAL(...)          GET_LOG_DEF(__VA_ARGS__, LOG_FATAL2, LOG_FATAL1) (__VA_ARGS__)

#endif