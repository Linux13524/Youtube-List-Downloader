// Copyright (c) 2018 Linus Klöckner

#ifndef INCLUDE_UTILS_FS_H_
#define INCLUDE_UTILS_FS_H_

#include <boost/regex.hpp>
#include <string>

namespace Filesystem {
    class Utils {
     public:
        Utils() = delete;

        static inline void ClearForbiddenChars(std::string* p_string) {
            boost::regex expr(R"([\/\\:*?"<>|])");

            // TODO: use similar chars
            *p_string = boost::regex_replace(*p_string, expr, "");
        }
    };
} // namespace Filesystem

#endif // INCLUDE_UTILS_FS_H_
