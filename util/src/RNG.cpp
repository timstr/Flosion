#include <Flosion/Util/RNG.hpp>

namespace util {

    std::default_random_engine& getRandomEngine(){
        static std::random_device dev{};
        static std::default_random_engine eng{dev()};
        return eng;
    }

} // namespace util
