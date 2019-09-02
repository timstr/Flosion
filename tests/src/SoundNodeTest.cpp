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

TEST(SoundNodeTest, ShallowDependencies1){
    auto root = BasicSoundNode{};
    auto leaf = BasicSoundNode{};
    
    EXPECT_TRUE(root.hasDependency(&root));
    EXPECT_TRUE(leaf.hasDependency(&leaf));
    EXPECT_FALSE(root.hasDependency(&leaf));
    EXPECT_FALSE(leaf.hasDependency(&root));
    
    EXPECT_TRUE(set_equals(root.getAllDependencies(), {&root}));
    EXPECT_TRUE(set_equals(root.getAllDependents(), {&root}));
    EXPECT_TRUE(set_equals(root.getDirectDependencies(), {}));
    EXPECT_TRUE(set_equals(root.getDirectDependents(), {}));
    
    EXPECT_TRUE(set_equals(leaf.getAllDependencies(), {&leaf}));
    EXPECT_TRUE(set_equals(leaf.getAllDependents(), {&leaf}));
    EXPECT_TRUE(set_equals(leaf.getDirectDependencies(), {}));
    EXPECT_TRUE(set_equals(leaf.getDirectDependents(), {}));
    
    EXPECT_TRUE(root.canAddDependency(&leaf));
    EXPECT_TRUE(leaf.canAddDependency(&root));
    EXPECT_FALSE(root.canAddDependency(&root));
    EXPECT_FALSE(leaf.canAddDependency(&leaf));

    EXPECT_FALSE(root.canRemoveDependency(&leaf));
    EXPECT_FALSE(leaf.canRemoveDependency(&root));
    EXPECT_FALSE(root.canRemoveDependency(&root));
    EXPECT_FALSE(leaf.canRemoveDependency(&leaf));

    root.addDependency(&leaf);

    EXPECT_TRUE(root.hasDependency(&root));
    EXPECT_TRUE(leaf.hasDependency(&leaf));
    EXPECT_TRUE(root.hasDependency(&leaf));
    EXPECT_FALSE(leaf.hasDependency(&root));
    
    EXPECT_TRUE(set_equals(root.getAllDependencies(), {&root, &leaf}));
    EXPECT_TRUE(set_equals(root.getAllDependents(), {&root}));
    EXPECT_TRUE(set_equals(root.getDirectDependencies(), {&leaf}));
    EXPECT_TRUE(set_equals(root.getDirectDependents(), {}));
    
    EXPECT_TRUE(set_equals(leaf.getAllDependencies(), {&leaf}));
    EXPECT_TRUE(set_equals(leaf.getAllDependents(), {&root, &leaf}));
    EXPECT_TRUE(set_equals(leaf.getDirectDependencies(), {}));
    EXPECT_TRUE(set_equals(leaf.getDirectDependents(), {&root}));
    
    EXPECT_FALSE(root.canAddDependency(&leaf));
    EXPECT_FALSE(leaf.canAddDependency(&root));
    EXPECT_FALSE(root.canAddDependency(&root));
    EXPECT_FALSE(leaf.canAddDependency(&leaf));

    EXPECT_TRUE(root.canRemoveDependency(&leaf));
    EXPECT_FALSE(leaf.canRemoveDependency(&root));
    EXPECT_FALSE(root.canRemoveDependency(&root));
    EXPECT_FALSE(leaf.canRemoveDependency(&leaf));
    
    root.removeDependency(&leaf);
    
    EXPECT_TRUE(root.hasDependency(&root));
    EXPECT_TRUE(leaf.hasDependency(&leaf));
    EXPECT_FALSE(root.hasDependency(&leaf));
    EXPECT_FALSE(leaf.hasDependency(&root));
    
    EXPECT_TRUE(set_equals(root.getAllDependencies(), {&root}));
    EXPECT_TRUE(set_equals(root.getAllDependents(), {&root}));
    EXPECT_TRUE(set_equals(root.getDirectDependencies(), {}));
    EXPECT_TRUE(set_equals(root.getDirectDependents(), {}));
    
    EXPECT_TRUE(set_equals(leaf.getAllDependencies(), {&leaf}));
    EXPECT_TRUE(set_equals(leaf.getAllDependents(), {&leaf}));
    EXPECT_TRUE(set_equals(leaf.getDirectDependencies(), {}));
    EXPECT_TRUE(set_equals(leaf.getDirectDependents(), {}));
}

