# Лабораторная работа №5: Итераторы и аллокаторы

## Описание проекта

Данный проект реализует однонаправленный список (Forward List) с кастомным polymorphic memory resource, использующим фиксированный блок памяти и std::map для отслеживания выделенных и свободных блоков. Проект демонстрирует использование современных возможностей C++20: PMR аллокаторы, итераторы, управление памятью.

## Требования

- Компилятор с поддержкой C++20 (GCC 14+, Clang 14+, MSVC 2022+)
- CMake 3.15+
- Google Test (автоматически загружается через CMake)
- Поддержка std::pmr (Polymorphic Memory Resources)

## Структура проекта
```
lab_05/
├── include/
│   ├── FixedBlockMapResource.h  # Кастомный memory_resource
│   └── ForwardList.h             # Однонаправленный список с итератором
├── src/
│   └── main.cpp                  # Демонстрационная программа
├── tests/
│   ├── test_all.cpp              # Автоматические тесты Google Test (22 теста)
│   └── simple_tests.cpp          # Упрощенные тесты (14 тестов)
├── build/
│   ├── lab_05_app.exe            # Исполняемая демонстрация
│   └── lab_05_tests.exe          # Исполняемые тесты
├── CMakeLists.txt                # Файл конфигурации CMake
├── BUILD_INSTRUCTIONS.md         # Подробные инструкции по сборке
├── EXPLANATION.md                # Подробные пояснения реализации
├── TEST_RESULTS.md               # Результаты тестирования
└── README.md                     # Этот файл
```

## Ключевые особенности реализации

### 1. FixedBlockMapResource (Вариант 3)

```cpp
class FixedBlockMapResource : public std::pmr::memory_resource {
    void* buffer_;                                      // Фиксированный блок памяти
    std::map<void*, size_t> allocated_blocks_;         // Занятые блоки
    std::map<size_t, std::vector<void*>> free_blocks_; // Свободные блоки
    // ...
};
```

* Наследуется от `std::pmr::memory_resource`
* Выделяет один фиксированный блок памяти при создании
* Использует `std::map` для отслеживания занятых и свободных блоков
* Поддерживает переиспользование освобожденной памяти
* Учитывает alignment при выделении

### 2. ForwardList - Однонаправленный список

```cpp
template<typename T>
class ForwardList {
    Node* head_;
    std::pmr::polymorphic_allocator<Node> allocator_;
    // ...
};
```

* Шаблонный контейнер с поддержкой `std::pmr::polymorphic_allocator`
* Операции: `push_front`, `pop_front`, `clear`, `front`, `size`, `empty`
* Forward iterator с поддержкой `std::forward_iterator_tag`
* Работает с простыми и сложными типами данных

### 3. Forward Iterator

```cpp
class Iterator {
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    // ...
};
```

* Соответствует требованиям `std::forward_iterator_tag`
* Поддерживает операции: `*`, `->`, `++`, `==`, `!=`
* Позволяет использовать range-based for loop

### 4. Управление памятью

* Использование PMR (Polymorphic Memory Resources) из C++20
* Переиспользование освобожденной памяти
* Автоматическое освобождение всего буфера при уничтожении
* Поддержка нескольких контейнеров с одним memory resource

## Сборка и запуск

### Быстрая сборка (рекомендуется)
```bash
cd d:\C++\OOP\lab_05
g++ -std=c++20 -Iinclude -o build\lab_05_app.exe src\main.cpp
.\build\lab_05_app.exe
```

### Сборка тестов
```bash
g++ -std=c++20 -Iinclude -o build\simple_tests.exe tests\simple_tests.cpp
.\build\simple_tests.exe
```

### Сборка через CMake
```bash
cd build
cmake ..
cmake --build . --config Release
```

### Запуск демонстрационной программы
```bash
.\build\lab_05_app.exe
```
Программа продемонстрирует:
- Работу с простыми типами (int)
- Работу со структурами (Person)
- Переиспользование памяти
- Несколько списков с одним memory resource

