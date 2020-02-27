#include <algorithm>
#include <cassert>
#include <type_traits>

namespace flo {

    template<typename DerivedNode>
    inline NodeBase<DerivedNode>::~NodeBase(){
        while (m_dependencies.size() > 0){
            static_cast<DerivedNode*>(this)->removeDependency(m_dependencies.back());
        }
        while (m_dependents.size() > 0){
            m_dependents.back()->removeDependency(static_cast<DerivedNode*>(this));
        }
    }

    template<typename DerivedNode>
    inline void NodeBase<DerivedNode>::addDependency(DerivedNode* node){
        assert(node);
        assert(std::count(m_dependencies.begin(), m_dependencies.end(), node) == 0);
        assert(std::count(node->m_dependents.begin(), node->m_dependents.end(), this) == 0);
        if (!static_cast<const DerivedNode*>(this)->canAddDependency(node)){
            throw std::runtime_error("Don't do that.");
        }
        m_dependencies.push_back(node);
        node->m_dependents.push_back(static_cast<DerivedNode*>(this));
        static_cast<DerivedNode*>(this)->afterDependencyAdded(node);
    }

    template<typename DerivedNode>
    inline void NodeBase<DerivedNode>::removeDependency(DerivedNode* node){
        assert(node);
        assert(std::count(m_dependencies.begin(), m_dependencies.end(), node) == 1);
        assert(std::count(node->m_dependents.begin(), node->m_dependents.end(), this) == 1);
        if (!hasDirectDependency(node)){
            throw std::runtime_error("Don't do that.");
        }
        static_cast<DerivedNode*>(this)->beforeDependencyRemoved(node);
        m_dependencies.erase(
            std::remove(m_dependencies.begin(), m_dependencies.end(), node),
            m_dependencies.end()
        );
        node->m_dependents.erase(
            std::remove(node->m_dependents.begin(), node->m_dependents.end(), this),
            node->m_dependents.end()
        );
    }

    template<typename DerivedNode>
    inline const std::vector<DerivedNode*>& NodeBase<DerivedNode>::getDirectDependencies() const noexcept {
        assert(std::count(m_dependencies.begin(), m_dependencies.end(), nullptr) == 0);
        return m_dependencies;
    }

    template<typename DerivedNode>
    inline const std::vector<DerivedNode*>& NodeBase<DerivedNode>::getDirectDependents() const noexcept {
        assert(std::count(m_dependents.begin(), m_dependents.end(), nullptr) == 0);
        return m_dependents;
    }

    template<typename DerivedNode>
    inline std::set<const DerivedNode*> NodeBase<DerivedNode>::getAllDependencies() const noexcept {
        std::set<const DerivedNode*> nodes;
        std::function<void(const DerivedNode*)> search = [&](const DerivedNode* n){
            assert(n);
            nodes.insert(n);
            for (const auto& d : n->getDirectDependencies()){
                search(d);
            }
        };
        search(static_cast<const DerivedNode*>(this));
        return nodes;
    }

    template<typename DerivedNode>
    inline std::set<const DerivedNode*> NodeBase<DerivedNode>::getAllDependents() const noexcept {
        std::set<const DerivedNode*> nodes;
        const std::function<void(const DerivedNode*)> search = [&](const DerivedNode* n){
            assert(n);
            nodes.insert(n);
            for (const auto& d : n->getDirectDependents()){
                search(d);
            }
        };
        search(static_cast<const DerivedNode*>(this));
        return nodes;
    }

    template<typename DerivedNode>
    inline bool NodeBase<DerivedNode>::hasDependency(const DerivedNode* node) const noexcept {
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

    template<typename DerivedNode>
    inline bool NodeBase<DerivedNode>::hasDirectDependency(const DerivedNode* node) const noexcept {
        return std::find(m_dependencies.begin(), m_dependencies.end(), node) != m_dependencies.end();
    }



    template<typename Traits>
    inline InputNodeBase<Traits>::InputNodeBase()
        : m_source(nullptr) {
    }

    template<typename Traits>
    inline InputNodeBase<Traits>::~InputNodeBase(){
        setSource(nullptr);
        onDestroy.broadcast();
    }

    template<typename Traits>
    inline void InputNodeBase<Traits>::setSource(TraitsOutput<Traits>* source){
        auto self = static_cast<TraitsInput<Traits>*>(this);
        auto lock = self->acquireLock();
        const auto oldSource = m_source;
        if (m_source){

            auto& inputs = m_source->m_inputs;

            assert(std::count(inputs.begin(), inputs.end(), self) == 1);
            auto it = std::find(inputs.begin(), inputs.end(), self);
            assert(it != inputs.end());
            inputs.erase(it);

            this->removeDependency(m_source);
        }
        m_source = source;
        if (m_source){
            this->addDependency(m_source);
            
            auto& inputs = m_source->m_inputs;

            assert(std::count(inputs.begin(), inputs.end(), self) == 0);
            inputs.push_back(self);
        }
        if (oldSource){
            oldSource->onInputRemoved.broadcast(self);
            onSourceRemoved.broadcast(oldSource);
        }
        if (m_source){
            onSourceAdded.broadcast(m_source);
            m_source->onInputAdded.broadcast(self);
        }
    }

    template<typename Traits>
    inline TraitsOutput<Traits>* InputNodeBase<Traits>::getSource() noexcept {
        return m_source;
    }

    template<typename Traits>
    inline const TraitsOutput<Traits>* InputNodeBase<Traits>::getSource() const noexcept {
        return m_source;
    }



    template<typename Traits>
    inline OutputNodeBase<Traits>::~OutputNodeBase(){
        while (m_inputs.size() > 0){
            auto i = m_inputs.back();
            assert(i->getSource() == static_cast<TraitsOutput<Traits>*>(this));
            i->setSource(nullptr);
        }
        onDestroy.broadcast();
    }

    template<typename Traits>
    inline const std::vector<TraitsInput<Traits>*>& OutputNodeBase<Traits>::getInputs() const noexcept {
        return m_inputs;
    }

    template<typename Traits>
    inline void OutputNodeBase<Traits>::disconnectAllInputs(){
        while (m_inputs.size() > 0){
            m_inputs.back()->setSource(nullptr);
        }
    }

} // namespace flo