TEST(SoundNodeTest, ShallowDependencies2){
    auto root = BasicSoundNode{};
    auto leaf1 = BasicSoundNode{};
    auto leaf2 = BasicSoundNode{};
    
    EXPECT_FALSE(root.hasDependency(&leaf1));
    EXPECT_FALSE(root.hasDependency(&leaf2));

    EXPECT_FALSE(leaf1.hasDependency(&root));
    EXPECT_FALSE(leaf1.hasDependency(&leaf2));
    EXPECT_FALSE(leaf2.hasDependency(&root));
    EXPECT_FALSE(leaf2.hasDependency(&leaf1));
    
    EXPECT_TRUE(root.canAddDependency(&leaf1));
    EXPECT_TRUE(root.canAddDependency(&leaf2));
    EXPECT_TRUE(leaf1.canAddDependency(&root));
    EXPECT_TRUE(leaf1.canAddDependency(&leaf2));
    EXPECT_TRUE(leaf2.canAddDependency(&root));
    EXPECT_TRUE(leaf2.canAddDependency(&leaf1));

    EXPECT_FALSE(root.canRemoveDependency(&leaf1));
    EXPECT_FALSE(root.canRemoveDependency(&leaf2));
    EXPECT_FALSE(leaf1.canRemoveDependency(&root));
    EXPECT_FALSE(leaf1.canRemoveDependency(&leaf2));
    EXPECT_FALSE(leaf2.canRemoveDependency(&root));
    EXPECT_FALSE(leaf2.canRemoveDependency(&leaf1));

    root.addDependency(&leaf1);
    root.addDependency(&leaf2);
    
    EXPECT_TRUE(root.hasDependency(&leaf1));
    EXPECT_TRUE(root.hasDependency(&leaf2));
    EXPECT_FALSE(leaf1.hasDependency(&root));
    EXPECT_FALSE(leaf1.hasDependency(&leaf2));
    EXPECT_FALSE(leaf2.hasDependency(&root));
    EXPECT_FALSE(leaf2.hasDependency(&leaf1));
    
    EXPECT_FALSE(root.canAddDependency(&leaf1));
    EXPECT_FALSE(root.canAddDependency(&leaf2));
    EXPECT_FALSE(leaf1.canAddDependency(&root));
    EXPECT_TRUE(leaf1.canAddDependency(&leaf2));
    EXPECT_FALSE(leaf2.canAddDependency(&root));
    EXPECT_TRUE(leaf2.canAddDependency(&leaf1));

    EXPECT_TRUE(root.canRemoveDependency(&leaf1));
    EXPECT_TRUE(root.canRemoveDependency(&leaf2));
    EXPECT_FALSE(leaf1.canRemoveDependency(&root));
    EXPECT_FALSE(leaf1.canRemoveDependency(&leaf2));
    EXPECT_FALSE(leaf2.canRemoveDependency(&root));
    EXPECT_FALSE(leaf2.canRemoveDependency(&leaf1));

    EXPECT_TRUE(set_equals(root.getDirectDependencies(), {&leaf1, &leaf2}));
    EXPECT_TRUE(set_equals(root.getDirectDependents(), {}));
    EXPECT_TRUE(set_equals(root.getAllDependencies(), {&root, &leaf1, &leaf2}));
    EXPECT_TRUE(set_equals(root.getAllDependents(), {&root}));

    EXPECT_TRUE(set_equals(leaf1.getDirectDependencies(), {}));
    EXPECT_TRUE(set_equals(leaf1.getDirectDependents(), {&root}));
    EXPECT_TRUE(set_equals(leaf1.getAllDependencies(), {&leaf1}));
    EXPECT_TRUE(set_equals(leaf1.getAllDependents(), {&root, &leaf1}));

    EXPECT_TRUE(set_equals(leaf2.getDirectDependencies(), {}));
    EXPECT_TRUE(set_equals(leaf2.getDirectDependents(), {&root}));
    EXPECT_TRUE(set_equals(leaf2.getAllDependencies(), {&leaf2}));
    EXPECT_TRUE(set_equals(leaf2.getAllDependents(), {&root, &leaf2}));
    
    root.removeDependency(&leaf1);
    root.removeDependency(&leaf2);
    
    EXPECT_FALSE(root.hasDependency(&leaf1));
    EXPECT_FALSE(root.hasDependency(&leaf2));

    EXPECT_FALSE(leaf1.hasDependency(&root));
    EXPECT_FALSE(leaf1.hasDependency(&leaf2));
    EXPECT_FALSE(leaf2.hasDependency(&root));
    EXPECT_FALSE(leaf2.hasDependency(&leaf1));
    
    EXPECT_TRUE(root.canAddDependency(&leaf1));
    EXPECT_TRUE(root.canAddDependency(&leaf2));
    EXPECT_TRUE(leaf1.canAddDependency(&root));
    EXPECT_TRUE(leaf1.canAddDependency(&leaf2));
    EXPECT_TRUE(leaf2.canAddDependency(&root));
    EXPECT_TRUE(leaf2.canAddDependency(&leaf1));

    EXPECT_FALSE(root.canRemoveDependency(&leaf1));
    EXPECT_FALSE(root.canRemoveDependency(&leaf2));
    EXPECT_FALSE(leaf1.canRemoveDependency(&root));
    EXPECT_FALSE(leaf1.canRemoveDependency(&leaf2));
    EXPECT_FALSE(leaf2.canRemoveDependency(&root));
    EXPECT_FALSE(leaf2.canRemoveDependency(&leaf1));

    EXPECT_TRUE(set_equals(root.getDirectDependencies(), {}));
    EXPECT_TRUE(set_equals(root.getDirectDependents(), {}));
    EXPECT_TRUE(set_equals(root.getAllDependencies(), {&root}));
    EXPECT_TRUE(set_equals(root.getAllDependents(), {&root}));

    EXPECT_TRUE(set_equals(leaf1.getDirectDependencies(), {}));
    EXPECT_TRUE(set_equals(leaf1.getDirectDependents(), {}));
    EXPECT_TRUE(set_equals(leaf1.getAllDependencies(), {&leaf1}));
    EXPECT_TRUE(set_equals(leaf1.getAllDependents(), {&leaf1}));

    EXPECT_TRUE(set_equals(leaf2.getDirectDependencies(), {}));
    EXPECT_TRUE(set_equals(leaf2.getDirectDependents(), {}));
    EXPECT_TRUE(set_equals(leaf2.getAllDependencies(), {&leaf2}));
    EXPECT_TRUE(set_equals(leaf2.getAllDependents(), {&leaf2}));
}

