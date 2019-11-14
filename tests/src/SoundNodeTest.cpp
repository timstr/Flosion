#include <Flosion/Core/SoundSourceTemplate.hpp>

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

template<typename Base>
class Named : public Base {
public:
    Named(std::string n) : name(std::move(n)) {}

    const std::string name;
};

using BasicSoundNode = Named<Realtime<Singular<SoundNode, EmptySoundState>>>;
using DivergentSoundNode = Named<Realtime<Divergent<SoundNode, EmptySoundState, int>>>;

TEST(SoundNodeTest, ShallowDependencies1){
    auto root = BasicSoundNode{"root"};
    auto leaf = BasicSoundNode{"leaf"};
    
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
    auto root = BasicSoundNode{"root"};
    auto leaf1 = BasicSoundNode{"leaf1"};
    auto leaf2 = BasicSoundNode{"leaf2"};
    
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
    auto root = BasicSoundNode{"root"};
    auto leaf1 = BasicSoundNode{"leaf1"};
    auto leaf2 = BasicSoundNode{"leaf2"};
    auto leaf3 = BasicSoundNode{"leaf3"};
    auto leaf4 = BasicSoundNode{"leaf4"};
    auto leaf5 = BasicSoundNode{"leaf5"};
    
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
    auto root1 = BasicSoundNode{"root1"};
    auto root2 = BasicSoundNode{"root2"};
    auto leaf = BasicSoundNode{"leaf"};
    
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
    auto root1 = BasicSoundNode{"root1"};
    auto root2 = BasicSoundNode{"root2"};
    auto root3 = BasicSoundNode{"root3"};
    auto root4 = BasicSoundNode{"root4"};
    auto root5 = BasicSoundNode{"root5"};
    auto leaf = BasicSoundNode{"leaf"};
    
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
    auto root = BasicSoundNode{"root"};
    auto inner = BasicSoundNode{"inner"};
    auto leaf = BasicSoundNode{"leaf"};

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
    auto root = BasicSoundNode{"root"};
    auto inner1 = BasicSoundNode{"inner1"};
    auto inner2 = BasicSoundNode{"inner2"};
    auto inner3 = BasicSoundNode{"inner3"};
    auto leaf = BasicSoundNode{"leaf"};
    
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
    auto root = BasicSoundNode{"root"};
    auto inner1 = BasicSoundNode{"inner1"};
    auto inner2 = BasicSoundNode{"inner2"};
    auto leaf = BasicSoundNode{"leaf"};

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
    NumberSoundNode(std::string name)
        : BasicSoundNode(std::move(name))
        , node(this){
    
    }

    class SoundNumberNode : public NumberNode {
    public:
        SoundNumberNode(NumberSoundNode* parent){
            setStateOwner(parent);
        }
    } node;
};

TEST(SoundNodeTest, DependencySafety1){
    auto root = NumberSoundNode{"root"};
    auto leaf = NumberSoundNode{"leaf"};

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
    auto root1 = NumberSoundNode{"root1"};
    auto root2 = NumberSoundNode{"root2"};
    auto leaf = NumberSoundNode{"leaf"};

    root1.addDependency(&leaf);
    
    EXPECT_TRUE(root1.canRemoveDependency(&leaf));
    
    ASSERT_TRUE(leaf.node.canAddDependency(&root1.node));

    leaf.node.addDependency(&root1.node);
    
    EXPECT_TRUE(leaf.node.canRemoveDependency(&root1.node));

    EXPECT_FALSE(root1.canRemoveDependency(&leaf));

    EXPECT_FALSE(root2.canAddDependency(&leaf));

    EXPECT_FALSE(leaf.node.canAddDependency(&root2.node));
}

TEST(SoundNodeTest, DependencySafety3){
    auto root1 = NumberSoundNode{"root1"};
    auto root2 = NumberSoundNode{"root2"};
    auto leaf = NumberSoundNode{"leaf"};
    
    root1.addDependency(&leaf);
    root2.addDependency(&leaf);
    
    EXPECT_TRUE(root1.canRemoveDependency(&leaf));
    EXPECT_TRUE(root2.canRemoveDependency(&leaf));
    
    EXPECT_FALSE(leaf.node.canAddDependency(&root1.node));
    EXPECT_FALSE(leaf.node.canAddDependency(&root2.node));
}

