#ifndef CCL_PANIC_HPP
#define CCL_PANIC_HPP

#include <string_view>
#include <array>

#include "source_location.hpp"

namespace ccl {

void panic(std::string_view msg, SourceLocation loc = SourceLocation::current());

}  // namespace ccl

#endif