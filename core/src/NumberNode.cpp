#include <NumberNode.hpp>

#include <SoundNode.hpp>

// TODO: delete the '#include "..\include\NumberNode.hpp"' that Visual Studio likes to insert above >:(

#include <algorithm>
#include <cassert>
#include <functional>

namespace flo {

    NumberNode::NumberNode() noexcept
        : m_network(nullptr) {
    
    }

    bool NumberNode::canAddDependency(const NumberNode* node) const noexcept {
        if (hasDependency(node) || node->hasDependency(this)){
            return false;
        }

        // Make sure that all stateful dependents have access to all their stateful sources

        // find all stateful dependents of this
        std::vector<const SoundNode*> dependentOwners;
        std::function<void(const NumberNode*)> findDependents = [&](const NumberNode* nn){
            if (auto o = nn->getStateOwner()){
                dependentOwners.push_back(o);
            }
            for (const auto& d : nn->getDirectDependents()){
                findDependents(d);
            }
        };
        findDependents(this);

        // find all stateful dependencies of node
        std::vector<const SoundNode*> dependencyOwners;
        std::function<void(const NumberNode*)> findDependencies = [&](const NumberNode* nn){
            if (auto o = nn->getStateOwner()){
                dependencyOwners.push_back(o);
            }
            for (const auto& d : nn->getDirectDependents()){
                findDependencies(d);
            }
        };
        findDependencies(node);

        // make sure every stateful dependent's state owner is a depencency of every stateful dependency's state owner
        // That might sound confusing but trust me
        for (const auto& dependency : dependencyOwners){
            for (const auto& dependent : dependentOwners){
                if (!dependency->hasDependency(dependent)){
                    return false;
                }
            }
        }

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

    const std::vector<NumberNode*>& NumberNode::getDirectDependencies() const noexcept {
        return m_dependencies;
    }

    const std::vector<NumberNode*>& NumberNode::getDirectDependents() const noexcept {
        return m_dependents;
    }

    bool NumberNode::hasDependency(const NumberNode* node) const noexcept {
        if (this == node){
            return true;
        }
        for (const auto& d : m_dependencies){
            if (d->hasDependency(node)){
                return true;
            }
        }
        return false;
    }

    NumberInput::NumberInput() noexcept
        : m_source(&m_constant) {
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
        return m_source;
    }

    const NumberSource* NumberInput::getSource() const noexcept {
        return m_source;
    }

    SoundNumberInput::SoundNumberInput(SoundNode* owner) noexcept
        : m_owner(owner) {
        // NOTE: this is done here to prevent a call to a yet-unimplemented
        // virtual function (getStateOwner() in canAddDependency())
        addDependency(&m_constant);
    }

    const SoundNode* SoundNumberInput::getStateOwner() const noexcept {
        return m_owner;
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

    const SoundNode* StatelessNumberSource::getStateOwner() const noexcept {
        return nullptr;
    }

    NumberSourceInput::NumberSourceInput(NumberSource* owner){
        // NOTE: this is done here to prevent a call to a yet-unimplemented
        // virtual function (getStateOwner() in canAddDependency())
        addDependency(&m_constant);
        owner->addDependency(this);
    }

    const SoundNode* NumberSourceInput::getStateOwner() const noexcept {
        return nullptr;
    }

} // namespace flo
