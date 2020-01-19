#pragma once

#include <mutex>
#include <unordered_map>

// @brief: thread safe unordered_map class
// this is wrapper class for stl unordered_map with thread safe
// operations, like locks.
//
// @improve(stel): in the future, we will improve this class by
// rewriting it and not relay on c++ stl container

namespace nile {

  template <typename Key, typename T, typename Hash = std::hash<Key>,
            typename KeyEqual = std::equal_to<Key>,
            typename Allocator = std::allocator<std::pair<const Key, T>>>
  class TSUnorderedMap {
  private:
    using map = std::unordered_map<Key, T, KeyEqual, Allocator>;
    using iterator = typename std::unordered_map<Key, T, Hash, KeyEqual, Allocator>::iterator;
    using const_iterator =
        typename std::unordered_map<Key, T, Hash, KeyEqual, Allocator>::const_iterator;
    using size_type = std::size_t;
    using key_type = Key;
    using mapped_type = T;
    using value_type = std::pair<const Key, T>;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = typename std::allocator_traits<Allocator>::pointer;
    using const_pointer = typename std::allocator_traits<Allocator>::const_pointer;


    map m_map;
    mutable std::mutex m_critticalMutex;

  public:
    // Iterators
    iterator begin() noexcept {
      return m_map.begin();
    }

    const_iterator begin() const noexcept {
      return m_map.begin();
    }

    const_iterator cbegin() const noexcept {
      return m_map.cbegin();
    }

    iterator end() noexcept {
      return m_map.end();
    }

    const_iterator end() const noexcept {
      return m_map.end();
    }

    const_iterator cend() const noexcept {
      return m_map.cend();
    }

    // Capacity
    bool empty() const noexcept {
      return m_map.empty();
    }

    size_type size() const noexcept {
      return m_map.size();
    }

    size_type max_size() const noexcept {
      return m_map.max_size();
    }

    // Modifiers
    // @missig operations:
    //  insert_or_assign
    //  emplace_hint
    //  swap
    //  extract
    //  merge
    void clear() noexcept {
      m_map.clear();
    }

    std::pair<iterator, bool> insert( const value_type &value ) {
      return m_map.insert( value );
    }

    std::pair<iterator, bool> insert( value_type &&value ) {
      return m_map.insert( std::move( value ) );
    }

    template <typename P>
    std::pair<iterator, bool> insert( P &&value ) {
      return m_map.insert( std::move( value ) );
    }

    iterator insert( const_iterator hint, const value_type &value ) {
      return m_map.insert( hint, value );
    }

    iterator insert( const_iterator hint, value_type &&value ) {
      return m_map.insert( hint, std::move( value ) );
    }

    template <typename P>
    iterator insert( const_iterator hint, P &&value ) {
      return m_map.insert( hint, std::move( value ) );
    }

    template <typename InputIt>
    void insert( InputIt first, InputIt last ) {
      m_map.insert( first, last );
    }

    void insert( std::initializer_list<value_type> ilist ) {
      m_map.insert( ilist );
    }

    template <typename... Args>
    std::pair<iterator, bool> emplace( Args &&... args ) {
      return m_map.emplace( std::forward<Args>( args )... );
    }

    iterator erase( const_iterator pos ) {
      return m_map.erase( pos );
    }

    iterator erase( const_iterator first, const_iterator second ) {
      return m_map.erase( first, second );
    }

    size_type erase( const key_type &key ) {
      return m_map.erase( key );
    }

    // Lookup
    T &at( const Key &key ) {
      return m_map.at( key );
    }

    const T &at( const Key &key ) const {
      return m_map.at( key );
    }

    T &operator[]( const Key &key ) {
      return m_map[ key ];
    }

    T &operator[]( Key &&key ) {
      return m_map[ std::move( key ) ];
    }

    size_type count( const Key &key ) const {
      return m_map.count( key );
    }

    template <typename K>
    size_type count( const K &x ) const {
      return m_map.count( x );
    }

    iterator find(const Key& k) {
      return m_map.find(k);
    }

    const_iterator find(const Key& key) const {
      return m_map.find(key);
    }
  };

}    // namespace nile
