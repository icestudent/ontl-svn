#ifndef NTL__EXT_HASHTABLE
#define NTL__EXT_HASHTABLE

#include "../memory.hxx"      // for allocator
#include "../functional.hxx"  // for hash & predicates
#include "../utility.hxx"     // for pair
#include "../initializer_list.hxx"

namespace ntl
{
  namespace ext
  {
    namespace hashtable
    {
      using namespace std;

      template<class Key, class Value, class Hash = std::hash<Key>, class Pred = std::equal_to<Key>, class Allocator = std::allocator<std::pair<const Key,Value> > >
      class chained_hashtable;

      template<class Key, class Value, class Hash, class Pred, class Allocator>
      class chained_hashtable
      {
        typedef pair<const Key,Value> T;
        typedef chained_hashtable                     hashtable;

        typedef typename
          Allocator::template rebind<T>::other        allocator;

      public:
        typedef           pair<const Key,Value>       value_type;
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
        // hash value type
        typedef size_t hash_t;

        struct node;
        typedef node double_linked;
        struct node
        {
          double_linked* prev;
          double_linked* next;

          value_type elem;
          hash_t hkey;

          node(const value_type& elem, hash_t h)
            :elem(elem), hkey(h), next(), prev()
          {}
        #ifdef NTL__CXX_RV
          node(value_type&& elem, hash_t h)
            :elem(forward<value_type>(elem)), hkey(h), next(), prev()
          {}
          node(node&& x)
            :elem(move(x.elem)), hkey(x.h), next(x.next), prev(x.prev)
          { 
            x.hkey = 0;
            x.prev = x.next = nullptr;
          }
        #endif
        protected:
          node(const node& n)
            :elem(n.elem), hkey(n.hkey), next(x.next), prev(x.prev)
          {}

        public:
          void link(double_linked * prev, double_linked * next)
          {
            this->prev = prev; this->next = next;
            prev->next = this; next->prev = this;
          }

          void link(double_linked* next)
          {
            next->next = this->next; next->prev = this;
            this->next = next;
          }

          void link_prev(double_linked* prev)
          {
            next->next = this; next->prev = this->prev;
            this->prev = prev;
          }

          void unlink()
          {
            if(prev) prev->next = next; 
            if(next) next->prev = prev;
          }

        };

        // node type
        typedef node   node_type;
        /**
         *	Bucket represented as list of collided nodes, count of it and hash value for collided nodes.
         **/
        //typedef tuple<node*, size_type, hash_t> bucket_type;
        struct bucket_type
        {
          node*     elems;
          size_type size;
          hash_t    hash;
          bool      dirty;
        };

        /** hash table represented as buckets array and buckets count */
        typedef pair<bucket_type*, bucket_type*>   table;
        
        typedef typename allocator_type::template rebind<node_type>::other    node_allocator;
        typedef typename allocator_type::template rebind<bucket_type>::other  bucket_allocator;

        struct base_iterator
        {
          node_type* p;
          bucket_type *b, *be;

          void increment()
          {
            if(!p->next){ // end of current bucket
              // find next nonempty bucket
              while(++b != be && !b->elems);
              p = b == be ? nullptr : b->elems;
            }else{
              p = static_cast<node_type*>(p->next);
            }
          }

          void decrement()
          {

          }
        };

        struct base_local_iterator
        {
          node_type *p;

          void increment()
          {
            if(p->next && static_cast<node_type*>(p->next)->hkey != p->hkey)
              p = nullptr; // end of bucket
            else
              p = static_cast<node_type*>(p->next);
          }

          void decrement()
          {

          }
        };

        struct iterator_impl:
          std::iterator<bidirectional_iterator_tag, value_type, difference_type, pointer, reference>,
          base_iterator
        {
          iterator_impl()
          {
            p = nullptr;
          }
          iterator_impl(double_linked* p, bucket_type* b, bucket_type* end)
          {
            this->p = static_cast<node_type*>(p);
            this->b = b;
            this->be = end;
          }
          iterator_impl(bucket_type* b, bucket_type* end)
          {
            this->p = b ? b->elems : nullptr;
            this->b = b;
            this->be = end;
          }

          reference operator* () const { return p->elem; }
          pointer   operator->() const { return &p->elem; }
          iterator_impl & operator++()
          {
            increment();
            return *this;
          }
          iterator_impl & operator--()
          {
            decrement();
            return *this;
          }
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
        };