TEST(SoundNodeTest, ShallowDependencies3){
    auto root = BasicSoundNode{};
    auto leaf1 = BasicSoundNode{};
    auto leaf2 = BasicSoundNode{};
    auto leaf3 = BasicSoundNode{};
    auto leaf4 = BasicSoundNode{};
    auto leaf5 = BasicSoundNode{};
    
    EXPECT_FALSE(root.hasDependency(&leaf1));
    EXPECT_FALSE(root.hasDependency(&leaf2));
    EXPECT_FALSE(root.hasDependency(&leaf3));
    EXPECT_FALSE(root.hasDependency(&leaf4));
    EXPECT_FALSE(root.hasDependency(&leaf5));

    EXPECT_FALSE(leaf1.hasDependency(&root));
    EXPECT_FALSE(leaf1.hasDependency(&leaf2));
    EXPECT_FALSE(leaf1.hasDependency(&leaf3));
    EXPECT_FALSE(leaf1.hasDependency(&leaf4));
    EXPECT_FALSE(leaf1.hasDependency(&leaf5));

    EXPECT_FALSE(leaf2.hasDependency(&root));
    EXPECT_FALSE(leaf2.hasDependency(&leaf1));
    EXPECT_FALSE(leaf2.hasDependency(&leaf3));
    EXPECT_FALSE(leaf2.hasDependency(&leaf4));
    EXPECT_FALSE(leaf2.hasDependency(&leaf5));

    EXPECT_FALSE(leaf3.hasDependency(&root));
    EXPECT_FALSE(leaf3.hasDependency(&leaf1));
    EXPECT_FALSE(leaf3.hasDependency(&leaf2));
    EXPECT_FALSE(leaf3.hasDependency(&leaf4));
    EXPECT_FALSE(leaf3.hasDependency(&leaf5));

    EXPECT_FALSE(leaf4.hasDependency(&root));
    EXPECT_FALSE(leaf4.hasDependency(&leaf1));
    EXPECT_FALSE(leaf4.hasDependency(&leaf2));
    EXPECT_FALSE(leaf4.hasDependency(&leaf3));
    EXPECT_FALSE(leaf4.hasDependency(&leaf5));

    EXPECT_FALSE(leaf5.hasDependency(&root));
    EXPECT_FALSE(leaf5.hasDependency(&leaf1));
    EXPECT_FALSE(leaf5.hasDependency(&leaf2));
    EXPECT_FALSE(leaf5.hasDependency(&leaf3));
    EXPECT_FALSE(leaf5.hasDependency(&leaf4));

    EXPECT_TRUE(root.canAddDependency(&leaf1));
    EXPECT_TRUE(root.canAddDependency(&leaf2));
    EXPECT_TRUE(root.canAddDependency(&leaf3));
    EXPECT_TRUE(root.canAddDependency(&leaf4));
    EXPECT_TRUE(root.canAddDependency(&leaf5));

    root.addDependency(&leaf1);
    root.addDependency(&leaf2);
    root.addDependency(&leaf3);
    root.addDependency(&leaf4);
    root.addDependency(&leaf5);
    
    EXPECT_TRUE(root.hasDependency(&leaf1));
    EXPECT_TRUE(root.hasDependency(&leaf2));
    EXPECT_TRUE(root.hasDependency(&leaf3));
    EXPECT_TRUE(root.hasDependency(&leaf4));
    EXPECT_TRUE(root.hasDependency(&leaf5));

    EXPECT_FALSE(leaf1.hasDependency(&root));
    EXPECT_FALSE(leaf1.hasDependency(&leaf2));
    EXPECT_FALSE(leaf1.hasDependency(&leaf3));
    EXPECT_FALSE(leaf1.hasDependency(&leaf4));
    EXPECT_FALSE(leaf1.hasDependency(&leaf5));

    EXPECT_FALSE(leaf2.hasDependency(&root));
    EXPECT_FALSE(leaf2.hasDependency(&leaf1));
    EXPECT_FALSE(leaf2.hasDependency(&leaf3));
    EXPECT_FALSE(leaf2.hasDependency(&leaf4));
    EXPECT_FALSE(leaf2.hasDependency(&leaf5));

    EXPECT_FALSE(leaf3.hasDependency(&root));
    EXPECT_FALSE(leaf3.hasDependency(&leaf1));
    EXPECT_FALSE(leaf3.hasDependency(&leaf2));
    EXPECT_FALSE(leaf3.hasDependency(&leaf4));
    EXPECT_FALSE(leaf3.hasDependency(&leaf5));

    EXPECT_FALSE(leaf4.hasDependency(&root));
    EXPECT_FALSE(leaf4.hasDependency(&leaf1));
    EXPECT_FALSE(leaf4.hasDependency(&leaf2));
    EXPECT_FALSE(leaf4.hasDependency(&leaf3));
    EXPECT_FALSE(leaf4.hasDependency(&leaf5));

    EXPECT_FALSE(leaf5.hasDependency(&root));
    EXPECT_FALSE(leaf5.hasDependency(&leaf1));
    EXPECT_FALSE(leaf5.hasDependency(&leaf2));
    EXPECT_FALSE(leaf5.hasDependency(&leaf3));
    EXPECT_FALSE(leaf5.hasDependency(&leaf4));

    EXPECT_TRUE(set_equals(root.getDirectDependencies(), {&leaf1, &leaf2, &leaf3, &leaf4, &leaf5}));
    EXPECT_TRUE(set_equals(root.getDirectDependents(), {}));
    EXPECT_TRUE(set_equals(root.getAllDependencies(), {&root, &leaf1, &leaf2, &leaf3, &leaf4, &leaf5}));
    EXPECT_TRUE(set_equals(root.getAllDependents(), {&root}));

    EXPECT_TRUE(set_equals(leaf1.getDirectDependencies(), {}));
    EXPECT_TRUE(set_equals(leaf1.getDirectDependents(), {&root}));
    EXPECT_TRUE(set_equals(leaf1.getAllDependencies(), {&leaf1}));
    EXPECT_TRUE(set_equals(leaf1.getAllDependents(), {&root, &leaf1}));

    EXPECT_TRUE(set_equals(leaf2.getDirectDependencies(), {}));
    EXPECT_TRUE(set_equals(leaf2.getDirectDependents(), {&root}));
    EXPECT_TRUE(set_equals(leaf2.getAllDependencies(), {&leaf2}));
    EXPECT_TRUE(set_equals(leaf2.getAllDependents(), {&root, &leaf2}));

    EXPECT_TRUE(set_equals(leaf3.getDirectDependencies(), {}));
    EXPECT_TRUE(set_equals(leaf3.getDirectDependents(), {&root}));
    EXPECT_TRUE(set_equals(leaf3.getAllDependencies(), {&leaf3}));
    EXPECT_TRUE(set_equals(leaf3.getAllDependents(), {&root, &leaf3}));

    EXPECT_TRUE(set_equals(leaf4.getDirectDependencies(), {}));
    EXPECT_TRUE(set_equals(leaf4.getDirectDependents(), {&root}));
    EXPECT_TRUE(set_equals(leaf4.getAllDependencies(), {&leaf4}));
    EXPECT_TRUE(set_equals(leaf4.getAllDependents(), {&root, &leaf4}));

    EXPECT_TRUE(set_equals(leaf5.getDirectDependencies(), {}));
    EXPECT_TRUE(set_equals(leaf5.getDirectDependents(), {&root}));
    EXPECT_TRUE(set_equals(leaf5.getAllDependencies(), {&leaf5}));
    EXPECT_TRUE(set_equals(leaf5.getAllDependents(), {&root, &leaf5}));
}

