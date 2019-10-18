#pragma once

#include <Flosion/Core/NumberSource.hpp>
#include <Flosion/Core/SoundChunk.hpp>
#include <Flosion/Core/SoundNode.hpp>
#include <Flosion/Core/SoundState.hpp>
#include <Flosion/Core/SoundInput.hpp>

#include <vector>

namespace flo {

    class SoundSource : public SoundNode {
    public:

        /**
         * Produces the next chunk of sound. The SoundSource's state corresponding
         * to the given context may be updated.
         */
        virtual void getNextChunkFor(SoundChunk& chunk, const SoundInput* dependent, const SoundState* context) = 0;


        //class Decorator : public SoundNode::Decorator {
            // TODO: ???
        //};
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