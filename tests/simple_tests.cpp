#include <cassert>
#include <iostream>
#include <string>

#include "FixedBlockMapResource.h"
#include "ForwardList.h"


// Счетчики для тестов
int tests_passed = 0;
int tests_total = 0;

#define TEST(name)                                         \
    void test_##name();                                    \
    void run_##name() {                                    \
        tests_total++;                                     \
        std::cout << "Тест " << #name << "... ";           \
        try {                                              \
            test_##name();                                 \
            std::cout << "✓ PASSED\n";                     \
            tests_passed++;                                \
        } catch (const std::exception& e) {                \
            std::cout << "✗ FAILED: " << e.what() << "\n"; \
        } catch (...) {                                    \
            std::cout << "✗ FAILED: Unknown exception\n";  \
        }                                                  \
    }                                                      \
    void test_##name()

// ========================================================================
// ТЕСТЫ ДЛЯ FixedBlockMapResource
// ========================================================================

TEST(ResourceCreateAndDestroy) {
    FixedBlockMapResource resource(1024);
    assert(true);
}

TEST(ResourceAllocate) {
    FixedBlockMapResource resource(1024);
    void* ptr = resource.allocate(10, alignof(int));
    assert(ptr != nullptr);
    resource.deallocate(ptr, 10, alignof(int));
}

TEST(ResourceMultipleAllocate) {
    FixedBlockMapResource resource(1024);
    void* ptr1 = resource.allocate(100, 1);
    void* ptr2 = resource.allocate(100, 1);
    void* ptr3 = resource.allocate(100, 1);

    assert(ptr1 != nullptr);
    assert(ptr2 != nullptr);
    assert(ptr3 != nullptr);
    assert(ptr1 != ptr2);
    assert(ptr2 != ptr3);

    resource.deallocate(ptr1, 100, 1);
    resource.deallocate(ptr2, 100, 1);
    resource.deallocate(ptr3, 100, 1);
}

TEST(ResourceReuseMemory) {
    FixedBlockMapResource resource(1024);
    void* ptr1 = resource.allocate(50, 1);
    resource.deallocate(ptr1, 50, 1);

    void* ptr2 = resource.allocate(50, 1);
    assert(ptr1 == ptr2);  // Должен переиспользовать
    resource.deallocate(ptr2, 50, 1);
}

// ========================================================================
// ТЕСТЫ ДЛЯ ForwardList
// ========================================================================

TEST(ListCreateEmpty) {
    FixedBlockMapResource resource(1024);
    ForwardList<int> list(&resource);
    assert(list.empty());
    assert(list.size() == 0);
}

TEST(ListPushFrontSingle) {
    FixedBlockMapResource resource(1024);
    ForwardList<int> list(&resource);
    list.push_front(42);
    assert(!list.empty());
    assert(list.size() == 1);
    assert(list.front() == 42);
}

TEST(ListPushFrontMultiple) {
    FixedBlockMapResource resource(1024);
    ForwardList<int> list(&resource);
    list.push_front(10);
    list.push_front(20);
    list.push_front(30);
    assert(list.size() == 3);
    assert(list.front() == 30);
}

TEST(ListPopFront) {
    FixedBlockMapResource resource(1024);
    ForwardList<int> list(&resource);
    list.push_front(10);
    list.push_front(20);
    list.pop_front();
    assert(list.size() == 1);
    assert(list.front() == 10);
}

TEST(ListClear) {
    FixedBlockMapResource resource(1024);
    ForwardList<int> list(&resource);
    list.push_front(1);
    list.push_front(2);
    list.push_front(3);
    list.clear();
    assert(list.empty());
    assert(list.size() == 0);
}

TEST(ListIterator) {
    FixedBlockMapResource resource(1024);
    ForwardList<int> list(&resource);
    list.push_front(1);
    list.push_front(2);
    list.push_front(3);

    int count = 0;
    int expected[] = {3, 2, 1};
    for (auto& val : list) {
        assert(val == expected[count]);
        count++;
    }
    assert(count == 3);
}

TEST(ListLarge) {
    FixedBlockMapResource resource(10000);
    ForwardList<int> list(&resource);
    for (int i = 0; i < 100; ++i) {
        list.push_front(i);
    }
    assert(list.size() == 100);
}

// Тесты со структурой
struct Person {
    int id;
    std::string name;
    Person() : id(0), name("") {}
    Person(int i, const std::string& n) : id(i), name(n) {}
};

TEST(ListWithStruct) {
    FixedBlockMapResource resource(2048);
    ForwardList<Person> list(&resource);
    list.push_front(Person{1, "Alice"});
    list.push_front(Person{2, "Bob"});
    assert(list.size() == 2);
    assert(list.front().id == 2);
}

// Интеграционные тесты
TEST(IntegrationMemoryReuse) {
    FixedBlockMapResource resource(1024);
    ForwardList<int> list(&resource);

    for (int i = 0; i < 10; ++i) {
        list.push_front(i);
    }

    while (!list.empty()) {
        list.pop_front();
    }

    for (int i = 0; i < 10; ++i) {
        list.push_front(i * 10);
    }

    assert(list.size() == 10);
}

TEST(IntegrationMultipleLists) {
    FixedBlockMapResource resource(4096);
    ForwardList<int> list1(&resource);
    ForwardList<int> list2(&resource);

    for (int i = 0; i < 5; ++i) {
        list1.push_front(i);
        list2.push_front(i * 10);
    }

    assert(list1.size() == 5);
    assert(list2.size() == 5);
}

int main() {
    std::cout << "=== ЗАПУСК УПРОЩЕННЫХ ТЕСТОВ ===" << std::endl;
    std::cout << "(Эти тесты работают без Google Test)\n\n";

    // Запускаем все тесты
    run_ResourceCreateAndDestroy();
    run_ResourceAllocate();
    run_ResourceMultipleAllocate();
    run_ResourceReuseMemory();
    run_ListCreateEmpty();
    run_ListPushFrontSingle();
    run_ListPushFrontMultiple();
    run_ListPopFront();
    run_ListClear();
    run_ListIterator();
    run_ListLarge();
    run_ListWithStruct();
    run_IntegrationMemoryReuse();
    run_IntegrationMultipleLists();

    std::cout << "\n=== РЕЗУЛЬТАТЫ ===" << std::endl;
    std::cout << "Пройдено: " << tests_passed << " / " << tests_total
              << std::endl;

    if (tests_passed == tests_total) {
        std::cout << "✓ ВСЕ ТЕСТЫ ПРОЙДЕНЫ!" << std::endl;
        return 0;
    } else {
        std::cout << "✗ Есть проваленные тесты" << std::endl;
        return 1;
    }
}
