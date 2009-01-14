#ifndef NTL__EXT_HASHTABLE
#define NTL__EXT_HASHTABLE

#include "../memory.hxx"
#include "../iterator.hxx"
#include "../functional.hxx"
#include "../tuple.hxx"
#include "../initializer_list.hxx"

namespace ntl
{
  namespace ext
  {
    namespace hashtable
    {
      using namespace stlx;

      template<class Key, class Value, class Hash, class Pred, class Allocator>
      class chained_hashtable
      {
        typedef Value T;
        typedef chained_hashtable                     hashtable;

        typedef typename
          Allocator::template rebind<T>::other        allocator;
      public:
        typedef           Value                       value_type;
        typedef           Key                         key_type;
        typedef           value_type                  mapped_type;
        typedef           Hash                        hasher;
        typedef           Pred                        key_equal;
        typedef           Allocator                   allocator_type;

        typedef typename  allocator::pointer          pointer;
        typedef typename  allocator::const_pointer    const_pointer;
        typedef typename  allocator::reference        reference;
        typedef typename  allocator::const_reference  const_reference;
        typedef typename  allocator::size_type        size_type;
        typedef typename  allocator::difference_type  difference_type;

      protected:
        struct double_linked
        {
          double_linked * prev, * next;

          void link(double_linked * prev, double_linked * next)
          {
            this->prev = prev; this->next = next;
            prev->next = this; next->prev = this;
          }

          void unlink() { prev->next = next; next->prev = prev; }
        };

        struct node: double_linked
        {
          T elem;
          node(const T& elem) : elem(elem)   {}
        #ifdef NTL__CXX_RV
          node(T&& elem) : elem(forward<T>(elem))  {}
          node(node&& x): elem(forward<T>(x.elem))
          {}
        #endif
        protected:
          node(const node& n) : elem(n.elem) {}
        };

        // node type
        typedef node node_type;
        // hash value type
        typedef size_t hash_t;
        /**
         *	Bucket represented as list of collided nodes, count of it and hash value for collided nodes.
         **/
        //typedef tuple<node*, size_type, hash_t> bucket_type;
        struct bucket_type
        {
          node*     elems;
          size_type size;
          hash_t    hash;


        };
        /** hash table represented as buckets array and buckets count */
        typedef pair<bucket_type*, size_type>   table;
        
        typedef typename allocator_type::template rebind<node_type>::other    node_allocator;
        typedef typename allocator_type::template rebind<bucket_type>::other  bucket_allocator;

        struct iterator_impl:
          stlx::iterator<bidirectional_iterator_tag, T, difference_type, pointer, reference>
        {
          iterator_impl()
          {}

          reference operator* () const
          { return static_cast<node_type*>(this->p)->elem; }
          pointer   operator->() const { return &operator*(); }
          iterator_impl & operator++() { p = p->next; return *this; }
          iterator_impl & operator--() { p = p->prev; return *this; }
          iterator_impl operator++(int)
          {
            iterator_impl tmp(*this);
            ++*this;
            return tmp;
          }
          iterator_impl operator--(int)
          {
            iterator_impl tmp(*this);
            --*this;
            return tmp;
          }

          friend bool operator==(const iterator_impl& x, const iterator_impl& y)
          { return x.p == y.p; }

          friend bool operator!=(const iterator_impl& x, const iterator_impl& y)
          { return x.p != y.p; }

        private:
          friend class hashtable;
          friend struct const_iterator_impl;
          double_linked p;
        };

        struct const_iterator_impl:
          stlx::iterator<bidirectional_iterator_tag, const T, difference_type, const_pointer, const_reference>
        {
          const_iterator_impl()
          {}
          const_iterator_impl(const typename hashtable::iterator_impl& i)
            :p(i.p)
          {}

          const_reference operator* () const
          { return static_cast<const node_type*>(this->p)->elem; }
          const_pointer   operator->() const { return &operator*(); }
          const_iterator_impl& operator++() { p = p->next; return *this; }
          const_iterator_impl& operator--() { p = p->prev; return *this; }
          const_iterator_impl operator++(int)
          {
            const_iterator_impl tmp(*this);
            ++*this;
            return tmp;
          }
          const_iterator_impl operator--(int)
          {
            const_iterator_impl tmp(*this);
            --*this;
            return tmp;
          }

          friend bool
            operator==(const const_iterator_impl& x, const const_iterator_impl& y)
          { return x.p == y.p; }

          friend bool
            operator!=(const const_iterator_impl& x, const const_iterator_impl& y)
          { return x.p != y.p; }

        private:
          friend class hashtable;
          double_linked p;
        };

        struct local_iterator_impl:
          stlx::iterator<bidirectional_iterator_tag, bucket_type, difference_type, typename bucket_allocator::pointer, typename bucket_allocator::reference>
        {
          local_iterator_impl(){}

          reference operator* () const
          { return static_cast<node_type*>(this->p)->elem; }
          pointer   operator->() const { return &operator*(); }
          local_iterator_impl & operator++() { p = p->next; return *this; }
          local_iterator_impl & operator--() { p = p->prev; return *this; }
          local_iterator_impl operator++(int)
          {
            local_iterator_impl tmp(*this);
            ++*this;
            return tmp;
          }
          local_iterator_impl operator--(int)
          {
            local_iterator_impl tmp(*this);
            --*this;
            return tmp;
          }

          friend bool operator==(const local_iterator_impl& x, const local_iterator_impl& y)
          { return x.p == y.p; }

          friend bool operator!=(const local_iterator_impl& x, const local_iterator_impl& y)
          { return x.p != y.p; }

        private:
          friend class hashtable;
          friend struct const_local_iterator_impl;
          double_linked p;
        };
        
