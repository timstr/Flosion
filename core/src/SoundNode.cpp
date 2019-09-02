#include <SoundNode.hpp>

#include <SoundResult.hpp>

#include <algorithm>
#include <set>

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

        // make sure that the node doesn't already have other
        // dependencies whose number sources it depends on
        {
            const auto dependents = getAllDependents();
            for (const auto& nn : node->getNumberNodes()){
                for (const auto& nd : nn->getAllDependencies()){
                    const auto ndo = nd->getStateOwner();
                    if (!ndo){
                        continue;
                    }
                    if (dependents.find(ndo) != dependents.end()){
                        return false;
                    }
                }
            }
        }

        if (node->hasUncontrolledDependency()){
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

    bool SoundNode::canRemoveDependency(const SoundNode* node) const noexcept {
        if (!hasDirectDependency(node)){
            return false;
        }

        std::function<bool(const SoundNode*, const SoundNode*)> wouldStillHaveDependency = [&](const SoundNode* dt, const SoundNode* dc){
            if (dt == this){
                return false;
            }
            if (dt == dc){
                return true;
            }
            for (const auto& dtdc : dt->getDirectDependencies()){
                if (wouldStillHaveDependency(dtdc, dc)){
                    return true;
                }
            }
            return false;
        };

        std::set<const NumberNode*> nndcs;
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

    void SoundNode::addDependency(SoundNode* node){
        if (!canAddDependency(node)){
            throw std::runtime_error("Don't do that.");
        }
        auto lock = getScopedWriteLock();
        m_dependencies.push_back(node);
        node->m_dependents.push_back(this);
        node->addDependentOffset(this);
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
        
        auto lock = getScopedWriteLock();
        if (numSlots() > 0){
            node->eraseDependentStates(this, 0, numSlots());
        }
        node->removeDependentOffset(this);

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

    std::set<const SoundNode*> SoundNode::getAllDependencies() const noexcept {
        std::set<const SoundNode*> nodes;
        std::function<void(const SoundNode*)> search = [&](const SoundNode* n){
            if (!n){
                return;
            }
            nodes.insert(n);
            for (const auto& d : n->getDirectDependencies()){
                search(d);
            }
        };
        search(this);
        return nodes;
    }

    std::set<const SoundNode*> SoundNode::getAllDependents() const noexcept {
        std::set<const SoundNode*> nodes;
        std::function<void(const SoundNode*)> search = [&](const SoundNode* n){
            if (!n){
                return;
            }
            nodes.insert(n);
            for (const auto& d : n->getDirectDependents()){
                search(d);
            }
        };
        search(this);
        return nodes;
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

    bool SoundNode::hasDirectDependency(const SoundNode* node) const noexcept {
        return std::find(
            m_dependencies.begin(),
            m_dependencies.end(),
            node
        ) != m_dependencies.end();
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

    const std::vector<NumberNode*>& SoundNode::getNumberNodes() const {
        return m_numberNodes;
    }
    
    SoundNode::Lock::Lock(std::vector<std::unique_lock<RecursiveSharedMutex>> locks) noexcept
        : m_locks(std::move(locks)) {
        
    }

    SoundNode::Lock SoundNode::getScopedWriteLock() noexcept {
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
        for (const auto& d : m_dependents){
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
