/*****************************************************************************/
/*!
\file lfsv.h
\author Connor Deakin
\par E-mail: connor.deakin\@digipen.edu
\par DigiPen login: connor.deakin
\par Course: CS 355
\par Assignment: lfsv
\date 06/04/2018
\brief
  Contains an interface and implementation for a "lock-free" sorted vector.
*/
/*****************************************************************************/

#include <iostream>       // std::cout
#include <atomic>         // std::atomic
#include <thread>         // std::thread
#include <vector>         // std::vector
#include <deque>          // std::deque
#include <mutex>          // std::mutex
#include <cstring>

template< typename T, int NUMBER, unsigned SIZE >
class MemoryBank {
  std::deque< T* > slots;
  std::mutex m;
  public:
    MemoryBank() : slots(NUMBER) {
      for ( int i=0; i<NUMBER; ++i ) {
        slots[i] = reinterpret_cast<T*>( new char[ SIZE ] );
      }
    }
    T* Get() {
      std::lock_guard<std::mutex> lock( m );
      T* p = slots[0];
      slots.pop_front();
      return p;
    }
    void Store( T* p ) {
      std::memset( p, 0, SIZE ); // clear data
      std::lock_guard<std::mutex> lock( m );
      slots.push_back( p );
    }
    ~MemoryBank() {
      for ( auto & el : slots ) { delete [] reinterpret_cast<char*>( el ); }
    }
};

// stores vector pointer and size
// reference_counter stores how many threads are currently reading a value in
// the vector
struct Data
{
  int * pointer;
  int size;
  int reference_count;

  void Print()
  {
    char str[100];
    sprintf(str, "Size: %i\nCount: %i\n", size, reference_count);
    std::cout << str;
  }
}; //  __attribute__((aligned(16),packed)); // bug in GCC 4.*, fixed in 5.1?
// alignment needed to stop std::atomic<Data>::load to segfault

static RetiredPointers
{
  
}

struct HazardPointerNode
{
  HazardPointerNode * next;
  void * hazard_pointer;
  bool active;
}

class HazardPointerList
{
  HazardPointerNode * head;

}



// why not do some sort of multi pointer
// where every pointer has a refcount
// and when that ref count reaches zero
// it can be freed
// that's essentially hazard pointers

// each thread will hold it's own list of retire pointers
// figuring out how to get that in here might be a bit difficult

// A "lock-free" sorted vector implementation
class LFSV
{
  //MemoryBank<std::vector<int>, 290000, sizeof(void*) > mb;
  MemoryBank<int, 2000, sizeof(int[10000])> mb2;
public:
  std::atomic<Data> data;

  LFSV() : mb2(), data(Data{mb2.Get(), 0, 0})
  {}

  ~LFSV()
  {
    Data temp = data.load();
    int* p = temp.pointer;
    if ( p != nullptr )
    {
      mb2.Store( p );
    }
  }

  // Inserts a value into the vector. The vector will stay sorted in an
  // ascending order.
  void Insert(int const & v)
  {
    // value of data will only change on when its reference_count is 0
    Data data_old;
    Data data_new;
    data_new.pointer = nullptr;
    data_new.size = 0;
    do
    {
      // get current pointer and size
      // reference_count needs to be set to zero everytime because data_old
      // might be modified by compare_exchange_strong
      data_old = data.load();
      data_old.reference_count = 0;
      // get new pointer for data
      if(data_new.pointer != nullptr)
      {
        mb2.Store(data_new.pointer);
      }
      data_new.pointer = mb2.Get();
      data_new.size = data_old.size;
      data_new.reference_count = 0;
      std::memcpy(data_new.pointer, data_old.pointer,
        data_new.size * sizeof(int));
      int * vector = data_new.pointer;
      // add new value to array sorted in ascending order
      vector[data_new.size] = v;
      // switch value's position to sort
      for (int i=0; i < data_new.size; ++i)
      {
        if (vector[i] > v )
        {
          for (int j = data_new.size; j > i; --j)
          {
            // move new element to proper position
            std::swap(vector[j], vector[j-1]);
          }
          break;
        }
      }
      // set new size
      ++data_new.size;
    } while(!data.compare_exchange_strong(data_old, data_new));
    // store unused pointer
    mb2.Store(data_old.pointer);
  }

  // access a value of pos within the vector
  int operator[] ( int pos ) {
    Data data_old;
    Data data_new;
    // increment reference_count
    do
    {
      data_old = data.load();
      data_new = data_old;
      ++data_new.reference_count;
    } while(!data.compare_exchange_strong(data_old, data_new));
    int ret_val = data.load().pointer[pos];
    // decrement reference_count
    do
    {
      data_old = data.load();
      data_new = data_old;
      --data_new.reference_count;
    } while(!data.compare_exchange_strong(data_old, data_new));
    return ret_val;
  }
};
