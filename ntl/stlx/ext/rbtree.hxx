#ifndef NTL__EXT_RBTREE
#define NTL__EXT_RBTREE
#pragma once

#include "../iterator.hxx"
#include "../memory.hxx"
#include "../functional.hxx"

namespace std 
{
  namespace ext
  {
    namespace tree
    {
      template<class T, class Compare = std::less<T>, class Allocator = std::allocator<T> >
      class rb_tree
      {
        typedef typename
          Allocator::template rebind<T>::other        allocator;
      public:
        typedef           T                           value_type;
        typedef           Compare                     value_compare;
        typedef           Allocator                   allocator_type;

        typedef typename  allocator::pointer          pointer;
        typedef typename  allocator::const_pointer    const_pointer;
        typedef typename  allocator::reference        reference;
        typedef typename  allocator::const_reference  const_reference;
        typedef typename  allocator::size_type        size_type;
        typedef typename  allocator::difference_type  difference_type;

      protected:

        enum direction_type  { left, right };

        static direction_type reverse_direction(direction_type d)
        {
          assert( d == left || d == right );
          return static_cast<direction_type>(d ^ left ^ right);
        }

        static direction_type right_direction(bool b)
        {
          return b ? right : left;
        }

        struct node
        {
          enum color_type { black, red, colors };
          node* child[colors];
          uintptr_t parent_and_color;// pointers are always aligned
          color_type color() const { return color_type(parent_and_color & 1); }
          void color(color_type c) { parent_and_color = (parent_and_color & ~1) | c; }
          node* parent() const { return reinterpret_cast<node*>(parent_and_color & ~1); }
          void parent(node* p) 
          { 
            assert( (reinterpret_cast<uintptr_t>(p) & 1) == 0 );
            parent_and_color = reinterpret_cast<uintptr_t>(p) | color();
          }
          
          T elem;

          explicit node(const T& elem, node* parent, node* l = nullptr, node* r = nullptr)
          : elem(elem),
            parent_and_color(reinterpret_cast<uintptr_t>(parent) | black)
          {
            child[left] = l;
            child[right]= r;
          }
          node(const T& elem)
          : elem(elem), parent_and_color(0 | red)
          {
            child[left] = child[right] = nullptr;
          }
#ifdef NTL__CXX_RV
          node(T&& elem)
          : elem(std::forward<T>(elem)), parent_and_color(0 | red)
          {
            child[left] = child[right] = nullptr;
          }
          node(node&& n)
          : elem(std::forward<T>(n.elem)), parent_and_color(n.parent_and_color)
          {
            child[left] = n.child[left];
            child[right]= n.child[right];
          }
#endif
          node(const node& n)
          : elem(n.elem), parent_and_color(n.parent_and_color)
          {
            child[left] = n.child[left];
            child[right]= n.child[right];
          }
        private:
          node& operator=(const node& n);
        };

        typedef typename rb_tree<T, Compare, Allocator>::node node_type;

        struct iterator_impl:
          std::iterator<std::bidirectional_iterator_tag, value_type, difference_type, pointer, reference>
        {
          iterator_impl(){}
          iterator_impl(const iterator_impl& i)
            :p(i.p), tree_(i.tree_) {}

          reference operator* () const { return p->elem; }
          pointer   operator->() const { return &p->elem; }
          iterator_impl& operator++()  { p = tree_->next(p, right); return *this; }
          iterator_impl& operator--()  { p = tree_->next(p, left); return *this; }
          iterator_impl operator++(int){ iterator_impl tmp( *this ); ++*this; return tmp; }
          iterator_impl operator--(int){ iterator_impl tmp( *this ); --*this; return tmp; }

          friend bool operator==(const iterator_impl& x, const iterator_impl& y)
          { return x.p == y.p; }
          friend bool operator!=(const iterator_impl& x, const iterator_impl& y)
          { return x.p != y.p; }

        protected:
          node_type* p;
          rb_tree<T, Compare, Allocator>* tree_;

          friend struct const_iterator_impl;
          friend class rb_tree<T,Compare, Allocator>;

          iterator_impl(node_type* const p, rb_tree<T, Compare, Allocator>* tree)
            :p(p), tree_(tree)
          {}

        };

