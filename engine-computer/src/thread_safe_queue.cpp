#include "mach/thread_safe_queue.hpp"
#include <unordered_map>
  
template <typename T> 
void ThreadSafeQueue<T>::push(T item) { 
    // Acquire lock 
    std::unique_lock<std::mutex> lock(m_mutex); 

    // Add item 
    m_queue.push(item); 

    // Notify one thread that 
    // is waiting 
    m_cond.notify_one(); 
}

template <typename T>
T ThreadSafeQueue<T>::pop() {
    // acquire lock 
    std::unique_lock<std::mutex> lock(m_mutex); 

    // wait until queue is not empty 
    m_cond.wait(lock, [this]() { return !m_queue.empty(); }); 

    // retrieve item 
    T item = m_queue.front(); 
    m_queue.pop(); 

    // return item 
    return item; 
} 

template class ThreadSafeQueue<std::pair<long, std::unordered_map<std::string, float>>>;