#ifndef ITER_COMBINATIONS_HPP_
#define ITER_COMBINATIONS_HPP_

#include "iterbase.hpp"

#include <vector>
#include <type_traits>
#include <iterator>
#include <initializer_list>

namespace iter {
    template <typename Container>
    class Combinator;

    template <typename Container>
    Combinator<Container> combinations(Container&&, std::size_t);

    template <typename T>
    Combinator<std::initializer_list<T>> combinations(
            std::initializer_list<T>, std::size_t);

    template <typename Container>
    class Combinator {
        private:
            Container container;
            std::size_t length;

            friend Combinator combinations<Container>(Container&&,std::size_t);
            template <typename T>
            friend Combinator<std::initializer_list<T>> combinations(
                    std::initializer_list<T>, std::size_t);

            Combinator(Container in_container, std::size_t in_length)
                : container(std::forward<Container>(in_container)),
                length{in_length}
            { }

            using CombIteratorDeref =
                std::vector<collection_item_type<Container>>;
        public:

        class Iterator :
            public std::iterator<std::input_iterator_tag, CombIteratorDeref>
        {
            private:
                typename std::remove_reference<Container>::type *container_p;
                std::vector<iterator_type<Container>> indicies;
                bool not_done = true;

            public:
                Iterator(Container& in_container, std::size_t n)
                    : container_p{&in_container},
                    indicies{n}
                {
                    if (n == 0) {
                        not_done = false;
                        return;
                    }
                    size_t inc = 0;
                    for (auto& iter : this->indicies) {
                        auto it = std::begin(*this->container_p);
                        dumb_advance(it, std::end(*this->container_p), inc);
                        if (it != std::end(*this->container_p)) {
                            iter = it;
                            ++inc;
                        } else {
                            not_done = false;
                            break;
                        }
                    }
                }

                CombIteratorDeref operator*() {
                    CombIteratorDeref values;
                    for (auto i : indicies) {
                        values.push_back(*i);
                    }
                    return values;
                }


                Iterator& operator++() {
                    for (auto iter = indicies.rbegin();
                            iter != indicies.rend();
                            ++iter) {
                        ++(*iter);

                        //what we have to check here is if the distance between
                        //the index and the end of indicies is >= the distance
                        //between the item and end of item
                        auto dist = std::distance(
                                this->indicies.rbegin(),iter);

                        if (!(dumb_next(*iter, dist) !=
                                std::end(*this->container_p))) {
                            if ( (iter + 1) != indicies.rend()) {
                                size_t inc = 1;
                                for (auto down = iter;
                                        down != indicies.rbegin()-1;
                                        --down) {
                                    (*down) = dumb_next(*(iter + 1), 1 + inc);
                                    ++inc;
                                }
                            } else {
                                not_done = false;
                                break;
                            }
                        } else {
                            break; 
                        }
                        //we break because none of the rest of the items need
                        //to be incremented
                    }
                    return *this;
                }

                Iterator operator++(int) {
                    auto ret = *this;
                    ++*this;
                    return ret;
                }

                bool operator!=(const Iterator&) const {
                    //because of the way this is done you have to start from
                    //the begining of the range and end at the end, you could
                    //break in the middle of the loop though, it's not
                    //different from the way that python's works
                    return not_done;
                }

                bool operator==(const Iterator& other) const {
                    return !(*this != other);
                }
        };

        Iterator begin() {
            return {this->container, this->length};
        }
        
        Iterator end() {
            return {this->container, this->length};
        }
    };

    template <typename Container>
    Combinator<Container> combinations(
            Container&& container, std::size_t length) {
        return {std::forward<Container>(container), length};
    }

    template <typename T>
    Combinator<std::initializer_list<T>> combinations(
            std::initializer_list<T> il, std::size_t length) {
        return {il, length};
    }
}
#endif
