#include <cassert>
#include <iostream>
#include "BigInt.h"
using namespace std;

CBigInt EuclidGcd(const CBigInt &a, const CBigInt &b)
{
    CBigInt smaller = (a > b) ? b : a;
    CBigInt biger = (a > b) ? a : b;

    biger = biger % smaller;
    while (biger != 0)
    {
        CBigInt tmp = biger;
        biger = smaller;
        smaller = tmp;
        biger = biger % smaller;
    }

    return smaller;
}

CBigInt SteinGcd(const CBigInt &a, const CBigInt &b)
{
    CBigInt biger = (a > b) ? a : b;
    CBigInt smaller = (a > b) ? b : a;

    if (smaller == 0)
        return biger;

    if (biger % 2 == 0 && smaller % 2 == 0)
        return SteinGcd(biger / 2, smaller / 2) * 2;

    if (biger % 2 == 0)
        return SteinGcd(biger / 2, smaller);

    if (smaller % 2 == 0)
        return SteinGcd(biger, smaller / 2);

    return SteinGcd((biger + smaller) / 2, (biger - smaller) / 2);
}

CBigInt SubstractGcd(const CBigInt &a, const CBigInt &b)
{
    CBigInt aa = a;
    CBigInt bb = b;
    while (aa != bb)
        if (aa > bb)
            aa -= bb;
        else
            bb -= aa;
    return aa;
}

CBigInt GcdLcm(const CBigInt &a, const CBigInt &b)
{
    return a * b / EuclidGcd(a, b);
}

CBigInt NormalLcm(const CBigInt &a, const CBigInt &b)
{
    CBigInt r = a;
    while (r % b != 0)
        r += a;
    return r;
}

void TestAdd()
{
    CBigInt add1;
    CBigInt add2;
    CBigInt r;
    CBigInt result;

    add1 = 0x7FFFFFFFFFFFFFFFL;
    add2 = 1;
    r = add1 + add2;
    result.GetFromString("8000000000000000", 16);
    assert(r == result);

    add1 = -120;
    add2 = 100;
    r = add1 + add2;
    result = -20;
    assert(r == result);

    add1 = -120;
    add2 = -100;
    r = add1 + add2;
    result = -220;
    assert(r == result);

    add1.GetFromString("313532612402", 10);
    add2.GetFromString("-2402", 10);
    r = add1 + add2;
    result.GetFromString("313532610000", 10);
    assert(r == result);

    add1.GetFromString("352563396108773804955", 10);
    add2.GetFromString("-4955", 10);
    r = add1 + add2;
    result.GetFromString("352563396108773800000", 10);
    assert(r == result);
}

void TestSub()
{
    CBigInt sub1;
    CBigInt sub2;
    CBigInt r;
    CBigInt result;

    sub1 = 0x7FFFFFFFFFFFFFFFL;
    sub2 = 1;
    r = sub1 - sub2;
    result = 0x7FFFFFFFFFFFFFFEL;
    assert(r == result);

    sub1 = -120;
    sub2 = 100;
    r = sub1 - sub2;
    result = -220;
    assert(r == result);

    sub1 = -120;
    sub2 = -100;
    r = sub1 - sub2;
    result = -20;
    assert(r == result);

    sub1 = 120;
    sub2 = -100;
    r = sub1 - sub2;
    result = 220;
    assert(r == result);

    sub1.GetFromString("352563396108773804955", 10);
    sub2.GetFromString("4955", 10);
    r = sub1 - sub2;
    result.GetFromString("352563396108773800000", 10);
    assert(r == result);

    sub1.GetFromString("10000000000000000", 16);
    sub2 = 1;
    r = sub2 - sub1;
    result.GetFromString("-FFFFFFFFFFFFFFFF", 16);
    assert(r == result);
}

void TestMul()
{
    CBigInt mul1;
    CBigInt mul2;
    CBigInt r;
    CBigInt result;

    mul1 = 0x7FFFFFFFFFFFFFFFL;
    mul2 = 8;
    r = mul1 * mul2;
    result.GetFromString("73786976294838206456", 10);
    assert(r == result);

    mul1.GetFromString("56FFFFFF32", 16);
    mul2 = 1;
    r = mul1 * mul2;
    result.GetFromString("56FFFFFF32", 16);
    assert(r == result);

    mul1 = 1;
    mul2.GetFromString("56FFFFFF32", 16);
    r = mul1 * mul2;
    result.GetFromString("56FFFFFF32", 16);
    assert(r == result);

    mul1 = 120;
    mul2 = -2;
    r = mul1 * mul2;
    result = -240;
    assert(r == result);

    mul1 = -120;
    mul2 = -2;
    r = mul1 * mul2;
    result = 240;
    assert(r == result);

    mul1.GetFromString("10000005F7FFFFFFF", 16);
    mul2 = 0x0F;
    r = mul1 * mul2;
    result.GetFromString("F000005987FFFFFF1", 16);
    assert(r == result);

    mul1.GetFromString("1445628445840946744607609235783709524795667819061352", 10);
    mul2.GetFromString("18446445674407667997096551006152135", 10);
    r = mul1 * mul2;
    result.GetFromString("26666706591583411822193194835830799779242590142029250059182106110878932434973010786520", 10);
    assert(r == result);
}

