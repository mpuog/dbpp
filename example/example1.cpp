#include "head.cpp"

using namespace dbpp;

void example()
{
  std::cerr << "\ndb::sqlite\n";
  auto connection = Connection::connect(db::sqlite, ":memory:");
}

#include "tail.cpp"
