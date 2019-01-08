#include <memory>
#include <vector>
#include <typeinfo>


namespace pjr
{
    struct concept
    {
        virtual ~concept(){}
        virtual const std::type_info& type() const = 0;
        virtual concept* clone() const = 0;
    };

    template <typename T>
    class model : public concept
    {
        T val;
        public:
        model(const T& m)
            : val(m)
        {}

        const std::type_info& type() const override 
        {
            return typeid(T);
        }

        concept* clone() const override
        {
            return new model<T>(val);
        }
    };

    class any
    {
        std::unique_ptr<concept> val;

        public:
        any() : val(nullptr)
        {}

        template <typename T>
        any(T&& t)
          : val(new model<T>(std::forward<T>(t)))
        {}

        any(const any& other)
            : val(other.val->clone())
        {}


        //any& operator=(any& rhs)
        //{
        //}

        // this function needs sfinae to exclude `any` from the overload set.
        // otherwise the any nests i.e. any<any<T>>
        template<typename U>
        any& operator=(U&& rhs)
        {
            any(rhs).swap(*this);
            return *this;
        }

        const std::type_info& type() const noexcept
        {
            return val->type();
        }

        void swap(any& other) noexcept
        {
            val.swap(other.val);
        }
    };

    template<typename T>
    T& any_cast(const any& a)
    {
        if(a.type() == typeid(T)) 
            return static_cast<T>(a);

        throw std::bad_cast();
    }
}

int main()
{
    using pjr::any;
    using pjr::concept;
    using pjr::model;

    // used outside any: note its cumbersome to deal with pointer sematics
    std::vector<std::unique_ptr<concept>> vals;

    concept* c = new model(1);
    delete c;

    std::unique_ptr<concept> x = std::make_unique<model<int>>(1);

    vals.push_back(std::make_unique<model<int>>(1));
    vals.push_back(std::make_unique<model<std::string>>("hi"));

    // used inside any now we have value semantics.

    std::vector<any> avals;

    avals.push_back(1);
    avals.push_back("123");
    avals.push_back(vals);

    any xi = 2;
    any y = std::string("waha");

    const any p(3);

    double fd = 4.56;
    const any da(fd);

    any w;

    // copy ctor
    any cc(p);
    // assign
    w = 2;

    // crumbs creates a nested any!!
    w = cc;
}

