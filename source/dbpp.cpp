#include <string>

#include "dbpp/dbpp.hpp"

exported_class::exported_class()
    : m_name {"dbpp"}
{
}

auto exported_class::name() const -> const char*
{
  return m_name.c_str();
}
