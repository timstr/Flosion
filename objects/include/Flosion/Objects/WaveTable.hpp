#pragma once

#include <Flosion/Core/NumberSource.hpp>

#include <array>
#include <atomic>

namespace flo {

    class WaveTable : public NumberSource {
    public:
        WaveTable();

        NumberSourceInput input;

        std::size_t length() const noexcept;

        double getValue(std::size_t idx) const noexcept;
        void setValue(std::size_t idx, double v) noexcept;

        flo::Signal<std::size_t> onChange;

    private:
        double evaluate(const SoundState*) const noexcept override;

        static constexpr std::size_t array_size = 256;
        std::array<std::atomic<double>, array_size> m_array;
    };

} // namespace flo
