#pragma once

#include <Immovable.hpp>
#include <StateBorrower.hpp>

#include <atomic>
#include <cassert>
#include <vector>

namespace flo {

    /*
    Types of NumberNodes:
        Stateful:
        - These have a SoundNode to which their state belongs
            Part of a soundnode:
                - The dependent NumberNode calling upon this node must must have access to the
                  soundnode, i.e. it cannot be global, and it must be for a sound node that is
                  a dependent of this node's sound node
                - Examples:
                    sound source's current time, note frequency, wave generator's phase, filter's frequency
            Borrows from soundnode:
                - As above, state must be accessible to caller. Same rules.
                - Examples:
                    accumulator, random walk, smoothing function
        Stateless (constants or pure functions)
        - This node itself needs no state, but any stateful dependencies must have access
          to the state they need in all stateful dependents (stateless dependents don't matter,
          i.e. a stateful number node connected through a pure function to nothing at all is fine).
        - Examples:
            All number inputs!
            plus, sin, spline, 2, bell curve

    */

    class Network;

    class NumberNode : private Immovable {
    public:
        NumberNode() noexcept;

        bool canAddDependency(const NumberNode*) const noexcept;
        void addDependency(NumberNode* node);
        void removeDependency(NumberNode* node);

        const std::vector<NumberNode*>& getDirectDependencies() const noexcept;
        const std::vector<NumberNode*>& getDirectDependents() const noexcept;

        /**
         * Returns true if this node depends (directly or indirectly)
         * on the given node.
         */
        bool hasDependency(const NumberNode*) const noexcept;

    private:
        Network* m_network;
        std::vector<NumberNode*> m_dependents;
        std::vector<NumberNode*> m_dependencies;

        virtual const SoundNode* getStateOwner() const noexcept = 0;
    };

    // TODO: how can NumberSources be "simulated?"
    // I.e. how can a NumberSource be made to effectively return a different
    // value when called through the same network of SoundNodes when a client
    // requests it, without affecting the normal functioning of the network?
    class NumberSource : public NumberNode {
    public:
        virtual double evaluate(const SoundState* context) const noexcept = 0;
    };

    class StatelessNumberSource : public NumberSource {
    private:
        const SoundNode* getStateOwner() const noexcept override final;
    };

    template<typename StateType>
    class BorrowingNumberSource : public NumberSource, public StateBorrower {
    public:

        virtual double evaluate(StateType* state, const SoundState* context) const noexcept = 0;

    private:
        double evaluate(const SoundState* context) const noexcept override final;

        const SoundNode* getStateOwner() const noexcept override final;
        std::unique_ptr<StateAllocator> makeAllocater() const override final;
    };

    template<typename SoundNodeType>
    class SoundNumberSource : public NumberSource {
    public:
        SoundNumberSource(SoundNodeType* owner) noexcept;

        using StateType = typename SoundNodeType::StateType;

        virtual double evaluate(const StateType* state, const SoundState* context) const noexcept = 0;

        SoundNodeType* getOwner() noexcept;
        const SoundNodeType* getOwner() const noexcept;

    private:
        const SoundNodeType* m_owner;

        const SoundNode* getStateOwner() const noexcept override final;

        double evaluate(const SoundState* context) const noexcept override;
    };

    class ConstantNumberSource : public StatelessNumberSource {
    public:
        ConstantNumberSource(double value = 0.0) noexcept;

        double getValue() const noexcept;
        void setValue(double) noexcept;

    private:
        std::atomic<double> m_value;
        double evaluate(const SoundState* context) const noexcept override final;
    };

    class NumberInput : public NumberNode {
    private:
        NumberInput() noexcept;
        friend class NumberSourceInput;
        friend class SoundNumberInput;
    public:

        double getValue(const SoundState* context) const noexcept;

        double getDefaultValue() const noexcept;
        void setDefaultValue(double) noexcept;

        void setSource(NumberSource*) noexcept;
        
        NumberSource* getSource() noexcept;
        const NumberSource* getSource() const noexcept;

    private:
        NumberSource* m_source;
        ConstantNumberSource m_constant;
    };

    class NumberSourceInput : public NumberInput {
    public:
        NumberSourceInput(NumberSource* owner);

    private:
        const SoundNode* getStateOwner() const noexcept override final;
    };


    class SoundNumberInput : public NumberInput {
    public:
        SoundNumberInput(SoundNode* owner) noexcept;

    private:
        const SoundNode* m_owner;
        const SoundNode* getStateOwner() const noexcept override final;
    };

    class SoundResult;

    class NumberResult : public flo::NumberNode {
    private:
        std::unique_ptr<SoundResult> m_hiddenNode;

    public:
        NumberResult() noexcept;

        double getValue() const noexcept;

        SoundNumberInput input;

    private:
        const SoundNode* getStateOwner() const noexcept override final;
    };



    class CurrentTime : public NumberSource {
    public:
        CurrentTime(SoundNode* owner) noexcept;

    private:
        SoundNode* const m_owner;

        double evaluate(const SoundState*) const noexcept override;

        const SoundNode* getStateOwner() const noexcept override final;
    };

    // TODO: move the definitions below this comment to their own .tpp file


    template<typename SoundNodeType>
    inline SoundNumberSource<SoundNodeType>::SoundNumberSource(SoundNodeType* owner) noexcept
        : m_owner(owner) {
    }

    template<typename SoundNodeType>
    inline SoundNodeType* SoundNumberSource<SoundNodeType>::getOwner() noexcept {
        return m_owner;
    }

    template<typename SoundNodeType>
    inline const SoundNodeType* SoundNumberSource<SoundNodeType>::getOwner() const noexcept {
        return m_owner;
    }

    template<typename SoundNodeType>
    inline const SoundNode* SoundNumberSource<SoundNodeType>::getStateOwner() const noexcept {
        return m_owner;
    }

    template<typename SoundNodeType>
    inline double SoundNumberSource<SoundNodeType>::evaluate(const SoundState* context) const noexcept {
        auto curr = context;
        while (curr){
            const auto owner = curr->getOwner();
            if (owner == m_owner){
                return evaluate(reinterpret_cast<const StateType*>(curr), context);
            }
            curr = curr->getDependentState();
        }
        assert(false);
        return 0.0;
    }

    template<typename StateType>
    inline double BorrowingNumberSource<StateType>::evaluate(const SoundState* context) const noexcept {
        if (auto lender = getStateLender()){
            return evaluate(reinterpret_cast<StateType*>(lender->getBorrowedState(context, this)), context);
        }
        return 0.0;
    }

    template<typename StateType>
    inline const SoundNode * BorrowingNumberSource<StateType>::getStateOwner() const noexcept {
        return getStateLender();
    }

    template<typename StateType>
    inline std::unique_ptr<StateAllocator> BorrowingNumberSource<StateType>::makeAllocater() const {
        return std::make_unique<ConcreteStateAllocator<StateType>>();
    }

} // namespace flo
