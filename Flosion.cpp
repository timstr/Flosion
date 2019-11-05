// TODO: make it easier to use SFML
// consider adding SFML as a submodule (directly or in tims-gui?)
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <Flosion/Core/Network.hpp>
#include <Flosion/Core/SoundResult.hpp>
#include <Flosion/Core/SoundState.hpp>
#include <Flosion/Core/SoundSourceTemplate.hpp>
#include <Flosion/Core/MultiSoundInput.hpp>
#include <Flosion/Core/NumberSource.hpp>
#include <Flosion/Core/BorrowingNumberSource.hpp>

#include <iostream>
#include <random>

#include <Flosion/UI/Core/FlosionUI.hpp>

//#include <Flosion/UI/Objects/Functions.hpp>

// TODO: include and dynamically link to ffmpeg for additional audio formats?

// TODO: add graphs back (in a safe and clean way)

// TODO: safeguard number sources from deletion while they are being used
// (i.e. do some kind of locking to prevent them from being deleted
// during their use in sound processing)

// UI-related things:
// TODO: decide on a user interface for borrowing number sources
// - wire link between borrower and its lender?
// - Borrowers get placed into a box inside/alongside the lender?
// - 
// TODO: remove duplication between sound/number pegs/wires
// TODO: add posRelativeTo(const Element* ancestor) to ui
// TODO: when a connected head is moused over, show the peg's label
// TODO: serialization/deserialization for objects and wires
// TODO: cut/copy/paste
// TODO: load/save project files (.flo extension???)
// TODO: Collapsible panels
// TODO: obstacle-avoiding wires
// TODO: hand-written math expressions
// TODO: record sound streams into an audioclip
// TODO: put wires in front of objects always

// TODO: audioclip
// - one-shot vs loop
// - start time
// - wave visualization and editing
// - number inputs for current time OR current speed
// TODO: bandpass filter
// TODO: IDCT
// TODO: IFFT
// TODO: IDWT
// TODO: how to implement vocoder (i.e. synth filtered by human voice) without breaking scope?
// TODO: custom waveform
// TODO: spline (with points that can be moved by numberinputs)
// TODO: cubic spline
// TODO: bezier spline
// TODO: gaussian radial basis function
// TODO: linear smoother (borrower; takes constant step size)
// TODO: exponential smoother (borrower; takes step size proportional to difference)
// TODO: random walk (borrower)
// TODO: oscillator (borrower; takes speed as input and outputs phase in range [0,1])
// TODO: accumulator (borrower)
// TODO: variable (borrower; when called, stores current input and returns previous value)

// TODO: put all the following lovely objects into their own files in the flosion_objects target

std::random_device ranDev;
auto ranEng = std::default_random_engine{ranDev()};


class SmootherState : public flo::State {
public:
    void reset() noexcept override {
        value = 0.0;
    }

    double value{};
};
class Smoother : public flo::BorrowingNumberSourceTemplate<SmootherState> {
public:
    Smoother() : input(this) {
    
    }

    flo::NumberSourceInput input;

private:
    double evaluate(SmootherState* state, const flo::SoundState* context) const noexcept override {
        auto v = input.getValue(context);
        state->value += 1.0 * (v - state->value);
        return state->value;
    }
};

class RandomWalkState : public flo::State {
public:

    void reset() noexcept override {
        value = 0.0;
        velocity = 0.0;
    }

    double value;
    double velocity;
};

class RandomWalk : public flo::BorrowingNumberSourceTemplate<RandomWalkState> {
public:
    RandomWalk()
        : speed(this, 0.001)
        , damping(this, 0.5)
        , bias(this, 0.1) {
        
    }

    flo::NumberSourceInput speed;
    flo::NumberSourceInput damping;
    flo::NumberSourceInput bias;

private:
    double evaluate(RandomWalkState* state, const flo::SoundState* context) const noexcept override {
        const auto sp = speed.getValue(context);
        const auto d = damping.getValue(context);
        const auto b = bias.getValue(context);
        auto dist = std::uniform_real_distribution<double>{-1.0, 1.0};
        auto deltaVelocity = (dist(ranEng) - state->value * b) * sp;
        state->velocity = (state->velocity * d) + deltaVelocity;
        state->value += state->velocity;
        return state->value;
    }
};





class MixerState : public flo::SoundState {
public:
    using SoundState::SoundState;

    void reset() noexcept override {
        buffer.silence();
    }

    flo::SoundChunk buffer;
};

class Router {
public:
    Router() : m_input(this) {
        
    }


