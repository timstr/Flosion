#include "..\include\NumberNode.hpp"
#include "..\include\NumberNode.hpp"
#include "..\include\NumberNode.hpp"
#include <NumberNode.hpp>

#include <algorithm>
#include <cassert>

namespace flo {

    NumberNode::NumberNode() noexcept
        : m_network(nullptr) {
    
    }

    bool NumberNode::canAddDependency(const NumberNode *) const noexcept {
        // TODO
        return true;
    }

    void NumberNode::addDependency(NumberNode* node) {
        assert(std::count(m_dependencies.begin(), m_dependencies.end(), node) == 0);
        assert(std::count(node->m_dependents.begin(), node->m_dependents.end(), this) == 0);
        if (!canAddDependency(node)){
            throw std::runtime_error("Don't do that.");
        }
        m_dependencies.push_back(node);
        node->m_dependents.push_back(this);
    }

    void NumberNode::removeDependency(NumberNode* node){
        assert(std::count(m_dependencies.begin(), m_dependencies.end(), node) == 1);
        assert(std::count(node->m_dependents.begin(), node->m_dependents.end(), this) == 1);

        m_dependencies.erase(
            std::remove(m_dependencies.begin(), m_dependencies.end(), node),
            m_dependencies.end()
        );
        node->m_dependents.erase(
            std::remove(node->m_dependents.begin(), node->m_dependents.end(), node),
            node->m_dependents.end()
        );
    }

    NumberInput::NumberInput() noexcept
        : m_source(&m_constant) {
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
    }

    void NumberInput::setSource(NumberSource* source) noexcept {
        assert(m_source);
        removeDependency(m_source);
        m_source = (source == nullptr ? &m_constant : source);
        addDependency(m_source);
    }

    SoundNumberInput::SoundNumberInput(SoundNode* owner) noexcept
        : m_owner(owner) {
        
    }

    bool SoundNumberInput::isStateless() const noexcept {
        return false;
    }

    NumberSource::NumberSource(EvaluationFunction evalFn) noexcept
        : m_evalFn(evalFn) {
    
    }

    double NumberSource::evaluate(const SoundState* context) const noexcept {
        return m_evalFn(this, context);
    }

    ConstantNumberSource::ConstantNumberSource() noexcept 
        : StatelessNumberSource(compute)
        , m_value(0.0) {

    }

    ConstantNumberSource::ConstantNumberSource(double value) noexcept 
        : StatelessNumberSource(compute)
        , m_value(value) {
    }

    double ConstantNumberSource::getValue() const noexcept {
        return m_value;
    }

    void ConstantNumberSource::setValue(double value) noexcept {
        m_value = value;
    }

    double ConstantNumberSource::compute(const NumberSource* self, const SoundState* context) noexcept {
        return reinterpret_cast<const ConstantNumberSource*>(self)->m_value;
    }

    bool StatelessNumberSource::isStateless() const noexcept {
        return true;
    }

    NumberSourceInput::NumberSourceInput(NumberSource* owner){
        owner->addDependency(this);
    }

    bool NumberSourceInput::isStateless() const noexcept {
        return true;
    }

} // namespace flo