        struct const_iterator_impl:
          std::iterator<std::bidirectional_iterator_tag, value_type, difference_type, const_pointer, const_reference>
        {
#ifdef NTL__DEBUG
          const_iterator_impl()
            :p(), tree_()
          {}
#else
          const_iterator_impl(){}
#endif
          const_iterator_impl(const iterator_impl& i)
            :p(i.p), tree_(i.tree_)
          {}
          const_iterator_impl(const const_iterator_impl& i)
            :p(i.p), tree_(i.tree_) {}

          const_reference operator* () const { return p->elem; }
          const_pointer   operator->() const { return &operator*(); }

          const_iterator_impl& operator++()  { p = tree_->next(p, right); return *this; }
          const_iterator_impl& operator--()  { p = tree_->next(p, left); return *this; }
          const_iterator_impl operator++(int){ const_iterator_impl tmp( *this ); ++*this; return tmp; }
          const_iterator_impl operator--(int){ const_iterator_impl tmp( *this ); --*this; return tmp; }

          friend bool operator==(const const_iterator_impl& x, const const_iterator_impl& y)
          { return x.p == y.p; }
          friend bool operator!=(const const_iterator_impl& x, const const_iterator_impl& y)
          { return x.p != y.p; }

        private:
          const node_type* p;
          const rb_tree<T, Compare, Allocator>* tree_;

          const_iterator_impl(const node_type* const p, const rb_tree<T, Compare, Allocator>* tree)
            :p(p), tree_(tree)
          {}

          friend class rb_tree;
        };

      public:
        typedef iterator_impl                         iterator;
        typedef const_iterator_impl                   const_iterator;
        typedef std::reverse_iterator<iterator>       reverse_iterator;
        typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

      public:
        explicit rb_tree(const Compare& comp /*= Compare()*/, const Allocator& a = Allocator())
          :comparator_(comp), node_allocator(a),
          root_(), first_(), last_(), count_(0)
        {}

        template<class InputIterator>
        rb_tree(InputIterator first, InputIterator last,
          const Compare& comp = Compare(), const Allocator& a = Allocator())
          :comparator_(comp), node_allocator(a),
          root_(), first_(), last_(), count_()
        {
          insert(first, last);
        }

        rb_tree(const rb_tree& x)
          :node_allocator(x.node_allocator), comparator_(x.comparator_),
          root_(), first_(), last_(), count_()
        {
          insert_range(x.cbegin(), x.cend());
        }

#ifdef NTL__CXX_RV
        rb_tree(rb_tree&& x)
          :root_(), first_(), last_(), count_(), node_allocator(), comparator_()
        {
          swap(x);
        }

        rb_tree(rb_tree&& x, const Allocator& a);
#endif

        rb_tree(const rb_tree& x, const Allocator& a)
          :node_allocator(a),
          comparator_(x.comparator_),
          root_(), first_(), last_(), count_()
        {
          insert(x.begin(), x.end());
        }

        rb_tree& operator=(const rb_tree& x)
        {
          assign(x);
          return *this;
        }

#ifdef NTL__CXX_RV
        rb_tree& operator=(rb_tree&& x)
        {
          if(this != &x){
            clear();
            swap(x);
          }
          return *this;
        }
#endif

        ~rb_tree() __ntl_nothrow
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
        iterator find(const value_type& x)
        {
          node* p = root_;
          while ( p )
          {
            if(elem_less(x, p->elem))
              p = p->child[left];
            else if(elem_greater(x, p->elem))
              p = p->child[right];
            else
              break;//return iterator(p, this);
          }
          return make_iterator(p);// returns end() if !p
        }

