#include <iostream>
#include <string>
#include <vector>

#include <chaiscript/chaiscript.hpp>

#include "assert.hh"
#include "buffer_manager.hh"
#include "buffer_utils.hh"
#include "context.hh"
#include "coord.hh"
#include "display_buffer.hh"
#include "client.hh"
#include "face_registry.hh"
#include "keys.hh"
#include "option.hh"
#include "option_manager.hh"
#include "option_types.hh"
#include "scope.hh"
#include "string.hh"
#include "user_interface.hh"
#include "unit_tests.hh"
#include "window.hh"

#include "script.hh"

using namespace std::string_literals;
using namespace chaiscript;


namespace Kakoune
{

namespace Chai
{

// TODO: dedup this
Scope* get_scope_ifp(StringView scope, const Context& context)
{
    if (prefix_match("global", scope))
        return &GlobalScope::instance();
    else if (prefix_match("buffer", scope))
        return &context.buffer();
    else if (prefix_match("window", scope))
        return &context.window();
    else if (prefix_match(scope, "buffer="))
        return &BufferManager::instance().get_buffer(scope.substr(7_byte));
    return nullptr;
}

Scope& get_scope(StringView scope, const Context& context)
{
    if (auto s = get_scope_ifp(scope, context))
        return *s;
    throw runtime_error(format("no such scope: '{}'", scope));
}

static OptionManager& get_options(StringView scope, const Context& context, StringView option_name)
{
    if (scope == "current")
        return context.options()[option_name].manager();
    return get_scope(scope, context).options();
}

struct ChaiBuffer {
    public:
    explicit ChaiBuffer(Buffer& buffer):
      m_buffer(buffer)
      {}

    const std::string read()
    {
        BufferCoord start { 0, 0 };
        auto end = m_buffer.end_coord();
        String contents = m_buffer.string(start, end);
        return std::string{contents.data()};
    }

    void write(const std::string& contents)
    {
        BufferCoord start { 0, 0 };
        m_buffer.insert(start, String{contents.c_str()});
    }

    private:
        Buffer& m_buffer;
};


struct Kak {
  public:
  Kak(Context& context):
     m_context(context) {}

  void debug(const std::string& message_s) {
    String message = String{message_s.c_str()};
    write_to_debug_buffer(message);
  }

  void echo(const std::string& message_s) {
    String message = String{message_s.c_str()};
    const auto status_line_face = m_context.faces()["StatusLine"];
    const DisplayLine status_line { std::move(message), status_line_face };
    m_context.print_status(status_line);
  }

  void info(const std::string& message_s) {
    // TODO: dedup code from commands.cc (info_cmd)
  }

  chaiscript::Boxed_Value get_option(const std::string& type, const std::string& name) {
    // TODO: all types
    if (type == "bool") {
    bool res = m_context.options()[name.c_str()].get<bool>();
        return chaiscript::Boxed_Value(res);
    }
    if (type == "int") {
    int res = m_context.options()[name.c_str()].get<int>();
        return chaiscript::Boxed_Value(res);
    }
    if (type == "string") {
    String res = m_context.options()[name.c_str()].get<String>();
        return chaiscript::Boxed_Value(std::string{res.data()});
    }
    throw std::runtime_error("Unknown type: " + type);
  }


  void set_option(const std::string& scope_s, const std::string& name_s, const std::vector<chaiscript::Boxed_Value>& params) {

    auto scope = String{scope_s.c_str()};
    auto name = String{name_s.c_str()};
    Option& option = get_options(scope, m_context, name).get_local_option(name);

    Vector<String> kak_params;
    for (const  auto param: params) {
        auto val = param.get();
       	const auto string_val_ptr = val.cast<std::shared_ptr<std::string>>();
        kak_params.push_back(String{string_val_ptr.get()->c_str()});
    }
    option.set_from_strings(kak_params);
  }

