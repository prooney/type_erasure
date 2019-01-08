
#include <iostream>
using std::cout;

struct place_holder
{
    virtual ~place_holder() = default;
    virtual void invoke() = 0;
};

template<typename Func>
struct holder : place_holder
{
    Func fn_;

    holder(Func&& fn) 
      : fn_(std::forward<Func>(fn))
    {}

    void invoke() override
    {
        cout << "will invoke function\n";
        // might be better to use std::invoke here
        fn_();
    }
};


void* pthread_func(void* ctx)
{
    cout << "pthread started\n";
    place_holder* ph = static_cast<place_holder*>(ctx);

    ph->invoke();

    delete ph;
    return nullptr;
}


// simple thread class that provides for an argless and void returning function
// to be passed to the new thread. It's obviously way more complex for the 
// various other combinations of variadic args and member function pointers 
// (detected and dispatched using sfinae).
class thread
{
    pthread_t handle_;

    static void* thread_fn(void* ctx)
    {
        place_holder* ph = static_cast<place_holder*>(ctx);

        ph->invoke();

        free(ph);
        return nullptr;
    }

    public:

    template<typename Func>
    thread(Func&& fn)
    {
        pthread_create(&handle_, 
                       nullptr, 
                       thread_fn, 
                       new holder(std::forward<Func>(fn)));
    }

    thread(const thread&) = delete;
    thread& operator=(const thread&) = delete;

    void join()
    {
        pthread_join(handle_, nullptr);
    }
};

int main()
{
    /*
     * use holder directly with pthread api.
     */
    pthread_t handle;

    // Its not possible to name the type here (at least not 
    // portably as the type signature involves a lambda).
    auto h = new holder([](){ cout << "hi from thread\n"; });

    pthread_create(&handle, nullptr, &pthread_func, h);
    pthread_join(handle, nullptr);

    /*
     * use thread object
     */
    thread t1([](){ cout << "hello again\n"; });
    t1.join();
}

