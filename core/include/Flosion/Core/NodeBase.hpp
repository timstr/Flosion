#pragma once

#include <Flosion/Core/Immovable.hpp>
#include <Flosion/Core/Signal.hpp>

#include <algorithm>
#include <cassert>
#include <set>
#include <type_traits>
#include <vector>

namespace flo {

    /**
     * NodeBase uses the CRTP to add directed graph node functions to a concrete type.
     * That type is expected to define these following member functions:
     * - bool canAddDependency(const DerivedNode*) const;
     * - void afterDependencyAdded(DerivedNode*);
     * - void beforeDependencyRemoved(DerivedNode*);
     */
    template<typename DerivedNode>
    class NodeBase : private Immovable {
    public:
        ~NodeBase();

        void addDependency(DerivedNode* node);
        void removeDependency(DerivedNode* node);

        const std::vector<DerivedNode*>& getDirectDependencies() const noexcept;
        const std::vector<DerivedNode*>& getDirectDependents() const noexcept;

        std::set<const DerivedNode*> getAllDependencies() const noexcept;
        std::set<const DerivedNode*> getAllDependents() const noexcept;

        /**
         * Returns true if this node depends (directly or indirectly)
         * on the given node.
         */
        bool hasDependency(const DerivedNode*) const noexcept;

        bool hasDirectDependency(const DerivedNode*) const noexcept;

    private:
        std::vector<DerivedNode*> m_dependencies;
        std::vector<DerivedNode*> m_dependents;
    };

    // TODO: consider putting these template parameters into a traits struct
    
    template<typename Traits>
    using TraitsBase = typename Traits::Base;

    template<typename Traits>
    using TraitsInput= typename Traits::InputType;

    template<typename Traits>
    using TraitsOutput = typename Traits::OutputType;

    template<typename Traits>
    class InputNodeBase;

    template<typename Traits>
    class OutputNodeBase;

    template<typename Traits>
    class InputNodeBase : public TraitsBase<Traits> {
    public:
        InputNodeBase();
        ~InputNodeBase();

        void setSource(TraitsOutput<Traits>*);

        TraitsOutput<Traits>* getSource() noexcept;
        const TraitsOutput<Traits>* getSource() const noexcept;
        
        Signal<const TraitsOutput<Traits>*> onSourceAdded;
        Signal<const TraitsOutput<Traits>*> onSourceRemoved;
        Signal<> onDestroy;

    private:
        TraitsOutput<Traits>* m_source;
    };

    template<typename Traits>
    class OutputNodeBase : public TraitsBase<Traits> {
    public:
        ~OutputNodeBase();

        Signal<const TraitsInput<Traits>*> onInputAdded;
        Signal<const TraitsInput<Traits>*> onInputRemoved;
        Signal<> onDestroy;

        const std::vector<TraitsInput<Traits>*>& getInputs() const noexcept;

        void disconnectAllInputs();

    private:
        std::vector<TraitsInput<Traits>*> m_inputs;

        friend InputNodeBase<Traits>;
    };

    // TODO: move the following definitions to a .tpp file

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

            removeDependency(m_source);
        }
        m_source = source;
        if (m_source){
            addDependency(m_source);
            
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
            assert(m_inputs.back()->getSource() == static_cast<TraitsOutput<Traits>*>(this));
            m_inputs.back()->setSource(nullptr);
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
