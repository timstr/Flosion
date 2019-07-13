#pragma once

namespace flo {

    class Immovable {
    public:
        Immovable() noexcept = default;
        ~Immovable() noexcept = default;

        Immovable(const Immovable&) = delete;
        Immovable(Immovable&&) = delete;
        Immovable& operator=(const Immovable&) = delete;
        Immovable& operator=(Immovable&&) = delete;
    };

} // namespace flo
