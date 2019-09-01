#include <SoundSourceTemplate.hpp>
#include <algorithm>
#include <type_traits>

#include <gtest/gtest.h>

template<typename Collection>
bool set_contains(const Collection& collection, const std::initializer_list<typename Collection::value_type>& items){
    return std::all_of(
        begin(items),
        end(items),
        [&](const typename Collection::value_type& item){
            return std::find(begin(collection), end(collection), item) != end(collection);
        }
    );
};

template<typename Collection>
bool set_disjoint(const Collection& collection, const std::initializer_list<typename Collection::value_type>& items){
    return std::none_of(
        begin(items),
        end(items),
        [&](const typename Collection::value_type& item){
            return std::find(begin(collection), end(collection), item) != end(collection);
        }
    );
}

template<typename Collection>
bool set_equals(const Collection& c, const std::initializer_list<typename Collection::value_type>& e){
    return std::is_permutation(begin(c), end(c), begin(e), end(e));
}

using namespace flo;

using BasicSoundNode =  Realtime<Singular<SoundNode, EmptySoundState>>;

TEST(SoundNodeTest, ShallowDependencies){
    auto sn1 = BasicSoundNode{};
    auto sn2 = BasicSoundNode{};
    
    EXPECT_TRUE(sn1.hasDependency(&sn1));
    EXPECT_TRUE(sn2.hasDependency(&sn2));
    EXPECT_FALSE(sn1.hasDependency(&sn2));
    EXPECT_FALSE(sn2.hasDependency(&sn1));
    
    EXPECT_TRUE(set_equals(sn1.getAllDependencies(), {&sn1}));
    EXPECT_TRUE(set_equals(sn1.getAllDependents(), {&sn1}));
    EXPECT_TRUE(set_equals(sn1.getDirectDependencies(), {}));
    EXPECT_TRUE(set_equals(sn1.getDirectDependents(), {}));
    
    EXPECT_TRUE(set_equals(sn2.getAllDependencies(), {&sn2}));
    EXPECT_TRUE(set_equals(sn2.getAllDependents(), {&sn2}));
    EXPECT_TRUE(set_equals(sn2.getDirectDependencies(), {}));
    EXPECT_TRUE(set_equals(sn2.getDirectDependents(), {}));
    
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
    
    EXPECT_TRUE(set_equals(sn1.getAllDependencies(), {&sn1, &sn2}));
    EXPECT_TRUE(set_equals(sn1.getAllDependents(), {&sn1}));
    EXPECT_TRUE(set_equals(sn1.getDirectDependencies(), {&sn2}));
    EXPECT_TRUE(set_equals(sn1.getDirectDependents(), {}));
    
    EXPECT_TRUE(set_equals(sn2.getAllDependencies(), {&sn2}));
    EXPECT_TRUE(set_equals(sn2.getAllDependents(), {&sn1, &sn2}));
    EXPECT_TRUE(set_equals(sn2.getDirectDependencies(), {}));
    EXPECT_TRUE(set_equals(sn2.getDirectDependents(), {&sn1}));
    
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
    
    EXPECT_TRUE(set_equals(sn1.getAllDependencies(), {&sn1}));
    EXPECT_TRUE(set_equals(sn1.getAllDependents(), {&sn1}));
    EXPECT_TRUE(set_equals(sn1.getDirectDependencies(), {}));
    EXPECT_TRUE(set_equals(sn1.getDirectDependents(), {}));
    
    EXPECT_TRUE(set_equals(sn2.getAllDependencies(), {&sn2}));
    EXPECT_TRUE(set_equals(sn2.getAllDependents(), {&sn2}));
    EXPECT_TRUE(set_equals(sn2.getDirectDependencies(), {}));
    EXPECT_TRUE(set_equals(sn2.getDirectDependents(), {}));
}


TEST(SoundNodeTest, DeepDependencies1){
    auto sn1 = BasicSoundNode{};
    auto sn2 = BasicSoundNode{};
    auto sn3 = BasicSoundNode{};

    EXPECT_TRUE(sn1.canAddDependency(&sn2));
    sn1.addDependency(&sn2);

    EXPECT_TRUE(sn2.canAddDependency(&sn3));
    sn2.addDependency(&sn3);

    EXPECT_TRUE(sn1.hasDependency(&sn1));
    EXPECT_TRUE(sn1.hasDependency(&sn3));
    EXPECT_FALSE(sn2.hasDependency(&sn1));
    EXPECT_TRUE(sn2.hasDependency(&sn3));
    EXPECT_FALSE(sn3.hasDependency(&sn1));
    EXPECT_FALSE(sn3.hasDependency(&sn2));
    
    EXPECT_TRUE(set_equals(sn1.getAllDependencies(), {&sn1, &sn2, &sn3}));
    EXPECT_TRUE(set_equals(sn2.getAllDependencies(), {&sn2, &sn3}));
    EXPECT_TRUE(set_equals(sn3.getAllDependencies(), {&sn3}));
    
    EXPECT_TRUE(set_equals(sn1.getAllDependents(), {&sn1}));
    EXPECT_TRUE(set_equals(sn2.getAllDependents(), {&sn1, &sn2}));
    EXPECT_TRUE(set_equals(sn3.getAllDependents(), {&sn1, &sn2, &sn3}));
    
    EXPECT_TRUE(set_equals(sn1.getDirectDependencies(), {&sn2}));
    EXPECT_TRUE(set_equals(sn2.getDirectDependencies(), {&sn3}));
    EXPECT_TRUE(set_equals(sn3.getDirectDependencies(), {}));
    
    EXPECT_TRUE(set_equals(sn1.getDirectDependents(), {}));
    EXPECT_TRUE(set_equals(sn2.getDirectDependents(), {&sn1}));
    EXPECT_TRUE(set_equals(sn3.getDirectDependents(), {&sn2}));
}