        const_iterator find(const value_type& x) const { return find(x); }

        // modifiers
      protected:
        void assign(const rb_tree& x)
        {
          if(this != &x){
            clear();
            insert_range(x.cbegin(), x.cend());
          }
        }

  #ifdef NTL__CXX_RV
        void assign(rb_tree&& x)
        {
          if(this != x){
            clear();
            swap(x);
          }
        }
  #endif

      protected:
        node_type* construct_node(const value_type& x)
        {
          node_type* const np = node_allocator.allocate(1);
          node_allocator.construct(np, x);
          return np;
        }

    #ifdef NTL__CXX_RV
        node_type* construct_node(value_type&& x)
        {
          node_type* const np = node_allocator.allocate(1);
          node_allocator.construct(np, std::forward<value_type>(x));
          return np;
        }

        std::pair<iterator, bool> insert_reference(value_type&& x)
        {
          return insert_impl(construct_node(std::forward<value_type>(x)));
        }
    #endif

        std::pair<iterator, bool> insert_impl(node* const np)
        {
          // insert this as the root node if the tree is empty
          if ( empty() )
          {
            np->color(node::black);
            first_ = last_ = root_ = np;
            ++count_;
            return std::make_pair(make_iterator(root_), true);
          }
          bool greater = false;
          node *q = nullptr;
          // check if node exists
          for ( node* p = root_; p; p = p->child[greater] )
          {
            greater = elem_greater(np->elem, p->elem);
            if ( !greater && !elem_less(np->elem, p->elem) )
              return std::make_pair(make_iterator(p), false);
            q = p;
          }
          // create node
          np->parent(q);
          q->child[greater] = np;
          if ( q == first_ && !greater )
            first_ = np;
          else if(q == last_ && greater)
            last_  = np;
          ++count_;
          // balance tree
          fixup_insert(np);
          return std::make_pair(make_iterator(np), true);
        }

      public:

        std::pair<iterator, bool> insert(const value_type& x)
        {
          return insert_impl(construct_node(x));
        }

        iterator insert(const_iterator /*position*/, const value_type& x)
        {
          // TODO: implement fast insert function based on position
          return insert_impl(construct_node(x)).first;
        }

        template <class InputIterator>
        __forceinline
          void insert(InputIterator first, InputIterator last)
        {
          insert_range(first, last);
        }

        iterator erase(const_iterator position)
        {
          node* const erasable = const_cast<node*>(position.p);
          if ( !erasable )
            return make_iterator(nullptr);

          ++position;
          // check limits
          if ( erasable == first_ )
            first_ = next(erasable, right);
          if ( erasable == last_ )
            last_  = next(erasable, left);

          // find tree successor with a NULL node as a child
          node* succ;
          if( !erasable->child[left] || !erasable->child[right] )
            succ = erasable;
          else
          {
            succ = erasable->child[right];
            while ( succ->child[left] )
              succ = succ->child[left];
          }
          // remove successor from the parent chain
          node* const x = succ->child[right_direction(!succ->child[left])];
          node* prev_root = nullptr;
          if ( x )
            x->parent(succ->parent());
          if ( succ->parent() )
            succ->parent()->child[ succ != succ->parent()->child[left] ] = x;
          else
          {
            prev_root = root_;
            root_ = x;
            //root_->color(node::black);
          }

          if ( succ != erasable )
          {
            std::swap(succ->elem, erasable->elem);
          }
          if ( succ->color() == node::black && x )
          {
            if ( !prev_root )
              fixup_delete(x);
            else
              x->color(node::black);
          }
          node_allocator.destroy(succ);
          node_allocator.deallocate(succ, 1);
          if ( count_ )
            --count_;
          return make_iterator(const_cast<node*>(position.p));
        }

        iterator erase(const_iterator first, const_iterator last)
        {
          while(first != last)
            first = erase(first);
          return empty() ? end() : make_iterator(const_cast<node*>(last.p));
        }

