#include <iostream>
#include <chaiscript/chaiscript.hpp>

#include "assert.hh"
#include "buffer_utils.hh"
#include "context.hh"
#include "coord.hh"
#include "display_buffer.hh"
#include "client.hh"
#include "face_registry.hh"
#include "string.hh"
#include "user_interface.hh"
#include "unit_tests.hh"

#include "script.hh"

using namespace std::string_literals;

namespace Kakoune
{

struct Kak {
  public:
  Kak(Context& context):
     m_context(context) {}

  void debug(const std::string& message) {
    const char* data = message.c_str();
    String string = String{data};
    write_to_debug_buffer(string);
  }

  void echo(const std::string& message) {
    const char* data = message.c_str();
    String kak_string = String{data};
    const auto status_line_face = this->m_context.faces()["StatusLine"];
    const DisplayLine status_line { std::move(kak_string), status_line_face };
    m_context.print_status(status_line);
  }

  void info(const std::string& message) {
  }

  private:
    Context& m_context;

};

Script::Script(Context& context):
  m_chai(new chaiscript::ChaiScript()),
  m_context(context)
{

  Kak kak(m_context);
  auto debug = [&kak](const std::string& message) {
    kak.debug(message);
  };

   m_chai->add(chaiscript::var(kak), "kak");
   m_chai->add(chaiscript::fun(&Kak::debug), "debug");
   m_chai->add(chaiscript::fun(&Kak::echo), "echo");
}



String Script::eval(String code) {
  const char* ptr = code.data();
  try {
    m_chai->eval(ptr);
  } catch (const chaiscript::exception::eval_error &e) {
    return String(e.what());
  }
  return String("");
}


String Script::eval_file(String file_path) {
  const char* ptr = file_path.data();
  try {
    m_chai->eval_file(ptr);
  } catch (const chaiscript::exception::eval_error &e) {
    return String(e.what());
  }
  return String("");
}

/*
UnitTest test_script{[]()
{
  String error;
  Script script;
  error =  script.eval(R"(
    import("kakoune")
    puts(kakoune.version)
  )"
  );
  kak_assert(error.empty());

  error = script.eval(R"(
    puts(no-such-obj)
  )"
  );
  kak_assert(!error.empty());
}};
*/

}
