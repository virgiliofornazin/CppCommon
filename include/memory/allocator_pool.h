/*!
    \file allocator_pool.h
    \brief Memory pool allocator definition
    \author Ivan Shynkarenka
    \date 08.05.2017
    \copyright MIT License
*/

#ifndef CPPCOMMON_MEMORY_ALLOCATOR_POOL_H
#define CPPCOMMON_MEMORY_ALLOCATOR_POOL_H

#include "allocator.h"

namespace CppCommon {

//! Memory pool manager class
/*!
    Memory pool manager uses a pre-allocated memory buffer or several memory
    chunks in order to create an effective free-list data structure, that
    allows to allocate and free memory.

    During the allocation memory pool manager will return a first-fit memory
    block in the free list!

    If the allocated block is huge and does not fit into the memory pool chunk
    then it will be allocated directly from auxiliary memory manager.

    Not thread-safe.
*/
template <class TAuxMemoryManager = DefaultMemoryManager>
class PoolMemoryManager
{
public:
    //! Initialize memory pool manager with an auxiliary memory manager
    /*!
        Memory pool will have unlimited chunks of size 65536.

        \param auxiliary - Auxiliary memory manager
    */
    explicit PoolMemoryManager(TAuxMemoryManager& auxiliary) : PoolMemoryManager(auxiliary, 65536, 0) {}
    //! Initialize memory pool manager with an auxiliary memory manager, single chunk size and max chunks count
    /*!
        \param auxiliary - Auxiliary memory manager
        \param chunk - Chunk size in bytes
        \param chunks - Max chunks count. Zero value means unlimited count (default is 0)
    */
    explicit PoolMemoryManager(TAuxMemoryManager& auxiliary, size_t chunk, size_t chunks = 0);
    //! Initialize memory pool manager with an auxiliary memory manager and a given buffer
    /*!
        \param auxiliary - Auxiliary memory manager
        \param buffer - Pool buffer
        \param size - Pool buffer capacity
    */
    explicit PoolMemoryManager(TAuxMemoryManager& auxiliary, uint8_t* buffer, size_t capacity);
    PoolMemoryManager(const PoolMemoryManager&) noexcept = delete;
    PoolMemoryManager(PoolMemoryManager&&) noexcept = default;
    ~PoolMemoryManager() { clear(); }

    PoolMemoryManager& operator=(const PoolMemoryManager&) noexcept = delete;
    PoolMemoryManager& operator=(PoolMemoryManager&&) noexcept = default;

    //! Allocated memory in bytes
    size_t allocated() const noexcept { return _allocated; }
    //! Count of active memory allocations
    size_t allocations() const noexcept { return _allocations; }

    //! Chunk size in bytes
    size_t chunk() const noexcept { return _chunk; }
    //! Max chunks size
    size_t chunks() const noexcept { return _chunks; }

    //! Maximum memory block size, that could be allocated by the memory manager
    size_t max_size() const noexcept { return _auxiliary.max_size(); }

    //! Auxiliary memory manager
    TAuxMemoryManager& auxiliary() noexcept { return _auxiliary; }

    //! Allocate a new memory block of the given size
    /*!
        \param size - Block size
        \param alignment - Block alignment (default is alignof(std::max_align_t))
        \return A pointer to the allocated memory block or nullptr in case of allocation failed
    */
    void* malloc(size_t size, size_t alignment = alignof(std::max_align_t));
    //! Free the previously allocated memory block
    /*!
        \param ptr - Pointer to the memory block
        \param size - Block size
    */
    void free(void* ptr, size_t size);

    //! Reset the memory manager
    void reset();
    //! Reset the memory manager with a given signle chunk size and max chunks count
    /*!
        \param chunk - Chunk size in bytes
        \param chunks - Max chunks count. Zero value means unlimited count (default is 0)
    */
    void reset(size_t chunk, size_t chunks = 0);
    //! Reset the memory manager with a given buffer
    /*!
        \param buffer - Pool buffer
        \param size - Pool buffer capacity
    */
    void reset(uint8_t* buffer, size_t capacity);

    //! Clear memory pool
    void clear();

private:
    // Pool chunk contains allocated and free blocks
    struct Chunk
    {
        uint8_t* buffer;
        Chunk* prev;
        Chunk* next;
    };
    // Allocated block
    struct AllocBlock
    {
        size_t size;
        uint8_t offset;
    };
    // Free block
    struct FreeBlock
    {
        size_t size;
        FreeBlock* next;
    };

    // Allocation statistics
    size_t _allocated;
    size_t _allocations;

    // Auxiliary memory manager
    TAuxMemoryManager& _auxiliary;

    // Pool chunks
    bool _external;
    size_t _chunk;
    size_t _chunks;
    Chunk* _current;

    // Free block
    FreeBlock* _free_block;

    //! Allocate memory pool
    Chunk* AllocateMemoryPool(size_t capacity, Chunk* prev);
    //! Clear memory pool
    void ClearMemoryPool();
};

//! Pool memory allocator class
template <typename T, class TAuxMemoryManager = DefaultMemoryManager, bool nothrow = false>
using PoolAllocator = Allocator<T, PoolMemoryManager<TAuxMemoryManager>, nothrow>;

} // namespace CppCommon

#include "allocator_pool.inl"

#endif // CPPCOMMON_MEMORY_ALLOCATOR_POOL_H