#include <gtest/gtest.h>

#include <string>

#include "FixedBlockMapResource.h"
#include "ForwardList.h"


// ========================================================================
// ТЕСТЫ ДЛЯ FixedBlockMapResource
// ========================================================================

TEST(FixedBlockMapResourceTest, CreateAndDestroy) {
    FixedBlockMapResource resource(1024);
    // Если дошли сюда без исключений - тест прошел
    SUCCEED();
}

TEST(FixedBlockMapResourceTest, AllocateMemory) {
    FixedBlockMapResource resource(1024);
    void* ptr = resource.allocate(10, alignof(int));
    ASSERT_NE(ptr, nullptr);
    resource.deallocate(ptr, 10, alignof(int));
}

TEST(FixedBlockMapResourceTest, AllocateMultipleBlocks) {
    FixedBlockMapResource resource(1024);
    void* ptr1 = resource.allocate(100, 1);
    void* ptr2 = resource.allocate(100, 1);
    void* ptr3 = resource.allocate(100, 1);

    ASSERT_NE(ptr1, nullptr);
    ASSERT_NE(ptr2, nullptr);
    ASSERT_NE(ptr3, nullptr);
    ASSERT_NE(ptr1, ptr2);
    ASSERT_NE(ptr2, ptr3);

    resource.deallocate(ptr1, 100, 1);
    resource.deallocate(ptr2, 100, 1);
    resource.deallocate(ptr3, 100, 1);
}

TEST(FixedBlockMapResourceTest, ReuseFreedMemory) {
    FixedBlockMapResource resource(1024);
    void* ptr1 = resource.allocate(50, 1);
    resource.deallocate(ptr1, 50, 1);

    void* ptr2 = resource.allocate(50, 1);
    // Должен переиспользовать тот же блок
    ASSERT_EQ(ptr1, ptr2);
    resource.deallocate(ptr2, 50, 1);
}

TEST(FixedBlockMapResourceTest, OutOfMemory) {
    FixedBlockMapResource resource(100);
    EXPECT_THROW({ resource.allocate(200, 1); }, std::bad_alloc);
}

// ========================================================================
// ТЕСТЫ ДЛЯ ForwardList с int
// ========================================================================

TEST(ForwardListIntTest, CreateEmpty) {
    FixedBlockMapResource resource(1024);
    ForwardList<int> list(&resource);

    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0);
}

TEST(ForwardListIntTest, PushFrontSingle) {
    FixedBlockMapResource resource(1024);
    ForwardList<int> list(&resource);

    list.push_front(42);

    EXPECT_FALSE(list.empty());
    EXPECT_EQ(list.size(), 1);
    EXPECT_EQ(list.front(), 42);
}

TEST(ForwardListIntTest, PushFrontMultiple) {
    FixedBlockMapResource resource(1024);
    ForwardList<int> list(&resource);

    list.push_front(10);
    list.push_front(20);
    list.push_front(30);

    EXPECT_EQ(list.size(), 3);
    EXPECT_EQ(list.front(), 30);  // Последний добавленный
}

TEST(ForwardListIntTest, PopFront) {
    FixedBlockMapResource resource(1024);
    ForwardList<int> list(&resource);

    list.push_front(10);
    list.push_front(20);
    list.push_front(30);

    list.pop_front();
    EXPECT_EQ(list.size(), 2);
    EXPECT_EQ(list.front(), 20);

    list.pop_front();
    EXPECT_EQ(list.size(), 1);
    EXPECT_EQ(list.front(), 10);
}

TEST(ForwardListIntTest, PopFrontEmpty) {
    FixedBlockMapResource resource(1024);
    ForwardList<int> list(&resource);

    EXPECT_THROW({ list.pop_front(); }, std::runtime_error);
}

TEST(ForwardListIntTest, Clear) {
    FixedBlockMapResource resource(1024);
    ForwardList<int> list(&resource);

    list.push_front(1);
    list.push_front(2);
    list.push_front(3);

    list.clear();
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0);
}

TEST(ForwardListIntTest, IteratorEmpty) {
    FixedBlockMapResource resource(1024);
    ForwardList<int> list(&resource);

    int count = 0;
    for (auto it = list.begin(); it != list.end(); ++it) {
        ++count;
    }
    EXPECT_EQ(count, 0);
}

TEST(ForwardListIntTest, IteratorTraversal) {
    FixedBlockMapResource resource(1024);
    ForwardList<int> list(&resource);

    list.push_front(1);
    list.push_front(2);
    list.push_front(3);

    std::vector<int> values;
    for (auto& val : list) {
        values.push_back(val);
    }

    ASSERT_EQ(values.size(), 3);
    EXPECT_EQ(values[0], 3);
    EXPECT_EQ(values[1], 2);
    EXPECT_EQ(values[2], 1);
}

