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

        // TODO: also provide access to the state owner of stateful number sources
        // so that dependencies can be figured out.
        // TODO: Consider replacing isStateless() with getStateOwner() or something similar
        virtual bool isStateless() const noexcept = 0;
    };

    class NumberSource : public NumberNode {
    public:
        using EvaluationFunction = double(*)(const NumberSource*, const SoundState*) noexcept;

        NumberSource(EvaluationFunction) noexcept;

        double evaluate(const SoundState* context) const noexcept;
        
    private:
        const EvaluationFunction m_evalFn;
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
        using DerivedEvaluationFunction = double (*)(const BorrowingNumberSource*, StateType*) noexcept;

        BorrowingNumberSource(DerivedEvaluationFunction) noexcept;

        // TODO

    private:
        const DerivedEvaluationFunction m_derivedEvalFn;

        bool isStateless() const noexcept override final;
        std::unique_ptr<StateAllocator> makeAllocater() const override final;

        static double findStateAndEvaluate(const NumberSource* self, SoundState* context) noexcept;
    };

    template<typename SoundNodeType>
    class SoundNumberSource : public NumberSource {
    public:
        using StateType = typename SoundNodeType::StateType;

        using DerivedEvaluationFunction = double (*)(const SoundNumberSource* self, const StateType* state) noexcept;

        SoundNumberSource(SoundNodeType* owner, DerivedEvaluationFunction) noexcept;

        // TODO

    private:
        const SoundNodeType* m_owner;
        const DerivedEvaluationFunction m_derivedEvalFn;

        bool isStateless() const noexcept override final;
        static double findStateAndEvaluate(const NumberSource* self, const SoundState* context) noexcept;
    };

    class ConstantNumberSource : public StatelessNumberSource {
    public:
        ConstantNumberSource() noexcept;
        ConstantNumberSource(double value) noexcept;

        double getValue() const noexcept;
        void setValue(double) noexcept;

    private:
        double m_value;
        static double compute(const NumberSource* self, const SoundState* context) noexcept;
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

        // TODO

    private:
        NumberSource* m_source;
        ConstantNumberSource m_constant;
        //bool isStateless() const noexcept override final;
    };

    class NumberSourceInput : public NumberInput {
    public:
        NumberSourceInput(NumberSource* owner);

    private:
        bool isStateless() const noexcept override final;
    };


    class SoundNumberInput : public NumberInput {
    public:
        SoundNumberInput(SoundNode* owner) noexcept;

    private:
        const SoundNode* m_owner;
        bool isStateless() const noexcept override final;
    };

    // TODO: move the definitions below this comment to their own .tpp file



    template<typename SoundNodeType>
    inline SoundNumberSource<SoundNodeType>::SoundNumberSource(SoundNodeType* owner, DerivedEvaluationFunction evalFn) noexcept
        : NumberSource(findStateAndEvaluate)
        , m_owner(owner)
        , m_derivedEvalFn(evalFn) {
    }

    template<typename SoundNodeType>
    inline bool SoundNumberSource<SoundNodeType>::isStateless() const noexcept {
        return false;
    }

    template<typename SoundNodeType>
    inline double SoundNumberSource<SoundNodeType>::findStateAndEvaluate(const NumberSource* self, const SoundState* context) noexcept {
        const auto selfDerived = reinterpret_cast<const SoundNumberSource*>(self);
        while (context){
            const auto owner = context->getOwner();
            if (owner == selfDerived->m_owner){
                return selfDerived->m_derivedEvalFn(selfDerived, reinterpret_cast<const StateType*>(context));
            }
            context = context->getDependentState();
        }
        assert(false);
        return 0.0;
    }

} // namespace flo
