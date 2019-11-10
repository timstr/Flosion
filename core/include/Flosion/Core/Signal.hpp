#pragma once

#include <functional>
#include <iostream>
#include <map>

namespace flo {

    class SignalBase;
    template<typename... Args>
    class Signal;

    // TODO: is there a convenient way to bundle many connections such
    // that they must be connected and reset simultaneously?

    /**
     * Connection represents a subscription to a signal.
     * A Connection may be connected or not connected to a signal.
     * When a signal is connected to with a handler function, a
     * Connection object is returned. When this object is destroyed,
     * the handler is removed. When the signal is destroyed, the
     * Connection object will be disconnected.
     */
    class Connection {
    public:
        Connection();
        Connection(Connection&&);
        Connection& operator=(Connection&&);
        ~Connection();

        Connection(const Connection&) = delete;
        Connection& operator=(const Connection&) = delete;

        void reset();

    private:
        SignalBase* m_signal;

        friend SignalBase;
        
        template<typename... Args>
        friend class Signal;
    };

    /**
     * SignalBase is an implementation detail. Look away.
     */
    class SignalBase {
    protected:
        virtual void moveConnection(Connection* from, Connection* to) = 0;
        virtual void removeConnection(Connection* c) = 0;
        
        friend Connection;
    };


    /**
     * Signal represents an event which can be subscribed to and broadcast from.
     * To subscribe to a Signal, pass a handler function of your choosing
     * (which accepts the correct arguments as determined by the Signal's
     * template parameters) to the connect() function, and store the returned
     * Connection for as long as you want to stay subscribed.
     * To notify all the subscribed Connection objects of an event, simply
     * call the broadcast() function with arguments of your choosing.
     */
    template<typename... Args>
    class Signal : private SignalBase {
    public:

        Signal();
        Signal(Signal&&);
        Signal& operator=(Signal&&);
        ~Signal();
        
        Signal(const Signal&) = delete;
        Signal& operator=(const Signal&) = delete;

        using Handler = std::function<void(Args...)>;

        Connection connect(Handler);
        
        void broadcast(Args...);

    private:
        void moveConnection(Connection* from, Connection* to) override;
        void removeConnection(Connection* c) override;

        std::map<Connection*, Handler> m_handlers;
    };

} // namespace flo

#include <Flosion/Core/Signal.tpp>