        struct const_iterator_impl:
          std::iterator<bidirectional_iterator_tag, const value_type, difference_type, const_pointer, const_reference>,
          base_iterator
        {
          const_iterator_impl()
          {
            p = nullptr;
          }
          const_iterator_impl(const iterator_impl& i)
          {
            p = i.p;
            b = i.b;
            be = i.be;
          }
          const_iterator_impl(double_linked* p, bucket_type* b, bucket_type* end)
          {
            this->p = static_cast<node_type*>(p);
            this->b = b;
            this->be = end;
          }

          const_reference operator* () const { return p->elem; }
          const_pointer   operator->() const { return &p->elem; }
          const_iterator_impl& operator++()
          {
            increment();
            return *this;
          }
          const_iterator_impl& operator--()
          {
            decrement();
            return *this;
          }
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
        };

        struct local_iterator_impl:
          std::iterator<bidirectional_iterator_tag, value_type, difference_type, pointer, reference>,
          base_local_iterator
        {
          local_iterator_impl(){}
          local_iterator_impl(double_linked* p)
          {
            this->p = static_cast<node_type*>(p);
          }

          reference operator* () const { return p->elem; }
          pointer   operator->() const { return &p->elem; }
          // TODO: iterate through elements only with the same hash value!
          local_iterator_impl & operator++()
          {
            increment();
            return *this;
          }
          local_iterator_impl & operator--()
          {
            decrement();
            return *this;
          }
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
        };

        struct const_local_iterator_impl:
          std::iterator<bidirectional_iterator_tag, const value_type, difference_type, const_pointer, const_reference>,
          base_local_iterator
        {
          const_local_iterator_impl()
          {}
          const_local_iterator_impl(const local_iterator_impl& i)
          {
            p = i.p;
          }
          const_local_iterator_impl(const double_linked* p)
          {
            p = static_cast<const node_type*>(p);
          }

          const_reference operator* () const { return p->elem; }
          const_pointer   operator->() const { return &p->elem; }
          const_local_iterator_impl& operator++()
          {
            increment();
            return *this;
          }
          const_local_iterator_impl& operator--()
          {
            decrement();
            return *this;
          }
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
        };

      public:
        typedef iterator_impl                         iterator;
        typedef const_iterator_impl                   const_iterator;
        typedef std::reverse_iterator<iterator>       reverse_iterator;
        typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

        typedef local_iterator_impl                   local_iterator;
        typedef const_local_iterator_impl             const_local_iterator;

      public:
        explicit chained_hashtable(size_type n, const hasher& hf = hasher(), const key_equal& eql = key_equal(), const allocator_type& a = allocator_type())
          :nalloc(a), balloc(a), hash_(hf), equal_(eql), count_(0), max_factor(1.0f)
        {
          init_table(n);
        }
        ~chained_hashtable()
        {
          clear();
        }

        chained_hashtable& operator=(const chained_hashtable& x)
        {
          clear();
          copy_from(x.buckets_);
          return *this;
        }

        // size and capacity
        bool empty() const { return count_ == 0; }
        size_type size() const { return count_;  }
        size_type max_size() const { return nalloc.max_size(); }

        // iterators
        iterator begin()
        {
          return count_ == 0 ? iterator() : iterator(head_, buckets_.second);
        }
        const_iterator begin() const  { return const_cast<hashtable*>(this)->begin(); }
        const_iterator cbegin() const { return const_cast<hashtable*>(this)->begin(); }
        iterator end()                { return iterator(); }
        const_iterator end() const    { return const_iterator(); }
        const_iterator cend() const   { return const_iterator(); }

        // modifiers
        std::pair<iterator, bool> insert(const value_type& v)
        {
          iterator i = find(v.first);
          if(i == end()){
            // insert new value
            return make_pair(insert(cend(), v), true);
          }else{
            // replace old value
        #ifdef NTL__CXX_RV
            i->second = move(v.second);
        #else
            i.p = move_element<0>(i.p, v, std::has_trivial_assign<value_type>());
        #endif
          }
          return make_pair(i, false);
        }

