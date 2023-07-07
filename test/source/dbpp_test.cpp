#include <string>

#include "dbpp/dbpp.hpp"

auto main() -> int
{
  auto const exported = exported_class {};

  return std::string("dbpp") == exported.name() ? 0 : 1;
}