TEST(ForwardListIntTest, IteratorIncrement) {
    FixedBlockMapResource resource(1024);
    ForwardList<int> list(&resource);

    list.push_front(10);
    list.push_front(20);

    auto it = list.begin();
    EXPECT_EQ(*it, 20);

    ++it;
    EXPECT_EQ(*it, 10);

    ++it;
    EXPECT_EQ(it, list.end());
}

TEST(ForwardListIntTest, LargeList) {
    FixedBlockMapResource resource(10000);
    ForwardList<int> list(&resource);

    for (int i = 0; i < 100; ++i) {
        list.push_front(i);
    }

    EXPECT_EQ(list.size(), 100);
    EXPECT_EQ(list.front(), 99);
}

// ========================================================================
// ТЕСТЫ ДЛЯ ForwardList со struct
// ========================================================================

struct TestStruct {
    int id;
    std::string name;

    TestStruct() : id(0), name("") {}
    TestStruct(int i, const std::string& n) : id(i), name(n) {}

    bool operator==(const TestStruct& other) const {
        return id == other.id && name == other.name;
    }
};

TEST(ForwardListStructTest, PushFrontStruct) {
    FixedBlockMapResource resource(2048);
    ForwardList<TestStruct> list(&resource);

    list.push_front(TestStruct{1, "Alice"});
    list.push_front(TestStruct{2, "Bob"});

    EXPECT_EQ(list.size(), 2);
    EXPECT_EQ(list.front().id, 2);
    EXPECT_EQ(list.front().name, "Bob");
}

TEST(ForwardListStructTest, IteratorStruct) {
    FixedBlockMapResource resource(2048);
    ForwardList<TestStruct> list(&resource);

    list.push_front(TestStruct{1, "Alice"});
    list.push_front(TestStruct{2, "Bob"});
    list.push_front(TestStruct{3, "Charlie"});

    std::vector<TestStruct> expected = {
        {3, "Charlie"}, {2, "Bob"}, {1, "Alice"}};

    size_t index = 0;
    for (auto& item : list) {
        EXPECT_EQ(item, expected[index]);
        ++index;
    }
}

TEST(ForwardListStructTest, ClearStruct) {
    FixedBlockMapResource resource(2048);
    ForwardList<TestStruct> list(&resource);

    list.push_front(TestStruct{1, "Test1"});
    list.push_front(TestStruct{2, "Test2"});

    list.clear();
    EXPECT_TRUE(list.empty());
}

// ========================================================================
// ИНТЕГРАЦИОННЫЕ ТЕСТЫ
// ========================================================================

TEST(IntegrationTest, MemoryReuse) {
    FixedBlockMapResource resource(1024);
    ForwardList<int> list(&resource);

    // Добавляем элементы
    for (int i = 0; i < 10; ++i) {
        list.push_front(i);
    }

    // Удаляем все
    while (!list.empty()) {
        list.pop_front();
    }

    // Добавляем снова - должна переиспользоваться память
    for (int i = 0; i < 10; ++i) {
        list.push_front(i * 10);
    }

    EXPECT_EQ(list.size(), 10);
    EXPECT_EQ(list.front(), 90);
}

TEST(IntegrationTest, MultipleLists) {
    FixedBlockMapResource resource(4096);

    ForwardList<int> list1(&resource);
    ForwardList<int> list2(&resource);
    ForwardList<int> list3(&resource);

    for (int i = 0; i < 5; ++i) {
        list1.push_front(i);
        list2.push_front(i * 10);
        list3.push_front(i * 100);
    }

    EXPECT_EQ(list1.size(), 5);
    EXPECT_EQ(list2.size(), 5);
    EXPECT_EQ(list3.size(), 5);

    EXPECT_EQ(list1.front(), 4);
    EXPECT_EQ(list2.front(), 40);
    EXPECT_EQ(list3.front(), 400);
}

TEST(IntegrationTest, IteratorModification) {
    FixedBlockMapResource resource(1024);
    ForwardList<int> list(&resource);

    for (int i = 0; i < 5; ++i) {
        list.push_front(i);
    }

    // Изменяем значения через итератор
    for (auto& val : list) {
        val *= 2;
    }

    EXPECT_EQ(list.front(), 8);  // 4 * 2
}

TEST(IntegrationTest, StressTest) {
    FixedBlockMapResource resource(50000);
    ForwardList<int> list(&resource);

    // Добавляем много элементов
    for (int i = 0; i < 500; ++i) {
        list.push_front(i);
    }

    EXPECT_EQ(list.size(), 500);

    // Удаляем половину
    for (int i = 0; i < 250; ++i) {
        list.pop_front();
    }

    EXPECT_EQ(list.size(), 250);

    // Добавляем еще
    for (int i = 0; i < 250; ++i) {
        list.push_front(i + 1000);
    }

    EXPECT_EQ(list.size(), 500);
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