TEST(SoundNodeTest, ShallowDependencies4){
    auto root1 = BasicSoundNode{};
    auto root2 = BasicSoundNode{};
    auto leaf = BasicSoundNode{};
    
    EXPECT_FALSE(root1.hasDependency(&leaf));
    EXPECT_FALSE(root1.hasDependency(&root2));
    EXPECT_FALSE(root2.hasDependency(&leaf));
    EXPECT_FALSE(root2.hasDependency(&root1));

    EXPECT_FALSE(leaf.hasDependency(&root1));
    EXPECT_FALSE(leaf.hasDependency(&root2));

    EXPECT_TRUE(root1.canAddDependency(&leaf));
    EXPECT_TRUE(root2.canAddDependency(&leaf));

    root1.addDependency(&leaf);
    root2.addDependency(&leaf);
    
    EXPECT_TRUE(root1.hasDependency(&leaf));
    EXPECT_FALSE(root1.hasDependency(&root2));
    EXPECT_TRUE(root2.hasDependency(&leaf));
    EXPECT_FALSE(root2.hasDependency(&root1));

    EXPECT_FALSE(leaf.hasDependency(&root1));
    EXPECT_FALSE(leaf.hasDependency(&root2));

    EXPECT_TRUE(set_equals(root1.getDirectDependencies(), {&leaf}));
    EXPECT_TRUE(set_equals(root1.getDirectDependents(), {}));
    EXPECT_TRUE(set_equals(root1.getAllDependencies(), {&root1, &leaf}));
    EXPECT_TRUE(set_equals(root1.getAllDependents(), {&root1}));

    EXPECT_TRUE(set_equals(root2.getDirectDependencies(), {&leaf}));
    EXPECT_TRUE(set_equals(root2.getDirectDependents(), {}));
    EXPECT_TRUE(set_equals(root2.getAllDependencies(), {&root2, &leaf}));
    EXPECT_TRUE(set_equals(root2.getAllDependents(), {&root2}));

    EXPECT_TRUE(set_equals(leaf.getDirectDependencies(), {}));
    EXPECT_TRUE(set_equals(leaf.getDirectDependents(), {&root1, &root2}));
    EXPECT_TRUE(set_equals(leaf.getAllDependencies(), {&leaf}));
    EXPECT_TRUE(set_equals(leaf.getAllDependents(), {&root1, &root2, &leaf}));
}

