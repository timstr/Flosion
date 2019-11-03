#include <Flosion/Core/NumberSource.hpp>

namespace flo {

    NumberSource::~NumberSource(){
        notifyReactors(&NumberSourceReactor::onDestroyNumberSource);
        while (m_inputs.size() > 0){
            assert(m_inputs.back());
            assert(m_inputs.back()->getSource() == this);
            m_inputs.back()->setSource(nullptr);
        }
    }

    NumberInput::NumberInput(double defaultValue) noexcept
        : m_constant(defaultValue)
        , m_source(&m_constant) {
        addDependency(&m_constant);
        m_constant.m_inputs.push_back(this);
    }

    NumberInput::~NumberInput(){
        notifyReactors(&NumberInputReactor::onDestroyNumberInput);
        if (m_source){
            setSource(nullptr);
        }
        assert(m_constant.m_inputs.size() == 1);
        assert(m_constant.m_inputs.front() == this);
        m_constant.m_inputs.clear();
        removeDependency(&m_constant);
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
        if (m_source != &m_constant){
            notifyReactors(&NumberInputReactor::beforeSourceRemoved, const_cast<const NumberSource*>(source));
        }
        {
            assert(std::count(m_source->m_inputs.begin(), m_source->m_inputs.end(), this) == 1);
            auto it = std::find(m_source->m_inputs.begin(), m_source->m_inputs.end(), this);
            assert(it != m_source->m_inputs.end());
            m_source->m_inputs.erase(it);
        }
        removeDependency(m_source);
        m_source = (source == nullptr ? &m_constant : source);
        addDependency(m_source);
        {
            assert(std::count(m_source->m_inputs.begin(), m_source->m_inputs.end(), this) == 0);
            m_source->m_inputs.push_back(this);
        }
        if (m_source != &m_constant){
            notifyReactors(&NumberInputReactor::afterSourceAdded, const_cast<const NumberSource*>(source));
        }
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
    }

    double Constant::evaluate(const SoundState* /* context */) const noexcept {
        return m_value.load(std::memory_order_relaxed);
    }

    void NumberInputReactor::onDefaultValueChanged(double value){

    }

    void NumberInputReactor::afterSourceAdded(const NumberSource*){

    }

    void NumberInputReactor::beforeSourceRemoved(const NumberSource*){

    }

    void NumberInputReactor::onDestroyNumberInput(){
    
    }

} // namespace flo
