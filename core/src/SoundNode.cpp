#include <Flosion/Core/SoundNode.hpp>

#include <Flosion/Core/SoundResult.hpp>

#include <algorithm>
#include <functional>
#include <set>

namespace flo {

    SoundNode::SoundNode()
        : StateTable(this)
        , m_network(nullptr) {
    }

    bool SoundNode::canAddDependency(const SoundNode* node) const noexcept {
        if (hasDependency(node) || node->hasDependency(this)){
            return false;
        }

        // make sure that the node doesn't already have other
        // dependencies whose number sources it depends on
        {
            for (const auto& dc : node->getAllDependencies()){
                for (const auto& nn : dc->getNumberNodes()){
                    for (const auto& nndc : nn->getAllDependencies()){
                        if (const auto so = nndc->getStateOwner()){
                            if (!node->hasDependency(so) && !so->hasDependency(this)){
                                return false;
                            }
                        }
                    }
                }
            }
        }

        if (node->hasUncontrolledDependency()){
            // make sure that there will be nothing but a line of realtime, singular dependents
            if (node->getDirectDependents().size() > 0){
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

    bool SoundNode::canSafelyRemoveDependency(const SoundNode* node) const noexcept {
        if (!hasDirectDependency(node)){
            return false;
        }

        std::function<bool(const SoundNode*, const SoundNode*)> wouldStillHaveDependency = [&](const SoundNode* dt, const SoundNode* dc){
            if (dt == dc){
                return true;
            }
            for (const auto& dtdc : dt->getDirectDependencies()){
                if (!(dt == this && dtdc == node)){
                    if (wouldStillHaveDependency(dtdc, dc)){
                        return true;
                    }
                }
            }
            return false;
        };

        for (const auto& dc : node->getAllDependencies()){
            for (const auto& dcnn : dc->getNumberNodes()){
                for (const auto& nndc : dcnn->getAllDependencies()){
                    if (const auto sn = nndc->getStateOwner()){
                        if (!wouldStillHaveDependency(sn, dc)){
                            return false;
                        }
                    }
                }
            }
        }

        return true;
    }

    void SoundNode::afterDependencyAdded(SoundNode* node){
        node->addDependentOffset(this);
        if (numSlots() > 0){
            node->insertDependentStates(this, 0, numSlots());
            node->repointStatesFor(this);
        }
    }

    void SoundNode::beforeDependencyRemoved(SoundNode* nodeToRemove){
        // If any number nodes belonging to any dependencies of the given
        // node would lose access to their state, break their connections
        
        std::function<bool(const SoundNode*, const SoundNode*)> wouldStillHaveAccess = [&](const SoundNode* dc, const SoundNode* dt){
            if (dc == dt){
                return true;
            }
            for (const auto& ddt : dc->getDirectDependents()){
                if (dc == nodeToRemove && ddt == this){
                    continue;
                }
                if (wouldStillHaveAccess(ddt, dt)){
                    return true;
                }
            }
            return false;
        };

        // NOTE: connections to break are cached to avoid modifying
        // containers while iterating over them
        // The stored pairs are ordered as (some node, its direct dependency)
        std::set<std::pair<NumberInput*, NumberNode*>> toDisconnect;

        for (const auto& dc : nodeToRemove->getAllDependencies()){
            for (const auto dcnn : dc->getNumberNodes()){
                if (auto dcni = dcnn->toNumberInput()){
                    for (const auto& dcniddc : dcni->getDirectDependencies()){
                        for (const auto& dcnidc : dcniddc->getAllDependencies()){
                            if (auto dcnidcso = dcnidc->getStateOwner()){
                                if (!wouldStillHaveAccess(dc, dcnidcso)){
                                    assert(dcni->hasDirectDependency(dcniddc));
                                    toDisconnect.insert({dcni, dcniddc});
                                }
                            }
                        }
                    }
                }
            }
        }

        for (const auto& [ni, dc] : toDisconnect){
            assert(ni->hasDirectDependency(dc));
            assert(ni->getSource() == dc);
            ni->setSource(nullptr);
        }

        if (numSlots() > 0){
            nodeToRemove->eraseDependentStates(this, 0, numSlots());
        }
        nodeToRemove->removeDependentOffset(this);
    }

    bool SoundNode::hasUncontrolledDependency() const noexcept {
        for (auto d : getAllDependencies()){
            if (d->isUncontrolled()){
                return true;
            }
        }
        return false;
    }

    const std::vector<NumberNode*>& SoundNode::getNumberNodes() const {
        return m_numberNodes;
    }
    
    SoundNode::Lock::Lock(std::vector<std::unique_lock<RecursiveSharedMutex>> locks) noexcept
        : m_locks(std::move(locks)) {
        
    }

    SoundNode::Lock SoundNode::acquireLock() noexcept {
        // Find all dependent sound results
        std::vector<SoundResult*> soundResults;
        findDependentSoundResults(soundResults);

        // Sort the sound results in order of address (this will
        // prevent deadlock when acquiring multiple locks)
        std::sort(soundResults.begin(), soundResults.end());

        // Remove duplicates
        soundResults.erase(
            std::unique(soundResults.begin(), soundResults.end()),
            soundResults.end()
        );

        // Acquire locks from mutexes of those sound results
        std::vector<std::unique_lock<RecursiveSharedMutex>> locks;
        locks.reserve(soundResults.size());
        std::transform(
            soundResults.begin(),
            soundResults.end(),
            std::back_inserter(locks),
            [](SoundResult* res){
                return std::unique_lock{res->m_mutex};
            }
        );

        // Return custom multi-lock
        return Lock{std::move(locks)};
    }

    void SoundNode::findDependentSoundResults(std::vector<SoundResult*>& out) noexcept {
        for (const auto& d : getDirectDependents()){
            d->findDependentSoundResults(out);
        }
    }

    void SoundNode::addNumberNode(NumberNode* node){
        assert(std::find(
            m_numberNodes.begin(),
            m_numberNodes.end(),
            node
        ) == m_numberNodes.end());
        m_numberNodes.push_back(node);
    }

    void SoundNode::removeNumberNode(NumberNode* node){
        assert(std::count(
            m_numberNodes.begin(),
            m_numberNodes.end(),
            node
        ) == 1);
        auto it = std::find(
            m_numberNodes.begin(),
            m_numberNodes.end(),
            node
        );
        assert(it != m_numberNodes.end());
        m_numberNodes.erase(it);
    }

    CurrentTime::CurrentTime(SoundNode* owner) noexcept
        : m_owner(owner) {
        setStateOwner(m_owner);
    }

    double CurrentTime::evaluate(const SoundState* context) const noexcept {
        return context->getElapsedTimeAt(m_owner);
    }

} // namespace flo
