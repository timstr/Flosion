#pragma once

#include <Immovable.hpp>
#include <SoundChunk.hpp>
#include <StateTable.hpp>

#include <optional>
#include <vector>

namespace flo {

    class SoundState;
    class Network;

    // TODO: add support for dynamically changing TimeBehavior
    // i.e. a Sampler may be 'Normal' until it is switched into
    // some interactive edit mode, when it becomes 'Uncontrolled'

    class ISoundNode;
    class OSoundNode;
    class IOSoundNode;

    // basic sound node type
    class SoundNode : private Immovable {
    public:
        SoundNode() noexcept = default;
        virtual ~SoundNode() noexcept = default;

        // For ISoundNode
        enum class Controllability : uint8_t {

            /**
             * A Controllable ISoundNode can have any number of states and
             * need not be called upon in realtime.
             */ 
            Controllable,

            /**
             * An Uncontrolled ISoundNode has exactly one state, and must
             * be called in realtime.
             */
            Uncontrolled
        };

        // For OSoundNode
        enum class Propagation : uint8_t {

            /**
             * A Singular OSoundNode propagates exactly one state
             * to each of its dependencies
             */
            Singular,

            /**
             * A Divergent OSoundNode propagates any number of states
             * to each of its dependencies
             */
            Divergent
        };

        // For IOSoundNode
        enum class TimeSync : uint8_t {

            /**
             * A Realtime IOSoundNode calls upon each of its inputs exactly
             * once for each of its propagated states. This means a Singular
             * Realtime node calls upon each input exactly once, and a Divergent
             * Realtime node with N propagated states per inbound state calls
             * upon each input N times.
             */
            Realtime,

            /**
             * An OutOfSync IOSoundNode may call upon each of its inputs
             * arbitrarily often and at arbitrary times.
             */
            OutOfSync
        };
        
        ISoundNode* toISoundNode() noexcept;
        OSoundNode* toOSoundNode() noexcept;
        IOSoundNode* toIOSoundNode() noexcept;
        virtual const ISoundNode* toISoundNode() const noexcept;
        virtual const OSoundNode* toOSoundNode() const noexcept;
        virtual const IOSoundNode* toIOSoundNode() const noexcept;

        // TODO: decorators for I?O?SoundNode

    private:
        Network* network;
    };

    // SoundNode that produces a sound stream
    class ISoundNode : virtual public SoundNode {
    public:

        ISoundNode(Controllability, std::unique_ptr<StateAllocator> mainStateAllocator);

        const std::vector<OSoundNode*>& getDirectDependents() const noexcept;

        Controllability getControllability() const noexcept;

        /**
         * Add, remove, and reset states for a dependent node and its given state.
         * States must be added to the state table as needed.
         * The state table will automatically propagate updates to dependencies.
         */
        virtual void addStateFor(const SoundNode* node, const SoundState* state) = 0;
        virtual void removeStateFor(const SoundNode* node, const SoundState* state) = 0;
        virtual void resetStateFor(const SoundNode* node, const SoundState* state) = 0;

        /**
         * Called when a dependent state moves location. This function should update all
         * of the nodes own states that map to that state to preserve the correct mapping.
         */
        virtual void onStateMoved(const SoundNode* node, const SoundState* from, const SoundState* to) = 0;

    protected:
        // TODO: how to use this most effectively in derived classes?
        StateTable stateTable;

    private:
        std::vector<OSoundNode*> m_dependents;
        Controllability m_controllability;

        const ISoundNode* toISoundNode() const noexcept override final;

        friend class OSoundNode;
    };

    // SoundNode that consumes sound streams
    class OSoundNode : virtual public SoundNode {
    public:

        OSoundNode(Propagation);
        
        bool canAddDependency(const ISoundNode*) const noexcept;
        void addDependency(ISoundNode* node);
        void removeDependency(ISoundNode* node);

        const std::vector<ISoundNode*>& getDirectDependencies() const noexcept;

        Propagation getPropagation() const noexcept;
        
        /**
         * Returns true if this node depends (directly or indirectly)
         * on the given node.
         */
        bool hasDependency(const ISoundNode*) const noexcept;

        bool hasUncontrolledDependency() const noexcept;
        
        /*
         * Returns Singular if there exists exactly one path to the dependency,
         * and all nodes along that path are Singular.
         * Otherwise, if there is more than one path, or any nodes along any a
         * path are Divergent, Divergent is returned.
         * Otherwise, if there is no path to the dependency, nothing is returned.
         */
        std::optional<Propagation> getPropagationTo(const ISoundNode* dependency) const noexcept;

        /*
         * Returns RealTime if all nodes along paths to the dependency are
         * RealTime.
         * Returns OutOfSync if any nodes along a path to the dependency are
         * OutOfSync.
         * Returns nothing if there is no path to the dependency.
         */
        std::optional<TimeSync> getTimeSyncTo(const ISoundNode* dependency) const noexcept;

    private:
        std::vector<ISoundNode*> m_dependencies;
        Propagation m_propagation;

        const OSoundNode* toOSoundNode() const noexcept override final;
    };

    // SoundNode that both consumes sound streams and produces a sound stream
    class IOSoundNode : public ISoundNode, public OSoundNode {
    public:
        
        IOSoundNode(Controllability, Propagation, TimeSync, std::unique_ptr<StateAllocator> mainStateAllocator);

        TimeSync getTimeSync() const noexcept;

        virtual double getTimeSpeed(const SoundState* context) const noexcept;

    private:
        TimeSync m_timeSync;

        const IOSoundNode* toIOSoundNode() const noexcept override final;
    };

#if 0

    // Basic unit of sound processing network
    // Base class to both sound input and soundsource
    // ...and maybe SoundResult?
    class DeprecatedSoundNode : protected Immovable {
    public:
        enum class TimeBehavior : uint8_t {

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

        enum class Propagation : uint8_t {
            /**
             * A singular soundnode creates exactly one state in each dependency
             * for each of its own states.
             */
            Singular,

            /**
             * A divergent soundnode may create any number of states in each
             * dependency for each of its own states.
             */
            Divergent
        };

        DeprecatedSoundNode(Network* network, TimeBehavior timeBehavior, Propagation statePropagation, std::unique_ptr<StateAllocator> allocator);
        ~DeprecatedSoundNode() = default;

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
         * States must be added to the state table as needed.
         * The state table will automatically propagate updates to dependencies.
         */
        virtual void addStateFor(const SoundNode* node, const SoundState* state) = 0;
        virtual void removeStateFor(const SoundNode* node, const SoundState* state) = 0;
        virtual void resetStateFor(const SoundNode* node, const SoundState* state) = 0;

    public:
        const StateTable& getStateTable() const noexcept;

    protected:
        StateTable& getStateTable() noexcept;

    public:

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
        bool isDivergent() const noexcept;

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
         * Returns whether or not the given sound node has exactly one state
         * for each state of this node.
         * It may be non-singular if there are any divergent nodes inbetween,
         * or if the sound network forks and recombines somewhere inbetween.
         */
        bool isSingularTo(const SoundNode*) const noexcept;

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
        Network* m_network;
        StateTable m_stateTable;
        std::vector<SoundNode*> m_dependents;
        std::vector<SoundNode*> m_dependencies;
        const TimeBehavior m_timeBehavior;
        const Propagation m_propagation;
        std::vector<std::unique_ptr<Decorator>> m_decorators;
    };

#endif
    
} // namespace flo