        size_type erase(const value_type& x)
        {
          if(empty())
            return 0;
          iterator pos = find(x);
          return pos == end() ? 0 : (erase(pos), 1);
        }

    #ifdef NTL__CXX_RV
        void swap(rb_tree<T, Compare, Allocator>&& tree)
        {
          if ( this != &tree )
          {
            using std::swap;
            swap(root_, tree.root_);
            swap(first_, tree.first_);
            swap(last_, tree.last_);
            swap(count_, tree.count_);
            swap(comparator_, tree.comparator_);
            swap(node_allocator, tree.node_allocator);
          }
        }
    #endif
    #if !defined(NTL__CXX_RV) || defined(NTL__CXX_RVFIX)
        void swap(rb_tree<T, Compare, Allocator>& tree)
        {
          if ( this != &tree )
          {
            using std::swap;
            swap(root_, tree.root_);
            swap(first_, tree.first_);
            swap(last_, tree.last_);
            swap(count_, tree.count_);
            swap(comparator_, tree.comparator_);
            swap(node_allocator, tree.node_allocator);
          }
        }
    #endif

        void clear()
        {
          if(!empty())
            erase(begin(), end());
        }

        // observes
        value_compare value_comp() const { return comparator_; }

      protected:
        node* next(node* from, direction_type direction) const __ntl_nothrow
        {
          // if --begin() || ++end(), do nothing;
          // if --end(), return last.
          if ( from == nullptr /* end() */ )
            return direction == left ? last_ : from;
          if ( from == first_ && direction == left )
            return from;
          // if node has a child in the required direction,
          // reverse iterate through its descendants 
          direction_type const r_direction = reverse_direction(direction);
          if ( from->child[direction] )
          {
            from = from->child[direction];
            while ( from->child[r_direction] )
              from = from->child[r_direction];
            return from;
          }
          // go to the root unless parent is in the required direction 
          for ( node *parent = from->parent(); ; from = parent, parent = parent->parent() )
            if ( !parent || from == parent->child[r_direction] )
              return parent;
        }

        const node* next(const node* from, direction_type direction) const __ntl_nothrow
        {
          return next(const_cast<node*>(from), direction);
        }

        void rotate(node* x, direction_type direction) __ntl_nothrow
        {
          direction_type const reverse = reverse_direction(direction);;
          node* const prev = x->child[reverse];
          x->child[reverse] = prev->child[direction];
          if ( prev->child[direction] )
            prev->child[direction]->parent(x);
          prev->parent(x->parent());

          if ( x->parent() )
          {
            bool l = x == x->parent()->child[direction];
            if(!direction)
              l = !l;
            x->parent()->child[ l ] = prev;
          }
          else
          {
            root_ = prev;
          }
          prev->child[direction] = x;
          x->parent(prev);
        }

        node* fixup_insert(node* x, direction_type direction) __ntl_nothrow
        {
          node* const y = x->parent()->parent()->child[direction];
          if( y && y->color() == node::red )
          {
            x->parent()->color(node::black);
            y->color(node::black);
            x->parent()->parent()->color(node::red);
            x = x->parent()->parent();
          }
          else
          {
            if ( x == x->parent()->child[direction] )
            {
              x = x->parent();
              rotate(x, reverse_direction(direction));
            }
            x->parent()->color(node::black);
            x->parent()->parent()->color(node::red);
            rotate(x->parent()->parent(), direction);
          }
          return x;
        }

