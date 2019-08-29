#include <NumberSource.hpp>

namespace flo {

    NumberInput::NumberInput(double defaultValue) noexcept
        : m_constant(defaultValue)
        , m_source(&m_constant) {
        // NOTE: there would be a call to addDependency(&m_constant)
        // but that would call a yet-unimplemented virtual function,
        // specifically getStateOwner() in canAddDependency().
        // For this reason, the dependency is added in the derived
        // SoundNumberInput and NumberSourceInput.
    }

    double NumberInput::getValue(const SoundState* context) const noexcept {
        assert(m_source);
        return m_source->evaluate(context);
    }

    double NumberInput::getDefaultValue() const noexcept {
        return m_constant.getValue();
    }

    void NumberInput::setDefaultValue(double value) noexcept {
        m_constant.setValue(value);
    }

    void NumberInput::setSource(NumberSource* source) noexcept {
        assert(m_source);
        removeDependency(m_source);
        m_source = (source == nullptr ? &m_constant : source);
        addDependency(m_source);
    }

    NumberSource* NumberInput::getSource() noexcept {
        return m_source == &m_constant ? nullptr : m_source;
    }

    const NumberSource* NumberInput::getSource() const noexcept {
        return m_source;
    }

    NumberSourceInput::NumberSourceInput(NumberSource* owner, double defaultValue)
        : NumberInput(defaultValue) {
        // NOTE: this is done here to prevent a call to a yet-unimplemented
        // virtual function (getStateOwner() in canAddDependency())
        addDependency(&m_constant);
        owner->addDependency(this);
    }

    ConstantNumberSource::ConstantNumberSource(double value) noexcept 
        : m_value(value) {
    }

    double ConstantNumberSource::getValue() const noexcept {
        return m_value.load(std::memory_order_relaxed);
    }

    void ConstantNumberSource::setValue(double value) noexcept {
        m_value.store(value, std::memory_order_relaxed);
    }

    double ConstantNumberSource::evaluate(const SoundState* /* context */) const noexcept {
        return m_value.load(std::memory_order_relaxed);
    }

} // namespace flo