        iterator insert(const_iterator hint, const value_type& v)
        {
          const hash_t hkey = hash_(v.first);
          const size_type n = mapkey(hkey);
          bucket_type& b = buckets_.first[n];

          // construct node(value, hash)
          node* p = nalloc.allocate(1);
          nalloc.construct(p, v, hkey);

          // ���� ����������� ������� ����� ��� �� ����, ��� � � ���������, ��������� ����� ���������? ��� �����?.
          // ����� ��������� � ������ �������

          // NOTE: ��� ������, ���� ���� �� ����������� ��������� �������?
          //b.size++;

          if(hint.p && hint.p->hkey == hkey){
            // link with the hint
            //const_cast<node_type*>(hint.p)->link(p);
            hint = hint;
          }
          //}else{
          {
            if(b.elems){
              const bool to_end = b.hash != hkey; // ������ �� ������� ��� �������
              if(!to_end)
                b.elems->link_prev(p);            // ������� ���������� ������ � ������
              else{
                node_type* n = b.elems; 
                while(n->next && n->hkey == hkey)
                  n = n->next;
                n->link(p);
                b.dirty = true;
              }
            }else{
              // construct bucket
              b.elems = p;
              b.hash = hkey;
              if(count_ == 0 || head_ > &b)
                head_ = &b;
            }
            b.size++;
          }
          count_++;
          return iterator(p, &b, buckets_.second);
        }

        template <class InputIterator>
        void insert(InputIterator first, InputIterator last)
        {
          for(iterator i = end(); first != last; ++first)
            i = insert(i, *first);
        }

        iterator  erase(const_iterator position)
        {
          if(!position.p)
            return end();
          node_type* next = position.p->next;
          if(position.b->elems == position.p){
            bucket_type* b = position.b;
            const bool head = head_ == b;
            b->elems = next;
            if(!--b->size || (b->dirty && b->hash != next->hkey)){
              b->dirty = false;
              if(next)
                b->hash = next->hkey;
            }
            if(!next || head){
              // find next nonempty bucket for head
              b = buckets_.first;
              while(b != buckets_.second && !b->elems) ++b;
              if(b != buckets_.second){
                if(head)
                  head_ = b;
                position.b = b;
                next = b->elems;
              }else if(head)
                head_ = nullptr;
            }
          }else{
            --position.b->size;
          }
          --count_;
          position.p->unlink();
          nalloc.destroy(position.p);
          nalloc.deallocate(position.p,1);
          return iterator(next, position.b, position.be);
        }
        size_type erase(const key_type& k)
        {
          const_iterator i =

        }
        iterator  erase(const_iterator first, const_iterator last)
        {
          while(first != last)
            first = erase(first);
          return iterator(last.p, last.b, buckets_.second);
        }

        void clear()
        {
          erase(cbegin(), cend());
        }

        void swap(hashtable& x)
        {
          std::swap(head_,    x.head_);
          std::swap(buckets_, x.buckets_);
          std::swap(nalloc,   x.nalloc);
          std::swap(balloc,   x.balloc);
          std::swap(hash_,    x.hash_);
          std::swap(equal_,   x.equal_);
          std::swap(count_,   x.count_);
          std::swap(max_factor, x.max_factor);
        }

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
          for(local_iterator li = begin(n), lend = end(n); li != lend; ++li){
            if(equal_(k, li.p->elem.first))
              return iterator(li.p, &b, buckets_.second);
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
          bucket_type& b = buckets_.first[idx];
          if(b.size == 0)
            return 0;
          size_type n = 0;
          for(const_local_iterator li = cbegin(idx), lend = cend(idx); li != lend; ++li, ++n);
          return n;
        }

        std::pair<iterator, iterator> equal_range(const key_type& k)
        {
          const hash_t hkey = hash_(k);
          const size_type n = mapkey(hkey);
          bucket_type& b = buckets_.first[n];
          iterator e = end();
          if(!b.size)
            return make_pair(e, e);

          node_type* p = b.elems;
          while(p && p->hkey != hkey)
            p = p->next;
          if(!p)
            return make_pair(e, e);

          e.p = p,
            e.b = &b,
            e.be = buckets_.second;
          iterator i = e;
          if(e.b->dirty){
            while(e.p->hkey == i.p->hkey && e.p)
              e.p = e.p->next;
          }else{
            e.p = nullptr;
          }
          return make_pair(i,e);
        }

