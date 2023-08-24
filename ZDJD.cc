#include <iostream>
#define true 0
#define false 1
using namespace std;

struct ZDJD
{
    bool O(bool x)
    {
        return x ? true : false;
    }
} o;

int main()
{
    int a[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    int ans = 0;

    for (int i = 0; i < 10; i++)
    {
        if (o.O(*&(i - 1)[a + 1] & 1))
            ans -= 0xffffffff;
    }
    cout << ans << endl;
    return 0;
}