        struct const_local_iterator_impl:
          stlx::iterator<bidirectional_iterator_tag, const bucket_type, difference_type, typename bucket_allocator::const_pointer, typename bucket_allocator::const_reference>
        {
          const_local_iterator_impl()
          {}
          const_local_iterator_impl(const local_iterator_impl& i)
            :p(i.p)
          {}

          const_reference operator* () const
          { return static_cast<const node_type*>(this->p)->elem; }
          const_pointer   operator->() const { return &operator*(); }
          const_local_iterator_impl& operator++() { p = p->next; return *this; }
          const_local_iterator_impl& operator--() { p = p->prev; return *this; }
          const_local_iterator_impl operator++(int)
          {
            const_local_iterator_impl tmp(*this);
            ++*this;
            return tmp;
          }
          const_local_iterator_impl operator--(int)
          {
            const_local_iterator_impl tmp(*this);
            --*this;
            return tmp;
          }

          friend bool
            operator==(const const_local_iterator_impl& x, const const_local_iterator_impl& y)
          { return x.p == y.p; }

          friend bool
            operator!=(const const_local_iterator_impl& x, const const_local_iterator_impl& y)
          { return x.p != y.p; }

        private:
          friend class hashtable;
          double_linked p;
        };


      public:
        typedef iterator_impl                         iterator;
        typedef const_iterator_impl                   const_iterator;
        typedef stlx::reverse_iterator<iterator>       reverse_iterator;
        typedef stlx::reverse_iterator<const_iterator> const_reverse_iterator;

        typedef local_iterator_impl                   local_iterator;
        typedef const_local_iterator_impl             const_local_iterator;

      public:
        explicit chained_hashtable(size_type n, const hasher& hf = hasher(), const key_equal& eql = key_equal(), const allocator_type& a = allocator_type())
          :allocator_(a), balloc_(a), hash_(hf), equal_(eql), count_(0)
        {
          rehash(n);
        }

        // size and capacity
        bool empty() const { return count_ != 0; }
        size_type size() const { return count_;  }
        size_type max_size() const { return allocator_.max_size(); }

        // modifiers
        stlx::pair<iterator, bool> insert(const key_type& k, const value_type& v)
        {
          iterator i = find(k);
          if(i == end()){
            // insert new value
          }else{
            // replace old value
          }
        }

        iterator insert(const_iterator hint, const value_type& obj);
        template <class InputIterator> void insert(InputIterator first, InputIterator last);
        void insert(initializer_list<value_type>);

        iterator  erase(const_iterator position);
        size_type erase(const key_type& k);
        iterator  erase(const_iterator first, const_iterator last);

        void clear();

        void swap(hashtable&);

        // observers
        hasher hash_function()  const { return hash_;  }
        key_equal key_eq()      const { return equal_; }

        // lookup
        iterator find(const key_type& k)
        {
          const size_type n = bucket(k);
          bucket_type& b = buckets_.first[n];
          if(b.size == 0)
            return end();
          for(const_local_iterator li = cbegin(n); li != cend(n); ++li){
            if(equal_(k, *li))
              return iterator(li.p);
          }
          return end();
        }

        const_iterator find(const key_type& k) const
        {
          return const_cast<hashtable*>(this)->find(k);
        }

        size_type count(const key_type& k) const
        {
          // if multi, than count it, otherwise return 0/1?
          const size_type idx = bucket(k);
          bucket_type& b = buckets_.first[n];
          if(b.size == 0)
            return 0;
          size_type n = 0;
          for(const_local_iterator li = cbegin(idx); li != cend(idx); ++li, ++n);
          return n;
        }

        stlx::pair<iterator, iterator> equal_range(const key_type& k);
        stlx::pair<const_iterator, const_iterator> equal_range(const key_type& k) const;

        mapped_type& at(const key_type& k);
        const mapped_type& at(const key_type& k) const;

        // bucket interface
        size_type bucket_count() const { return buckets_.second; }
        size_type max_bucket_count() const;

        size_type bucket_size(size_type n) const
        {
          assert(n >= 0 && n < bucket_count());
          return buckets_.first[n].size;
        }

        size_type bucket(const key_type& k) const
        {
          return mapkey(hash_(k));
        }

        local_iterator begin(size_type n);
        const_local_iterator begin(size_type n) const;
        local_iterator end(size_type n);
        const_local_iterator end(size_type n) const;

        // hash policy
        float load_factor() const;
        float max_load_factor() const;
        void max_load_factor(float z);

        void rehash(size_type n);

      protected:
        void init_table(size_type n)
        {
          // init head
          head_.prev = head_.next = &head_;

          // init table
          buckets_.first = balloc_.allocate(n);
          buckets_.second = n;
        }

      protected:

        /**
         *	@brief mapkey function maps hash value of the key to hash table ceil index
         *
         *  This function uses formula from 11.3.2: \f$ mapkey(k) = \left \lfloor m( kA \mbox{mod 1} ) \right \rfloor \f$
         *  where A = s/2^w, where w is bit count, 0 < s < 2^w = hi*2^w + lo, hi is high word and so on. Hi is cell index.
         *  For example: p = 14, m = 2^p, w = 32, A = s / 2^w ~ (sqrt(5)-1)/2.
         *
         *	@param[in] h hash value
         *	@return table cell index
         *
         **/
        size_type mapkey(hash_t h)
        {
          return h % buckets_.second;
        }


      protected:
        double_linked head_;
        table buckets_;

        node_allocator allocator_;
        bucket_allocator balloc_;
        hasher hash_;
        key_equal equal_;
        size_type count_;

      };
    }
  }
}

#endif // NTL__EXT_HASHTABLE