        std::pair<const_iterator, const_iterator> equal_range(const key_type& k) const
        {
          return const_cast<hashtable*>(this)->equal_range(k);
        }

        mapped_type& at(const key_type& k);
        const mapped_type& at(const key_type& k) const;

        // bucket interface
        size_type bucket_count() const { return buckets_.second - buckets_.first; }
        size_type max_bucket_count() const { return balloc.max_size(); }

        size_type bucket_size(size_type n) const
        {
          assert(n >= 0 && n < bucket_count());
          bucket_type& b = buckets_.first[n];
          if(!b.dirty)
            n = b.size;
          else{
            n = 0;
            const hash_t hkey = b.hash;
            for(node_type* p = b.elems; p && p->hkey == hkey; p = p->next, ++n);
          }
          return n;
        }

        size_type bucket(const key_type& k) const
        {
          return mapkey(hash_(k));
        }

        local_iterator begin(size_type n)
        {
          assert(n >= 0 && n < bucket_count());
          return buckets_.first[n].elems;
        }
        const_local_iterator begin(size_type n) const
        {
          assert(n >= 0 && n < bucket_count());
          return buckets_.first[n].elems;
        }
        const_local_iterator cbegin(size_type n)const { return begin(n); }

        local_iterator end(size_type n)
        {
          assert(n >= 0 && n < bucket_count());
          return local_iterator(nullptr);
        }
        const_local_iterator end(size_type n)   const
        {
          assert(n >= 0 && n < bucket_count());
          return const_local_iterator(nullptr);
        }
        const_local_iterator cend(size_type n)  const { return end(n); }

        // hash policy
        float load_factor() const     { return float(count_) / bucket_count(); }
        float max_load_factor() const { return max_factor; }
        void max_load_factor(float z)
        {
          assert(z > 0);
          max_factor = z;
        }

        void rehash(size_type n)
        {
          n = capacity_factor(n);

          // init the new bucket table
          bucket_type* b = balloc.allocate(n);
          table buckets(b, b + n);
          memset(b, 0, sizeof(bucket_type)*n);

          // swap it with old
          std::swap(buckets, buckets_);
          head_ = nullptr;
          count_ = 0; // increased by following inserts
          copy_from(buckets);
        }

      protected:
        void init_table(size_type n)
        {
          // init table
          n = capacity_factor(n);
          buckets_.first = balloc.allocate(n);
          buckets_.second = buckets_.first + n;
          memset(buckets_.first, 0, sizeof(bucket_type)*n);
        }

        void copy_from(const table& buckets)
        {
          bucket_type* b = buckets.first;

          // iterate through old table
          const_iterator hint = end();
          while(b != buckets.second){
            if(b->elems){
              node_type* p = b->elems;
              while(p){
                hint = insert(hint, std::move(p->elem));
                p = p->next;
              }
            }
            ++b;
          }
        }

        template<bool> node_type* move_element(node_type* to, const value_type& v, true_type)
        {
          to->elem = v;
          return to;
        }

        template<bool> node_type* move_element(node_type* to, const value_type& v, false_type)
        {
          node_type* p = nalloc.allocate(1);
          nalloc.construct(p, v, to->hkey);
          p->next = to->next; p->prev = to->prev;
          nalloc.destroy(to);
          nalloc.deallocate(to,1);
          return p;
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
        size_type mapkey(hash_t h) const
        {
          return h % (buckets_.second-buckets_.first);
        }

        size_type capacity_factor(size_type n) const
        {
          return static_cast<size_type>(n * max_load_factor() * 2 - 1);
        }

      protected:
        bucket_type* head_;
        table buckets_;

        node_allocator nalloc;
        bucket_allocator balloc;
        hasher hash_;
        key_equal equal_;
        size_type count_;
        float max_factor;
      };
    }
  }
}

#endif // NTL__EXT_HASHTABLE