TEST(SoundNodeTest, ShallowDependencies5){
    auto root1 = BasicSoundNode{};
    auto root2 = BasicSoundNode{};
    auto root3 = BasicSoundNode{};
    auto root4 = BasicSoundNode{};
    auto root5 = BasicSoundNode{};
    auto leaf = BasicSoundNode{};
    
    EXPECT_FALSE(root1.hasDependency(&leaf));
    EXPECT_FALSE(root1.hasDependency(&root2));
    EXPECT_FALSE(root1.hasDependency(&root3));
    EXPECT_FALSE(root1.hasDependency(&root4));
    EXPECT_FALSE(root1.hasDependency(&root5));
    
    EXPECT_FALSE(root2.hasDependency(&leaf));
    EXPECT_FALSE(root2.hasDependency(&root1));
    EXPECT_FALSE(root2.hasDependency(&root3));
    EXPECT_FALSE(root2.hasDependency(&root4));
    EXPECT_FALSE(root2.hasDependency(&root5));
    
    EXPECT_FALSE(root3.hasDependency(&leaf));
    EXPECT_FALSE(root3.hasDependency(&root1));
    EXPECT_FALSE(root3.hasDependency(&root2));
    EXPECT_FALSE(root3.hasDependency(&root4));
    EXPECT_FALSE(root3.hasDependency(&root5));
    
    EXPECT_FALSE(root4.hasDependency(&leaf));
    EXPECT_FALSE(root4.hasDependency(&root1));
    EXPECT_FALSE(root4.hasDependency(&root2));
    EXPECT_FALSE(root4.hasDependency(&root3));
    EXPECT_FALSE(root4.hasDependency(&root5));
    
    EXPECT_FALSE(root5.hasDependency(&leaf));
    EXPECT_FALSE(root5.hasDependency(&root1));
    EXPECT_FALSE(root5.hasDependency(&root2));
    EXPECT_FALSE(root5.hasDependency(&root3));
    EXPECT_FALSE(root5.hasDependency(&root4));

    EXPECT_FALSE(leaf.hasDependency(&root1));
    EXPECT_FALSE(leaf.hasDependency(&root2));
    EXPECT_FALSE(leaf.hasDependency(&root3));
    EXPECT_FALSE(leaf.hasDependency(&root4));
    EXPECT_FALSE(leaf.hasDependency(&root5));

    EXPECT_TRUE(root1.canAddDependency(&leaf));
    EXPECT_TRUE(root2.canAddDependency(&leaf));
    EXPECT_TRUE(root3.canAddDependency(&leaf));
    EXPECT_TRUE(root4.canAddDependency(&leaf));
    EXPECT_TRUE(root5.canAddDependency(&leaf));

    root1.addDependency(&leaf);
    root2.addDependency(&leaf);
    root3.addDependency(&leaf);
    root4.addDependency(&leaf);
    root5.addDependency(&leaf);
    
    EXPECT_TRUE(root1.hasDependency(&leaf));
    EXPECT_FALSE(root1.hasDependency(&root2));
    EXPECT_FALSE(root1.hasDependency(&root3));
    EXPECT_FALSE(root1.hasDependency(&root4));
    EXPECT_FALSE(root1.hasDependency(&root5));
    
    EXPECT_TRUE(root2.hasDependency(&leaf));
    EXPECT_FALSE(root2.hasDependency(&root1));
    EXPECT_FALSE(root2.hasDependency(&root3));
    EXPECT_FALSE(root2.hasDependency(&root4));
    EXPECT_FALSE(root2.hasDependency(&root5));
    
    EXPECT_TRUE(root3.hasDependency(&leaf));
    EXPECT_FALSE(root3.hasDependency(&root1));
    EXPECT_FALSE(root3.hasDependency(&root2));
    EXPECT_FALSE(root3.hasDependency(&root4));
    EXPECT_FALSE(root3.hasDependency(&root5));
    
    EXPECT_TRUE(root4.hasDependency(&leaf));
    EXPECT_FALSE(root4.hasDependency(&root1));
    EXPECT_FALSE(root4.hasDependency(&root2));
    EXPECT_FALSE(root4.hasDependency(&root3));
    EXPECT_FALSE(root4.hasDependency(&root5));
    
    EXPECT_TRUE(root5.hasDependency(&leaf));
    EXPECT_FALSE(root5.hasDependency(&root1));
    EXPECT_FALSE(root5.hasDependency(&root2));
    EXPECT_FALSE(root5.hasDependency(&root3));
    EXPECT_FALSE(root5.hasDependency(&root4));

    EXPECT_FALSE(leaf.hasDependency(&root1));
    EXPECT_FALSE(leaf.hasDependency(&root2));
    EXPECT_FALSE(leaf.hasDependency(&root3));
    EXPECT_FALSE(leaf.hasDependency(&root4));
    EXPECT_FALSE(leaf.hasDependency(&root5));

    EXPECT_TRUE(set_equals(root1.getDirectDependencies(), {&leaf}));
    EXPECT_TRUE(set_equals(root1.getDirectDependents(), {}));
    EXPECT_TRUE(set_equals(root1.getAllDependencies(), {&root1, &leaf}));
    EXPECT_TRUE(set_equals(root1.getAllDependents(), {&root1}));

    EXPECT_TRUE(set_equals(root2.getDirectDependencies(), {&leaf}));
    EXPECT_TRUE(set_equals(root2.getDirectDependents(), {}));
    EXPECT_TRUE(set_equals(root2.getAllDependencies(), {&root2, &leaf}));
    EXPECT_TRUE(set_equals(root2.getAllDependents(), {&root2}));

    EXPECT_TRUE(set_equals(root3.getDirectDependencies(), {&leaf}));
    EXPECT_TRUE(set_equals(root3.getDirectDependents(), {}));
    EXPECT_TRUE(set_equals(root3.getAllDependencies(), {&root3, &leaf}));
    EXPECT_TRUE(set_equals(root3.getAllDependents(), {&root3}));

    EXPECT_TRUE(set_equals(root4.getDirectDependencies(), {&leaf}));
    EXPECT_TRUE(set_equals(root4.getDirectDependents(), {}));
    EXPECT_TRUE(set_equals(root4.getAllDependencies(), {&root4, &leaf}));
    EXPECT_TRUE(set_equals(root4.getAllDependents(), {&root4}));

    EXPECT_TRUE(set_equals(root5.getDirectDependencies(), {&leaf}));
    EXPECT_TRUE(set_equals(root5.getDirectDependents(), {}));
    EXPECT_TRUE(set_equals(root5.getAllDependencies(), {&root5, &leaf}));
    EXPECT_TRUE(set_equals(root5.getAllDependents(), {&root5}));

    EXPECT_TRUE(set_equals(leaf.getDirectDependencies(), {}));
    EXPECT_TRUE(set_equals(leaf.getDirectDependents(), {&root1, &root2, &root3, &root4, &root5}));
    EXPECT_TRUE(set_equals(leaf.getAllDependencies(), {&leaf}));
    EXPECT_TRUE(set_equals(leaf.getAllDependents(), {&leaf, &root1, &root2, &root3, &root4, &root5}));
}

