#include "Utilities.hpp"
#include "Precomp.hpp"

namespace Renderer
{

bool StrEqual(const char* left, const char* right)
{
    return std::strcmp(left, right) == 0;
}

} // namespace Renderer
