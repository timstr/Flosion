#include <SoundNode.hpp>

namespace flo {

    ISoundNode* SoundNode::toISoundNode() noexcept {
        return const_cast<ISoundNode*>(const_cast<const SoundNode*>(this)->toISoundNode());
    }

    OSoundNode* SoundNode::toOSoundNode() noexcept {
        return const_cast<OSoundNode*>(const_cast<const SoundNode*>(this)->toOSoundNode());
    }

    IOSoundNode* SoundNode::toIOSoundNode() noexcept {
        return const_cast<IOSoundNode*>(const_cast<const SoundNode*>(this)->toIOSoundNode());
    }

    const ISoundNode* SoundNode::toISoundNode() const noexcept {
        return nullptr;
    }

    const OSoundNode* SoundNode::toOSoundNode() const noexcept {
        return nullptr;
    }

    const IOSoundNode* SoundNode::toIOSoundNode() const noexcept {
        return nullptr;
    }

    ISoundNode::ISoundNode(Controllability controllability, std::unique_ptr<StateAllocator> mainStateAllocator)
        : stateTable(this, std::move(mainStateAllocator))
        , m_controllability(controllability) {
        
    }

    const std::vector<OSoundNode*>& ISoundNode::getDirectDependents() const noexcept {
        return m_dependents;
    }

    SoundNode::Controllability ISoundNode::getControllability() const noexcept {
        return m_controllability;
    }

    const ISoundNode* ISoundNode::toISoundNode() const noexcept {
        return this;
    }

    OSoundNode::OSoundNode(Propagation statePropagation)
        : m_propagation(statePropagation) {
        
    }

    bool OSoundNode::canAddDependency(const ISoundNode* node) const noexcept {
        if (hasDependency(node)){
            return false;
        }

        auto onode = node->toOSoundNode();
        bool uncontrolled = (node->getControllability() == Controllability::Uncontrolled)
            || (onode && onode->hasUncontrolledDependency());

        if (uncontrolled){
            if (getPropagation() == Propagation::Divergent){
                return false;
            }

            // make sure all dependents form a line graph,
            // which is realtime and singular at all points
            auto curr = this->toOSoundNode();
            while (curr){
                if (curr->getPropagation() == Propagation::Divergent){
                    return false;
                }
                auto ioCurr = curr->toIOSoundNode();
                if (!ioCurr){
                    break;
                }
                if (ioCurr->getTimeSync() == TimeSync::OutOfSync){
                    return false;
                }
                auto dependents = ioCurr->getDirectDependents();
                if (dependents.size() > 1){
                    return false;
                }
                if (dependents.size() > 0){
                    curr = dependents.front();
                } else {
                    break;
                }
            }
        }

        return true;
    }

    void OSoundNode::addDependency(ISoundNode* node){
        if (!canAddDependency(node)){
            // TODO: make custom exception type, i.e. NetworkException or SoundNodeException
            throw std::runtime_error("Don't do that.");
        }

        m_dependencies.push_back(node);
        node->m_dependents.push_back(this);

        if (auto iSelf = toISoundNode()){
            const auto& table = iSelf->stateTable;
            for (size_t i = 0; i < table.size(); ++i){
                node->addStateFor(this, table.getState(i));
            }
        }
    }

    void OSoundNode::removeDependency(ISoundNode* node){
        if (auto iSelf = toISoundNode()){
            const auto& table = iSelf->stateTable;
            for (size_t i = 0; i < table.size(); ++i){
                node->removeStateFor(this, table.getState(i));
            }
        }

        m_dependencies.erase(
            std::remove(m_dependencies.begin(), m_dependencies.end(), node),
            m_dependencies.end()
        );
        node->m_dependents.erase(
            std::remove(node->m_dependents.begin(), node->m_dependents.end(), this),
            node->m_dependents.end()
        );
    }

    const std::vector<ISoundNode*>& OSoundNode::getDirectDependencies() const noexcept {
        return m_dependencies;
    }

    SoundNode::Propagation OSoundNode::getPropagation() const noexcept {
        return m_propagation;
    }

    bool OSoundNode::hasDependency(const ISoundNode* node) const noexcept {
        for (const auto& d : m_dependencies){
            if (node == d){
                return true;
            }

            if (auto onode = d->toOSoundNode(); onode->hasDependency(node)){
                return true;
            }
        }

        return false;
    }

    bool OSoundNode::hasUncontrolledDependency() const noexcept {
        for (const auto& d : m_dependencies){
            if (d->getControllability() == Controllability::Uncontrolled){
                return true;
            }
            if (auto od = d->toOSoundNode(); od->hasUncontrolledDependency()){
                return true;
            }
        }
        return false;
    }

    std::optional<SoundNode::Propagation> OSoundNode::getPropagationTo(const ISoundNode* dependency) const noexcept {
        bool alreadySingular = false;
        for (const auto& d : m_dependencies){
            if (d == dependency){
                return m_propagation;
            }
            if (auto od = d->toOSoundNode()){
                auto p = od->getPropagationTo(dependency);
                if (!p.has_value()){
                    continue;
                } else if (*p == Propagation::Singular){
                    if (alreadySingular){
                        return Propagation::Divergent;
                    }
                    alreadySingular = true;
                } else if (*p == Propagation::Divergent){
                    return Propagation::Divergent;
                }
            }
        }
        return {};
    }

    std::optional<SoundNode::TimeSync> OSoundNode::getTimeSyncTo(const ISoundNode* dependency) const noexcept {
        bool foundRealtime = false;
        for (const auto& d : m_dependencies){
            if (d == dependency){
                return TimeSync::Realtime;
            }
            if (auto iod = d->toIOSoundNode()){
                auto netTs = iod->getTimeSyncTo(dependency);
                if (!netTs.has_value()){
                    continue;
                }
                if (*netTs == TimeSync::OutOfSync){
                    return TimeSync::OutOfSync;
                }
                auto iodTs = iod->getTimeSync();
                if (iodTs == TimeSync::OutOfSync){
                    return TimeSync::OutOfSync;
                }
                foundRealtime = true;
            }
        }
        if (foundRealtime){
            return TimeSync::Realtime;
        }
        return {};
    }

    const OSoundNode* OSoundNode::toOSoundNode() const noexcept {
        return this;
    }

    IOSoundNode::IOSoundNode(Controllability c, Propagation sp, TimeSync ts, std::unique_ptr<StateAllocator> sa)
        : ISoundNode(c, std::move(sa))
        , OSoundNode(sp)
        , m_timeSync(ts) {

    }

    SoundNode::TimeSync IOSoundNode::getTimeSync() const noexcept {
        return m_timeSync;
    }

    double IOSoundNode::getTimeSpeed(const SoundState* context) const noexcept {
        return 1.0;
    }

    const IOSoundNode* IOSoundNode::toIOSoundNode() const noexcept {
        return this;
    }

} // namespace flo