TEST(SoundNodeTest, DeepDependencies2){
    auto sn1 = BasicSoundNode{};
    auto sn2 = BasicSoundNode{};
    auto sn3 = BasicSoundNode{};
    auto sn4 = BasicSoundNode{};
    auto sn5 = BasicSoundNode{};

    EXPECT_TRUE(sn1.canAddDependency(&sn2));
    sn1.addDependency(&sn2);

    EXPECT_TRUE(sn2.canAddDependency(&sn3));
    sn2.addDependency(&sn3);

    EXPECT_TRUE(sn3.canAddDependency(&sn4));
    sn3.addDependency(&sn4);

    EXPECT_TRUE(sn4.canAddDependency(&sn5));
    sn4.addDependency(&sn5);
    
    EXPECT_TRUE(sn1.hasDependency(&sn2));
    EXPECT_TRUE(sn1.hasDependency(&sn3));
    EXPECT_TRUE(sn1.hasDependency(&sn4));
    EXPECT_TRUE(sn1.hasDependency(&sn5));

    EXPECT_FALSE(sn2.hasDependency(&sn1));
    EXPECT_TRUE(sn2.hasDependency(&sn3));
    EXPECT_TRUE(sn2.hasDependency(&sn4));
    EXPECT_TRUE(sn2.hasDependency(&sn5));

    EXPECT_FALSE(sn3.hasDependency(&sn1));
    EXPECT_FALSE(sn3.hasDependency(&sn2));
    EXPECT_TRUE(sn3.hasDependency(&sn4));
    EXPECT_TRUE(sn3.hasDependency(&sn5));

    EXPECT_FALSE(sn4.hasDependency(&sn1));
    EXPECT_FALSE(sn4.hasDependency(&sn2));
    EXPECT_FALSE(sn4.hasDependency(&sn3));
    EXPECT_TRUE(sn4.hasDependency(&sn5));

    EXPECT_FALSE(sn5.hasDependency(&sn1));
    EXPECT_FALSE(sn5.hasDependency(&sn2));
    EXPECT_FALSE(sn5.hasDependency(&sn3));
    EXPECT_FALSE(sn5.hasDependency(&sn4));
    
    EXPECT_TRUE(set_equals(sn1.getAllDependencies(), {&sn1, &sn2, &sn3, &sn4, &sn5}));
    EXPECT_TRUE(set_equals(sn2.getAllDependencies(), {&sn2, &sn3, &sn4, &sn5}));
    EXPECT_TRUE(set_equals(sn3.getAllDependencies(), {&sn3, &sn4, &sn5}));
    EXPECT_TRUE(set_equals(sn4.getAllDependencies(), {&sn4, &sn5}));
    EXPECT_TRUE(set_equals(sn5.getAllDependencies(), {&sn5}));
    
    EXPECT_TRUE(set_equals(sn1.getAllDependents(), {&sn1}));
    EXPECT_TRUE(set_equals(sn2.getAllDependents(), {&sn1, &sn2}));
    EXPECT_TRUE(set_equals(sn3.getAllDependents(), {&sn1, &sn2, &sn3}));
    EXPECT_TRUE(set_equals(sn4.getAllDependents(), {&sn1, &sn2, &sn3, &sn4}));
    EXPECT_TRUE(set_equals(sn5.getAllDependents(), {&sn1, &sn2, &sn3, &sn4, &sn5}));
    
    EXPECT_TRUE(set_equals(sn1.getDirectDependencies(), {&sn2}));
    EXPECT_TRUE(set_equals(sn2.getDirectDependencies(), {&sn3}));
    EXPECT_TRUE(set_equals(sn3.getDirectDependencies(), {&sn4}));
    EXPECT_TRUE(set_equals(sn4.getDirectDependencies(), {&sn5}));
    EXPECT_TRUE(set_equals(sn5.getDirectDependencies(), {}));
    
    EXPECT_TRUE(set_equals(sn1.getDirectDependents(), {}));
    EXPECT_TRUE(set_equals(sn2.getDirectDependents(), {&sn1}));
    EXPECT_TRUE(set_equals(sn3.getDirectDependents(), {&sn2}));
    EXPECT_TRUE(set_equals(sn4.getDirectDependents(), {&sn3}));
    EXPECT_TRUE(set_equals(sn5.getDirectDependents(), {&sn4}));
}

