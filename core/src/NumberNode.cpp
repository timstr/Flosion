#include <Flosion/Core/NumberNode.hpp>

#include <Flosion/Core/SoundNode.hpp>
#include <Flosion/Core/SoundResult.hpp>

#include <algorithm>
#include <cassert>
#include <functional>

namespace flo {

    NumberNode::NumberNode()
        : m_network(nullptr)
        , m_stateOwner(nullptr) {

    }

    bool NumberNode::canAddDependency(const NumberNode* node) const noexcept {
        if (hasDependency(node) || node->hasDependency(this)){
            return false;
        }

        // make sure every stateful dependent's state owner is a dependency of every stateful dependency's state owner.
        // That might sound confusing but trust me.
        const auto dcs = node->getAllDependencies();
        const auto dts = getAllDependents();
        for (const auto& dc : dcs){
            const auto dcso = dc->getStateOwner();
            if (!dcso){
                continue;
            }
            for (const auto& dt : dts){
                const auto dtso = dt->getStateOwner();
                if (!dtso){
                    continue;
                }
                if (!dcso->hasDependency(dtso)){
                    return false;
                }
            }
        }

        return true;
    }

    bool NumberNode::canRemoveDependency(const NumberNode *) const noexcept {
        return true;
    }

    void NumberNode::afterDependencyAdded(NumberNode*){
        // Nothing to do
    }

    void NumberNode::beforeDependencyRemoved(NumberNode*){
        // Nothing to do
    }
    
    SoundNode* NumberNode::getStateOwner() noexcept {
        return m_stateOwner;
    }

    const SoundNode* NumberNode::getStateOwner() const noexcept {
        return m_stateOwner;
    }

    void NumberNode::setStateOwner(SoundNode* sn){
        if (m_stateOwner){
            m_stateOwner->removeNumberNode(this);
        }
        m_stateOwner = sn;
        if (m_stateOwner){
            m_stateOwner->addNumberNode(this);
        }
    }

} // namespace flo
