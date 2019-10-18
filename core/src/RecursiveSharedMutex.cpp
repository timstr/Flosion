#include <Flosion/Core/RecursiveSharedMutex.hpp>

namespace flo {

    RecursiveSharedMutex::RecursiveSharedMutex() noexcept
        : m_count(0) {
        
    }

} // namespace flo

