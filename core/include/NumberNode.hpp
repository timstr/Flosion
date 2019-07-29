#pragma once

#include <Immovable.hpp>
#include <StateBorrower.hpp>

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

        virtual bool isStateless() const noexcept = 0;
    };

    class NumberSource : public NumberNode {
        using EvaluationFunction = double(*)(const NumberNode*, const SoundState*) noexcept;

        NumberSource(EvaluationFunction) noexcept;

        void evaluate(const SoundState* context) const noexcept;
        
    private:
        EvaluationFunction m_evalFn;
    };

    class NumberInput : public NumberNode {
        double getValue(const SoundState* context) const noexcept;

        void setSource(NumberSource*) noexcept;
        
        NumberSource* getSource() noexcept;
        const NumberSource* getSource() const noexcept;

        // TODO

    private:
        NumberSource* m_source;
        bool isStateless() const noexcept override final;
    };

    class StatelessNumberSource : public NumberSource {
    public:
        using NumberSource::NumberSource;

        // TODO

    private:
        bool isStateless() const noexcept override final;
    };

    template<typename StateType>
    class BorrowingNumberSource : public NumberSource, public StateBorrower {
    public:
        using EvaluationFunction = double (*)(const BorrowingNumberSource*, StateType*) noexcept;

        BorrowingNumberSource(EvaluationFunction) noexcept;

        // TODO

    private:
        bool isStateless() const noexcept override final;
        std::unique_ptr<StateAllocator> makeAllocater() const override final;

        static double findStateAndEvaluate(const BorrowingNumberSource* self, SoundState* context) noexcept;
    };

    template<typename SoundNodeType>
    class SoundNumberSource : public NumberSource {
    public:
        using StateType = typename SoundNodeType::StateType;

        using EvaluationFunction = double (*)(const SoundNumberSource* self, StateType* state) noexcept;

        SoundNumberSource(SoundNodeType* parent, EvaluationFunction) noexcept;

        // TODO

    private:
        bool isStateless() const noexcept override final;
        static double findStateAndEvaluate(const SoundNumberSource* self, SoundState* context) noexcept;
    };

} // namespace flo