    void addNumberSource(size_t n){
        m_input.addNumberSource(n);
        for (auto& o : m_outputs){
            o->addNumberInput(n);
            //m_input.getNumberSource(n)->addDependency(o->getNumberInput(n));
        }
    }
    void removeNumberSource(size_t n){
        for (auto& o : m_outputs){
            o->removeNumberInput(n);
        }
        m_input.removeNumberSource(n);
    }
    size_t numNumberSources() const {
        return m_input.numNumberSources();
    }

    void addSoundSource(size_t o){
        assert(o <= m_outputs.size());
        auto ss = std::make_unique<Output>(this);
        ss->addDependency(&m_input);
        for (size_t i = 0, iEnd = m_input.numNumberSources(); i != iEnd; ++i){
            ss->addNumberInput(i);
            //m_input.getNumberSource(i)->addDependency(ss->getNumberInput(i));
        }
        m_outputs.insert(m_outputs.begin() + o, std::move(ss));
    }
    void removeSoundSource(size_t o){
        assert(o < m_outputs.size());
        auto it = m_outputs.begin() + o;
        const auto& ss = *it;
        ss->removeDependency(&m_input);
        m_outputs.erase(it);
    }
    size_t numSoundSources() const {
        return m_outputs.size();
    }

    flo::NumberSource* getNumberSource(size_t n){
        return m_input.getNumberSource(n);
    }
    const flo::NumberSource* getNumberSource(size_t n) const {
        return m_input.getNumberSource(n);
    }
    
    flo::NumberInput* getNumberInput(size_t n, size_t o){
        assert(o < numSoundSources());
        return m_outputs[o]->getNumberInput(n);
    }
    const flo::NumberInput* getNumberInput(size_t n, size_t o) const {
        assert(o < numSoundSources());
        return m_outputs[o]->getNumberInput(n);
    }

    flo::SoundSource* getSoundSource(size_t o){
        assert(o < numSoundSources());
        return m_outputs[o].get();
    }
    const flo::SoundSource* getSoundSource(size_t o) const {
        assert(o < numSoundSources());
        return m_outputs[o].get();
    }

    flo::SingleSoundInput* getSoundInput(){
        return &m_input;
    }
    const flo::SingleSoundInput* getSoundInput() const {
        return &m_input;
    }

private:
    class Output : public flo::Realtime<flo::ControlledSoundSource<flo::EmptySoundState>> {
    public:
        Output(Router* parent) : m_parent(parent) {

        }

        void addNumberInput(size_t where){
            assert(where <= m_numberInputs.size());
            auto ni = std::make_unique<flo::SoundNumberInput>(this);
            //m_parent->getNumberSource(where)->addDependency(ni.get());
            m_numberInputs.insert(m_numberInputs.begin() + where, std::move(ni));
        }
        void removeNumberInput(size_t where){
            assert(where < m_numberInputs.size());
            auto it = m_numberInputs.begin() + where;
            //m_parent->getNumberSource(where)->removeDependency(it->get());
            m_numberInputs.erase(it);
        }
        flo::NumberInput* getNumberInput(size_t which){
            assert(which < m_numberInputs.size());
            return m_numberInputs[which].get();
        }
        const flo::NumberInput* getNumberInput(size_t which) const {
            assert(which < m_numberInputs.size());
            return m_numberInputs[which].get();
        }

    private:

        void renderNextChunk(flo::SoundChunk& chunk, flo::EmptySoundState* state) override {
            m_parent->m_input.getNextChunkFor(chunk, this, state);
        }

        Router* const m_parent;

        std::vector<std::unique_ptr<flo::SoundNumberInput>> m_numberInputs;
    };

    class Input : public flo::SingleSoundInput {
    public:
        Input(Router* router) : m_router(router) {
        
        }

        void addNumberSource(size_t where){
            assert(where <= m_numberSources.size());
            auto ns = std::make_unique<InputNumberSource>(this);
            m_numberSources.insert(m_numberSources.begin() + where, std::move(ns));
            for (size_t i = 0, iEnd = m_numberSources.size(); i != iEnd; ++i){
                m_numberSources[i]->m_numberSourceIdx = i;
            }
        }
        void removeNumberSource(size_t where){
            assert(where < m_numberSources.size());
            m_numberSources.erase(m_numberSources.begin() + where);
            for (size_t i = 0, iEnd = m_numberSources.size(); i != iEnd; ++i){
                m_numberSources[i]->m_numberSourceIdx = i;
            }
        }
        size_t numNumberSources() const {
            return m_numberSources.size();
        }

