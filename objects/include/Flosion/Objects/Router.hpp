#pragma once

#include <Flosion/Core/SoundSourceTemplate.hpp>
#include <Flosion/Core/SingleSoundInput.hpp>

namespace flo {

    class Router {
    public:
        Router();

        SingleSoundInput* getSoundInput();
        const SingleSoundInput* getSoundInput() const;

        NumberSource* addNumberSource();
        void removeNumberSource(const NumberSource*);
        size_t numNumberSources() const;
        NumberSource* getNumberSource(size_t n);
        const NumberSource* getNumberSource(size_t n) const;

        SoundSource* addSoundSource();
        void removeSoundSource(const SoundSource*);
        size_t numSoundSources() const;
        SoundSource* getSoundSource(size_t o);
        const SoundSource* getSoundSource(size_t o) const;

    
        NumberInput* getNumberInput(size_t n, size_t o);
        const NumberInput* getNumberInput(size_t n, size_t o) const;
    
        NumberInput* getNumberInput(const NumberSource*, const SoundSource*);
        const NumberInput* getNumberInput(const NumberSource*, const SoundSource*) const;


    private:
        class Output : public Realtime<ControlledSoundSource<EmptySoundState>> {
        public:
            Output(Router* parent);

            void addNumberInput(size_t n);
            void removeNumberInput(size_t n);
            NumberInput* getNumberInput(size_t which);
            const NumberInput* getNumberInput(size_t which) const;

        private:

            void renderNextChunk(SoundChunk& chunk, EmptySoundState* state) override;

            Router* const m_parent;

            std::vector<std::unique_ptr<SoundNumberInput>> m_numberInputs;
        };

        class Input : public SingleSoundInput {
        public:
            Input(Router* router);

            void addNumberSource(size_t n);
            void removeNumberSource(size_t n);
            size_t numNumberSources() const;

            NumberSource* getNumberSource(size_t which);
            const NumberSource* getNumberSource(size_t which) const;

            Router* getRouter();
            const Router* getRouter() const;

        private:
            class InputNumberSource : public SoundNumberSource<Input> {
            public:
                InputNumberSource(Input* parent);

                double evaluate(const StateType* state, const SoundState* context) const noexcept;

            private:
                const Input* m_parent;
                size_t m_numberSourceIdx;

                friend class Input;
            };

            Router* const m_router;

            std::vector<std::unique_ptr<InputNumberSource>> m_numberSources;
        };

        Input m_input;
        std::vector<std::unique_ptr<Output>> m_outputs;
    };

} // namespace flo
