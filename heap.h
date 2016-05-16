/*
 * HW02, Heap with support for modification
 * Author: David Kuťák, 433409
*/
#include <functional> // less
#include <initializer_list>
#include <vector>
#include <utility>
#include <list>
#include <iterator>
#include <type_traits>

#ifndef CPP14_HEAP
#define CPP14_HEAP

/*
The heap type, parametrized by the type of elements and by the type that
defines a comparator. The comparator is expected to be stateless and default
constructible, so it does not need to be stored in the Heap. The ordering of
the heap depends on the type supplied to Compare, for example, with std::less<
T > it creates a max-heap (this behaviour mirrors the one of
std::priority_queue), see the end of this file for some pre-defined heaps with
comparator.

Unlike std::priority_queue, this Heap type supports in-place update of a value.
For this reason it defines the Handle member type which represents a handle to
an element of the heap. This handle needs to remain valid until the element is
removed from the heap, or the heap is destructed. The handle can be used to
update or remove the value from the heap, and to read the value. However, Heap
does not define iterators.

You should implement Heap as standard binary heap in an array
<https://en.wikipedia.org/wiki/Binary_heap>, using std::vector as the
underlying container (for the purpose of complexity analysis, you can consider
push_back to have constant complexity, although in fact it is only amortized
constant). For each of the operations there is a complexity requirement that you
have to meet. You are free to choose the type of the elements stored in the vector
and you can use any additional data structures that you wish.
 */
template< typename T, typename Compare >
class Heap
{
public:
    struct Handle; // Forward declare handle for use in private functions
private:
    std::vector<std::pair<T, std::list<size_t>::iterator>> mHeapData;
    std::list<size_t> mValPos;

    const T& getValAtPos(size_t index) const
    {
        return mHeapData[index].first;
    }

    void swapElementsAtIndices(size_t first, size_t second)
    {
        if(first == second) { return; }

        *mHeapData[first].second = second;
        *mHeapData[second].second = first;
        std::swap(mHeapData[first], mHeapData[second]);
    }

    unsigned getLeftChildOf(size_t index) const
    {
        return 2 * index + 1;
    }

    unsigned getRightChildOf(size_t index) const
    {
        return 2 * index + 2;
    }

    unsigned getParentOf(size_t index) const
    {
        return (index - 1) / 2;
    }

    void bubbleUp(size_t index)
    {
        if(index <= 0 || index >= mHeapData.size()) { return; }

        size_t parent = getParentOf(index);
        if(!Compare()(getValAtPos(index), getValAtPos(parent)))
        {
            swapElementsAtIndices(index, parent);
            bubbleUp(parent);
        }
    }

    void bubbleDown(size_t index)
    {
        if(index < 0 || index >= mHeapData.size()) { return; }

        size_t leftChild = getLeftChildOf(index);
        size_t rightChild = getRightChildOf(index);
        size_t selectedChild = index;

        if(leftChild < mHeapData.size() &&
           !Compare()(getValAtPos(leftChild), getValAtPos(selectedChild)))
        {
            selectedChild = leftChild;
        }

        if(rightChild < mHeapData.size() &&
           !Compare()(getValAtPos(rightChild), getValAtPos(selectedChild)))
        {
            selectedChild = rightChild;
        }

        if(selectedChild != index)
        {
            swapElementsAtIndices(index, selectedChild);
            bubbleDown(selectedChild);
        }
    }

    void buildHeap()
    {
        for(int i=(mHeapData.size()-1)/2; i>=0; --i)
        {
            bubbleDown(i);
        }
    }

    void updateOp(const Handle& h, T&& value)
    {
        mHeapData[*h.mIter].first = std::move(value);

        size_t pos = *h.mIter;
        bubbleDown(pos);
        bubbleUp(pos);
    }

    Handle insertOp(T&& value)
    {
        Handle result(mValPos.insert(mValPos.end(), mHeapData.size()));
        mHeapData.push_back(std::make_pair(std::move(value), result.mIter));

        bubbleUp(mHeapData.size()-1);

        return result;
    }

public:
    using value_type = T;

    /*
    Handle has to be default constructible, copy and move constructible, copy
    and move assignable, and support equality. Otherwise it is an opaque data
    structure (it does not define any public member functions or data).
     */
    struct Handle
    {
    private:
        std::list<size_t>::iterator mIter;

        Handle(std::list<size_t>::iterator iter)
            :mIter(iter) { }

    public:
        Handle() = default;

        Handle(const Handle&) = default;

        Handle(Handle&&) noexcept = default;

        Handle& operator=(const Handle&) = default;

        Handle& operator=(Handle&&) noexcept = default;

        bool operator==(const Handle &o) const
        {
            return mIter == o.mIter;
        }

        bool operator!=(const Handle &o) const
        {
            return !(*this == o);
        }

        friend class Heap<T, Compare>;
    };

    // O(1). Heap is default constructible in constant time.
    Heap() = default;

    // O(n) where n is the size of other. Heap is copy constructible. Copy does
    // not affect other (and its handles) in any way, the handles from other
    // should not be used with this.
    Heap( const Heap & other )
    {
        for(size_t i = 0; i < other.mHeapData.size(); ++i)
        {
            mHeapData.push_back(std::make_pair(other.mHeapData[i].first, mValPos.insert(mValPos.end(), i)));
        }
    }

