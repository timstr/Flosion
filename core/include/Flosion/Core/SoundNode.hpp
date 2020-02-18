#pragma once

#include <Flosion/Core/NodeBase.hpp>
#include <Flosion/Core/NumberSource.hpp>
#include <Flosion/Core/RecursiveSharedMutex.hpp>
#include <Flosion/Core/SoundState.hpp>
#include <Flosion/Core/StateTable.hpp>

#include <optional>
#include <vector>

namespace flo {

    class Network;
    class SoundResult;
    class SoundState;
    class SoundInput;
    class SoundSource;

    // basic sound node type
    class SoundNode : public StateTable, public NodeBase<SoundNode> {
    public:
        SoundNode();
        virtual ~SoundNode() = default;
        
        bool canAddDependency(const SoundNode*) const noexcept;
        bool canSafelyRemoveDependency(const SoundNode*) const noexcept;

        // TODO: hide these
        void afterDependencyAdded(SoundNode*);
        void beforeDependencyRemoved(SoundNode*);

        bool hasUncontrolledDependency() const noexcept;

        // TODO: add all number inputs, sources, and borrowers
        const std::vector<NumberNode*>& getNumberNodes() const;

        /**
         * NOTE: this interface is really meant for getNextChunkFor(...), but its
         * arguments differ significantly between concrete sound nodes.
         */

        SoundSource* toSoundSource() noexcept;
        virtual const SoundSource* toSoundSource() const noexcept;

        SoundInput* toSoundInput() noexcept;
        virtual const SoundInput* toSoundInput() const noexcept;

        virtual bool isDivergent() const noexcept = 0;
        virtual bool isUncontrolled() const noexcept = 0;
        virtual bool isOutOfSync() const noexcept = 0;

    protected:

        class Lock {
        public:
            ~Lock() noexcept = default;

        private:
            Lock(std::vector<std::unique_lock<RecursiveSharedMutex>> locks) noexcept;

            std::vector<std::unique_lock<RecursiveSharedMutex>> m_locks;

            friend class SoundNode;
        };

        [[nodiscard]]
        Lock acquireLock() noexcept;

        virtual double getTimeSpeed(const SoundState* mainState) const noexcept = 0;
        
        using Bound = std::optional<std::size_t>;
        
    private:

        virtual bool isDivergent() const noexcept = 0;
        virtual bool isUncontrolled() const noexcept = 0;
        virtual bool isOutOfSync() const noexcept = 0;
        virtual bool isBounded() const noexcept = 0;

        virtual Bound getBound(const SoundState*) const noexcept = 0;

        virtual void findDependentSoundResults(std::vector<SoundResult*>& soundResults) noexcept;

    private:
        void addNumberNode(NumberNode*);
        void removeNumberNode(NumberNode*);

        friend class NumberNode;

    private:

        // TODO: populate this with number inputs and sources that belong to this soundnode,
        // and with stateful numbernodes that are borrowing state from this soundnode.
        std::vector<NumberNode*> m_numberNodes;
        Network* m_network;

        friend class SoundState;
    };

    // To create a concrete sound node, choose one from each of the following:
    // - Singular, Divergent, or Uncontrolled
    // - Realtime or OutOfSync
    // - Bounded, Unbounded, or MaybeBounded

    // Implements a singular soundnode.
    template<typename SoundNodeType, typename SoundStateType>
    class Singular : public SoundNodeType {
    public:
        using StateType = SoundStateType;

        template<typename... Args>
        Singular(Args&&...);

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
        using StateType = SoundStateType;
        
        using SoundNodeType::SoundNodeType;

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

        size_t getKeyIndex(const KeyType&) const noexcept;

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
        using StateType = SoundStateType;

        template<typename... Args>
        Uncontrolled(Args&&... args);

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
    public:
        using SoundNodeType::SoundNodeType;

    private:
        bool isOutOfSync() const noexcept override final;
        double getTimeSpeed(const SoundState*) const noexcept override final;
    };

    // Implements an out-of-sync (non-realtime) soundnode
    template<typename SoundNodeType>
    class OutOfSync : public SoundNodeType {
    public:
        using SoundNodeType::SoundNodeType;

        using SoundNode::getTimeSpeed;

    private:
        bool isOutOfSync() const noexcept override final;
    };

    class CurrentTime : public NumberSource {
    public:
        CurrentTime(SoundNode* owner) noexcept;

    private:
        SoundNode* const m_owner;

        double evaluate(const SoundState*) const noexcept override;
    };

    // Implements a strictly-bounded SoundNode
    template<typename SoundNodeType>
    class Bounded : public SoundNodeType {
    protected:
        virtual std::size_t getBoundFor(const StateType&) const noexcept = 0;

    private:
        bool isBounded() const noexcept override final;

        SoundNode::Bound getBound(const SoundState*) const noexcept override final;
    };

    // Implements a strictly-unbounded SoundNode
    template<typename SoundNodeType>
    class Unbounded : public SoundNodeType {
    private:
        bool isBounded() const noexcept override final;

        Bound getBound(const SoundState*) const noexcept final override;
    };

    // Implements a SoundNode that is conditionally bounded or unbounded
    template<typename SoundNodeType>
    class MaybeBounded : public SoundNodeType {
    protected:
        bool isCurrentlyBounded() const noexcept = 0;
        std::size_t getBoundFor(const StateType&) const noexcept = 0;

    private:
        bool isBounded() const noexcept override final;

        Bound getBound(const SoundState*) const noexcept final override;
    };

    // Convenient mix-in template for adding a CurrentTime SoundNumberSource
    template<typename SoundNodeType>
    class WithCurrentTime : public SoundNodeType {
    public:
        template<typename... Args>
        WithCurrentTime(Args&&...);

        CurrentTime currentTime;
    };

} // namespace flo

#include <Flosion/Core/SoundNode.tpp>
