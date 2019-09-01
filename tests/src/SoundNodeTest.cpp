#include <SoundSourceTemplate.hpp>

#include <gtest/gtest.h>

using namespace flo;

using BasicSoundNode =  Realtime<Singular<SoundNode, EmptySoundState>>;

const auto contains = [](const auto& collection, const auto& item) -> bool {
    return std::find(begin(collection), end(collection), item) != end(collection);
};

template<typename Collection, typename ElementType = decltype(*std::declval<Collection>().begin())>
bool same_elements(const Collection& c, const std::initializer_list<ElementType>& e){
    return std::is_permutation(begin(c), end(c), begin(e), end(e));
}

TEST(SoundNodeTest, Dependencies){
    auto sn1 = BasicSoundNode{};
    auto sn2 = BasicSoundNode{};
    
    EXPECT_TRUE(sn1.hasDependency(&sn1));
    EXPECT_TRUE(sn2.hasDependency(&sn2));
    EXPECT_FALSE(sn1.hasDependency(&sn2));
    EXPECT_FALSE(sn2.hasDependency(&sn1));
    
    EXPECT_TRUE(same_elements(sn1.getAllDependencies(), {&sn1}));
    EXPECT_TRUE(same_elements(sn1.getAllDependents(), {&sn1}));
    EXPECT_TRUE(same_elements(sn1.getDirectDependencies(), {}));
    EXPECT_TRUE(same_elements(sn1.getDirectDependents(), {}));
    
    EXPECT_TRUE(same_elements(sn2.getAllDependencies(), {&sn2}));
    EXPECT_TRUE(same_elements(sn2.getAllDependents(), {&sn2}));
    EXPECT_TRUE(same_elements(sn2.getDirectDependencies(), {}));
    EXPECT_TRUE(same_elements(sn2.getDirectDependents(), {}));
    
    EXPECT_TRUE(sn1.canAddDependency(&sn2));
    EXPECT_TRUE(sn2.canAddDependency(&sn1));
    EXPECT_FALSE(sn1.canAddDependency(&sn1));
    EXPECT_FALSE(sn2.canAddDependency(&sn2));

    EXPECT_FALSE(sn1.canRemoveDependency(&sn2));
    EXPECT_FALSE(sn2.canRemoveDependency(&sn1));
    EXPECT_FALSE(sn1.canRemoveDependency(&sn1));
    EXPECT_FALSE(sn2.canRemoveDependency(&sn2));

    sn1.addDependency(&sn2);

    EXPECT_TRUE(sn1.hasDependency(&sn1));
    EXPECT_TRUE(sn2.hasDependency(&sn2));
    EXPECT_TRUE(sn1.hasDependency(&sn2));
    EXPECT_FALSE(sn2.hasDependency(&sn1));
    
    EXPECT_TRUE(same_elements(sn1.getAllDependencies(), {&sn1, &sn2}));
    EXPECT_TRUE(same_elements(sn1.getAllDependents(), {&sn1}));
    EXPECT_TRUE(same_elements(sn1.getDirectDependencies(), {&sn2}));
    EXPECT_TRUE(same_elements(sn1.getDirectDependents(), {}));
    
    EXPECT_TRUE(same_elements(sn2.getAllDependencies(), {&sn2}));
    EXPECT_TRUE(same_elements(sn2.getAllDependents(), {&sn1, &sn2}));
    EXPECT_TRUE(same_elements(sn2.getDirectDependencies(), {}));
    EXPECT_TRUE(same_elements(sn2.getDirectDependents(), {&sn1}));
    
    EXPECT_FALSE(sn1.canAddDependency(&sn2));
    EXPECT_FALSE(sn2.canAddDependency(&sn1));
    EXPECT_FALSE(sn1.canAddDependency(&sn1));
    EXPECT_FALSE(sn2.canAddDependency(&sn2));

    EXPECT_TRUE(sn1.canRemoveDependency(&sn2));
    EXPECT_FALSE(sn2.canRemoveDependency(&sn1));
    EXPECT_FALSE(sn1.canRemoveDependency(&sn1));
    EXPECT_FALSE(sn2.canRemoveDependency(&sn2));
    
    sn1.removeDependency(&sn2);
    
    EXPECT_TRUE(sn1.hasDependency(&sn1));
    EXPECT_TRUE(sn2.hasDependency(&sn2));
    EXPECT_FALSE(sn1.hasDependency(&sn2));
    EXPECT_FALSE(sn2.hasDependency(&sn1));
    
    EXPECT_TRUE(same_elements(sn1.getAllDependencies(), {&sn1}));
    EXPECT_TRUE(same_elements(sn1.getAllDependents(), {&sn1}));
    EXPECT_TRUE(same_elements(sn1.getDirectDependencies(), {}));
    EXPECT_TRUE(same_elements(sn1.getDirectDependents(), {}));
    
    EXPECT_TRUE(same_elements(sn2.getAllDependencies(), {&sn2}));
    EXPECT_TRUE(same_elements(sn2.getAllDependents(), {&sn2}));
    EXPECT_TRUE(same_elements(sn2.getDirectDependencies(), {}));
    EXPECT_TRUE(same_elements(sn2.getDirectDependents(), {}));

}