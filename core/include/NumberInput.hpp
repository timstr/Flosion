#pragma once

#include <NumberSource.hpp>

namespace flo {

    // class SoundState;

    // TODO: make NumberInput also store a Constant
    // so that it can be assigned a default value
    // and so that a nullptr source never needs to
    // be checked for during evaluation
    // (but getSource() should return nullptr when
    // source points to internal constant)

    /*
    class NumberInput {
        NumberInput() noexcept;
        
        double getValue(const SoundState* context) const noexcept;

        void setSource(NumberSource*) noexcept;
        
        NumberSource* getSource() noexcept;
        const NumberSource* getSource() const noexcept;

    private:
        NumberSource* m_source;
    };
    */

} // namespace flo
