#ifndef ITER_ENUMERATE_H_
#define ITER_ENUMERATE_H_

#include "iterbase.hpp"

#include <utility>
#include <iterator>
#include <functional>
#include <initializer_list>
#include <type_traits>


// enumerate functionality for python-style for-each enumerate loops
// for (auto e : enumerate(vec)) {
//     std::cout << e.index
//               << ": "
//               << e.element
//               << '\n';
// }


namespace iter {

    //Forward declarations of Enumerable and enumerate
    template <typename Container>
    class Enumerable;

    template <typename Container>
    Enumerable<Container> enumerate(Container&&);

    template <typename T>
    Enumerable<std::initializer_list<T>> enumerate(std::initializer_list<T>);

    template <typename Container>
    class Enumerable {
        private:
            Container container;

            // The only thing allowed to directly instantiate an Enumerable is
            // the enumerate function
            friend Enumerable enumerate<Container>(Container&&);
            template <typename T>
            friend Enumerable<std::initializer_list<T>> enumerate(
                    std::initializer_list<T>);

            // for IterYield
            using BasePair = std::pair<std::size_t, iterator_deref<Container>>;

            // Value constructor for use only in the enumerate function
            Enumerable(Container container)
                : container(std::forward<Container>(container))
            { }
            
        public:
            // "yielded" by the Enumerable::Iterator.  Has a .index, and a 
            // .element referencing the value yielded by the subiterator
            class IterYield : public BasePair {
                public:
                    using BasePair::BasePair;
                    decltype(BasePair::first)& index = BasePair::first;
                    decltype(BasePair::second)& element = BasePair::second;
            };

            //  Holds an iterator of the contained type and a size_t for the
            //  index.  Each call to ++ increments both of these data members.
            //  Each dereference returns an IterYield.
            class Iterator :
                public std::iterator<std::input_iterator_tag, IterYield>
            {
                private:
                    iterator_type<Container> sub_iter;
                    std::size_t index;
                public:
                    Iterator (iterator_type<Container> si)
                        : sub_iter{si},
                        index{0}
                    { } 

                    IterYield operator*() {
                        return {this->index, *this->sub_iter};
                    }

                    Iterator& operator++() { 
                        ++this->sub_iter;
                        ++this->index;
                        return *this;
                    }

                    Iterator operator++(int) {
                        auto ret = *this;
                        ++*this;
                        return ret;
                    }

                    bool operator!=(const Iterator& other) const {
                        return this->sub_iter != other.sub_iter;
                    }

                    bool operator==(const Iterator& other) const {
                        return !(*this != other);
                    }
            };

            Iterator begin() {
                return {std::begin(this->container)};
            }

            Iterator end() {
                return {std::end(this->container)};
            }

    };

    template <typename Container>
    Enumerable<Container> enumerate(Container&& container) {
        return {std::forward<Container>(container)};
    }

    // for initializer lists.  copy constructs the list into the Enumerable
    template <typename T>
    Enumerable<std::initializer_list<T>> enumerate(
            std::initializer_list<T> il)
    {   
        return {il};
    }
}

#endif //#ifndef ITER_ENUMERATE_H_