TEST(SoundNodeTest, DependencySafety4){
    auto root = NumberSoundNode{"root"};
    auto inner1 = NumberSoundNode{"inner1"};
    auto inner2 = NumberSoundNode{"inner2"};
    auto leaf = NumberSoundNode{"leaf"};

    root.addDependency(&inner1);
    root.addDependency(&inner2);
    inner1.addDependency(&leaf);
    inner2.addDependency(&leaf);
    
    EXPECT_TRUE(inner1.canRemoveDependency(&leaf));
    EXPECT_TRUE(inner2.canRemoveDependency(&leaf));

    leaf.node.addDependency(&root.node);
    
    EXPECT_TRUE(root.canRemoveDependency(&inner1));
    EXPECT_TRUE(root.canRemoveDependency(&inner2));
    EXPECT_TRUE(inner1.canRemoveDependency(&leaf));
    EXPECT_TRUE(inner2.canRemoveDependency(&leaf));

    inner2.removeDependency(&leaf);
    
    EXPECT_FALSE(root.canRemoveDependency(&inner1));
    EXPECT_TRUE(root.canRemoveDependency(&inner2));
    EXPECT_FALSE(inner1.canRemoveDependency(&leaf));
    EXPECT_TRUE(inner2.canAddDependency(&leaf));

    inner2.addDependency(&leaf);
    
    EXPECT_TRUE(root.canRemoveDependency(&inner1));
    EXPECT_TRUE(root.canRemoveDependency(&inner2));
    EXPECT_TRUE(inner1.canRemoveDependency(&leaf));
    EXPECT_TRUE(inner2.canRemoveDependency(&leaf));

    inner1.removeDependency(&leaf);
    
    EXPECT_TRUE(root.canRemoveDependency(&inner1));
    EXPECT_FALSE(root.canRemoveDependency(&inner2));
    EXPECT_TRUE(inner1.canAddDependency(&leaf));
    EXPECT_FALSE(inner2.canRemoveDependency(&leaf));
}

TEST(SoundNodeTest, DependencySafety5){
    auto root = NumberSoundNode{"root"};
    auto innera1 = NumberSoundNode{"innera1"};
    auto innera2 = NumberSoundNode{"innera2"};
    auto innerb1 = NumberSoundNode{"innerb1"};
    auto innerb2 = NumberSoundNode{"innerb2"};
    auto leaf = NumberSoundNode{"leaf"};

    root.addDependency(&innera1);
    root.addDependency(&innera2);
    innera1.addDependency(&innerb1);
    innera2.addDependency(&innerb2);
    innerb1.addDependency(&leaf);
    innerb2.addDependency(&leaf);
    
    EXPECT_TRUE(innerb1.canRemoveDependency(&leaf));
    EXPECT_TRUE(innerb2.canRemoveDependency(&leaf));
    EXPECT_TRUE(innera1.canRemoveDependency(&innerb1));
    EXPECT_TRUE(innera2.canRemoveDependency(&innerb2));
    EXPECT_TRUE(root.canRemoveDependency(&innera1));
    EXPECT_TRUE(root.canRemoveDependency(&innera2));

    leaf.node.addDependency(&root.node);
    
    EXPECT_TRUE(innerb1.canRemoveDependency(&leaf));
    EXPECT_TRUE(innerb2.canRemoveDependency(&leaf));
    EXPECT_TRUE(innera1.canRemoveDependency(&innerb1));
    EXPECT_TRUE(innera2.canRemoveDependency(&innerb2));
    EXPECT_TRUE(root.canRemoveDependency(&innera1));
    EXPECT_TRUE(root.canRemoveDependency(&innera2));

    innerb2.removeDependency(&leaf);
    
    EXPECT_FALSE(innerb1.canRemoveDependency(&leaf));
    EXPECT_TRUE(innerb2.canAddDependency(&leaf));
    EXPECT_FALSE(innera1.canRemoveDependency(&innerb1));
    EXPECT_TRUE(innera2.canRemoveDependency(&innerb2));
    EXPECT_FALSE(root.canRemoveDependency(&innera1));
    EXPECT_TRUE(root.canRemoveDependency(&innera2));

    innerb2.addDependency(&leaf);
    
    EXPECT_TRUE(innerb1.canRemoveDependency(&leaf));
    EXPECT_TRUE(innerb2.canRemoveDependency(&leaf));
    EXPECT_TRUE(innera1.canRemoveDependency(&innerb1));
    EXPECT_TRUE(innera2.canRemoveDependency(&innerb2));
    EXPECT_TRUE(root.canRemoveDependency(&innera1));
    EXPECT_TRUE(root.canRemoveDependency(&innera2));
}

using BasicUncontrolled = Named<Realtime<Uncontrolled<SoundNode, EmptySoundState>>>;

