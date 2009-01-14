#pragma once

#include "memory.hxx"
#include "functional.hxx"

namespace tree
{
  template<class T, class Compare = stlx::less<T>, class Allocator = stlx::allocator<T> >
  class fib
  {
  public:
    typedef           T                           value_type;
    typedef           Compare                     value_compare;

    typedef Allocator                             allocator_type;
    typedef typename  
      Allocator::template rebind<T>::other        allocator;

    typedef typename  allocator::pointer          pointer;
    typedef typename  allocator::const_pointer    const_pointer;
    typedef typename  allocator::reference        reference;
    typedef typename  allocator::const_reference  const_reference;
    typedef typename  allocator::size_type        size_type;
    typedef typename  allocator::difference_type  difference_type;

  private:
    struct node
    {
      int degree;
      int mark;

      node *p, *child, *left *right;
      T elem;

      node(const T& elem, node* p)
        :elem(elem), 
        p(p), left(this), right(this), child()
        degree(), mark()
      {}
    };
    typedef typename fib<T, Compare, Allocator>::node node_type;

    struct iterator_impl:
      stlx::iterator<stlx::bidirectional_iterator_tag, value_type, difference_type, pointer, reference>
    {
      iterator_impl(){}

    protected:
      node_type* p;
      fib<T, Compare, Allocator>* tree_;

      friend struct const_iterator_impl;
      friend class fib<T,Compare, Allocator>;

      iterator_impl(node_type* p, fib<T, Compare, Allocator>* tree)
        :p(p), tree_(tree)
      {}
    };

    struct const_iterator_impl:
      stlx::iterator<stlx::bidirectional_iterator_tag, value_type, difference_type, const_pointer, const_reference>
    {
      const_iterator_impl(){}

      const_iterator_impl(const iterator_impl& i)
        :p(i.p), tree_(i.tree_)
      {}
      const_iterator_impl(const const_iterator_impl& i)
        :p(i.p), tree_(i.tree_) {}

    protected:
      const node_type* p;
      const fib<T, Compare, Allocator>* tree_;

      friend struct const_iterator_impl;
      friend class fib<T,Compare, Allocator>;

      const_iterator_impl(const node_type* p, const fib<T, Compare, Allocator>* tree)
        :p(p), tree_(tree)
      {}
    };
  public:
    typedef iterator_impl                         iterator;
    typedef const_iterator_impl                   const_iterator;
    typedef stlx::reverse_iterator<iterator>       reverse_iterator;
    typedef stlx::reverse_iterator<const_iterator> const_reverse_iterator;

  public:
    explicit fib(const Compare& comp /*= Compare()*/, const Allocator& a = Allocator())
      :comparator_(comp), node_allocator(a),
      count_(), root_(), min_(), cons_(),
      dl(-1)
    {}

    ~fib() __ntl_nothrow
    {
      clear();
    }

    allocator_type get_allocator() const { return static_cast<allocator_type>(node_allocator); }

  public:
    // capacity
    bool      empty() const     { return count_ == 0; }
    size_type size()  const     { return count_; }
    size_type max_size()  const { return node_allocator.max_size(); }

    // iterators
    iterator                begin()        { return iterator(first_, this); }
    const_iterator          begin()  const { return const_iterator(first_, this); }
    iterator                end()          { return iterator(NULL, this);  }
    const_iterator          end()    const { return const_iterator(NULL, this);  }

    reverse_iterator        rbegin()       { return reverse_iterator(iterator(NULL, this));  }
    const_reverse_iterator  rbegin() const { return const_reverse_iterator(const_iterator(NULL, this));  }
    reverse_iterator        rend()         { return reverse_iterator(iterator(first_, this)); }
    const_reverse_iterator  rend()   const { return const_reverse_iterator(const_iterator(first_, this)); }

    const_iterator          cbegin() const { return begin(); }
    const_iterator          cend()   const { return end();   }
    const_reverse_iterator  crbegin()const { return rbegin();}
    const_reverse_iterator  crend()  const { return rend();  }

    // access
    iterator find(const value_type& x);
    const_iterator find(const value_type& x) const;

    // modifiers
    iterator insert(const value_type& x)
    {
      node* node = node_allocator.allocate(1);
      node_allocator.construct(node, x);
      insert_root(node);

      if(min_ == nullptr || comparator_(node, min_))
        min_ = node;
      count_++;
      return make_iterator(x);
    }

    iterator erase(iterator position)
    {
      node* x = *position;
      if(!x)
        return position;

      node* y = x == x->left ? nullptr : x->left;
      
      // fix the parent pointer
      if(x->p && x->p->child == x)
        x->p->child = y;
      
      // unlink
      x->right->left = x->left;
      x->left->right = x->right;

      x->p = nullptr;
      x->left = x->right = x;
      
      return make_iterator(y);
    }

    iterator erase(iterator first, iterator last)
    {
      while(first != last)
        first = erase(first);

      return empty() ? end() : last;
    }

    size_type erase(const value_type& x)
    {
      if(empty())
        return 0;
      iterator pos = find(x);
      return pos == end() ? 0 : (erase(pos), 1);
    }


    void clear()
    {
      if(!empty())
        erase(begin(), end());
    }

    // observes
    value_compare value_comp() const { return comparator_; }



  protected:
    void insert_root(node* x)
    {
      if(empty()){
        root_ = x;
        return;
      }
      insert_after(root_, x);
    }

    static void insert_after(node* a, node* b)
    {
      if(a == a->right){
        a->left = a->right = b;
        b->left = b->right = a;
      }else{
        b->right = a->right;
        a->right->left = b;
        a->right = b;
        b->left = a;
      }
    }

    static void insert_before(node* a, node* b)
    {
      insert_after(b, a);
    }

  protected:
    iterator min()
    {
      return make_iterator(min_ ? min_ : nullptr);
    }

    const_iterator min() const
    {
      return make_iterator(min_ ? min_ : nullptr);
    }



  protected:
    iterator make_iterator(node* x) 
    {
      return iterator(x, this);
    }

    const_iterator make_iterator(const node* x) const
    {
      return const_iterator(x, this);
    }
  private:
    typename allocator_type::template rebind<node_type>::other node_allocator;
    value_compare comparator_;

    int dl;
    size_type count_;

    node *root_, *min_, **cons_;
  };
}