    // O(1). Heap is move constructible. After the move, no operations other
    // than destruction or assignment should be done with other and all handles
    // for other should now be valid handles for this.
    Heap( Heap && other ) noexcept = default;

    // O(n) where n is the distance from begin to end. Heap can be created from
    // an iterator range in linear time (provided that the iterator has
    // constant time dereference and increment).
    template< typename Iterator >
    Heap( Iterator begin, Iterator end ) : Heap()
    {
        unsigned i = 0;
        for(auto it = begin; it != end; ++it, ++i)
        {
            mHeapData.push_back(std::make_pair(*it, mValPos.insert(mValPos.end(), i)));
        }

        buildHeap();
    }

    // O(n) where n is the number of elements in list.
    Heap( std::initializer_list< T > list ) : Heap(list.begin(), list.end()) { }

    // O(n) where n is the size of other. Heap is copy assignable. Assignment
    // does not affect other in any way, the handles from other should not be
    // used with this.
    Heap &operator=( Heap other )
    {
        swap(other);
        return *this;
    }

    // O(1). Heap is move assignable. After the move-assign, no operations
    // other than destruction or assignment should be done with other and all
    // handles for other should now be valid handles for this.
    //Heap &operator=( Heap && other ) = default;

    // O(n). Invalidates all handles to this.
    ~Heap() = default;

    // O(1). Heap is swappable. After the swap all handles for this should be
    // valid handles for other and vice versa.
    void swap( Heap &other )
    {
        using std::swap;
        swap(mHeapData, other.mHeapData);
        swap(mValPos, other.mValPos);
    }

    // O(1). Get the top (e.g. maximal for max-heap) element of the heap.
    const T &top() const
    {
        return getValAtPos(0);
    }

    // O(1). Get handle to the top element of the heap.
    Handle topHandle() const
    {
        return Handle(mHeapData[0].second);
    }

    // O(log n). Remove the top element from the heap. This invalidates handle
    // to the removed element, handles to other elements must remain valid.
    void pop()
    {
        if(!empty()) { erase(topHandle()); }
    }

    // O(log n). Insert an element and return a handle for it. No handles are
    // invalidated.
    Handle insert( const T & value )
    {
        return insertOp(T(value));
    }

    // O(log n). A version of insert which moves the element into the
    // underlying container instead of copying it.
    Handle insert( T && value )
    {
        return insertOp(std::move(value));
    }

    // O(1). Get value of an element represented by the given handle.
    // Precondition: h must be a valid handle for this.
    const T &get( const Handle &h ) const
    {
        return getValAtPos(*h.mIter);
    }

    // O(log n). Update the value represented by the given handle (replace it
    // by the new value).
    // Precondition: h must be a valid handle for this.
    void update( const Handle &h, const T &value )
    {
        if(empty()) { return; }

        updateOp(h, T(value));
    }

    // O(log n). A version of update which uses move assign instead of copy
    // assign to replace the value.
    void update( const Handle &h, T &&value )
    {
        if(empty()) { return; }

        updateOp(h, std::move(value));
    }

    // O(log n). Erase the value represented by the given handle from the heap.
    // Invalidates h, but does not invalidate handles to other elements.
    void erase( const Handle &h )
    {
        if(empty()) { return; }

        unsigned position = *h.mIter;

        swapElementsAtIndices(position, mHeapData.size()-1);
        mValPos.erase(h.mIter);
        mHeapData.erase(mHeapData.end()-1);

        bubbleDown(position);
        bubbleUp(position);
    }

    // O(1). Get size (number of elements) of the heap.
    size_t size() const
    {
        return mHeapData.size();
    }

    // O(1). Is the heap empty?
    bool empty() const
    {
        return mHeapData.empty();
    }
};

// O(n log n). Assigns values of the heap in the sorted order (top first) to the output
// iterator. The complexity should hold if both increment and assignment to o
// can be done in constant time.
template< typename OutputIterator, typename T, typename Cmp >
void copySorted( Heap< T, Cmp > heap, OutputIterator o )
{
    while(!heap.empty())
    {
        o = heap.top();
        heap.pop();
    }
}

// O(n log n). Create sorted vector from the given heap.
template< typename T, typename Cmp >
std::vector< T > toSortedVector( Heap< T, Cmp > heap )
{
    std::vector<T> result;
    copySorted(std::move(heap), std::back_inserter(result));
    return result;
}

// O(1). Swaps two heaps. See Heap::swap for more.
template< typename T, typename Cmp >
void swap( Heap< T, Cmp > & a, Heap< T, Cmp > & b ) { a.swap( b ); }

// examples of concrete heaps

template< typename T >
using MaxHeap = Heap< T, std::less< T > >;

template< typename T >
using MinHeap = Heap< T, std::greater< T > >;

template< typename A, typename B, typename Cmp >
struct PairCompare {
    bool operator()( const std::pair< A, B > & a, const std::pair< A, B > & b ) const {
        return Cmp()( a.first, b.first );
    }
};

template< typename P, typename V, typename Cmp = std::less< P > >
using PriorityQueue = Heap< std::pair< P, V >, PairCompare< P, V, Cmp > >;


#endif // CPP14_HEAP