TEST(SoundNodeTest, Uncontrolled1){
    auto uncontrolled = BasicUncontrolled{"uncontrolled node"};
    
    auto singular = BasicSoundNode{"singular"};
    auto divergent = DivergentSoundNode{"divergent"};
    
    ASSERT_TRUE(singular.canAddDependency(&uncontrolled));
    ASSERT_FALSE(divergent.canAddDependency(&uncontrolled));
    
    EXPECT_FALSE(singular.hasUncontrolledDependency());
    EXPECT_FALSE(divergent.hasUncontrolledDependency());
    EXPECT_TRUE(uncontrolled.hasUncontrolledDependency());

    singular.addDependency(&uncontrolled);

    EXPECT_TRUE(singular.hasUncontrolledDependency());
    EXPECT_TRUE(uncontrolled.hasUncontrolledDependency());

    ASSERT_FALSE(divergent.canAddDependency(&singular));
    
    EXPECT_TRUE(uncontrolled.canAddDependency(&divergent));
    uncontrolled.addDependency(&divergent);

    auto singular2 = BasicSoundNode{"singular"};
    EXPECT_TRUE(uncontrolled.canAddDependency(&singular2));
    uncontrolled.addDependency(&singular2);

    EXPECT_FALSE(divergent.hasUncontrolledDependency());
    EXPECT_FALSE(singular2.hasUncontrolledDependency());
}

TEST(SoundNodeTest, Uncontrolled2){
    auto root1 = BasicUncontrolled{"uncontrolled 1"};
    auto root2 = BasicUncontrolled{"uncontrolled 2"};

    EXPECT_EQ(root1.numSlots(), 1);
    EXPECT_EQ(root2.numSlots(), 1);

    {
        auto leaf = BasicSoundNode{"basic"};

        EXPECT_EQ(leaf.numSlots(), 0);

        root1.addDependency(&leaf);

        EXPECT_EQ(leaf.numSlots(), 1);

        root2.addDependency(&leaf);

        EXPECT_EQ(leaf.numSlots(), 2);
    }
}

TEST(SoundNodeTest, Uncontrolled3){
    auto root = BasicUncontrolled{"uncontrolled"};

    EXPECT_EQ(root.numSlots(), 1);

    {
        auto leaf = DivergentSoundNode{"divergent 0"};

        EXPECT_EQ(leaf.numSlots(), 0);

        root.addDependency(&leaf);

        EXPECT_EQ(leaf.numSlots(), 0);
    }

    ASSERT_EQ(root.getDirectDependencies().size(), 0);

    {
        auto leaf = DivergentSoundNode{"divergent 1"};
        leaf.addKey(1);

        EXPECT_EQ(leaf.numSlots(), 0);

        root.addDependency(&leaf);

        EXPECT_EQ(leaf.numSlots(), 1);
    }

    ASSERT_EQ(root.getDirectDependencies().size(), 0);

    {
        auto leaf = DivergentSoundNode{"divergent 2"};
        leaf.addKey(1);
        leaf.addKey(2);

        EXPECT_EQ(leaf.numSlots(), 0);

        root.addDependency(&leaf);

        EXPECT_EQ(leaf.numSlots(), 2);
    }

    ASSERT_EQ(root.getDirectDependencies().size(), 0);

    {
        auto leaf = DivergentSoundNode{"divergent 4"};
        leaf.addKey(1);
        leaf.addKey(2);
        leaf.addKey(3);
        leaf.addKey(4);

        EXPECT_EQ(leaf.numSlots(), 0);

        root.addDependency(&leaf);

        EXPECT_EQ(leaf.numSlots(), 4);

        leaf.removeKey(1);

        EXPECT_EQ(leaf.numSlots(), 3);

        leaf.removeKey(2);

        EXPECT_EQ(leaf.numSlots(), 2);

        leaf.removeKey(3);

        EXPECT_EQ(leaf.numSlots(), 1);

        leaf.removeKey(4);

        EXPECT_EQ(leaf.numSlots(), 0);
    }

    ASSERT_EQ(root.getDirectDependencies().size(), 0);
}

