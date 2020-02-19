#pragma once

#include <Flosion/Core/BorrowingNumberSource.hpp>
#include <Flosion/Core/NumberSource.hpp>
#include <Flosion/Core/Signal.hpp>
#include <Flosion/Core/SoundChunk.hpp>
#include <Flosion/Core/SoundNode.hpp>
#include <Flosion/Core/SoundState.hpp>

#include <vector>

namespace flo {

    class SoundInput;
    class SoundSource;

    struct SoundTraits {
        using Base = SoundNode;
        using InputType = SoundInput;
        using OutputType = SoundSource;
    };

    class SoundInput : public InputNodeBase<SoundTraits> {
    public:
        ~SoundInput();

        // TODO: make private
        const SoundInput* toSoundInput() const noexcept override final;
    };

    class SoundSource : public OutputNodeBase<SoundTraits> {
    public:
        ~SoundSource();

        // TODO: this can and should be made const
        /**
         * Produces the next chunk of sound. The SoundSource's state corresponding
         * to the given context may be updated.
         */
        virtual void getNextChunkFor(SoundChunk& chunk, const SoundInput* dependent, const SoundState* context) = 0;

        const SoundSource* toSoundSource() const noexcept override final;

        Signal<const BorrowingNumberSource*> onBorrowerAdded;
        Signal<const BorrowingNumberSource*> onBorrowerRemoved;

    private:
        std::vector<BorrowingNumberSource*> m_borrowers;

        friend BorrowingNumberSource;
    };
    
    
    template<typename SoundNodeType>
    class SoundNumberSource : public NumberSource {
    public:
        SoundNumberSource(SoundNodeType* owner) noexcept;

        using StateType = typename SoundNodeType::StateType;

        virtual double evaluate(const StateType* state, const SoundState* context) const noexcept = 0;

        SoundNodeType* getOwner() noexcept;
        const SoundNodeType* getOwner() const noexcept;

    private:
        double evaluate(const SoundState* context) const noexcept override;
    };


    class SoundNumberInput : public NumberInput {
    public:
        SoundNumberInput(SoundNode* owner, double defaultValue = 0.0) noexcept;
    };

} // namespace flo

#include <Flosion/Core/SoundSource.tpp>
