#pragma once

#include <Flosion/Core/NodeBase.hpp>
#include <Flosion/Core/Reactable.hpp>
#include <Flosion/Core/StateBorrower.hpp>
#include <Flosion/Core/StateBorrower.hpp>

#include <atomic>
#include <cassert>

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
    class SoundNode;

    class NumberNode : public NodeBase<NumberNode> {
    public:
        NumberNode();
        virtual ~NumberNode() = default;

        bool canAddDependency(const NumberNode*) const noexcept;
        bool canRemoveDependency(const NumberNode*) const noexcept;

        // TODO: hide these
        void afterDependencyAdded(NumberNode*);
        void beforeDependencyRemoved(NumberNode*);

        SoundNode* getStateOwner() noexcept;
        const SoundNode* getStateOwner() const noexcept;

    protected:
        void setStateOwner(SoundNode*);

    private:
        Network* m_network;
        SoundNode* m_stateOwner;
    };

} // namespace flo
