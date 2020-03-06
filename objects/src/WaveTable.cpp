#include <Flosion/Objects/WaveTable.hpp>

namespace flo {

    WaveTable::WaveTable()
        : input(this)
        , m_array{} {

    }

    std::size_t WaveTable::length() const noexcept {
        return array_size;
    }

    double WaveTable::getValue(std::size_t idx) const noexcept {
        assert(idx < array_size);
        return m_array[idx].load(std::memory_order_relaxed);
    }

    void WaveTable::setValue(std::size_t idx, double v) noexcept {
        assert(idx < array_size);
        m_array[idx].store(v, std::memory_order_relaxed); // TODO: is this ok?
        onChange.broadcast(idx);
    }

    double WaveTable::evaluate(const SoundState* context) const noexcept {
        auto x = input.getValue(context);
        x -= std::floor(x); // wrap to [0, 1]
        x *= static_cast<double>(array_size);
        auto prev = std::floor(x);
        auto next = std::ceil(x);
        auto s0 = static_cast<std::size_t>(prev) % array_size;
        auto s1 = static_cast<std::size_t>(next) % array_size;
        if (s0 == s1) {
            return m_array[s0].load(std::memory_order_relaxed);
        }
        auto t = x - prev;
        auto v0 = m_array[s0].load(std::memory_order::memory_order_relaxed);
        auto v1 = m_array[s1].load(std::memory_order::memory_order_relaxed);
        return v0 + t * (v1 - v0);
    }

} // namespace flo
