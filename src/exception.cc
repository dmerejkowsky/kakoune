#include <kakoune/exception.hh>

#include <kakoune/string.hh>

#include <typeinfo>

namespace Kakoune
{

StringView exception::what() const
{
    return typeid(*this).name();
}

}
