/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include <cmath>
#include <type_traits>
#include <cassert>
#include "utility.hpp"

// first version
// AVL tree
namespace sjtu {

template<
    class Key,
    class T,
    class Compare = std::less <Key>
> class map {
public:
  typedef pair<const Key, T> value_type;
private:
  struct node {
    int height;
    value_type *value;
    node *Leftson, *Rightson, *Parent;
    node(const Key &_key,const T &_value) : height(1), value(new value_type ({_key, _value})), Leftson(nullptr), Rightson(nullptr), Parent(nullptr) {}
    node(const value_type &_value) : height(1), value(new value_type (_value)), Leftson(nullptr), Rightson(nullptr), Parent(nullptr) {}
    // 析构函数，递归删除所有子树
    ~node() {
      delete value;
      if (Leftson != nullptr) delete Leftson;
      if (Rightson != nullptr) delete Rightson;
    }
    // 找到左子树的 Height
    int getLeftHeight() {
      if (Leftson == nullptr) return 0;
      else return Leftson->height;
    }
    // 找到右子树的 Height
    int getRightHeight() {
      if (Rightson == nullptr) return 0;
      else return Rightson->height;
    }
    // 更新 Height
    void updateHeight() {
      height = std::max(getLeftHeight(), getRightHeight()) + 1;
    }
  };
 public:
  /**
   * the internal type of data.
   * it should have a default constructor, a copy constructor.
   * You can use sjtu::map as value_type by typedef.
   */
  /**
   * see BidirectionalIterator at CppReference for help.
   *
   * if there is anything wrong throw invalid_iterator.
   *     like it = map.begin(); --it;
   *       or it = map.end(); ++end();
   */
   class iterator;
  class const_iterator {
  private:
    // 树上节点的指针，以及对应的 map 的指针
    const node *pos;
    const map *mp;
  public:
    friend class map;
    friend class iterator;
    const_iterator() : pos(nullptr), mp(nullptr) {}
    const_iterator(const node *_pos, const map *_mp) : pos(_pos), mp(_mp) {}
    const_iterator(const const_iterator &other) : pos(other.pos), mp(other.mp) {}

    const_iterator &operator++() {
    //   if (pos == nullptr) throw invalid_iterator();
      if (pos->Rightson != nullptr) {
        pos = pos->Rightson;
        while(pos->Leftson != nullptr) pos = pos->Leftson;
      }
      else {
        while(pos->Parent != nullptr && pos->Parent->Rightson == pos) pos = pos->Parent;
        pos = pos->Parent;
      }
      return *this;
    }
    const_iterator operator++(int x) {
      const_iterator tmp = *this;
      ++(*this);
      return tmp;
    }
    
    const_iterator &operator--() {
    //   if (*this == mp->cbegin()) throw invalid_iterator();
      if (pos == nullptr) {
        pos = mp->root;
        while(pos->Rightson != nullptr) pos = pos->Rightson;
      }
      else if (pos->Leftson != nullptr) {
        pos = pos->Leftson;
        while(pos->Rightson != nullptr) pos = pos->Rightson;
      }
      else {
        while(pos->Parent != nullptr && pos->Parent->Leftson == pos) pos = pos->Parent;
        assert(pos->Parent != nullptr);
        pos = pos->Parent;
      }
      return *this;
    }
    const_iterator operator--(int x) {
      const_iterator tmp = *this;
      --(*this);
      return tmp;
    }
    const value_type &operator*() const {
    //   if (pos == nullptr) throw invalid_iterator();
      return *pos->value;
    }

    bool operator==(const const_iterator &rhs) const {
      return pos == rhs.pos && mp == rhs.mp;
    }
    bool operator!=(const const_iterator &rhs) const {
      return pos != rhs.pos || mp != rhs.mp;
    }

    /**
      * for the support of it->first.
      * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
      */
    const value_type *operator->() const
    noexcept {
      return pos->value;
    }
  };
  class iterator {
  private:
    // 树上节点的指针，以及对应的 map 的指针
    node *pos;
    map *mp;
  public:
    friend class map;
    friend class const_iterator;
    iterator() : pos(nullptr), mp(nullptr) {}
    iterator(node *_pos, map *_mp) : pos(_pos), mp(_mp) {}
    iterator(const iterator &other) : pos(other.pos), mp(other.mp) {}

    iterator &operator++() {
    //   if (pos == nullptr) throw invalid_iterator();
      if (pos->Rightson != nullptr) {
        pos = pos->Rightson;
        while(pos->Leftson != nullptr) pos = pos->Leftson;
      }
      else {
        while(pos->Parent != nullptr && pos->Parent->Rightson == pos) pos = pos->Parent;
        pos = pos->Parent;
      }
      return *this;
    }
    iterator operator++(int x) {
      iterator tmp = *this;
      ++(*this);
      return tmp;
    }

    iterator &operator--() {
    //   if (*this == mp->begin()) throw invalid_iterator();
      if (pos == nullptr) {
        pos = mp->root;
        while(pos->Rightson != nullptr) pos = pos->Rightson;
      }
      else if (pos->Leftson != nullptr) {
        pos = pos->Leftson;
        while(pos->Rightson != nullptr) pos = pos->Rightson;
      }
      else {
        while(pos->Parent != nullptr && pos->Parent->Leftson == pos) pos = pos->Parent;
        assert(pos->Parent != nullptr);
        pos = pos->Parent;
      }
      return *this;
    }
    iterator operator--(int x) {
      iterator tmp = *this;
      --(*this);
      return tmp;
    }

    value_type &operator*() const {
    //   if (pos == nullptr) throw invalid_iterator();
      return *pos->value;
    }

    bool operator==(const iterator &rhs) const {
      return pos == rhs.pos && mp == rhs.mp;
    }
    bool operator==(const const_iterator &rhs) const {
      return pos == rhs.pos && mp == rhs.mp;
    }
    bool operator!=(const iterator &rhs) const {
      return pos != rhs.pos || mp != rhs.mp;
    }
    bool operator!=(const const_iterator &rhs) const {
      return pos != rhs.pos || mp != rhs.mp;
    }

    /**
      * for the support of it->first.
      * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
      */
    value_type *operator->() const
    noexcept {
      return pos->value;
    }
    // 到 const_iterator 的隐式转换
    operator const_iterator() {
      return const_iterator(pos, mp);
    }
  };
private:
  // 左旋 pos，返回新的根
  // pos 一定有 Rightson
  node *rotateLeft(node *pos) {
    node *Rs = pos->Rightson;
    pos->Rightson = Rs->Leftson;
    Rs->Leftson = pos;
    if (pos->Rightson != nullptr) pos->Rightson->Parent = pos;
    Rs->Parent = pos->Parent;
    pos->Parent = Rs;
    pos->updateHeight();
    Rs->updateHeight();
    return Rs;
  }
  // 右旋 pos，返回新的根的指针
  // pos 一定有 Leftson
  node *rotateRight(node *pos) {
    node *Ls = pos->Leftson;
    pos->Leftson = Ls->Rightson;
    Ls->Rightson = pos;
    if (pos->Leftson != nullptr) pos->Leftson->Parent = pos;
    Ls->Parent = pos->Parent;
    pos->Parent = Ls;
    pos->updateHeight();
    Ls->updateHeight();
    return Ls;
  }
  // 将 root 所在的子树复制一遍，并且返回根节点和 begin 所在的节点
  pair<node*, node*> copy(node *_root) {
    if (_root == nullptr) return {nullptr, nullptr};
    node *root = new node (*_root->value), *begin = root;
    if (_root->Rightson != nullptr) {
      root->Rightson = copy(_root->Rightson).first;
      root->Rightson->Parent = root;
    }
    if (_root->Leftson != nullptr) {
      pair<node*, node*> Ls = copy(_root->Leftson);
      root->Leftson = Ls.first;
      begin = Ls.second;
      root->Leftson->Parent = root;
    }
    return {root, begin};
  }
  // 找到 root 子树内为 key 的节点的指针，若没有返回 nullptr
  node *find(node *root, const Key &key) const {
    if (root == nullptr) return nullptr;
    if (Compare()(root->value->first, key)) return find(root->Rightson, key);
    if (Compare()(key, root->value->first)) return find(root->Leftson, key);
    return root;
  }
  // rt 可能会失衡，进行调整，并且返回新的根
  node* maintain(node *rt) {
    rt->updateHeight();
    if (std::abs(rt->getLeftHeight() - rt->getRightHeight()) > 1) {
      if (rt->getLeftHeight() < rt->getRightHeight()) {
        node *Rs = rt->Rightson;
        if (Rs->getRightHeight() >= Rs->getLeftHeight())
          return rotateLeft(rt);
        else {
          rt->Rightson = rotateRight(Rs);
          return rotateLeft(rt);
        }
      }
      else {
        node *Ls = rt->Leftson;
        if (Ls->getLeftHeight() >= Ls->getRightHeight())
          return rotateRight(rt);
        else {
          rt->Leftson = rotateLeft(Ls);
          return rotateRight(rt);
        }
      }
    }
    else return rt;
  }
  // 在 root 的子树内插入节点，返回插入节点的指针
  node* insert(node *rt, const Key &key, const T &value) {
    if (rt == nullptr) {
      root = _begin = new node(key, value);
      return root;
    }
    node *res = nullptr;
    if (Compare()(key, rt->value->first)) {
      if (rt->Leftson != nullptr) res = insert(rt->Leftson, key, value);
      else {
        node* son = new node(key, value);
        son->Parent = rt;
        if (rt == _begin) _begin = son;
        res = rt->Leftson = son;
      }
      rt->Leftson = maintain(rt->Leftson);
    }
    else {
      if (rt->Rightson != nullptr) res = insert(rt->Rightson, key, value);
      else {
        node *son = new node(key, value);
        son->Parent = rt;
        res = rt->Rightson = son;
      }
      rt->Rightson = maintain(rt->Rightson);
    }
    if (rt == root) root = maintain(root);
    return res;
  }
  node* root, *_begin;
  size_t Size;
public:
  /**
   * TODO two constructors
   */
  map() : root(nullptr), _begin(nullptr), Size(0) {}


  map(const map &other) : Size(other.Size) {
    pair<node*, node*> tmp = copy(other.root);
    root = tmp.first, _begin = tmp.second;
  }

  map &operator=(const map &other) {
    if (this == &other) return *this;
    delete root;
    Size = other.Size;
    pair<node*, node*> tmp = copy(other.root);
    root = tmp.first, _begin = tmp.second;
    return *this;
  }

  ~map() {delete root; root = _begin = nullptr;}

  /**
   * TODO
   * access specified element with bounds checking
   * Returns a reference to the mapped value of the element with key equivalent to key.
   * If no such element exists, an exception of type `index_out_of_bound'
   */
  T &at(const Key &key) {
    node* p = find(root, key);
    // if (p == nullptr) throw index_out_of_bound();
    return p->value->second;
  }

  const T &at(const Key &key) const {
    node* p = find(root, key);
    // if (p == nullptr) throw index_out_of_bound();
    return p->value->second;
  }

  /**
   * TODO
   * access specified element
   * Returns a reference to the value that is mapped to a key equivalent to key,
   *   performing an insertion if such key does not already exist.
   */
  T &operator[](const Key &key) {
    node* p = find(root, key);
    if (p == nullptr) {
      Size++;
      p = insert(root, key, T());
    }
    return p->value->second;
  }

  /**
   * behave like at() throw index_out_of_bound if such key does not exist.
   */
  const T &operator[](const Key &key) const {return at(key);}

  /**
   * return a iterator to the beginning
   */
  iterator begin() {return {_begin, this};}

  const_iterator cbegin() const {return {_begin, this};}

  /**
   * return a iterator to the end
   * in fact, it returns past-the-end.
   */
  iterator end() {return {nullptr, this};}

  const_iterator cend() const {return {nullptr, this};}

  /**
   * checks whether the container is empty
   * return true if empty, otherwise false.
   */
  bool empty() const {return Size == 0;}

  /**
   * returns the number of elements.
   */
  size_t size() const {return Size;}

  /**
   * clears the contents
   */
  void clear() {
    delete root;
    root = _begin = nullptr;
    Size = 0;
  }

  /**
   * insert an element.
   * return a pair, the first of the pair is
   *   the iterator to the new element (or the element that prevented the insertion),
   *   the second one is true if insert successfully, or false.
   */
  pair<iterator, bool> insert(const value_type &value) {
    node *p = find(root, value.first);
    if (p == nullptr) {
      Size++;
      p = insert(root, value.first, value.second);
      return {{p, this}, true};
    }
    else return {{p, this}, false};
  }

  /**
   * erase the element at pos.
   *
   * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
   */
  void erase(iterator pos) {
    // if (pos.mp != this || pos.pos == nullptr) throw invalid_iterator();
    // if (root == nullptr) throw invalid_iterator();
    node *cursor = pos.pos;
    Size--;
    // 如果删掉了最小值
    if (cursor == _begin) {
      if (_begin->Rightson != nullptr) {
        _begin = _begin->Rightson;
        while(_begin->Leftson != nullptr) _begin = _begin->Leftson;
      }
      else _begin = _begin->Parent;
    }
    if (cursor->Leftson != nullptr && cursor->Rightson != nullptr) {
      // nextpos 找到后继
      node *nextpos = cursor->Rightson;
      while(nextpos->Leftson != nullptr) nextpos = nextpos->Leftson;
      // 需要交换 cursor 和 nextpos 指针的内容
      node *cursorparent = cursor->Parent;
      node *nextposparent = nextpos->Parent;
      if (nextposparent == cursor) {
        node *cursorLs = cursor->Leftson, *cursorRs = cursor->Rightson;

        cursor->Leftson = nextpos->Leftson;
        if (cursor->Leftson != nullptr) cursor->Leftson->Parent = cursor;
        cursor->Rightson = nextpos->Rightson;
        if (cursor->Rightson != nullptr) cursor->Rightson->Parent = cursor;

        if (cursorLs == nextpos) nextpos->Leftson = cursor;
        else nextpos->Leftson = cursorLs, nextpos->Leftson->Parent = nextpos;
        if (cursorRs == nextpos) nextpos->Rightson = cursor;
        else nextpos->Rightson = cursorRs, nextpos->Rightson->Parent = nextpos;
        
        if (cursorparent != nullptr) {
          if (cursorparent->Leftson == cursor) cursorparent->Leftson = nextpos;
          else cursorparent->Rightson = nextpos;
        }
        else root = nextpos;
        cursor->Parent = nextpos, nextpos->Parent = cursorparent;
        std::swap(cursor->height, nextpos->height);
      }
      else {
        node *cursorLs = cursor->Leftson, *cursorRs = cursor->Rightson;
        cursor->Leftson = nextpos->Leftson;
        if (cursor->Leftson != nullptr) cursor->Leftson->Parent = cursor;
        cursor->Rightson = nextpos->Rightson;
        if (cursor->Rightson != nullptr) cursor->Rightson->Parent = cursor;
        nextpos->Leftson = cursorLs;
        if (nextpos->Leftson != nullptr) nextpos->Leftson->Parent = nextpos;
        nextpos->Rightson = cursorRs;
        if (nextpos->Rightson != nullptr) nextpos->Rightson->Parent = nextpos;

        cursor->Parent = nextposparent, nextpos->Parent = cursorparent;
        if (cursorparent != nullptr) {
          if (cursorparent->Leftson == cursor) cursorparent->Leftson = nextpos;
          else cursorparent->Rightson = nextpos;
        }
        else root = nextpos;
        if (nextposparent->Leftson == nextpos) nextposparent->Leftson = cursor;
        else nextposparent->Rightson = cursor;
        std::swap(cursor->height, nextpos->height);
      }
    }
    assert(cursor->Leftson == nullptr || cursor->Rightson == nullptr);
    if (cursor->Leftson != nullptr) {
      // Rightson = nullptr
      node *parent = cursor->Parent;
      node *Ls = cursor->Leftson;
      Ls->Parent = parent;
      if (parent == nullptr) {
        root = Ls;
        cursor->Leftson = nullptr;
        delete cursor;
        return;
      }
      else {
        if (parent->Leftson == cursor) parent->Leftson = Ls;
        else parent->Rightson = Ls;
        cursor->Leftson = nullptr;
        delete cursor;
        cursor = parent;
      }
    }
    else if (cursor->Rightson != nullptr) {
      // Leftson = nullptr
      node *parent = cursor->Parent;
      node *Rs = cursor->Rightson;
      Rs->Parent = parent;
      if (parent == nullptr) {
        root = Rs;
        cursor->Rightson = nullptr;
        delete cursor;
        return;
      }
      else {
        if (parent->Leftson == cursor) parent->Leftson = Rs;
        else parent->Rightson = Rs;
        cursor->Rightson = nullptr;
        delete cursor;
        cursor = parent;
      }
    }
    else {
      // Leftson = Rightson = nullptr
      node *parent = cursor->Parent;
      if (parent == nullptr) {
        root = nullptr;
        delete cursor;
        return;
      }
      else {
        if (parent->Leftson == cursor) parent->Leftson = nullptr;
        else parent->Rightson = nullptr;
        delete cursor;
        cursor = parent;
      }
    }
    while(cursor->Parent != nullptr) {
      node *parent = cursor->Parent;
      if (parent->Leftson == cursor) parent->Leftson = maintain(cursor);
      else parent->Rightson = maintain(cursor);
      cursor = parent;
    }
    root = maintain(cursor);
  }

  /**
   * Returns the number of elements with key
   *   that compares equivalent to the specified argument,
   *   which is either 1 or 0
   *     since this container does not allow duplicates.
   * The default method of check the equivalence is !(a < b || b > a)
   */
  size_t count(const Key &key) const {return find(root, key) != nullptr;}

  /**
   * Finds an element with key equivalent to key.
   * key value of the element to search for.
   * Iterator to an element with key equivalent to key.
   *   If no such element is found, past-the-end (see end()) iterator is returned.
   */
  iterator find(const Key &key) {
    node *p = find(root, key);
    return {p, this};
  }

  const_iterator find(const Key &key) const {
    node *p = find(root, key);
    return {p, this};
  }
};

}

#endif