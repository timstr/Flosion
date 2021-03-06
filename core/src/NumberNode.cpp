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

    NumberNode::~NumberNode(){
        setStateOwner(nullptr);
    }

    bool NumberNode::canAddDependency(const NumberNode* node) const noexcept {
        if (hasDependency(node) || node->hasDependency(this)){
            return false;
        }

        // make sure every stateful dependent's state owner is a dependency of every stateful dependency's state owner
        // and that every dependent of the dependent's state owner is a dependent or dependency of the dependency's
        // state owner.
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
                for (const auto dtsodt : dtso->getAllDependents()){
                    if (!(dtsodt->hasDependency(dcso) || dcso->hasDependency(dtsodt))){
                        return false;
                    }
                }
            }
        }

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

    NumberInput* NumberNode::toNumberInput() noexcept {
        return const_cast<NumberInput*>(const_cast<const NumberNode*>(this)->toNumberInput());
    }

    const NumberInput* NumberNode::toNumberInput() const noexcept {
        return nullptr;
    }

    NumberSource* NumberNode::toNumberSource() noexcept {
        return const_cast<NumberSource*>(const_cast<const NumberNode*>(this)->toNumberSource());
    }

    const NumberSource* NumberNode::toNumberSource() const noexcept {
        return nullptr;
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
