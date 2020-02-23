cmake_minimum_required(VERSION 3.10.0)

set(include_path "include/Flosion/Objects")

set(flosion_objects_headers
    ${include_path}/Accumulator.hpp
    ${include_path}/ADSR.hpp
	${include_path}/AudioClip.hpp
	${include_path}/DAC.hpp
	${include_path}/Ensemble.hpp
    ${include_path}/ExponentialSmoother.hpp
    ${include_path}/FlipFlop.hpp
	${include_path}/Functions.hpp
	${include_path}/FunctionsBase.hpp
	${include_path}/WaveForms.hpp
	${include_path}/WaveGenerator.hpp
    ${include_path}/BitCrush.hpp
    ${include_path}/Compressor.hpp
    ${include_path}/Convolver.hpp
    ${include_path}/Delay.hpp
    ${include_path}/Distortion.hpp
    ${include_path}/Equalizer.hpp
    ${include_path}/Feedback.hpp
    ${include_path}/Highpass.hpp
    ${include_path}/LinearSmoother.hpp
    ${include_path}/Lowpass.hpp
    ${include_path}/Melody.hpp
    ${include_path}/Mixer.hpp
    ${include_path}/Oscillator.hpp
    ${include_path}/PhaseVocoder.hpp
    ${include_path}/Pointwise.hpp
    ${include_path}/RandomWalk.hpp
    ${include_path}/Resampler.hpp
    ${include_path}/Router.hpp
    ${include_path}/Scatter.hpp
    ${include_path}/Sequencer.hpp
    ${include_path}/Splicer.hpp
    ${include_path}/StringModel.hpp
    ${include_path}/Variable.hpp
)

set(flosion_objects_srcs
    src/Accumulator.cpp
    src/ADSR.cpp
    src/AudioClip.cpp
	src/DAC.cpp
    src/Ensemble.cpp
    src/ExponentialSmoother.cpp
    src/FlipFlop.cpp
	src/Functions.cpp
	src/FunctionsBase.cpp
	src/WaveForms.cpp
	src/WaveGenerator.cpp
    src/BitCrush.cpp
    src/Compressor.cpp
    src/Convolver.cpp
    src/Delay.cpp
    src/Distortion.cpp
    src/Equalizer.cpp
    src/Feedback.cpp
    src/Highpass.cpp
    src/LinearSmoother.cpp
    src/Lowpass.cpp
    src/Melody.cpp
    src/Mixer.cpp
    src/Oscillator.cpp
    src/PhaseVocoder.cpp
    src/Pointwise.cpp
    src/RandomWalk.cpp
    src/Resampler.cpp
    src/Router.cpp
    src/Scatter.cpp
    src/Sequencer.cpp
    src/Splicer.cpp
    src/StringModel.cpp
    src/Variable.cpp
)

add_library(flosion_objects STATIC ${flosion_objects_headers} ${flosion_objects_srcs})

target_include_directories(flosion_objects PUBLIC include)

set_target_properties(flosion_objects PROPERTIES LINKER_LANGUAGE CXX)

set_property(TARGET flosion_objects PROPERTY CXX_STANDARD 17)

target_link_libraries(flosion_objects
    PUBLIC flosion_core
    PUBLIC tims-gui
)