class NumberSoundNode : public BasicSoundNode {
public:
    NumberSoundNode()
        : node(this){
    
    }

    class SoundNumberNode : public NumberNode {
    public:
        SoundNumberNode(NumberSoundNode* parent){
            setStateOwner(parent);
        }
    } node;
};

TEST(SoundNodeTest, DependencySafety){
    auto sn1 = NumberSoundNode{};
    auto sn2 = NumberSoundNode{};

    EXPECT_TRUE(sn1.canAddDependency(&sn2));
    EXPECT_TRUE(sn2.canAddDependency(&sn1));
    
    EXPECT_EQ(sn1.node.getStateOwner(), &sn1);
    EXPECT_EQ(sn2.node.getStateOwner(), &sn2);
    
    EXPECT_FALSE(sn1.hasDependency(&sn2));
    EXPECT_FALSE(sn2.hasDependency(&sn1));
    
    EXPECT_TRUE(set_disjoint(sn1.node.getDirectDependencies(), {&sn1.node, &sn2.node}));
    EXPECT_TRUE(set_disjoint(sn2.node.getDirectDependencies(), {&sn1.node, &sn2.node}));

    EXPECT_TRUE(set_disjoint(sn1.node.getDirectDependents(), {&sn1.node, &sn2.node}));
    EXPECT_TRUE(set_disjoint(sn2.node.getDirectDependents(), {&sn1.node, &sn2.node}));
    
    EXPECT_TRUE(set_contains(sn1.node.getAllDependencies(), {&sn1.node}));
    EXPECT_TRUE(set_disjoint(sn1.node.getAllDependencies(), {&sn2.node}));
    EXPECT_TRUE(set_contains(sn2.node.getAllDependencies(), {&sn2.node}));
    EXPECT_TRUE(set_disjoint(sn2.node.getAllDependencies(), {&sn1.node}));

    EXPECT_TRUE(set_contains(sn1.node.getAllDependents(), {&sn1.node}));
    EXPECT_TRUE(set_disjoint(sn1.node.getAllDependents(), {&sn2.node}));
    EXPECT_TRUE(set_contains(sn2.node.getAllDependents(), {&sn2.node}));
    EXPECT_TRUE(set_disjoint(sn2.node.getAllDependents(), {&sn1.node}));

    EXPECT_FALSE(sn1.node.canAddDependency(&sn1.node));
    EXPECT_FALSE(sn2.node.canAddDependency(&sn2.node));
    EXPECT_FALSE(sn1.node.canAddDependency(&sn2.node));
    EXPECT_FALSE(sn2.node.canAddDependency(&sn1.node));

    EXPECT_TRUE(sn1.canAddDependency(&sn2));
    sn1.addDependency(&sn2);
    
    EXPECT_FALSE(sn1.node.canAddDependency(&sn1.node));
    EXPECT_FALSE(sn2.node.canAddDependency(&sn2.node));
    EXPECT_FALSE(sn1.node.canAddDependency(&sn2.node));
    EXPECT_TRUE(sn2.node.canAddDependency(&sn1.node));
    
    sn2.node.addDependency(&sn1.node);
    
    EXPECT_TRUE(set_disjoint(sn1.node.getDirectDependencies(), {&sn1.node, &sn2.node}));
    EXPECT_TRUE(set_contains(sn2.node.getDirectDependencies(), {&sn1.node}));
    EXPECT_TRUE(set_disjoint(sn2.node.getDirectDependencies(), {&sn2.node}));
    
    EXPECT_TRUE(set_contains(sn1.node.getDirectDependents(), {&sn2.node}));
    EXPECT_TRUE(set_disjoint(sn1.node.getDirectDependents(), {&sn1.node}));
    EXPECT_TRUE(set_disjoint(sn2.node.getDirectDependents(), {&sn1.node, &sn2.node}));
    
    EXPECT_TRUE(set_contains(sn1.node.getAllDependencies(), {&sn1.node}));
    EXPECT_TRUE(set_disjoint(sn1.node.getAllDependencies(), {&sn2.node}));
    EXPECT_TRUE(set_contains(sn2.node.getAllDependencies(), {&sn1.node, &sn2.node}));

    EXPECT_TRUE(set_contains(sn1.node.getAllDependents(), {&sn1.node, &sn2.node}));
    EXPECT_TRUE(set_contains(sn2.node.getAllDependents(), {&sn2.node}));
    EXPECT_TRUE(set_disjoint(sn2.node.getAllDependents(), {&sn1.node}));
    
    EXPECT_FALSE(sn1.node.canAddDependency(&sn1.node));
    EXPECT_FALSE(sn2.node.canAddDependency(&sn2.node));
    EXPECT_FALSE(sn1.node.canAddDependency(&sn2.node));
    EXPECT_FALSE(sn2.node.canAddDependency(&sn1.node));
}