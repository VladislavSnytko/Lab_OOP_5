#ifndef FIXED_BLOCK_MAP_RESOURCE_H
#define FIXED_BLOCK_MAP_RESOURCE_H

#include <cstddef>
#include <map>
#include <memory_resource>
#include <vector>


class FixedBlockMapResource : public std::pmr::memory_resource {
   private:
    void* buffer_;        // Фиксированный блок памяти
    size_t buffer_size_;  // Размер буфера
    size_t offset_;  // Текущая позиция для выделения

    // Карта занятых блоков: адрес -> размер
    std::map<void*, size_t> allocated_blocks_;

    // Карта свободных блоков: размер -> список адресов
    std::map<size_t, std::vector<void*>> free_blocks_;

   protected:
    // Выделить память из буфера
    void* do_allocate(size_t bytes, size_t alignment) override {
        // Сначала ищем в свободных блоках
        for (auto it = free_blocks_.begin(); it != free_blocks_.end(); ++it) {
            if (it->first >= bytes && !it->second.empty()) {
                void* ptr = it->second.back();
                it->second.pop_back();

                // Если вектор пустой, удаляем запись
                if (it->second.empty()) {
                    free_blocks_.erase(it);
                }

                allocated_blocks_[ptr] = bytes;
                return ptr;
            }
        }

        // Выравниваем offset по alignment
        size_t padding = (alignment - (offset_ % alignment)) % alignment;
        size_t aligned_offset = offset_ + padding;

        // Проверяем, хватает ли места в буфере
        if (aligned_offset + bytes > buffer_size_) {
            throw std::bad_alloc();
        }

        void* ptr = static_cast<char*>(buffer_) + aligned_offset;
        allocated_blocks_[ptr] = bytes;
        offset_ = aligned_offset + bytes;
        return ptr;
    }

    // Освободить блок (добавить в список свободных)
    void do_deallocate(void* ptr, size_t bytes, size_t alignment) override {
        auto it = allocated_blocks_.find(ptr);
        if (it == allocated_blocks_.end()) {
            return;
        }

        size_t block_size = it->second;
        allocated_blocks_.erase(it);

        // Добавляем в список свободных блоков
        free_blocks_[block_size].push_back(ptr);
    }

    // Сравнение с другим resource
    bool do_is_equal(
        const std::pmr::memory_resource& other) const noexcept override {
        return this == &other;
    }

   public:
    // Конструктор: выделяем один большой блок памяти
    explicit FixedBlockMapResource(size_t size)
        : buffer_size_(size), offset_(0) {
        buffer_ = ::operator new(buffer_size_);
    }

    // Деструктор: освобождаем весь буфер
    ~FixedBlockMapResource() { ::operator delete(buffer_); }

    // Запрет копирования
    FixedBlockMapResource(const FixedBlockMapResource&) = delete;
    FixedBlockMapResource& operator=(const FixedBlockMapResource&) = delete;
};

#endif
