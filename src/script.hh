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

struct Script {
  public:
    explicit Script(Context& context);
    String eval_file(String file_path);
  private:
    Context& m_context;
    chaiscript::ChaiScript* m_chai;
};

}

#endif // src/script_hh_INCLUDED
