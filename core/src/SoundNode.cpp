#include <SoundNode.hpp>

#include <algorithm>

namespace flo {

    SoundNode::SoundNode() noexcept
        : StateTable(this)
        , m_network(nullptr) {
    }

    SoundNode::~SoundNode() noexcept {
        while (m_dependencies.size() > 0){
            removeDependency(m_dependencies.back());
        }
        while (m_dependents.size() > 0){
            m_dependents.back()->removeDependency(this);
        }
    }

    bool SoundNode::canAddDependency(const SoundNode* node) const noexcept {
        if (hasDependency(node) || node->hasDependency(this)){
            return false;
        }

        if (node->isUncontrolled() || node->hasUncontrolledDependency()){
            // make sure that there will be nothing but a line of realtime, singular dependents
            const auto dds = node->getDirectDependents();
            if (dds.size() > 0){
                return false;
            }

            auto curr = this;
            while (curr){
                if (curr->isDivergent() || curr->isOutOfSync()){
                    return false;
                }
                const auto dds = curr->getDirectDependents();
                if (dds.size() == 0){
                    return true;
                } else if (dds.size() == 1){
                    curr = dds[0];
                } else {
                    return false;
                }
            }
        }

        return true;
    }

    bool SoundNode::canRemoveDependency(const SoundNode *) const noexcept {
        // TODO: make sure that no numbersources would lose access to the
        // state they need
        return true;
    }

    void SoundNode::addDependency(SoundNode* node){
        if (!canAddDependency(node)){
            throw std::runtime_error("Don't do that.");
        }
        m_dependencies.push_back(node);
        node->m_dependents.push_back(this);
        node->m_dependentOffsets.push_back({this, node->numDependentStates()});

        if (numSlots() > 0){
            node->insertDependentStates(this, 0, numSlots());
            node->repointStatesFor(this);
        }
    }

    void SoundNode::removeDependency(SoundNode* node){
        if (!canRemoveDependency(node)){
            throw std::runtime_error("Don't do that.");
        }

        if (std::find(m_dependencies.begin(), m_dependencies.end(), node) == m_dependencies.end()){
            throw std::runtime_error("Don't do that.");
        }
        
        if (numSlots() > 0){
            node->eraseDependentStates(this, 0, numSlots());
        }
        
        node->m_dependentOffsets.erase(
            std::remove_if(node->m_dependentOffsets.begin(),
                node->m_dependentOffsets.end(),
                [&](const StateTable::DependentOffset& d){
                    return d.dependent == this;
                }),
            node->m_dependentOffsets.end()
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

    bool SoundNode::hasDependency(const SoundNode* node) const noexcept {
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

    bool SoundNode::hasUncontrolledDependency() const noexcept {
        if (isUncontrolled()){
            return true;
        }
        for (const auto& d : m_dependencies){
            if (d->hasUncontrolledDependency()){
                return true;
            }
        }
        return false;
    }
    

} // namespace flo
