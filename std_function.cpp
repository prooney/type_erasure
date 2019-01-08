#include <iostream>
#include <memory>
using std::cout;

namespace pjr
{
    template<typename>
    class function;

    template<typename R, typename... Args>
    class function<R(Args...)>
    {
        struct concept
        {
            virtual ~concept() = default;
            virtual R invoke(Args&&...) = 0;
        };

        template <typename F>
        struct model : public concept
        {
            model(F&& fn) : _fn(std::forward<F>(fn)) {} 
            R invoke(Args&&... args) override 
            { 
                return _fn(std::forward<Args>(args)...); 
            }
            private:
            F _fn;
        };

        std::unique_ptr<concept> _stored;
        public:

        using result_type = R;

        template<typename F>
        function(F&& fn)
        {
            _stored = std::make_unique<model<F>>(std::forward<F>(fn));
        }
        
        R operator()(Args... args)
        {
            return _stored->invoke(std::forward<Args>(args)...);
        }
    };
}

void foo()
{
    cout << "foo\n";
}

void bar(int)
{
    cout << "bar\n";
}

int baz(int)
{
    cout << "baz\n";
    return 1;
}

struct functor
{
    void operator()()
    {
        cout << "functor\n";
    } 
};

int main()
{
    using pjr::function;

    function<void()> f1 = foo;
    function<void(int)> f2 = bar;
    function<int(int)> f3 = baz;
    function<void()> f4 = [](){ cout << "lambda\n"; };

    f1();
    f2(1);
    int ret = f3(2);
    f4();

}

