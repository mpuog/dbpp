
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

  try {
    example();
  } catch (const dbpp::Error& err) {
    std::cout << "DBPP_EXCEPTION:" << err.what() << "\n";
  } catch (const std::exception& err) {
    std::cout << "STD_EXCEPTION:" << err.what() << "\n";
  }
  std::cout << ">>>>>>>>>>>>>>\n";

    return 0;
}