  void set_option(const std::string& scope_s, const std::string& name_s, const chaiscript::Boxed_Value& bv) {
    auto scope = String{scope_s.c_str()};
    auto name = String{name_s.c_str()};
    Option& option = get_options(scope, m_context, name).get_local_option(name);

    Vector<String> params;
    std::string as_string;
    if (bv.is_type(chaiscript::user_type<std::string>())) {
       	as_string = chaiscript::boxed_cast<std::string>(bv);
    } else if (bv.is_type(chaiscript::user_type<bool>())) {
        bool bool_val = chaiscript::boxed_cast<bool>(bv);
        as_string = bool_val ? "true"s : "false"s;
    } else if (bv.is_type(chaiscript::user_type<int>())) {
        int int_val = chaiscript::boxed_cast<int>(bv);
        as_string = std::to_string(int_val);
    }

    params.push_back(String{as_string.c_str()});
    option.set_from_strings(params);
  }


  void send_keys(const std::vector<chaiscript::Boxed_Value> bvs) {
        KeyList keys;
        for (auto& bv : bvs)
        {
            auto value = chaiscript::boxed_cast<std::string>(bv);
            KeyList param_keys = parse_keys(String{value.c_str()});
            keys.insert(keys.end(), param_keys.begin(), param_keys.end());
        }

        for (auto& key : keys)
            m_context.input_handler().handle_key(key);
  }

  ChaiBuffer buffer() {
      Kakoune::Buffer& kak_buffer = m_context.buffer();
      return ChaiBuffer(kak_buffer);
  }

  private:
    Context& m_context;

};

} // namespace Chai

Script::Script(Context& context):
  m_chai(new chaiscript::ChaiScript()),
  m_context(context)
{

  m_chai->add(chaiscript::bootstrap::standard_library::vector_type<std::vector<std::string>>("VectorString"));

  Chai::Kak kak(m_context);

   m_chai->add(chaiscript::user_type<Chai::Kak>(), "Kak");
   m_chai->add(chaiscript::var(kak), "kak");
   m_chai->add(chaiscript::fun(&Chai::Kak::debug), "debug");
   m_chai->add(chaiscript::fun(&Chai::Kak::echo), "echo");

   m_chai->add(chaiscript::fun(&Chai::Kak::get_option), "get_option");

   m_chai->add(chaiscript::fun<void, Chai::Kak, const std::string&, const std::string&, const chaiscript::Boxed_Value& >(&Chai::Kak::set_option), "set_option");
   m_chai->add(chaiscript::fun<void, Chai::Kak, const std::string&, const std::string&, const std::vector<chaiscript::Boxed_Value>&>(&Chai::Kak::set_option), "set_option");

   m_chai->add(chaiscript::fun(&Chai::Kak::send_keys), "send_keys");

   m_chai->add(chaiscript::fun(&Chai::Kak::buffer), "buffer");
   m_chai->add(chaiscript::fun(&Chai::ChaiBuffer::read), "read");
   m_chai->add(chaiscript::fun(&Chai::ChaiBuffer::write), "write");

}



void Script::eval_file(String file_path) {
  const char* ptr = file_path.data();
  try {
    m_chai->eval_file(ptr, chaiscript::exception_specification<const std::string&>());
  } catch (const chaiscript::exception::eval_error &e) {  // error while parsing/running script
    throw runtime_error(e.what());
  } catch (const std::exception &e) {  // exception thrown in C++ code above
    throw runtime_error(e.what());
  }
  catch (const std::string &message) { // exception thrown from chaiscript
    throw runtime_error(message.c_str());
  }
}

void Script::eval_code(String code) {
  // FIXME: dedup this
  try {
    m_chai->eval(code.data(), chaiscript::exception_specification<const std::string&>());
  } catch (const chaiscript::exception::eval_error &e) {  // error while parsing/running script
    throw runtime_error(e.what());
  } catch (const std::exception &e) {  // exception throw in C++
    throw runtime_error(e.what());
  }
  catch (const std::string &message) { // exception throw from chaiscript
    throw runtime_error(message.c_str());
  }
}


} // namespace Kakoune
