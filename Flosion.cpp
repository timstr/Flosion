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

// TODO: actually use flo::Network

// TODO: safegaurd sound processing objects from being deleted
// while they are inside getNextChunk

// TODO: include and dynamically link to ffmpeg for additional audio formats?

// TODO: add graphs back (in a safe and clean way)

// TODO: update wires when objects change size

// TODO: safeguard number sources from deletion while they are being used
// (i.e. do some kind of locking to prevent them from being deleted
// during their use in sound processing)

// UI-related things:
// TODO: decide on a user interface for borrowing number sources
// - wire link between borrower and its lender?
// - Borrowers get placed into a box inside/alongside the lender?
// - 
// TODO: allow default value of NumberInput to be edited via NumberInputPeg
// TODO: add posRelativeTo(const Element* ancestor) to ui and use it
// TODO: when a connected head is moused over, show the peg's label
// TODO: serialization/deserialization for objects and wires
// TODO: cut/copy/paste
// TODO: load/save project files (.flo extension???)
// TODO: Collapsible panels
// TODO: obstacle-avoiding wires
// TODO: hand-written math expressions
// TODO: record sound streams into an audioclip
// TODO: put wires in front of objects always

// TODO: full keyboard naviation
// 'letter' is used to mean that each applicatable object
// is highlighted with a letter from the keyboard, and
// pressing that letter selects the indicated object
// - panel scope:
//     - [something] to create an object
//     - arrow keys or letter to select nearby object
// - object scope:
//     - arrow keys or letter to select peg
//     - shift + arrow keys to move object?
//     - escape to return to panel scope
// - peg scope:
//     - letter to select target peg for adding/removing
//       wire (only applicable pegs are indicated)


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