        flo::NumberSource* getNumberSource(size_t which){
            assert(which < m_numberSources.size());
            return m_numberSources[which].get();
        }
        const flo::NumberSource* getNumberSource(size_t which) const {
            assert(which < m_numberSources.size());
            return m_numberSources[which].get();
        }

        Router* getRouter(){
            return m_router;
        }
        const Router* getRouter() const {
            return m_router;
        }

    private:
        class InputNumberSource : public flo::SoundNumberSource<Input> {
        public:
            InputNumberSource(Input* parent)
                : SoundNumberSource(parent)
                , m_parent(parent)
                , m_numberSourceIdx(-1) {
                
            }

            double evaluate(const StateType* state, const flo::SoundState* context) const noexcept override final {
                auto ds = state->getDependentState();
                assert(ds);
                auto router = m_parent->getRouter();
                const auto& outputs = router->m_outputs;
                const auto it = std::find_if(
                    outputs.begin(),
                    outputs.end(),
                    [&](const std::unique_ptr<Output>& op){
                        return ds->getOwner() == op.get();
                    }
                );
                assert(it != outputs.end());
                return (*it)->getNumberInput(m_numberSourceIdx)->getValue(context);
            }

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

class Mixer : public flo::Realtime<flo::ControlledSoundSource<MixerState>> {
public:

    void renderNextChunk(flo::SoundChunk& chunk, MixerState* state) override {
        chunk.silence();

        for (auto& input : m_inputs){
            input->getNextChunkFor(state->buffer, this, state);
            for (size_t i = 0; i < flo::SoundChunk::size; ++i){
                chunk[i] += state->buffer[i] * 0.1f;
            }
        }
    }

    void addSource(SoundSource* source){
        m_inputs.push_back(std::make_unique<flo::SingleSoundInput>());
        const auto& input = m_inputs.back();
        addDependency(input.get());
        input->setSource(source);
    }

private:
    std::vector<std::unique_ptr<flo::SingleSoundInput>> m_inputs;
};

class ResamplerState : public flo::SoundState {
public:
    using SoundState::SoundState;

    void reset() noexcept override {
        buffer.silence();
        posCoarse = 0;
        posFine = 0.0;
    }

    flo::SoundChunk buffer;
    std::uint16_t posCoarse;
    double posFine;
};

class Resampler : public flo::OutOfSync<flo::ControlledSoundSource<ResamplerState>> {
public:
    Resampler() : timeSpeed(this, 1.0) {
        addDependency(&input);
    }

    void renderNextChunk(flo::SoundChunk& chunk, ResamplerState* state) override {
        // TODO: different types of interpolation and smoothing and junk

        const auto refillBuffer = [&]() -> void {
            input.getNextChunkFor(state->buffer, this, state);
            assert(state->posCoarse + 1 == flo::SoundChunk::size);
            state->posCoarse = 0;
        };

        const auto getNextSample = [&](double speed) -> flo::Sample {
            auto acc = flo::Sample{};
            auto count = std::uint8_t{0};

            state->posFine += speed;

            if (state->posFine < 1.0){
                return flo::Sample{state->buffer[state->posCoarse]};
            }

            while (state->posFine >= 1.0){
                state->posFine -= 1.0;
                acc += state->buffer[state->posCoarse];
                ++count;
                ++state->posCoarse;
                if (state->posCoarse == flo::SoundChunk::size){
                    refillBuffer();
                }
            }

            return acc / static_cast<float>(count);
        };

        for (std::uint16_t i = 0; i < flo::SoundChunk::size; ++i){
            state->adjustTime(i);
            const auto speed = timeSpeed.getValue(state);
            chunk[i] = getNextSample(speed);
        }
    }

    double getTimeSpeed(const flo::SoundState* context) const noexcept override {
        return timeSpeed.getValue(context);
    }

    flo::WithCurrentTime<flo::SingleSoundInput> input;

