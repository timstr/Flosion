#include <Flosion/Core/NumberSource.hpp>

namespace flo {

    NumberInput::NumberInput(double defaultValue) noexcept
        : m_constant(defaultValue)
        , m_source(&m_constant) {
        addDependency(&m_constant);
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
        notifyReactors(&NumberInputReactor::onDefaultValueChanged, value);
    }

    void NumberInput::setSource(NumberSource* source) noexcept {
        assert(m_source);
        notifyReactors(&NumberInputReactor::beforeSourceRemoved, const_cast<const NumberSource*>(source));
        removeDependency(m_source);
        m_source = (source == nullptr ? &m_constant : source);
        addDependency(m_source);
        notifyReactors(&NumberInputReactor::afterSourceAdded, const_cast<const NumberSource*>(source));
    }

    NumberSource* NumberInput::getSource() noexcept {
        return m_source == &m_constant ? nullptr : m_source;
    }

    const NumberSource* NumberInput::getSource() const noexcept {
        return m_source;
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
        notifyReactors(&ConstantReactor::onValueChanged, value);
    }

    double Constant::evaluate(const SoundState* /* context */) const noexcept {
        return m_value.load(std::memory_order_relaxed);
    }

    void ConstantReactor::onValueChanged(double){

    }

    void NumberInputReactor::onDefaultValueChanged(double value){

    }

    void NumberInputReactor::afterSourceAdded(const NumberSource*){

    }

    void NumberInputReactor::beforeSourceRemoved(const NumberSource*){

    }

} // namespace flo
