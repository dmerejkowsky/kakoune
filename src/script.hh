#ifndef script_hh_INCLUDED
#define script_hh_INCLUDED

#include "context.hh"
#include "face_registry.hh"
#include "safe_ptr.hh"

namespace chaiscript {
  struct ChaiScript;
}

namespace Kakoune
{

namespace Chai {
  struct Kak;
}


struct Script {
  public:
    explicit Script(Context& context);
    void eval_file(String file_path, ConstArrayView<String> args);
    void eval_code(String code);
  private:
    Context& m_context;
    chaiscript::ChaiScript* m_chai;
    Chai::Kak* m_kak;
};

}

#endif // src/script_hh_INCLUDED