### Запуск тестов
```bash
# Упрощенные тесты (14 тестов)
.\build\simple_tests.exe

# Полные тесты Google Test (22 теста)
.\build\lab_05_tests.exe
```

### Запуск конкретных тестов
```bash
.\build\lab_05_tests.exe --gtest_filter="FixedBlockMapResourceTest.*"
.\build\lab_05_tests.exe --gtest_filter="ForwardListIntTest.*"
.\build\lab_05_tests.exe --gtest_filter="IntegrationTest.*"
```

### Реализованные компоненты

#### 1. FixedBlockMapResource
- Фиксированный блок памяти (выделяется один раз)
- `std::map<void*, size_t>` для отслеживания занятых блоков
- `std::map<size_t, std::vector<void*>>` для свободных блоков
- Методы: `do_allocate()`, `do_deallocate()`, `do_is_equal()`
- Поддержка alignment

#### 2. ForwardList<T>
- Однонаправленный список с PMR аллокатором
- Операции: `push_front()`, `pop_front()`, `clear()`, `front()`, `size()`, `empty()`
- Работает с любым типом `T`
- Итератор с `std::forward_iterator_tag`

#### 3. Iterator
- Forward iterator для обхода списка
- Операторы: `*`, `->`, `++`, `==`, `!=`
- Позволяет использовать range-based for: `for (auto& x : list)`

### Тестирование

Проект включает 36 автоматических тестов, покрывающих:

#### Тесты FixedBlockMapResource (5 тестов)
```
CreateAndDestroy: создание и уничтожение
AllocateMemory: выделение памяти
AllocateMultipleBlocks: множественные выделения
ReuseFreedMemory: переиспользование памяти
OutOfMemory: обработка нехватки памяти
```

#### Тесты ForwardList<int> (10 тестов)
```
CreateEmpty: создание пустого списка
PushFrontSingle/Multiple: добавление элементов
PopFront/PopFrontEmpty: удаление элементов
Clear: очистка списка
IteratorEmpty/Traversal/Increment: работа итератора
LargeList: большой список (500 элементов)
```

#### Тесты ForwardList<struct> (3 теста)
```
PushFrontStruct: работа со структурами
IteratorStruct: итератор для структур
ClearStruct: очистка списка структур
```

#### Интеграционные тесты (4 теста)
```
MemoryReuse: переиспользование памяти в реальном сценарии
MultipleLists: несколько списков с одним resource
IteratorModification: модификация через итератор
StressTest: стресс-тест (500 элементов)
```

#### Упрощенные тесты (14 тестов)
```
Тесты без Google Test для быстрой проверки работоспособности
```

### Примечания

- Использован **Вариант 3**: Однонаправленный список + Фиксированный блок памяти (std::map)
- Memory resource выделяет один фиксированный блок памяти при создании
- Освобожденная память автоматически переиспользуется
- Список поддерживает forward iterator
- Программа демонстрирует принципы работы с PMR аллокаторами и управления памятью

### Результаты тестирования

**Упрощенные тесты:** 14/14 ✓ PASSED  
**Google Test:** 22/22 ✓ PASSED  
**Компиляция:** без ошибок и warnings

Подробнее см. `TEST_RESULTS.md`

### Дополнительная документация

- `BUILD_INSTRUCTIONS.md` - подробные инструкции по сборке
- `EXPLANATION.md` - подробные пояснения реализации
- `TEST_RESULTS.md` - результаты тестирования
- `SUMMARY.md` - итоговая сводка проекта

## Автор

Студент: Снытко Владислав Александрович
Группа: М8О-203БВ-24  
Вариант: 3 (Однонаправленный список + Фиксированный блок памяти с std::map)  
Лабораторная работа №5 по курсу "Объектно-ориентированное программирование"
