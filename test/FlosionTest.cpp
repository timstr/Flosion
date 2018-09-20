#include "gtest/gtest.h"
#include <iostream>

TEST(TestTest, Test1){
	ASSERT_EQ(1, 1);
	EXPECT_EQ(1, 0);
	ASSERT_EQ(1, 0);
}

int main(int argc, char** argv){
	::testing::InitGoogleTest(&argc, argv);
	const auto res = RUN_ALL_TESTS();
	char ch;
	std::cin >> ch;
	return res;
}