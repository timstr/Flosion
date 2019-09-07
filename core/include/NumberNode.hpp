#pragma once

#include <Immovable.hpp>
#include <StateBorrower.hpp>

#include <atomic>
#include <cassert>
#include <set>
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
    class SoundNode;

    class NumberNode : private Immovable {
    public:
        NumberNode();
        virtual ~NumberNode();

        bool canAddDependency(const NumberNode*) const noexcept;
        void addDependency(NumberNode* node);
        void removeDependency(NumberNode* node);

        const std::vector<NumberNode*>& getDirectDependencies() const noexcept;
        const std::vector<NumberNode*>& getDirectDependents() const noexcept;

        std::set<const NumberNode*> getAllDependencies() const noexcept;
        std::set<const NumberNode*> getAllDependents() const noexcept;

        /**
         * Returns true if this node depends (directly or indirectly)
         * on the given node.
         */
        bool hasDependency(const NumberNode*) const noexcept;

        SoundNode* getStateOwner() noexcept;
        const SoundNode* getStateOwner() const noexcept;

    protected:
        void setStateOwner(SoundNode*);

    private:
        Network* m_network;
        std::vector<NumberNode*> m_dependents;
        std::vector<NumberNode*> m_dependencies;
        SoundNode* m_stateOwner;
    };

} // namespace flo