    flo::SoundNumberInput timeSpeed;
};

int main() {

    {
        auto& win = ui::Window::create(1000, 700, "Flosion haha");

        win.setRoot<flui::FlosionUI>();

        ui::run();

        return 0;
    }

    /*{
        auto dac = DAC{};

        auto osc = Oscillator{};

        auto saw = Saw{};
        saw.input.setSource(&osc.phase);
        osc.waveFunction.setSource(&saw);

        auto ens = Ensemble{};

        ens.input.setSource(&osc);
        ens.frequencyIn.setDefaultValue(70.0);

        auto rw = RandomWalk{};
        rw.borrowFrom(&osc);

        rw.bias.setDefaultValue(1.0);
        rw.speed.setDefaultValue(0.0003);
        rw.damping.setDefaultValue(0.8);

        auto one = flo::ConstantNumberSource{1.0};

        auto add = Add{};
        add.inputA.setSource(&one);
        add.inputB.setSource(&rw);

        auto mul = Multiply{};
        mul.inputA.setSource(&add);
        mul.inputB.setSource(&ens.input.frequencyOut);

        osc.frequency.setSource(&mul);

        dac.soundResult.setSource(&ens);

        dac.play();
        std::cout << "Playing...\n";
        std::this_thread::sleep_for(std::chrono::seconds(8));
        std::cout << "Paused\n";
        dac.pause();

        return 0;
    }*/

    /*{
        auto dac = DAC{};

        auto osc = Oscillator{};

        auto sine = Sine{};


        osc.waveFunction.setSource(&sine);
        sine.input.setSource(&osc.phase);

        auto ens = Ensemble{};

        ens.input.setSource(&osc);

        osc.frequency.setSource(&ens.input.frequencyOut);

        dac.soundResult.setSource(&ens);

        auto rw = RandomWalk{};
        rw.borrowFrom(&ens);

        auto add = Add{};

        auto mul = Multiply{};

        add.inputA.setDefaultValue(1.0);
        add.inputB.setSource(&rw);

        mul.inputA.setDefaultValue(300.0);
        mul.inputB.setSource(&add);

        //rw.smoothing.setDefaultValue(0.95);

        ens.frequencyIn.setSource(&mul);

        dac.play();
        std::cout << "Playing...\n";
        std::this_thread::sleep_for(std::chrono::seconds(8));
        std::cout << "Paused\n";
        dac.pause();

        return 0;
    }


    // Testing:
	flo::Network network;

    auto osc = Oscillator{};
    //osc.setPhaseSync(false);

    //auto smoo = Smoother{};
    //smoo.borrowFrom(&osc);

    //auto sine = Sine{};
    auto saw = Saw{};

    auto ens = Ensemble{};

    //sine.input.setSource(&osc.phase);
    saw.input.setSource(&osc.phase);

    //std::random_device rd;
    //auto eng = std::default_random_engine{rd()};
    //auto dist = std::uniform_int_distribution<int>{0,1};
    //if (dist(eng)){
    //    smoo.input.setSource(&sine);
    //} else {
    //    smoo.input.setSource(&saw);
    //}

    ens.input.setSource(&osc);

    //osc.waveFunction.setSource(&smoo);
    //osc.waveFunction.setSource(&sine);
    osc.waveFunction.setSource(&saw);

    //osc.frequency.setDefaultValue(0.005);
    osc.frequency.setSource(&ens.input.frequencyOut);

    auto router = Router{};
    router.getSoundInput()->setSource(&ens);

    router.addNumberSource(0);
    ens.frequencyIn.setSource(router.getNumberSource(0));
    
    auto mixer = Mixer{};
    auto dac = DAC{};
    //auto rs = Resampler{};
    //rs.timeSpeed.setDefaultValue(1.0);
    //rs.input.setSource(&mixer);
    //dac.soundResult.setSource(&rs);
    dac.soundResult.setSource(&mixer);

    auto mul1 = Multiply{};
    router.addSoundSource(0);
    mixer.addSource(router.getSoundSource(0));
    router.getNumberInput(0, 0)->setSource(&mul1);
    mul1.inputA.setDefaultValue(100.0 / 44100.0);
    mul1.inputB.setSource(&dac.soundResult.currentTime);
    
    auto mul2 = Multiply{};
    router.addSoundSource(1);
    mixer.addSource(router.getSoundSource(1));
    router.getNumberInput(0, 1)->setSource(&mul2);
    mul2.inputA.setDefaultValue(125.0 / 44100.0);
    mul2.inputB.setSource(&dac.soundResult.currentTime);

    auto mul3 = Multiply{};
    router.addSoundSource(2);
    mixer.addSource(router.getSoundSource(2));
    router.getNumberInput(0, 2)->setSource(&mul3);
    mul3.inputA.setDefaultValue(150.0 / 44100.0);
    mul3.inputB.setSource(&dac.soundResult.currentTime);

    dac.play();
    std::cout << "Playing...\n";
    std::this_thread::sleep_for(std::chrono::seconds(8));
    std::cout << "Done.\n";
    dac.pause();

	return 0;*/
}