TEST(SoundNodeTest, DeepDependencies1){
    auto root = BasicSoundNode{};
    auto inner = BasicSoundNode{};
    auto leaf = BasicSoundNode{};

    EXPECT_FALSE(root.hasDependency(&inner));
    EXPECT_FALSE(root.hasDependency(&leaf));
    EXPECT_FALSE(inner.hasDependency(&root));
    EXPECT_FALSE(inner.hasDependency(&leaf));
    EXPECT_FALSE(leaf.hasDependency(&root));
    EXPECT_FALSE(leaf.hasDependency(&inner));

    EXPECT_TRUE(root.canAddDependency(&inner));
    root.addDependency(&inner);

    EXPECT_TRUE(root.hasDependency(&inner));
    EXPECT_FALSE(root.hasDependency(&leaf));
    EXPECT_FALSE(inner.hasDependency(&root));
    EXPECT_FALSE(inner.hasDependency(&leaf));
    EXPECT_FALSE(leaf.hasDependency(&root));
    EXPECT_FALSE(leaf.hasDependency(&inner));

    EXPECT_TRUE(inner.canAddDependency(&leaf));
    inner.addDependency(&leaf);

    EXPECT_TRUE(root.hasDependency(&inner));
    EXPECT_TRUE(root.hasDependency(&leaf));
    EXPECT_FALSE(inner.hasDependency(&root));
    EXPECT_TRUE(inner.hasDependency(&leaf));
    EXPECT_FALSE(leaf.hasDependency(&root));
    EXPECT_FALSE(leaf.hasDependency(&inner));
    
    EXPECT_TRUE(set_equals(root.getAllDependencies(), {&root, &inner, &leaf}));
    EXPECT_TRUE(set_equals(inner.getAllDependencies(), {&inner, &leaf}));
    EXPECT_TRUE(set_equals(leaf.getAllDependencies(), {&leaf}));
    
    EXPECT_TRUE(set_equals(root.getAllDependents(), {&root}));
    EXPECT_TRUE(set_equals(inner.getAllDependents(), {&root, &inner}));
    EXPECT_TRUE(set_equals(leaf.getAllDependents(), {&root, &inner, &leaf}));
    
    EXPECT_TRUE(set_equals(root.getDirectDependencies(), {&inner}));
    EXPECT_TRUE(set_equals(inner.getDirectDependencies(), {&leaf}));
    EXPECT_TRUE(set_equals(leaf.getDirectDependencies(), {}));
    
    EXPECT_TRUE(set_equals(root.getDirectDependents(), {}));
    EXPECT_TRUE(set_equals(inner.getDirectDependents(), {&root}));
    EXPECT_TRUE(set_equals(leaf.getDirectDependents(), {&inner}));
}

TEST(SoundNodeTest, DeepDependencies2){
    auto root = BasicSoundNode{};
    auto inner1 = BasicSoundNode{};
    auto inner2 = BasicSoundNode{};
    auto inner3 = BasicSoundNode{};
    auto leaf = BasicSoundNode{};
    
    EXPECT_FALSE(root.hasDependency(&inner1));
    EXPECT_FALSE(root.hasDependency(&inner2));
    EXPECT_FALSE(root.hasDependency(&inner3));
    EXPECT_FALSE(root.hasDependency(&leaf));

    EXPECT_FALSE(inner1.hasDependency(&root));
    EXPECT_FALSE(inner1.hasDependency(&inner2));
    EXPECT_FALSE(inner1.hasDependency(&inner3));
    EXPECT_FALSE(inner1.hasDependency(&leaf));

    EXPECT_FALSE(inner2.hasDependency(&root));
    EXPECT_FALSE(inner2.hasDependency(&inner1));
    EXPECT_FALSE(inner2.hasDependency(&inner3));
    EXPECT_FALSE(inner2.hasDependency(&leaf));

    EXPECT_FALSE(inner3.hasDependency(&root));
    EXPECT_FALSE(inner3.hasDependency(&inner1));
    EXPECT_FALSE(inner3.hasDependency(&inner2));
    EXPECT_FALSE(inner3.hasDependency(&leaf));

    EXPECT_FALSE(leaf.hasDependency(&root));
    EXPECT_FALSE(leaf.hasDependency(&inner1));
    EXPECT_FALSE(leaf.hasDependency(&inner2));
    EXPECT_FALSE(leaf.hasDependency(&inner3));

    EXPECT_TRUE(root.canAddDependency(&inner1));
    root.addDependency(&inner1);

    EXPECT_TRUE(inner1.canAddDependency(&inner2));
    inner1.addDependency(&inner2);

    EXPECT_TRUE(inner2.canAddDependency(&inner3));
    inner2.addDependency(&inner3);

    EXPECT_TRUE(inner3.canAddDependency(&leaf));
    inner3.addDependency(&leaf);
    
    EXPECT_TRUE(root.hasDependency(&inner1));
    EXPECT_TRUE(root.hasDependency(&inner2));
    EXPECT_TRUE(root.hasDependency(&inner3));
    EXPECT_TRUE(root.hasDependency(&leaf));

    EXPECT_FALSE(inner1.hasDependency(&root));
    EXPECT_TRUE(inner1.hasDependency(&inner2));
    EXPECT_TRUE(inner1.hasDependency(&inner3));
    EXPECT_TRUE(inner1.hasDependency(&leaf));

    EXPECT_FALSE(inner2.hasDependency(&root));
    EXPECT_FALSE(inner2.hasDependency(&inner1));
    EXPECT_TRUE(inner2.hasDependency(&inner3));
    EXPECT_TRUE(inner2.hasDependency(&leaf));

    EXPECT_FALSE(inner3.hasDependency(&root));
    EXPECT_FALSE(inner3.hasDependency(&inner1));
    EXPECT_FALSE(inner3.hasDependency(&inner2));
    EXPECT_TRUE(inner3.hasDependency(&leaf));

    EXPECT_FALSE(leaf.hasDependency(&root));
    EXPECT_FALSE(leaf.hasDependency(&inner1));
    EXPECT_FALSE(leaf.hasDependency(&inner2));
    EXPECT_FALSE(leaf.hasDependency(&inner3));
    
    EXPECT_TRUE(set_equals(root.getAllDependencies(), {&root, &inner1, &inner2, &inner3, &leaf}));
    EXPECT_TRUE(set_equals(inner1.getAllDependencies(), {&inner1, &inner2, &inner3, &leaf}));
    EXPECT_TRUE(set_equals(inner2.getAllDependencies(), {&inner2, &inner3, &leaf}));
    EXPECT_TRUE(set_equals(inner3.getAllDependencies(), {&inner3, &leaf}));
    EXPECT_TRUE(set_equals(leaf.getAllDependencies(), {&leaf}));
    
    EXPECT_TRUE(set_equals(root.getAllDependents(), {&root}));
    EXPECT_TRUE(set_equals(inner1.getAllDependents(), {&root, &inner1}));
    EXPECT_TRUE(set_equals(inner2.getAllDependents(), {&root, &inner1, &inner2}));
    EXPECT_TRUE(set_equals(inner3.getAllDependents(), {&root, &inner1, &inner2, &inner3}));
    EXPECT_TRUE(set_equals(leaf.getAllDependents(), {&root, &inner1, &inner2, &inner3, &leaf}));
    
    EXPECT_TRUE(set_equals(root.getDirectDependencies(), {&inner1}));
    EXPECT_TRUE(set_equals(inner1.getDirectDependencies(), {&inner2}));
    EXPECT_TRUE(set_equals(inner2.getDirectDependencies(), {&inner3}));
    EXPECT_TRUE(set_equals(inner3.getDirectDependencies(), {&leaf}));
    EXPECT_TRUE(set_equals(leaf.getDirectDependencies(), {}));
    
    EXPECT_TRUE(set_equals(root.getDirectDependents(), {}));
    EXPECT_TRUE(set_equals(inner1.getDirectDependents(), {&root}));
    EXPECT_TRUE(set_equals(inner2.getDirectDependents(), {&inner1}));
    EXPECT_TRUE(set_equals(inner3.getDirectDependents(), {&inner2}));
    EXPECT_TRUE(set_equals(leaf.getDirectDependents(), {&inner3}));
}

