

#ifdef _WIN32
#  include <Windows.h>
#endif  // WIN32

#include <iostream>

int main()
{
#ifdef _WIN32
  SetConsoleOutputCP(65001);
  SetConsoleCP(65001);
#endif  // WIN32
  printf(">>> ");
  std::cout << __DATE__ " " __TIME__ "  "
#ifdef _MSC_VER
            << "MS:" << _MSC_VER
#else
            << "GCC:" << __GNUC__
#endif

#ifdef _WIN32
            << " WINDOWS "
#else
            << " LINUX "
#endif

#ifdef _DEBUG
            << " DEBUG "
#else
            << " RELEASE(MAY BE) "
#endif

            << "\n";

  return 0;
}