TEST(SoundNodeTest, Uncontrolled4){
    auto root1 = BasicUncontrolled{"uncontrolled root 1"};
    auto root2 = BasicUncontrolled{"uncontrolled root 2"};

    EXPECT_EQ(root1.numSlots(), 1);
    EXPECT_EQ(root2.numSlots(), 1);

    {
        auto leaf = DivergentSoundNode{"divergent leaf"};

        leaf.addKey(1);
        leaf.addKey(2);

        EXPECT_EQ(leaf.numSlots(), 0);

        root1.addDependency(&leaf);

        EXPECT_EQ(leaf.numSlots(), 2);

        root2.addDependency(&leaf);

        EXPECT_EQ(leaf.numSlots(), 4);

        {
            const auto r1k1 = leaf.getState(&root1, root1.getMonoState(), 1);
            const auto r1k2 = leaf.getState(&root1, root1.getMonoState(), 2);
            const auto r2k1 = leaf.getState(&root2, root2.getMonoState(), 1);
            const auto r2k2 = leaf.getState(&root2, root2.getMonoState(), 2);
            EXPECT_EQ(r1k1->getDependentState(), root1.getMonoState());
            EXPECT_EQ(r1k2->getDependentState(), root1.getMonoState());
            EXPECT_EQ(r2k1->getDependentState(), root2.getMonoState());
            EXPECT_EQ(r2k2->getDependentState(), root2.getMonoState());
            EXPECT_EQ(r1k1->getOwner(), &leaf);
            EXPECT_EQ(r1k2->getOwner(), &leaf);
            EXPECT_EQ(r2k1->getOwner(), &leaf);
            EXPECT_EQ(r2k2->getOwner(), &leaf);
        }

        leaf.removeKey(2);

        EXPECT_EQ(leaf.numSlots(), 2);

        {
            const auto r1k1 = leaf.getState(&root1, root1.getMonoState(), 1);
            const auto r2k1 = leaf.getState(&root2, root2.getMonoState(), 1);
            EXPECT_EQ(r1k1->getDependentState(), root1.getMonoState());
            EXPECT_EQ(r2k1->getDependentState(), root2.getMonoState());
            EXPECT_EQ(r1k1->getOwner(), &leaf);
            EXPECT_EQ(r2k1->getOwner(), &leaf);
        }

        leaf.removeKey(1);

        EXPECT_EQ(leaf.numSlots(), 0);

        root1.removeDependency(&leaf);
        root2.removeDependency(&leaf);
    }
}

TEST(SoundNodeTest, Uncontrolled5){
    auto root1 = BasicUncontrolled{"uncontrolled root 1"};
    auto root2 = BasicUncontrolled{"uncontrolled root 2"};

    auto inner = BasicSoundNode{"basic inner"};

    EXPECT_EQ(root1.numSlots(), 1);
    EXPECT_EQ(root2.numSlots(), 1);

    
    EXPECT_EQ(inner.numSlots(), 0);
    root1.addDependency(&inner);
    EXPECT_EQ(inner.numSlots(), 1);
    root2.addDependency(&inner);
    EXPECT_EQ(inner.numSlots(), 2);

    {
        auto leaf = DivergentSoundNode{"divergent leaf"};

        leaf.addKey(1);
        leaf.addKey(2);

        EXPECT_EQ(leaf.numSlots(), 0);

        inner.addDependency(&leaf);

        EXPECT_EQ(leaf.numSlots(), 4);

        {
            const auto i1k1 = leaf.getState(&inner, inner.getState(0), 1);
            const auto i1k2 = leaf.getState(&inner, inner.getState(0), 2);
            const auto i2k1 = leaf.getState(&inner, inner.getState(1), 1);
            const auto i2k2 = leaf.getState(&inner, inner.getState(1), 2);
            EXPECT_EQ(i1k1->getDependentState(), inner.getState(0));
            EXPECT_EQ(i1k2->getDependentState(), inner.getState(0));
            EXPECT_EQ(i2k1->getDependentState(), inner.getState(1));
            EXPECT_EQ(i2k2->getDependentState(), inner.getState(1));
            EXPECT_EQ(i1k1->getOwner(), &leaf);
            EXPECT_EQ(i1k2->getOwner(), &leaf);
            EXPECT_EQ(i2k1->getOwner(), &leaf);
            EXPECT_EQ(i2k2->getOwner(), &leaf);
        }

        leaf.removeKey(2);

        EXPECT_EQ(leaf.numSlots(), 2);

        {
            const auto i1k1 = leaf.getState(&inner, inner.getState(0), 1);
            const auto i2k1 = leaf.getState(&inner, inner.getState(1), 1);
            EXPECT_EQ(i1k1->getDependentState(), inner.getState(0));
            EXPECT_EQ(i2k1->getDependentState(), inner.getState(1));
            EXPECT_EQ(i1k1->getOwner(), &leaf);
            EXPECT_EQ(i2k1->getOwner(), &leaf);
        }

        leaf.removeKey(1);

        EXPECT_EQ(leaf.numSlots(), 0);

        root1.removeDependency(&inner);
        root2.removeDependency(&inner);

        inner.removeDependency(&leaf);
    }
}