TEST(SoundNodeTest, CrissCross1){
    auto root = BasicSoundNode{};
    auto inner1 = BasicSoundNode{};
    auto inner2 = BasicSoundNode{};
    auto leaf = BasicSoundNode{};

    EXPECT_FALSE(root.hasDependency(&inner1));
    EXPECT_FALSE(root.hasDependency(&inner2));
    EXPECT_FALSE(root.hasDependency(&leaf));

    EXPECT_FALSE(inner1.hasDependency(&root));
    EXPECT_FALSE(inner1.hasDependency(&inner2));
    EXPECT_FALSE(inner1.hasDependency(&leaf));

    EXPECT_FALSE(inner2.hasDependency(&root));
    EXPECT_FALSE(inner2.hasDependency(&inner1));
    EXPECT_FALSE(inner2.hasDependency(&leaf));

    EXPECT_FALSE(leaf.hasDependency(&root));
    EXPECT_FALSE(leaf.hasDependency(&inner1));
    EXPECT_FALSE(leaf.hasDependency(&inner2));

    EXPECT_TRUE(root.canAddDependency(&inner1));
    EXPECT_TRUE(root.canAddDependency(&inner2));

    root.addDependency(&inner1);
    root.addDependency(&inner2);

    EXPECT_TRUE(inner1.canAddDependency(&leaf));
    EXPECT_TRUE(inner2.canAddDependency(&leaf));
    
    inner1.addDependency(&leaf);
    inner2.addDependency(&leaf);

    EXPECT_TRUE(set_equals(root.getDirectDependencies(), {&inner1, &inner2}));
    EXPECT_TRUE(set_equals(root.getDirectDependents(), {}));
    EXPECT_TRUE(set_equals(root.getAllDependencies(), {&root, &inner1, &inner2, &leaf}));
    EXPECT_TRUE(set_equals(root.getAllDependents(), {&root}));

    EXPECT_TRUE(set_equals(inner1.getDirectDependencies(), {&leaf}));
    EXPECT_TRUE(set_equals(inner1.getDirectDependents(), {&root}));
    EXPECT_TRUE(set_equals(inner1.getAllDependencies(), {&inner1, &leaf}));
    EXPECT_TRUE(set_equals(inner1.getAllDependents(), {&inner1, &root}));

    EXPECT_TRUE(set_equals(inner2.getDirectDependencies(), {&leaf}));
    EXPECT_TRUE(set_equals(inner2.getDirectDependents(), {&root}));
    EXPECT_TRUE(set_equals(inner2.getAllDependencies(), {&inner2, &leaf}));
    EXPECT_TRUE(set_equals(inner2.getAllDependents(), {&inner2, &root}));

    EXPECT_TRUE(set_equals(leaf.getDirectDependencies(), {}));
    EXPECT_TRUE(set_equals(leaf.getDirectDependents(), {&inner1, &inner2}));
    EXPECT_TRUE(set_equals(leaf.getAllDependencies(), {&leaf}));
    EXPECT_TRUE(set_equals(leaf.getAllDependents(), {&leaf, &inner1, &inner2, &root}));
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

TEST(SoundNodeTest, DependencySafety1){
    auto root = NumberSoundNode{};
    auto leaf = NumberSoundNode{};

    EXPECT_TRUE(root.canAddDependency(&leaf));
    EXPECT_TRUE(leaf.canAddDependency(&root));
    
    EXPECT_EQ(root.node.getStateOwner(), &root);
    EXPECT_EQ(leaf.node.getStateOwner(), &leaf);
    
    EXPECT_FALSE(root.hasDependency(&leaf));
    EXPECT_FALSE(leaf.hasDependency(&root));
    
    EXPECT_TRUE(set_disjoint(root.node.getDirectDependencies(), {&root.node, &leaf.node}));
    EXPECT_TRUE(set_disjoint(leaf.node.getDirectDependencies(), {&root.node, &leaf.node}));

    EXPECT_TRUE(set_disjoint(root.node.getDirectDependents(), {&root.node, &leaf.node}));
    EXPECT_TRUE(set_disjoint(leaf.node.getDirectDependents(), {&root.node, &leaf.node}));
    
    EXPECT_TRUE(set_contains(root.node.getAllDependencies(), {&root.node}));
    EXPECT_TRUE(set_disjoint(root.node.getAllDependencies(), {&leaf.node}));
    EXPECT_TRUE(set_contains(leaf.node.getAllDependencies(), {&leaf.node}));
    EXPECT_TRUE(set_disjoint(leaf.node.getAllDependencies(), {&root.node}));

    EXPECT_TRUE(set_contains(root.node.getAllDependents(), {&root.node}));
    EXPECT_TRUE(set_disjoint(root.node.getAllDependents(), {&leaf.node}));
    EXPECT_TRUE(set_contains(leaf.node.getAllDependents(), {&leaf.node}));
    EXPECT_TRUE(set_disjoint(leaf.node.getAllDependents(), {&root.node}));

    EXPECT_FALSE(root.node.canAddDependency(&root.node));
    EXPECT_FALSE(leaf.node.canAddDependency(&leaf.node));
    EXPECT_FALSE(root.node.canAddDependency(&leaf.node));
    EXPECT_FALSE(leaf.node.canAddDependency(&root.node));

    EXPECT_TRUE(root.canAddDependency(&leaf));
    root.addDependency(&leaf);
    
    EXPECT_FALSE(root.node.canAddDependency(&root.node));
    EXPECT_FALSE(leaf.node.canAddDependency(&leaf.node));
    EXPECT_FALSE(root.node.canAddDependency(&leaf.node));
    EXPECT_TRUE(leaf.node.canAddDependency(&root.node));
    
    EXPECT_FALSE(root.canAddDependency(&leaf));
    EXPECT_FALSE(leaf.canAddDependency(&root));
    EXPECT_TRUE(root.canRemoveDependency(&leaf));
    EXPECT_FALSE(leaf.canRemoveDependency(&root));
    
    leaf.node.addDependency(&root.node);
    
    EXPECT_FALSE(root.canAddDependency(&leaf));
    EXPECT_FALSE(leaf.canAddDependency(&root));
    EXPECT_FALSE(root.canRemoveDependency(&leaf));
    EXPECT_FALSE(leaf.canRemoveDependency(&root));
    
    EXPECT_TRUE(set_disjoint(root.node.getDirectDependencies(), {&root.node, &leaf.node}));
    EXPECT_TRUE(set_contains(leaf.node.getDirectDependencies(), {&root.node}));
    EXPECT_TRUE(set_disjoint(leaf.node.getDirectDependencies(), {&leaf.node}));
    
    EXPECT_TRUE(set_contains(root.node.getDirectDependents(), {&leaf.node}));
    EXPECT_TRUE(set_disjoint(root.node.getDirectDependents(), {&root.node}));
    EXPECT_TRUE(set_disjoint(leaf.node.getDirectDependents(), {&root.node, &leaf.node}));
    
    EXPECT_TRUE(set_contains(root.node.getAllDependencies(), {&root.node}));
    EXPECT_TRUE(set_disjoint(root.node.getAllDependencies(), {&leaf.node}));
    EXPECT_TRUE(set_contains(leaf.node.getAllDependencies(), {&root.node, &leaf.node}));

    EXPECT_TRUE(set_contains(root.node.getAllDependents(), {&root.node, &leaf.node}));
    EXPECT_TRUE(set_contains(leaf.node.getAllDependents(), {&leaf.node}));
    EXPECT_TRUE(set_disjoint(leaf.node.getAllDependents(), {&root.node}));
    
    EXPECT_FALSE(root.node.canAddDependency(&root.node));
    EXPECT_FALSE(leaf.node.canAddDependency(&leaf.node));
    EXPECT_FALSE(root.node.canAddDependency(&leaf.node));
    EXPECT_FALSE(leaf.node.canAddDependency(&root.node));
}

TEST(SoundNodeTest, DependencySafety2){
    auto root1 = NumberSoundNode{};
    auto root2 = NumberSoundNode{};
    auto leaf = NumberSoundNode{};

    root1.addDependency(&leaf);
    
    EXPECT_TRUE(root1.canRemoveDependency(&leaf));

    leaf.node.addDependency(&root1.node);

    EXPECT_FALSE(root1.canRemoveDependency(&leaf));

    EXPECT_FALSE(root2.canAddDependency(&leaf));
}

// TODO: uncontrolled soundnodes and dependency safety