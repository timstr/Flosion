#include <SoundNode.hpp>

#include <algorithm>

namespace flo {

    SoundNode::SoundNode() noexcept
        : StateTable(this)
        , m_network(nullptr) {
    }

    bool SoundNode::canAddDependency(const SoundNode* node) const noexcept {
        // TODO
        return true;
    }

    void SoundNode::addDependency(SoundNode* node){
        if (!canAddDependency(node)){
            throw std::runtime_error("Don't do that.");
        }
        m_dependencies.push_back(node);
        node->m_dependents.push_back(this);
        node->m_dependentOffsets.push_back({this, numDependentStates()});

        node->insertDependentStates(this, 0, numSlots());
    }

    void SoundNode::removeDependency(SoundNode* node){
        if (std::find(m_dependencies.begin(), m_dependencies.end(), node) == m_dependencies.end()){
            throw std::runtime_error("Don't do that.");
        }
        
        node->eraseDependentStates(this, 0, numSlots());
        
        node->m_dependentOffsets.erase(
            std::remove_if(m_dependentOffsets.begin(), m_dependentOffsets.end(), [&](const StateTable::DependentOffset& d){ return d.dependent == this; }),
            m_dependentOffsets.end()
        );
        m_dependencies.erase(
            std::remove(m_dependencies.begin(), m_dependencies.end(), node),
            m_dependencies.end()
        );
        node->m_dependents.erase(
            std::remove(node->m_dependents.begin(), node->m_dependents.end(), this),
            node->m_dependents.end()
        );
    }

    const std::vector<SoundNode*>& SoundNode::getDirectDependencies() const noexcept {
        return m_dependencies;
    }

    const std::vector<SoundNode*>& SoundNode::getDirectDependents() const noexcept {
        return m_dependents;
    }
    

} // namespace flo
