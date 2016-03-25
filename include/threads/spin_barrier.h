/*!
    \file spin_barrier.h
    \brief Spin barrier synchronization primitive definition
    \author Ivan Shynkarenka
    \date 16.03.2016
    \copyright MIT License
*/

#ifndef CPPCOMMON_THREADS_SPIN_BARRIER_H
#define CPPCOMMON_THREADS_SPIN_BARRIER_H

#include <atomic>
#include <cassert>

namespace CppCommon {

//! Spin barrier synchronization primitive
/*!
    A barrier for a group of threads in the source code means any thread must stop at this point and cannot
    proceed until all other threads reach this barrier.

    Spin version: threads will busy-wait and waste CPU cycles instead of yielding the CPU to another thread.
    Do not use spinlocks unless you are certain that you understand the consequences!

    Thread-safe.

    https://en.wikipedia.org/wiki/Barrier_(computer_science)
    https://en.wikipedia.org/wiki/Spinlock
*/
class SpinBarrier
{
public:
    //! Default class constructor
    /*!
        \param threads - Count of threads to wait at the barrier
    */
    explicit SpinBarrier(int threads) noexcept;
    SpinBarrier(const SpinBarrier&) = delete;
    SpinBarrier(SpinBarrier&&) = default;
    ~SpinBarrier() = default;

    SpinBarrier& operator=(const SpinBarrier&) = delete;
    SpinBarrier& operator=(SpinBarrier&&) = default;

    //! Wait at the barrier until all other threads reach this barrier
    /*!
        Will block.

        \return 'true' for the last thread that reach barrier, 'false' for each of the remaining threads
    */
    bool Wait() noexcept;

private:
    // Count of synchronizing threads
    int _threads;
    // Count of waiting threads
    std::atomic<int> _waiting;
    // Count of barrier synchronizations completed generations
    std::atomic<int> _generation;
};

/*! \example threads_spin_barrier.cpp Spin barrier synchronization primitive example */

} // namespace CppCommon

#include "spin_barrier.inl"

#endif // CPPCOMMON_THREADS_SPIN_BARRIER_H
