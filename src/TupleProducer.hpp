#ifndef TUPLE_PRODUCER_HPP
#define TUPLE_PRODUCER_HPP


#include <vector>
#include <optional>


// Template class for producing tuples of a fixed size from a collection
template <typename Collection, size_t Size, typename = std::enable_if_t<(Size > 0)>>
class TupleProducer {
public:
    using Iterator = typename Collection::iterator;
    using ValueType = typename Collection::value_type;

    // Generate TupleType with Size elements of type std::optional<ValueType>
    template <std::size_t... Indexes>
    static auto create_empty_tuple(std::index_sequence<Indexes...>) {
        return std::make_tuple((static_cast<void>(Indexes), std::optional<ValueType>{})...);
    }

    using TupleType = decltype(create_empty_tuple(std::make_index_sequence<Size>{}));

    class IteratorWrapper {
    private:
        Iterator current;
        Iterator end;
        TupleType current_tuple;

    public:
        explicit IteratorWrapper(Iterator start, Iterator finish) : current(start), end(finish) {
            advance();
        }

        const TupleType& operator*() const {
            return current_tuple;
        }

        IteratorWrapper& operator++() {
            ++current;
            advance();
            return *this;
        }

        bool operator!=(const IteratorWrapper& other) const {
            return current != other.current;
        }

    private:
        void advance() {
            if (current == end) {
                return;
            }

            std::vector<std::optional<ValueType>> elements;
            for (size_t i = 0; i < Size && current != end; ++i, ++current) {
                elements.push_back(*current);
            }

            --current;

            while (elements.size() < Size) {
                elements.push_back(std::nullopt);
            }

            current_tuple = to_tuple(elements);
        }

        static TupleType to_tuple(const std::vector<std::optional<ValueType>>& elements) {
            return create_tuple(elements, std::make_index_sequence<Size>{});
        }

        template <std::size_t... Indexes>
        static TupleType create_tuple(const std::vector<std::optional<ValueType>>& elements, std::index_sequence<Indexes...>) {
            // use some template magic to unpack values into tuple
            return std::make_tuple((Indexes < elements.size() ? elements[Indexes] : std::nullopt)...);
        }
    };

private:
    Collection collection;

public:
    explicit TupleProducer(const Collection& coll) : collection(coll) {}

    IteratorWrapper begin() {
        return IteratorWrapper(collection.begin(), collection.end());
    }

    IteratorWrapper end() {
        return IteratorWrapper(collection.end(), collection.end());
    }
};

#endif // TUPLE_PRODUCER_HPP