#ifndef _GENERATORS_H_
#define _GENERATORS_H_

#include<functional>
#include<pthread.h>

namespace generator_utils {

// Needed because pthreads wants a raw C function
void* invoke_callback(void* callback) {
  (*static_cast<std::function<void()>*>(callback))();
  return NULL;
}

template<typename T>
class ForLooping {
private:
  T* gen;
public:
  ForLooping(T* _gen) : gen(_gen) {};
  ForLooping<T>& operator++() {
    ++(*gen);
    return *this;
  }
  typename T::Output operator*(){
    return **gen;
  }
  operator bool() const{
    return gen && *gen;
  }
  operator T&() {
    return *gen;
  }
  bool operator!=(const ForLooping<T>& oth) {
    return static_cast<bool>(*this) != static_cast<bool>(oth);
  }
};

}

template<typename Heart>
class Gen; 

template<typename OutputT>
class GeneratorHeart {
private:
  OutputT value;
  bool hasOutputted;
  pthread_mutex_t* mutex;
public:
  typedef OutputT Output;
  template<typename T>
  friend class Gen;
  void yield(OutputT v) {
    value = v;
    hasOutputted = true;
    while(hasOutputted) {
      pthread_mutex_unlock(mutex);
      pthread_yield();
      pthread_mutex_lock(mutex);
    }
  }
};


template<typename Heart>
class Gen {
private:
  std::function<void()> callback;
  Heart heart;
  pthread_mutex_t mutex;
  pthread_t thread;
  bool done;
public:
  typedef typename Heart::Output Output;
  template<typename... ARGS>
  Gen(ARGS... args) {
    // Sanity checks for input
    if (false) {
      GeneratorHeart<Output>* just_checking = &heart;
      heart.run(args...);
    }
    done = false;
    // Use bind instead of capture because capture does not get along with parameter packs
    callback=std::bind([&](Gen<Heart>* gen, ARGS... args){ gen->threadmain(args...); }, this, args...);
    pthread_mutex_init(&this->mutex, NULL);
    pthread_mutex_lock(&this->mutex);
    heart.mutex = &this->mutex;
    pthread_create (&this->thread, NULL, generator_utils::invoke_callback, static_cast<void*>(&callback));
    ++(*this);
  }
  ~Gen() {
    if (!done) {
      pthread_cancel(this->thread);
    }
    pthread_mutex_destroy(&this->mutex);
  }
  template<typename... ARGS>
  void threadmain(ARGS... args) {
    pthread_mutex_lock(&this->mutex);
    heart.run(args...);
    done = true;
    pthread_mutex_unlock(&this->mutex);
    pthread_exit(NULL);
  }
  operator bool() {
    return !done;
  }
  Gen<Heart>& operator++() {
    heart.hasOutputted = false;
    while(!heart.hasOutputted && !done){
      pthread_mutex_unlock(&mutex);
      pthread_yield();
      pthread_mutex_lock(&mutex);
    }
    return *this;
  }
  Output operator*() {
    return heart.value;
  }
  typedef generator_utils::ForLooping<Gen<Heart>> ForLooping;
  ForLooping begin() { return ForLooping(this); }
  ForLooping end() { return ForLooping(NULL); }
};  

#endif  // _GENERATORS_H_
