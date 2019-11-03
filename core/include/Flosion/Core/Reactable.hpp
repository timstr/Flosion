#pragma once

#include <Flosion/Core/Immovable.hpp>

#include <algorithm>
#include <cassert>
#include <functional>
#include <type_traits>
#include <utility>
#include <vector>

namespace flo {

    // TODO: use static_cast<Derived*> instead of dynamic_cast where possible

    // NOTE: it's possible to make Reactor and Reactable moveable,
    // but for now they're simply Immovable for simplicity

    // Forward declarations
    template<typename Self, typename Target>
    class Reactor;

    template<typename Self, typename ReactorType>
    class Reactable;

    // CRTP-mix-in base class for a subscriber to events from some object.
    // Derive from this class to define a custom reactor.
    // Pass both the derived type and the type of the target
    // object as template parameters. Then, define virtual
    // functions to deal with changes however you please.
    //
    // class MyReactor : public Reactor<Self, Target> {
    // public:
    //     virtual void whenThisHappens(int data) = 0;
    //     virtual void afterThingAdded(const Thing& which) = 0;
    //     virtual void beforeThingRemoved(const Thing& which) = 0;
    // }
    template<typename Self, typename Target>
    class Reactor : private Immovable {
    public:
        using TargetType = Target;

    protected:
        Reactor() noexcept;
        virtual ~Reactor() noexcept;

        const Target& target() const noexcept;

    private:
        Reactable<Target, Self>* m_target;
        
        friend class Reactable<Target, Self>;
    };

    // CRTP-mix-in base class for an object to which Reactors
    // can subscribe. To derive from this class, pass
    // both the derived type and the type of the custom
    // reactor class which you've created to handle changes.
    // Reactors can be subscribed with attachReactor(),
    // and unsubscribed with detachReactor(). Note that reactors
    // automatically detach themselves upon their destruction,
    // and that all reactors are detached when the Reactable
    // is destroyed.
    // Call notifyReactors() anytime you wish to notify the attached
    // reactors of a change. Example:
    //
    // void addThing(int param1, std::string param2){
    //     m_thingVector.push_back(Thing{param1, std::move(param2)});
    //     notifyReactors(&MyReactor::afterThingAdded, m_thingVector.back());
    // }
    template<typename Self, typename ReactorType>
    class Reactable : private Immovable {
    public:
        virtual ~Reactable();
    
        void attachReactor(ReactorType* reactor);
        void detachReactor(const ReactorType* reactor);
        bool hasReactor(const ReactorType* reactor) const noexcept;

        template<typename... Args>
        void notifyReactors(void (ReactorType::*function)(Args...), Args... args) const;

    private:

        std::vector<ReactorType*> m_reactors;
    };

    // TODO: move the following definitions to a .tpp file

    template<typename Self, typename Target>
    inline Reactor<Self, Target>::Reactor() noexcept
        : m_target(nullptr) {

    }

    template<typename Self, typename Target>
    inline Reactor<Self, Target>::~Reactor() noexcept {
        if (m_target){
            auto t = dynamic_cast<Target*>(m_target);
            auto s = dynamic_cast<Self*>(this);
            assert(t);
            assert(s);
            t->Reactable<Target, Self>::detachReactor(s);
        }
    }

    template<typename Self, typename Target>
    inline const Target& Reactor<Self, Target>::target() const noexcept {
        assert(m_target);
        return *static_cast<Target*>(m_target);
    }

    template<typename Self, typename ReactorType>
    inline Reactable<Self, ReactorType>::~Reactable(){
        while (m_reactors.size() > 0){
            detachReactor(m_reactors.back());
        }
    }

    template<typename Self, typename ReactorType>
    inline void Reactable<Self, ReactorType>::attachReactor(ReactorType* reactor){
        assert(!hasReactor(reactor));
        assert(reactor->m_target == nullptr);
        m_reactors.push_back(reactor);
        reactor->m_target = this;
    }

    template<typename Self, typename ReactorType>
    inline void Reactable<Self, ReactorType>::detachReactor(const ReactorType* reactor){
        assert(hasReactor(reactor));
        assert(reactor->m_target == this);
        auto it = std::find(m_reactors.begin(), m_reactors.end(), reactor);
        assert(it != m_reactors.end());
        (*it)->m_target = nullptr;
        m_reactors.erase(it);
    }

    template<typename Self, typename ReactorType>
    inline bool Reactable<Self, ReactorType>::hasReactor(const ReactorType* reactor) const noexcept {
        return std::find(m_reactors.begin(), m_reactors.end(), reactor) != m_reactors.end();
    }

    template<typename Self, typename ReactorType>
    template<typename... Args>
    inline void Reactable<Self, ReactorType>::notifyReactors(void(ReactorType::* function)(Args...), Args ...args) const {
        for (auto& r : m_reactors){
            (r->*function)(args...);
        }
    }

} // namespace flo
