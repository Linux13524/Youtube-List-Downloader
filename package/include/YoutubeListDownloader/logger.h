// Copyright (c) 2018 Linus Klöckner

#ifndef INCLUDE_LOGGER_H_
#define INCLUDE_LOGGER_H_

static const char* const LOG_TAG = "Youtube-List-Downloader";

#ifdef ANDROID

#include <android/log.h>
#include <iostream>
#include <sstream>

#define LogDebug() LogStream(ANDROID_LOG_DEBUG) << __FUNCTION__ << "():" << __LINE__ << " : "
#define LogInfo() LogStream(ANDROID_LOG_INFO) << __FUNCTION__ << "():" << __LINE__ << " : "
#define LogWarn() LogStream(ANDROID_LOG_WARN) << __FUNCTION__ << "():" << __LINE__ << " : "
#define LogError() LogStream(ANDROID_LOG_ERROR) << __FUNCTION__ << "():" << __LINE__ << " : "
#define LogException(EXCEPTION) LogStream(ANDROID_LOG_ERROR) << __FUNCTION__ << "():" << __LINE__ << " : "

class LogStream {
 private:
    std::stringstream m_ss;
    int m_logLevel;

 public:
    explicit LogStream(int logLevel) {
        m_logLevel = logLevel;
    }

    ~LogStream() {
        __android_log_print(m_logLevel, LOG_TAG, "%s", m_ss.str().c_str());
    }

    template <typename T>
    LogStream& operator<<(T const& val) {
        m_ss << val;
        return *this;
    }
};

#else

#include "boost/log/trivial.hpp"

#define LogDebug() BOOST_LOG_TRIVIAL(severity_level::debug)
#define LogInfo() BOOST_LOG_TRIVIAL(info)
#define LogWarn() BOOST_LOG_TRIVIAL(warning)
#define LogError() BOOST_LOG_TRIVIAL(error)
#define LogException(EXCEPTION) BOOST_LOG_TRIVIAL(error) << "Exception: " << (EXCEPTION).what()

#endif

#endif // INCLUDE_LOGGER_H_
