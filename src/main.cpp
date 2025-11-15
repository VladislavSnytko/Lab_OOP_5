#include <iostream>
#include <string>

#include "FixedBlockMapResource.h"
#include "ForwardList.h"


// Простая структура для демонстрации работы со сложными типами
struct Person {
    int id;
    std::string name;

    Person() : id(0), name("") {}
    Person(int i, const std::string& n) : id(i), name(n) {}
};

// Оператор вывода для Person
std::ostream& operator<<(std::ostream& os, const Person& p) {
    os << "Person{id=" << p.id << ", name='" << p.name << "'}";
    return os;
}

int main() {
    // ТЕСТ 1: Работа с простыми типами (int)
    {
        FixedBlockMapResource resource(1024);
        ForwardList<int> list(&resource);

        for (int i = 1; i <= 5; ++i) {
            list.push_front(i * 10);
        }

        std::cout << "Список: ";
        for (auto& val : list) {
            std::cout << val << " ";
        }
        std::cout << "\n";

        std::cout << "Размер: " << list.size() << "\n";
        std::cout << "Первый элемент: " << list.front() << "\n";

        list.pop_front();
        std::cout << "После удаления первого: " << list.front() << "\n";
    }

    // ТЕСТ 2: Работа со структурой Person
    {
        FixedBlockMapResource resource(2048);
        ForwardList<Person> people(&resource);

        people.push_front(Person{1, "Alice"});
        people.push_front(Person{2, "Bob"});
        people.push_front(Person{3, "Charlie"});

        std::cout << "\nЛюди:\n";
        for (auto& person : people) {
            std::cout << person << "\n";
        }
    }

    // ТЕСТ 3: Переиспользование памяти
    {
        FixedBlockMapResource resource(1024);
        ForwardList<int> list(&resource);

        list.push_front(100);
        list.push_front(200);
        list.push_front(300);

        while (!list.empty()) {
            list.pop_front();
        }

        list.push_front(111);
        list.push_front(222);
        list.push_front(333);

        std::cout << "\nПосле переиспользования: ";
        for (auto& val : list) {
            std::cout << val << " ";
        }
        std::cout << "\n";
    }

    // ТЕСТ 4: Несколько списков с одним resource
    {
        FixedBlockMapResource resource(2048);
        ForwardList<int> list1(&resource);
        ForwardList<int> list2(&resource);

        for (int i = 1; i <= 3; ++i) {
            list1.push_front(i);
        }

        for (int i = 10; i <= 13; ++i) {
            list2.push_front(i);
        }

        std::cout << "\nСписок 1: ";
        for (auto& val : list1) {
            std::cout << val << " ";
        }

        std::cout << "\nСписок 2: ";
        for (auto& val : list2) {
            std::cout << val << " ";
        }
        std::cout << "\n";
    }

    return 0;
}
