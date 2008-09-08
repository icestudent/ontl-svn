#include <nt/process_information.hxx>
#pragma warning(disable:4101 4189)

namespace process_info_test{
  void test1()
  {
    using namespace ntl::nt;
    // ok
    process_information<process_session_information> read_session;
    process_information<process_session_information> write_session(current_process(), 0);

    // query fail
    //process_information<process_base_priority_information> read_priority;
    process_information<process_base_priority_information> write_priority(current_process(), 0);

    // write fail
    process_basic_information bi;
    process_information<process_basic_information> read_info;
    //process_information<process_basic_information> write_info(current_process(), bi);
  }
}