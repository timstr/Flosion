#pragma once

#include <Immovable.hpp>
#include <SoundChunk.hpp>

#include <vector>

namespace flo {

    class SoundState;
    class SoundNode;
    class SoundNetwork;
    class StateTable;

    class SoundNetwork : private Immovable {
    public:

        std::vector<SoundNode*> getAllNodes() noexcept;
        const std::vector<SoundNode*> getAllNodes() const noexcept;

        // TODO: ???

    private:
        std::vector<SoundNode*> m_nodes;
    };

    /*
    Examples:
        Normal sound nodes:
        - many SoundSources (e.g. sampler, time stretch, phase vocoder)
        Realtime sound nodes:
        - sound inputs (single and multi)
        - many SoundSource (e.g. mixer, filter, distortion)
        Uncontrolled sound nodes:
        - some SoundSources (e.g. live keyboard, live mic input, network stream)
        - DAC


    */

    // Basic unit of sound processing network
    // Base class to both sound input and soundsource
    // ...and maybe SoundResult?
    class SoundNode : protected Immovable {
    public:
        enum class Type : uint8_t {

            /**
             *A normal soundnode can have any number of states, may
             * call upon its dependents whenever it chooses, and may change
             * the speed of time.
             */
            Normal,

            /**
             * A realtime soundnode may have any number of states, calls upon
             * its dependents once per chunk, and may not change the speed of time.
             */
            RealTime,

            /**
             * An uncontrolled soundnode has exactly one state at all times.
             * Uncontrolled also implies realtime.
             */
            Uncontrolled
        };

        SoundNode(SoundNetwork* network, Type type);
        ~SoundNode() = default;

        // NOTE: this interface is meant for getNextChunk(...), but its arguments vary.
        // For example, SingleSoundInputs need only the parent state, but SoundSources
        // also need the dependent node (destination SoundInput), and MultiSoundInputs
        // need a key as well.
        
        bool canAddDependency(const SoundNode*) const noexcept;
        void addDependency(SoundNode* node);
        void removeDependency(SoundNode* node);

        const std::vector<SoundNode*>& getDirectDependencies() const noexcept;
        const std::vector<SoundNode*>& getDirectDependents() const noexcept;
        
        /**
         * Returns true if this node depends (directly or indirectly)
         * on the given node.
         */
        bool hasDependency(const SoundNode*) const noexcept;

        /**
         * Add, remove, and reset states for a dependent node and its given state.
         * Changes must be propagated to dependencies, but this implementation is
         * left open to allow for nodes with more than one state per input state.
         * The node must already be a dependent.
         */
        virtual void addStateFor(const SoundNode* node, const SoundState* state) = 0;
        virtual void removeStateFor(const SoundNode* node, const SoundState* state) = 0;
        virtual void resetStateFor(const SoundNode* node, const SoundState* state) = 0;

        /**
         * Called when a dependent state moves location. This function should update all
         * of the nodes own states that map to that state to preserve the correct mapping.
         */
        virtual void onStateMoved(const SoundNode* node, const SoundState* from, const SoundState* to) = 0;

        /**
         * If the dynamic type of the node is a StateTable, a pointer to that table
         * is returned. This is used for borrowing state and for easy computation
         * of dependency state mapping.
         */
        virtual StateTable* toStateTable() noexcept;

        /**
         * The total number of states owned by the node.
         * Effectively returns the sum of all dependent states times this node's
         * multiplicity.
         * If this node is uncontrolled, 1 is always returned.
         */
        size_t numStates() const noexcept;

        /**
         * Returns the number of states per each dependent state.
         * The default (non-overridden) behavior is to always return 1.
         */
        virtual size_t getMultiplicity() const noexcept;

        /**
         * Returns true if the node is realtime, which is to say that it calls on its
         * dependents exactly once per chunk.
         */
        bool isRealTime() const noexcept;

        /*
         * Retrieves the current (relative to the given context) speed of time.
         * The default (non-overridden) behavior is to always return 1.
         * For realtime SoundNodes, this MUST always be 1.
         */
        virtual double getTimeSpeed(const SoundState* context) const noexcept;

        /**
         * Returns true if the node is uncontrolled, which is to say that its state
         * is not managed by the sound graph. It effectively has exactly one state
         * at all times, and it is an error to add more than one dependent state.
         * NOTE: this function is recursive. All dependents of an uncontrolled node
         * are transitively uncontrolled as well.
         */
        bool isUncontrolled() const noexcept;
        
        /*
         * Returns the maximum number of times that a given SoundSource
         * may be queried for a single chunk of processing from this
         * input.
         * Returns 0 if the SoundNode is not a dependency of this node
         * If the SoundNode is uncontrolled, it is only safe to use it as input
         * if the multiplicity is 1.
         */
        size_t getMultiplicityTo(const SoundNode*) const noexcept;

        /*
         * Returns true if all SoundNodes between this and the given node are
         * realtime.
         * Returns false if the given SoundNode is not a dependency of this
         */
        bool isRealTimeTo(const SoundNode*) const noexcept;

    public:

        
        class Decorator {
        public:
            Decorator() noexcept;
            virtual ~Decorator() noexcept;
            
            virtual void afterDependencyAdded(const SoundNode*);
            virtual void beforeDependencyRemoved(const SoundNode*);

            virtual void afterDependentAdded(const SoundNode*);
            virtual void beforeDependentRemoved(const SoundNode*);

            SoundNode& self() noexcept;
            const SoundNode& self() const noexcept;

        private:
            SoundNode* m_selfPTr;
        };

        void addDecorator(std::unique_ptr<Decorator>);
        void removeDecorator(const Decorator*);
        bool hasDecorator(const Decorator*) const noexcept;

    private:
        SoundNetwork* m_network;
        std::vector<SoundNode*> m_dependents;
        std::vector<SoundNode*> m_dependencies;
        const Type m_type;
        std::vector<std::unique_ptr<Decorator>> m_decorators;
    };

} // namespace flo
