#include <SoundSourceTemplate.hpp>

#include <gtest/gtest.h>

using namespace flo;

class BasicSoundNode : public Realtime<ControlledSoundSource<EmptySoundState>> {
    void renderNextChunk(SoundChunk& chunk, EmptySoundState* state) override {
    
    }
};

TEST(SoundNodeTest, Dependencies){
    auto sn1 = BasicSoundNode{};
    auto sn2 = BasicSoundNode{};
    
    EXPECT_TRUE(sn1.hasDependency(&sn1));
    EXPECT_TRUE(sn2.hasDependency(&sn2));

    EXPECT_FALSE(sn1.hasDependency(&sn2));
    EXPECT_FALSE(sn2.hasDependency(&sn1));

    // TODO: more tests
}