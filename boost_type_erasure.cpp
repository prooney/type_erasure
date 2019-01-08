
#include <boost/type_erasure/any.hpp>
#include <boost/type_erasure/member.hpp>
#include <boost/type_erasure/iterator.hpp>
#include <boost/type_erasure/operators.hpp>
#include <boost/type_erasure/tuple.hpp>
#include <boost/type_erasure/same_type.hpp>

#include <vector>
#include <list>
#include <iostream>

using namespace boost::type_erasure;

BOOST_TYPE_ERASURE_MEMBER(push_back)

void append_many(any<has_push_back<void(int)>, _self&> container) 
{
    for(int i = 0; i < 10; ++i)
        container.push_back(i);
}

int main()
{
    using std::cout;

    std::vector<int> vi;
    append_many(vi);

    std::list<int> li;
    append_many(li);

    cout << "from vector\n";
    for(int x: vi) cout << x << '\n';
    cout << "from list\n";
    for(int x: li) cout << x << '\n';
}