void TestDiv()
{
    CBigInt div1;
    CBigInt div2;
    CBigInt r;
    CBigInt result;

    div1.GetFromString("73786976294838206456", 10);
    div2 = 8;
    r = div1 / div2;
    result = 0x7FFFFFFFFFFFFFFFL;
    assert(r == result);

    div1 = 120;
    div2 = -2;
    r = div1 / div2;
    result = -60;
    assert(r == result);

    div1 = -120;
    div2 = -2;
    r = div1 / div2;
    result = 60;
    assert(r == result);

    div1 = -120;
    div2 = 2;
    r = div1 / div2;
    result = -60;
    assert(r == result);
}

void TestMod()
{
    CBigInt mod1;
    CBigInt mod2;
    CBigInt r;
    CBigInt result;

    mod1.GetFromString("73786976294838206456", 10);
    mod2 = 8;
    r = mod1 % mod2;
    result = 0;
    assert(r == result);

    mod1 = 18;
    mod2 = -5;
    r = mod1 % mod2;
    result = 3;
    assert(r == result);

    mod1 = -18;
    mod2 = -5;
    r = mod1 % mod2;
    result = -3;
    assert(r == result);

    mod1 = 18;
    mod2 = -5;
    r = mod1 % mod2;
    result = 3;
    assert(r == result);
}

void TestCompare()
{
    CBigInt com1;
    CBigInt com2;

    com1 = 1;
    com2 = 0;
    assert(com1 > com2);

    com1 = 0;
    com2 = 0;
    assert(com1 == com2);
    assert(com1 <= com2);
    assert(com1 >= com2);

    com1.GetFromString("-73786976294838206456", 10);
    com2 = -1;
    assert(com1 < com2);

    com1.GetFromString("-73786976294838206456", 10);
    com2.GetFromString("-73786976294838206455", 10);
    assert(com1 < com2);

    com1.GetFromString("73786976294838206455", 10);
    com2.GetFromString("73786976294838206455", 10);
    assert(com1 == com2);
    assert(com1 <= com2);
    assert(com1 >= com2);

    com1.GetFromString("0", 10);
    com2.GetFromString("-73786976294838206455", 10);
    assert(com1 > com2);

    com1.GetFromString("-1", 10);
    com2.GetFromString("5567", 10);
    assert(com1 < com2);

    com1.GetFromString("-1", 10);
    com2 = -1;
    assert(com1 == com2);
    assert(com1 <= com2);
    assert(com1 >= com2);
}

void TestPow()
{
    CBigInt value;
    CBigInt n;
    CBigInt r;
    CBigInt result;

    value = 0;
    n = 3;
    r = value ^ n;
    result = 0;
    assert(r == result);

    value = 123;
    n = 0;
    r = value ^ n;
    result = 1;
    assert(r == result);

    value = 123;
    n = 1;
    r = value ^ n;
    result = 123;
    assert(r == result);

    value.GetFromString("56FFFFFF32", 16);
    n = 1;
    r = value ^ n;
    result.GetFromString("56FFFFFF32", 16);
    assert(r == result);

    value.GetFromString("56FFFF32", 16);
    n = 2;
    r = value ^ n;
    result.GetFromString("1D90FF73FC00A5C4", 16);
    assert(r == result);

    value.GetFromString("89", 16);
    n = 6;
    r = value ^ n;
    result.GetFromString("6037157D6F1", 16);
    assert(r == result);

    value.GetFromString("-89", 16);
    n = 6;
    r = value ^ n;
    result.GetFromString("6037157D6F1", 16);
    assert(r == result);

    value.GetFromString("-113", 16);
    n = 7;
    r = value ^ n;
    result.GetFromString("-1A68F8796D1357B", 16);
    assert(r == result);
}

void TestGcd()
{
    CBigInt gcd;

    gcd = EuclidGcd(15, 12);
    assert(gcd == 3);

    gcd = SteinGcd(15, 12);
    assert(gcd == 3);

    gcd = SubstractGcd(15, 12);
    assert(gcd == 3);
}

void TestLcm()
{
    CBigInt lcm;
    CBigInt result;

    result = 0x3c;

    lcm = GcdLcm(15, 12);
    assert(result == lcm);

    lcm = NormalLcm(15, 12);
    assert(result == lcm);
}

int main()
{
    TestAdd();
    TestSub();
    TestMul();
    TestDiv();
    TestMod();
    TestCompare();
    TestPow();
    TestGcd();
    TestLcm();

    CBigInt value = 9;
    CBigInt n = 81;
    CBigInt r = value ^ n;

    string rrr;

    r /= 100000000000;
    r /= 3600;
    r /= 24;
    r /= 365;
    r.PutToString(rrr, 10);
    cout << rrr << endl;

    value.GetFromString("6670903752021072936960", 10);
    n.GetFromString("6000000000", 10);

    r = value / n;
    r.PutToString(rrr, 10);
    cout << rrr << endl;

    cout << "Over!\n";
    return 0;
}
