#ifndef FORWARD_LIST_H
#define FORWARD_LIST_H

#include <iterator>
#include <memory_resource>
#include <stdexcept>


// Шаблонный однонаправленный
template <typename T>
class ForwardList {
   private:
    // Узел списка
    struct Node {
        T value;
        Node* next;

        Node(const T& val) : value(val), next(nullptr) {}
    };

    using Allocator = std::pmr::polymorphic_allocator<Node>;

    Node* head_;
    Allocator allocator_;
    size_t size_;

   public:
    // Конструктор с memory_resource
    explicit ForwardList(
        std::pmr::memory_resource* mr = std::pmr::get_default_resource())
        : head_(nullptr), allocator_(mr), size_(0) {}

    ~ForwardList() { clear(); }

    // Запрет копирования
    ForwardList(const ForwardList&) = delete;
    ForwardList& operator=(const ForwardList&) = delete;

    // Добавить элемент в начало
    void push_front(const T& value) {
        Node* new_node = allocator_.allocate(1);
        allocator_.construct(new_node, value);
        new_node->next = head_;
        head_ = new_node;
        ++size_;
    }

    // Удалить первый элемент
    void pop_front() {
        if (head_ == nullptr) {
            throw std::runtime_error("Список пуст");
        }
        Node* old_head = head_;
        head_ = head_->next;
        std::allocator_traits<Allocator>::destroy(allocator_, old_head);
        allocator_.deallocate(old_head, 1);
        --size_;
    }

    // Очистить список
    void clear() {
        while (head_ != nullptr) {
            Node* next = head_->next;
            std::allocator_traits<Allocator>::destroy(allocator_, head_);
            allocator_.deallocate(head_, 1);
            head_ = next;
        }
        size_ = 0;
    }

    size_t size() const { return size_; }

    bool empty() const { return head_ == nullptr; }

    // Получить первый элемент
    T& front() {
        if (head_ == nullptr) {
            throw std::runtime_error("Список пуст");
        }
        return head_->value;
    }

    const T& front() const {
        if (head_ == nullptr) {
            throw std::runtime_error("Список пуст");
        }
        return head_->value;
    }

    // Итератор
    class Iterator {
       private:
        Node* current_;

       public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        explicit Iterator(Node* node) : current_(node) {}

        reference operator*() const { return current_->value; }

        pointer operator->() const { return &current_->value; }

        Iterator& operator++() {
            current_ = current_->next;
            return *this;
        }

        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const Iterator& other) const {
            return current_ == other.current_;
        }

        bool operator!=(const Iterator& other) const {
            return !(*this == other);
        }
    };

    Iterator begin() { return Iterator(head_); }

    Iterator end() { return Iterator(nullptr); }
};

#endif