        node* fixup_delete(node* x, direction_type direction) __ntl_nothrow
        {
          direction_type const reverse = reverse_direction(direction);;
          node* w = x->parent()->child[direction];
          if ( !w )
            return x->parent();
          if ( w->color() == node::red )
          {
            w->color(node::black);
            x->parent()->color(node::red);
            rotate(x->parent(), reverse);
            w = x->parent()->child[direction];
          }
          if ( !w )
            return x->parent();
          if(
            (!w->child[reverse] || w->child[reverse]->color() == node::black) &&
            (!w->child[direction]|| w->child[direction]->color() == node::black))
          {
            w->color(node::red);
            x = x->parent();
          }
          else
          {
            if(!w->child[direction] || w->child[direction]->color() == node::black)
            {
              w->child[reverse]->color(node::black);
              w->color(node::red);
              rotate(w, direction);
              w = x->parent()->child[direction];
            }
            w->color(x->parent()->color());
            x->parent()->color(node::black);
            w->child[direction]->color(node::black);
            rotate(x->parent(), reverse);
            x = root_;
          }
          return x;
        }

        void fixup_insert(node* x) __ntl_nothrow
        {
          while ( x != root_ && x->parent()->color() == node::red )
          {
            x = fixup_insert(x,
              right_direction(x->parent() == x->parent()->parent()->child[left]));
          }
          root_->color(node::black);
        }

        void fixup_delete(node* x) __ntl_nothrow
        {
          if ( !x ) return;
          while ( x != root_ && x->color() == node::black )
          {
            x = fixup_delete(x, right_direction(x == x->parent()->child[left]));
          }
          x->color(node::black);
        }

        bool elem_less(const T& x, const T& y) const
        {
          return comparator_(x, y);
        }
        bool elem_equal(const T& x, const T& y) const
        {
          return !elem_less(x, y) && !elem_greater(x, y);
        }
        bool elem_greater(const T& x, const T& y) const
        {
          return comparator_(y, x);
        }

        iterator make_iterator(node* p)
        {
          return iterator(p, this);
        }

        const_iterator make_iterator(const node* p) const
        {
          return const_iterator(p, this);
        }

        template<class InputIterator>
        void insert_range(InputIterator first, InputIterator last)
        {
          while(first != last){
            insert(*first);
            ++first;
          }
        }

      protected:

        node* root_;
        node *first_, *last_;
        size_type count_;

        value_compare comparator_;
        typename allocator_type::template rebind<node_type>::other node_allocator;

      };

      template<class T, class Compare, class Allocator>
      bool operator == (const rb_tree<T, Compare, Allocator>& x, const rb_tree<T, Compare, Allocator>& y)
      {
        return x.size() == y.size() && equal(x.cbegin(), x.cend(), y.cbegin());
      }

      template<class T, class Compare, class Allocator>
      bool operator != (const rb_tree<T, Compare, Allocator>& x, const rb_tree<T, Compare, Allocator>& y)
      {
        return std::rel_ops::operator !=(x, y);
      }

      template<class T, class Compare, class Allocator>
      bool operator < (const rb_tree<T, Compare, Allocator>& x, const rb_tree<T, Compare, Allocator>& y)
      {
        return lexicographical_compare(x.cbegin(), x.cend(), y.cbegin(), y.cend());
      }

      template<class T, class Compare, class Allocator>
      bool operator > (const rb_tree<T, Compare, Allocator>& x, const rb_tree<T, Compare, Allocator>& y)
      {
        return std::rel_ops::operator >(x, y);
      }

      template<class T, class Compare, class Allocator>
      bool operator <= (const rb_tree<T, Compare, Allocator>& x, const rb_tree<T, Compare, Allocator>& y)
      {
        return std::rel_ops::operator <=(x, y);
      }

      template<class T, class Compare, class Allocator>
      bool operator >= (const rb_tree<T, Compare, Allocator>& x, const rb_tree<T, Compare, Allocator>& y)
      {
        return std::rel_ops::operator >=(x, y);
      }

      // specialized algorithms
      template<class T, class Compare, class Allocator>
      void swap(rb_tree<T, Compare, Allocator>& x, rb_tree<T, Compare, Allocator>& y)
      {
        x.swap(y);
      }
    } // tree
  } // ext
} // std
#endif // NTL__EXT_RBTREE
