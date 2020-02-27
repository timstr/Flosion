#pragma once

#include <Flosion/Core/Immovable.hpp>
#include <Flosion/Core/Signal.hpp>

#include <set>
#include <vector>

namespace flo {

    /**
     * NodeBase uses the CRTP to add directed graph node functions to a concrete type.
     * That type is expected to define these following member functions:
     * - bool canAddDependency(const DerivedNode*) const;
     * - void afterDependencyAdded(DerivedNode*);
     * - void beforeDependencyRemoved(DerivedNode*);
     * - acquireLock
     */
    template<typename DerivedNode>
    class NodeBase : private Immovable {
    public:
        ~NodeBase();

        void addDependency(DerivedNode* node);
        void removeDependency(DerivedNode* node);

        const std::vector<DerivedNode*>& getDirectDependencies() const noexcept;
        const std::vector<DerivedNode*>& getDirectDependents() const noexcept;
        // TODO: why is the constness different here?
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

} // namespace flo

#include <Flosion/Core/NodeBase.tpp>

