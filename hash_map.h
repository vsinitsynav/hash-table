#include <bits/stdc++.h>
#include <cassert>
#include <iostream>
#include <vector>

template<class KeyType, class ValueType, class Hash = std::hash<KeyType>> 
class HashMap {
public:
    using vector_iterator = typename std::vector<std::pair<const KeyType, ValueType>>::iterator;
    using vector_const_iterator = typename std::vector<std::pair<const KeyType, ValueType>>::const_iterator;

    class iterator {
    public:

        iterator() : begin_(), end_(), index_(0), used_begin_() {}

        iterator(vector_iterator begin, vector_iterator end, size_t id, std::vector<bool>::iterator used_beg)
                          : begin_(begin), end_(end), index_(id), used_begin_(used_beg) {
            while (begin_ + index_ != end_ && *(used_begin_ + index_) == false) {
                ++index_;
            }
        }

        iterator& operator++() {
            ++index_;
            while (begin_ + index_ != end_ && *(used_begin_ + index_) == false) {
                ++index_;
            }
            return *this;
        }

        iterator operator++(int) {
            auto tmp = *this;
            ++index_;
            while (begin_ + index_ != end_ && *(used_begin_ + index_) == false) {
                ++index_;
            }
            return tmp;
        }

        std::pair<const KeyType, ValueType>& operator*() const {
            return *(begin_ + index_);
        }

        std::pair<const KeyType, ValueType>* operator->() const {
            return &(*(begin_ + index_));
        }

        bool operator==(const iterator& other) const {
            return index_ == other.index_ && begin_ == other.begin_ && end_ == other.end_ && used_begin_ == other.used_begin_;
        }

        bool operator!=(const iterator& other) const {
            return !(*this == other);
        }

        vector_iterator begin_;
        vector_iterator end_;
        size_t index_;
        std::vector<bool>::iterator used_begin_;
    };

 class const_iterator {
    public:

        const_iterator() : begin_(), end_(), index_(0), used_begin_() {}

        const_iterator(vector_const_iterator begin, vector_const_iterator end, size_t id, std::vector<bool>::const_iterator used_beg)
                          : begin_(begin), end_(end), index_(id), used_begin_(used_beg) {
            while (begin_ + index_ != end_ && *(used_begin_ + index_) == false) {
                ++index_;
            }
        }

        const_iterator& operator++() {    
            ++index_;
            while (begin_ + index_ != end_ && *(used_begin_ + index_) == false) {
                ++index_;
            }
            return *this;
        }

        const_iterator operator++(int) {
            auto tmp = *this;
            ++index_;
            while (begin_ + index_ != end_ && *(used_begin_ + index_) == false) {
                ++index_;
            }
            return tmp;
        }

        const std::pair<const KeyType, ValueType>& operator*() const {
            return *(begin_ + index_);
        }

        const std::pair<const KeyType, ValueType>* operator->() const {
            return &(*(begin_ + index_));
        }

        bool operator==(const const_iterator& other) const {
            return index_ == other.index_ && begin_ == other.begin_ && end_ == other.end_ && used_begin_ == other.used_begin_;
        }

        bool operator!=(const const_iterator& other) const {
            return !(*this == other);
        }

        vector_const_iterator begin_;
        vector_const_iterator end_;
        size_t index_;
        std::vector<bool>::const_iterator used_begin_;
    };

    HashMap(Hash hash = Hash()) : hasher_(hash), default_value() {
        size_ = 0;
        used_.resize(capacity_ + 1, false);
        index_.resize(capacity_, capacity_);
        dist_.resize(capacity_, 0);
    
    }

    size_t size() const {
        return size_;
    }

    HashMap& operator=(const HashMap& other) {
        capacity_ = other.capacity_;
        default_value = other.default_value;
        size_ = other.size_;
        hasher_ = other.hasher_;
        used_ = other.used_;
        std::vector<std::pair<const KeyType, ValueType>> temporary = other.storage_;
        storage_.clear();
        for (std::pair<const KeyType, ValueType> element : temporary)
            storage_.push_back(element);
        index_ = other.index_;
        dist_ = other.dist_;
        return *this;
    }

    HashMap(const HashMap& other) {
        capacity_ = other.capacity_;
        size_ = other.size_;
        default_value = other.default_value;
        hasher_ = other.hasher_;
        used_ = other.used_;
        size_ = 0;
        for (std::pair<const KeyType, ValueType> element : other.storage_)
            storage_.push_back(element);
        index_ = other.index_;
        dist_ = other.dist_;
    }

    bool empty() const {
        return (size_ == 0);
    }

    Hash hash_function() const {
        return hasher_;
    }

    iterator begin() {
        return { storage_.begin(), storage_.end(), 0, used_.begin()};
    }

    iterator end() {
        return { storage_.begin(), storage_.end(), storage_.size(), used_.begin() };
    }

    const_iterator begin() const {
        return const_iterator(storage_.begin(), storage_.end(), 0, used_.begin());
    }

    const_iterator end() const {
        return const_iterator(storage_.begin(), storage_.end(), storage_.size(), used_.begin());
    }

