#include <Flosion/Core/NumberSource.hpp>

namespace flo {

    int NumberInput::acquireLock(){
        return 0;
    }

    NumberInput::NumberInput(double defaultValue) noexcept
        : m_defaultValue(defaultValue) {

    }

    double NumberInput::getValue(const SoundState* context) const noexcept {
        if (auto s = getSource()){
            return s->evaluate(context);
        }
        return m_defaultValue;
    }

    double NumberInput::getDefaultValue() const noexcept {
        return m_defaultValue;
    }

    void NumberInput::setDefaultValue(double value) noexcept {
        m_defaultValue = value;
        onDefaultValueChanged.broadcast(value);
    }

    NumberSourceInput::NumberSourceInput(NumberSource* owner, double defaultValue)
        : NumberInput(defaultValue) {
        owner->addDependency(this);
    }

    Constant::Constant(double value) noexcept 
        : m_value(value) {
    }

    double Constant::getValue() const noexcept {
        return m_value.load(std::memory_order_relaxed);
    }

    void Constant::setValue(double value) noexcept {
        m_value.store(value, std::memory_order_relaxed);
        onChangeValue.broadcast(value);
    }

    double Constant::evaluate(const SoundState* /* context */) const noexcept {
        return m_value.load(std::memory_order_relaxed);
    }

} // namespace flo
