#pragma once

#include <Flosion/Core/Immovable.hpp>
#include <Flosion/Core/NumberNode.hpp>

namespace flo {

    class NumberInput;

    // TODO: split this into multiple files

    // TODO: how can NumberSources be "simulated?"
    // I.e. how can a NumberSource be made to effectively return a different
    // value when called through the same network of SoundNodes when a client
    // requests it, without affecting the normal functioning of the network?

    class NumberSource : public NumberNode {
    public:
        virtual double evaluate(const SoundState* context) const noexcept = 0;
    };


    class Constant;

    class ConstantReactor : public Reactor<ConstantReactor, Constant> {
    public:
        virtual void onValueChanged(double value);
    };

    class Constant : public NumberSource, public Reactable<Constant, ConstantReactor> {
    public:
        Constant(double value = 0.0) noexcept;

        double getValue() const noexcept;
        void setValue(double) noexcept;

    private:
        std::atomic<double> m_value;
        double evaluate(const SoundState* context) const noexcept override final;
    };



    class NumberInput;

    class NumberInputReactor : public Reactor<NumberInputReactor, NumberInput> {
    public:
        virtual void onDefaultValueChanged(double value);
        virtual void afterSourceAdded(const NumberSource*);
        virtual void beforeSourceRemoved(const NumberSource*);
    };


    class NumberInput : public NumberNode, public Reactable<NumberInput, NumberInputReactor> {
    private:
        NumberInput(double defaultValue = 0.0) noexcept;
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
        Constant m_constant;
    };

    class NumberSourceInput : public NumberInput {
    public:
        NumberSourceInput(NumberSource* owner, double defaultValue = 0.0);
    };


} // namespace flo
