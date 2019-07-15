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

    const ISoundNode* ISoundNode::toISoundNode() const noexcept {
        return this;
    }

    OSoundNode::OSoundNode(Propagation statePropagation)
        : m_propagation(statePropagation) {
        
    }

    bool OSoundNode::canAddDependency(const ISoundNode* node) const noexcept {
        // TODO
        return {};
    }

    void OSoundNode::addDependency(ISoundNode* node){
        // TODO
    }

    void OSoundNode::removeDependency(ISoundNode* node){
        // TODO
    }

    const std::vector<ISoundNode*>& OSoundNode::getDirectDependencies() const noexcept {
        return m_dependencies;
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

    std::optional<SoundNode::Propagation> OSoundNode::getPropagationTo(const ISoundNode* dependency) const noexcept {
        // TODO
        return {};
    }

    std::optional<SoundNode::TimeSync> OSoundNode::getTimeSyncTo(const ISoundNode* dependency) const noexcept {
        // TODO
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

    double IOSoundNode::getTimeSpeed(const SoundState* context) const noexcept {
        return 1.0;
    }

    const IOSoundNode* IOSoundNode::toIOSoundNode() const noexcept {
        return this;
    }

} // namespace flo
