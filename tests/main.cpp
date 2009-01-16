#include <tut/tut.hpp>
#include <tut/tut_reporter.hpp>

tut::test_runner_singleton runner;

int main()
{
  tut::reporter writer;
  runner.get().set_callback(&writer);
  runner.get().run_tests();

  return 0;
}
