#pragma once

#include <Immovable.hpp>
#include <StateTable.hpp>

#include <vector>

namespace flo {

    class SoundState;
    class Network;

    // TODO: add support for dynamically changing TimeBehavior
    // i.e. a Sampler may be 'Normal' until it is switched into
    // some interactive edit mode, when it becomes 'Uncontrolled'
    // NO NO NO. That will be horribly complicated, and the only
    // use case I can think of for this is switching a normally
    // controllable soundsource like NoteSequencer into an interactive
    // keyboard playing mode. It would be simpler to internally
    // substitute in a different sound source that is always uncontrolled.

    /*
    TODO:
    - Every node shall BE a state table (maybe make SoundNode derive from StateTable?)
    - StateTable shall have the pure virtual methods currently defined in StateAllocator
      (but ConcreteStateAllocator should still override these and be co-derived from by derived SoundNodes)
    - No more addStateFor/removeStateFor, it all happens when adding and removing dependencies and keys
      and updates are propagated automatically

    */

    // basic sound node type
    // TODO: derive from StateTable (and make StateTable derive from
    // StateAllocator for its main state) to make this interface less
    // complicated
    class SoundNode : public StateTable, private Immovable {
    public:
        SoundNode() noexcept;
        virtual ~SoundNode() noexcept;
        
        bool canAddDependency(const SoundNode*) const noexcept;
        void addDependency(SoundNode* node);
        void removeDependency(SoundNode* node);

        const std::vector<SoundNode*>& getDirectDependencies() const noexcept;
        const std::vector<SoundNode*>& getDirectDependents() const noexcept;

        bool hasDependency(const SoundNode*) const noexcept;

        bool hasUncontrolledDependency() const noexcept;

        bool isDivergentTo(const SoundNode* dependency) const noexcept;

        bool isOutOfSyncTo(const SoundNode* dependency) const noexcept;

        /**
         * NOTE: this interface is really meant for getNextChunkFor(...), but its
         * arguments differ significantly between concrete sound nodes.
         */

    private:

        /*
        void addDependentStates(const SoundNode* dependent, size_t beginIndex, size_t endIndex);
        void eraseDependentStates(const SoundNode* dependent, size_t beginIndex, size_t endIndex);

        void insertKeys(size_t beginIndex, size_t endIndex);
        void eraseKeys(size_t beginIndex, size_t endIndex);

        void addBorrower(StateBorrower*);
        void removeBorrower(StateBorrower*);

        SoundState* getState(const SoundNode* dependent, size_t dependentStateIndex, size_t keyIndex) noexcept;
        const SoundState* getState(const SoundNode* dependent, size_t dependentStateIndex, size_t keyIndex) const noexcept;

        SoundState* getState(size_t index) noexcept;
        const SoundState* getState(size_t index) const noexcept;

        State* getBorrowedState(const SoundState* mainState, const StateBorrower* borrower) noexcept;
        const State* getBorrowedState(const SoundState* mainState, const StateBorrower* borrower) const noexcept;
        */

    private:

        virtual bool isDivergent() const noexcept = 0;
        virtual bool isUncontrolled() const noexcept = 0;
        virtual bool isOutOfSync() const noexcept = 0;
        virtual double getTimeSpeed(const SoundState* mainState) const noexcept = 0;

    private:
        std::vector<SoundNode*> m_dependents;
        std::vector<SoundNode*> m_dependencies;
        Network* m_network;
    };

    // To create a concrete sound node, choose from the following:
    // - Singular or Divergent or Uncontrolled
    // - Realtime or OutOfSync

    // Implements a singular soundnode.
    template<typename SoundNodeType, typename SoundStateType>
    class Singular : public SoundNodeType {
    public:
        Singular();

        SoundStateType* getState(const SoundNode* dependent, const SoundState* dependentState) noexcept;
        const SoundStateType* getState(const SoundNode* dependent, const SoundState* dependentState) const noexcept;

        SoundStateType* getState(size_t index) noexcept;
        const SoundStateType* getState(size_t index) const noexcept;

        using StateType = SoundStateType;

    private:
        bool isDivergent() const noexcept override final;
        bool isUncontrolled() const noexcept override final;
        std::unique_ptr<StateAllocator> makeAllocator() const override final;
    };

    // Implements a divergent soundnode
    template<typename SoundNodeType, typename SoundStateType, typename KeyType>
    class Divergent : public SoundNodeType {
    public:

        SoundStateType* getState(const SoundNode* dependent, const SoundState* dependentState, const KeyType& key) noexcept;
        const SoundStateType* getState(const SoundNode* dependent, const SoundState* dependentState, const KeyType& key) const noexcept;

        SoundStateType* getState(size_t index) noexcept;
        const SoundStateType* getState(size_t index) const noexcept;

        using StateType = SoundStateType;

        void addKey(const KeyType&);
        bool hasKey(const KeyType&) const noexcept;
        void removeKey(const KeyType&);

    private:
        bool isDivergent() const noexcept override final;
        bool isUncontrolled() const noexcept override final;
        std::unique_ptr<StateAllocator> makeAllocator() const override final;

        std::vector<KeyType> m_keys; // indices here are also indices into columns of state table. Keep sorted.
    };

    // Implements an uncontrolled soundnode
    // An uncontrolled soundnode always has exactly one state,
    // and may have at most one dependent and with at most one
    // dependent state. If there is no dependent state, the
    // uncontrolled soundnode's single state will have null for
    // its dependent state.
    // Uncontrolled implies singular, i.e. an uncontrolled soundnode
    // has exactly one state per dependent state.
    template<typename SoundNodeType, typename SoundStateType>
    class Uncontrolled : public SoundNodeType {
    public:
        Uncontrolled();

        const SoundStateType* getMonoState() const noexcept;

        using StateType = SoundStateType;

    private:
        bool isDivergent() const noexcept override final;
        bool isUncontrolled() const noexcept override final;
        std::unique_ptr<StateAllocator> makeAllocator() const override final;
    };

    // Implements a realtime soundnode
    template<typename SoundNodeType>
    class Realtime : public SoundNodeType {
    private:
        bool isOutOfSync() const noexcept override final;
        double getTimeSpeed(const SoundState*) const noexcept override final;
    };

    // Implements an out-of-sync (non-realtime) soundnode
    template<typename SoundNodeType>
    class OutOfSync : public SoundNodeType {
    public:
        using SoundNode::getTimeSpeed;

    private:
        bool isOutOfSync() const noexcept override final;
    };

    /* Example usage:

    class SoundInput : Realtime<Controllable<SoundNode>> { ... };

    class SingleSoundInput : Singular<SoundInput, EmptyState> { ... };

    template<typename SoundStateType, typename KeyType
    class MultiSoundInput : Divergent<SoundInput, EmptyState, KeyType> { ... };


    class SoundSource : SoundNode { ... };

    template<typename SoundStateType>
    class ConcreteSoundSource : Singular<SoundNode, SoundStateType> { ... };

    class PhaseVocoderState { ... };
    
    class PhaseVocoder : OutOfSync<Controllable<ConcreteSoundSource<PhaseVocoderState>>> { ... };

    */
    

} // namespace flo

#include <SoundNode.tpp>
