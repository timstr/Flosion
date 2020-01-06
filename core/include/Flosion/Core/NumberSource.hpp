#pragma once

#include <Flosion/Core/Immovable.hpp>
#include <Flosion/Core/NumberNode.hpp>
#include <Flosion/Core/Signal.hpp>

#include <mutex>

namespace flo {

    // TODO: split this into multiple files

    // TODO: how can NumberSources be "simulated?"
    // I.e. how can a NumberSource be made to effectively return a different
    // value when called through the same network of SoundNodes when a client
    // requests it, without affecting the normal functioning of the network?

    class NumberInput;
    class NumberSource;

    struct NumberTraits {
        using Base = NumberNode;
        using InputType = NumberInput;
        using OutputType = NumberSource;
    };


    class NumberInput : public InputNodeBase<NumberTraits> {
    public:
        double getValue(const SoundState* context) const noexcept;

        double getDefaultValue() const noexcept;
        void setDefaultValue(double) noexcept;

        Signal<double> onDefaultValueChanged;

        // TODO: get a real lock
        std::lock_guard<std::mutex> acquireLock();

    private:
        NumberInput(double defaultValue = 0.0) noexcept;

        double m_defaultValue;

        // TODO: remove
        std::mutex m_uselessMutexPleaseRemove;
        
        const NumberInput* toNumberInput() const noexcept override final;
        const NumberSource* toNumberSource() const noexcept override final;

        friend class NumberSourceInput;
        friend class SoundNumberInput;
    };


    class NumberSource : public OutputNodeBase<NumberTraits> {
    public:
        virtual double evaluate(const SoundState* context) const noexcept = 0;

    private:
        const NumberInput* toNumberInput() const noexcept override final;
        const NumberSource* toNumberSource() const noexcept override final;
    };

    class Constant : public NumberSource {
    public:
        Constant(double value = 0.0) noexcept;

        double getValue() const noexcept;
        void setValue(double) noexcept;

        Signal<double> onChangeValue;

    private:

        std::atomic<double> m_value;

        double evaluate(const SoundState* context) const noexcept override final;
    };

    class NumberSourceInput : public NumberInput {
    public:
        NumberSourceInput(NumberSource* owner, double defaultValue = 0.0);
    };


} // namespace flo