    void insert(std::pair<const KeyType, ValueType> to_add) {
        size_t id = hasher_(to_add.first) % capacity_;
        size_t dist = 0;
        while (id < capacity_ && index_[id] != capacity_) {
            if (used_[index_[id]] && storage_[index_[id]].first == to_add.first) {
                return;
            }
            ++id;
            ++dist;
        }
        if (id == capacity_) {
            resize();
            insert(to_add);
        } else {
            ++size_;
            dist_[id] = dist;
            index_[id] = storage_.size();
            used_[index_[id]] = true;
            storage_.push_back(to_add);
            size_t new_id = id;
            id = hasher_(to_add.first) % capacity_;
            while (id < new_id) {
                if (used_[index_[id]] && id - hasher_(storage_[index_[new_id]].first) % capacity_ > dist_[id]) {
                    dist_[id] = id - hasher_(storage_[index_[new_id]].first) % capacity_;
                    dist_[new_id] = new_id - hasher_(storage_[index_[id]].first) % capacity_; 
                    std::swap(index_[id], index_[new_id]);
                }
                ++id;
            }
        }
        if (storage_.size()*3 > capacity_) {
            resize();
        }
    }
    
    template<class Iterator>
    HashMap(Iterator begin_it, Iterator end_it, Hash hash = Hash()) :  hasher_(hash), default_value() {
        size_ = 0;
        used_.resize(capacity_ + 1);
        index_.resize(capacity_, capacity_);
        dist_.resize(capacity_, 0);
        for (Iterator it = begin_it; it != end_it; ++it) {
            insert(*it);
        }
    }

    HashMap(std::initializer_list<std::pair<KeyType, ValueType>> pair_list, Hash hash_ = Hash())
            : HashMap(pair_list.begin(), pair_list.end(), hash_) {}

    void erase(const KeyType& key) {
        size_t id = hasher_(key) % capacity_;
        while (id < capacity_ && index_[id] != capacity_) {
            if (used_[index_[id]] && storage_[index_[id]].first == key) {
                --size_;
                used_[index_[id]] = false;
                return;
            }
            ++id;
        }
    }

    const ValueType& at(const KeyType& key) const {
        size_t id = hasher_(key) % capacity_;
        while (id < capacity_ && index_[id] != capacity_) {
            if (used_[index_[id]] && storage_[index_[id]].first == key) {
                return storage_[index_[id]].second;
            }
            ++id;
        }
        throw std::out_of_range("key is invalid");
        assert(0);
        return storage_[index_[id]].second;
    }

    iterator find(const KeyType& key) {
        size_t id = hasher_(key) % capacity_;
        while (id < capacity_ && index_[id] != capacity_) {
            if (used_[index_[id]] && storage_[index_[id]].first == key) {
                return iterator(storage_.begin(), storage_.end(), index_[id], used_.begin());
            }
            ++id;
        }
        return end();
    }

    const_iterator find(const KeyType& key) const {
        size_t id = hasher_(key) % capacity_;
        while (id < capacity_ && index_[id] != capacity_) {
            if (used_[index_[id]] && storage_[index_[id]].first == key) {
                return const_iterator(storage_.begin(), storage_.end(), index_[id], used_.begin());
            }
            ++id;
        }
        return end();
    }

    ValueType& operator[](const KeyType& key) {
        size_t id = hasher_(key) % capacity_;
        while (id < capacity_ && index_[id] != capacity_) {
            if (used_[index_[id]] && storage_[index_[id]].first == key) {
                return storage_[index_[id]].second;
            }
            ++id;
        }
        insert(std::make_pair(key, default_value));
        id = hasher_(key) % capacity_;
        while (id < capacity_ && index_[id] != capacity_) {
            if (used_[index_[id]] && storage_[index_[id]].first == key) {
                return storage_[index_[id]].second;
            }
            ++id;
        }
        assert(0);
        return storage_[index_[id]].second;
    }

    void clear() {
        size_ = 0;
        fill(used_.begin(), used_.end(), false);
        fill(index_.begin(), index_.end(), capacity_);
        fill(dist_.begin(), dist_.end(), 0);
        storage_.clear();
    }

private:
    void resize() {
        size_t tmp_capacity = capacity_;
        capacity_ = capacity_*2 + 1;
        std::vector<bool> tmp_used = used_;
        std::vector<size_t> tmp_index = index_;
        std::vector<std::pair<const KeyType, ValueType>> tmp = storage_;
        used_.resize(capacity_ + 1);
        index_.resize(capacity_);
        dist_.resize(capacity_);
        size_ = 0;
        fill(used_.begin(), used_.end(), false);
        fill(index_.begin(), index_.end(), capacity_);
        fill(dist_.begin(), dist_.end(), 0);
        storage_.clear();
        for (size_t i = 0; i < tmp_capacity; ++i) {
            if (tmp_used[tmp_index[i]]) {
                insert(tmp[tmp_index[i]]);
            }
        }
    }

    size_t capacity_ = 5;
    size_t size_;
    Hash hasher_;
    ValueType default_value;

    std::vector<bool> used_;
    std::vector<size_t> index_;
    std::vector<std::pair<const KeyType, ValueType>> storage_;
    std::vector<size_t> dist_;
};