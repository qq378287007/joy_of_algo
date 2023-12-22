
#include <windows.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <cassert>
#include <cmath>
using namespace std;

#define MAX_BI_LEN 256
#define DEC 10
#define HEX 16
#define MSBIT ((unsigned long)1 << 31)
#define OBITS (MSBIT - 1)
class CBigInt
{
public:
    // 符号位，0表示正数，1表示负数
    unsigned int m_Sign;
    // 大数在0x100000000进制下的数字位数
    unsigned int m_nLength;
    // 用数组记录大数在0x100000000进制下每一位的值
    unsigned long m_ulValue[MAX_BI_LEN];

public:
    CBigInt()
    {
        ZeroData(MAX_BI_LEN);
    }
    CBigInt(const CBigInt &value)
    {
        SetValue(value);
    }
    CBigInt(__int64 value)
    {
        SetValue(value);
    }

    int ToInt()
    {
        int result = m_ulValue[0];
        if (m_Sign != 0)
            result = -result;

        return result;
    }

    bool IsZero() const
    {
        if ((m_nLength == 1) && (m_ulValue[0] == 0))
            return true;

        return false;
    }

    bool IsEven() const
    {
        if (m_ulValue[0] & 0x00000001)
            return false;
        else
            return true;
    }

    CBigInt GetNegative() const
    {
        CBigInt r = *this;

        r.m_Sign = (this->m_Sign == 0) ? 1 : 0;
        return r;
    }

    const CBigInt &operator=(__int64 value)
    {
        SetValue(value);
        return *this;
    }

    const CBigInt &operator=(const CBigInt &value)
    {
        SetValue(value);
        return *this;
    }

    bool operator<(const CBigInt &value) const
    {
        if ((m_Sign == 0) && (value.m_Sign == 1))
            return false;
        else if ((m_Sign == 1) && (value.m_Sign == 0))
            return true;
        else if ((m_Sign == 0) && (value.m_Sign == 0))
            return (CompareNoSign(value) < 0);
        else
            return (CompareNoSign(value) > 0);
    }

    bool operator<=(const CBigInt &value) const
    {
        if ((m_Sign == 0) && (value.m_Sign == 1))
            return false;
        else if ((m_Sign == 1) && (value.m_Sign == 0))
            return true;
        else if ((m_Sign == 0) && (value.m_Sign == 0))
            return (CompareNoSign(value) <= 0);
        else
            return (CompareNoSign(value) >= 0);
    }

    bool operator==(const CBigInt &value) const
    {
        if (m_Sign != value.m_Sign)
            return false;
        else
            return (CompareNoSign(value) == 0);
    }

    bool operator!=(const CBigInt &value) const
    {
        if ((m_Sign == value.m_Sign) && (CompareNoSign(value) == 0))
            return false;
        else
            return true;
    }

    bool operator>(const CBigInt &value) const
    {
        if ((m_Sign == 0) && (value.m_Sign == 1))
            return true;
        else if ((m_Sign == 1) && (value.m_Sign == 0))
            return false;
        else if ((m_Sign == 0) && (value.m_Sign == 0))
            return (CompareNoSign(value) > 0);
        else
            return (CompareNoSign(value) < 0);
    }

    bool operator>=(const CBigInt &value) const
    {
        if ((m_Sign == 0) && (value.m_Sign == 1))
            return true;
        else if ((m_Sign == 1) && (value.m_Sign == 0))
            return false;
        else if ((m_Sign == 0) && (value.m_Sign == 0))
            return (CompareNoSign(value) >= 0);
        else
            return (CompareNoSign(value) <= 0);
    }

    CBigInt operator+(const CBigInt &value) const
    {
        CBigInt r;

        if (m_Sign == value.m_Sign)
        {
            CBigInt::Add(*this, value, r);
            r.m_Sign = m_Sign;
        }
        else
        {
            if (CompareNoSign(value) >= 0)
            {
                CBigInt::Sub(*this, value, r);
                if (r.IsZero())
                    r.m_Sign = 0;
                else
                    r.m_Sign = m_Sign;
            }
            else
            {
                CBigInt::Sub(value, *this, r);
                r.m_Sign = value.m_Sign;
            }
        }

        return r;
    }

    const CBigInt &operator+=(const CBigInt &value)
    {
        *this = this->operator+(value);

        return *this;
    }

    CBigInt operator-(const CBigInt &value) const
    {
        CBigInt r;

        if (m_Sign != value.m_Sign)
        {
            CBigInt::Add(*this, value, r);
            r.m_Sign = m_Sign;
        }
        else
        {
            if (CompareNoSign(value) >= 0)
            {
                CBigInt::Sub(*this, value, r);
                if (r.IsZero())
                    r.m_Sign = 0;
                else
                    r.m_Sign = m_Sign;
            }
            else
            {
                CBigInt::Sub(value, *this, r);
                r.m_Sign = (m_Sign == 0) ? 1 : 0; // 需要变号
            }
        }

        return r;
    }

    const CBigInt &operator-=(const CBigInt &value)
    {
        *this = this->operator-(value);

        return *this;
    }

    CBigInt operator*(const CBigInt &value) const
    {
        CBigInt r;

        if (m_Sign != value.m_Sign)
        {
            CBigInt::Mul(*this, value, r);
            r.m_Sign = 1;
        }
        else
        {
            CBigInt::Mul(*this, value, r);
            r.m_Sign = 0;
        }

        return r;
    }

    const CBigInt &operator*=(const CBigInt &value)
    {
        *this = this->operator*(value);

        return *this;
    }

    CBigInt operator/(const CBigInt &value) const
    {
        CBigInt r, m;

        if (m_Sign != value.m_Sign)
        {
            CBigInt::Div(*this, value, r, m);
            r.m_Sign = 1;
        }
        else
        {
            CBigInt::Div(*this, value, r, m);
            r.m_Sign = 0;
        }

        return r;
    }

    const CBigInt &operator/=(const CBigInt &value)
    {
        *this = this->operator/(value);

        return *this;
    }

    CBigInt operator%(const CBigInt &value) const
    {
        CBigInt r, m;

        CBigInt::Div(*this, value, r, m);
        m.m_Sign = m_Sign;

        return m;
    }

    const CBigInt &operator%=(const CBigInt &value)
    {
        *this = this->operator%(value);

        return *this;
    }

    CBigInt operator^(const CBigInt &n) const
    {
        CBigInt r;

        CBigInt::Power(*this, n, r);
        if (n.IsEven())
        {
            r.m_Sign = 0;
        }
        else
        {
            r.m_Sign = this->m_Sign;
        }

        return r;
    }

    const CBigInt &operator^=(const CBigInt &n)
    {
        *this = this->operator^(n);

        return *this;
    }

    CBigInt operator<<(__int64 bits) const
    {
        CBigInt r;

        CBigInt::ShiftLeft(*this, bits, r);
        r.m_Sign = this->m_Sign;

        return r;
    }

    const CBigInt &operator<<=(__int64 bits)
    {
        CBigInt::ShiftLeft(*this, bits, *this);

        return *this;
    }

    CBigInt operator>>(__int64 bits) const
    {
        CBigInt r;

        CBigInt::ShiftRight(*this, bits, r);
        r.m_Sign = this->m_Sign;

        return r;
    }

    const CBigInt &operator>>=(__int64 bits)
    {
        CBigInt::ShiftRight(*this, bits, *this);

        return *this;
    }

    // 大整数位的操作
    void GetRightBigN(unsigned int N, CBigInt &rightN)
    {
        rightN = 0;
        unsigned int rN = std::min(m_nLength, N);
        for (unsigned int i = 0; i < rN; i++)
        {
            rightN.m_ulValue[i] = m_ulValue[i];
        }
        rightN.m_nLength = rN;
        rightN.m_Sign = m_Sign;
    }

    inline int GetBigNCount() const { return m_nLength; };

    bool ShiftLeftBigN(unsigned int N)
    {
        if ((m_nLength + N) > MAX_BI_LEN)
            return false;

        int i;
        for (i = m_nLength - 1; i >= 0; i--)
        {
            m_ulValue[N + i] = m_ulValue[i];
        }
        for (i = 0; i < N; i++)
        {
            m_ulValue[i] = 0;
        }
        m_nLength += N;
        return true;
    }

    void ShiftRightBigN(unsigned int N)
    {
        if (N >= m_nLength)
        {
            ZeroData(m_nLength);
            return;
        }
        for (unsigned int i = 0; i < m_nLength; i++)
        {
            if (i < (m_nLength - N))
                m_ulValue[i] = m_ulValue[N + i];
            else
                m_ulValue[i] = 0;
        }

        m_nLength -= N;
    }

    void RemRightBigN(unsigned int N)
    {
        if (m_nLength > N)
        {
            for (unsigned int i = N; i < m_nLength; i++)
            {
                m_ulValue[i] = 0;
            }
            m_nLength = N;

            while ((m_ulValue[m_nLength - 1] == 0) && (m_nLength > 1))
                m_nLength--;
        }
    }

    void TrimRightBigN(unsigned int N)
    {
        if (m_nLength <= N)
        {
            ZeroData(m_nLength);
            return;
        }
        unsigned int left = m_nLength - N;
        for (unsigned int i = 0; i < m_nLength; i++)
        {
            if (i < left)
            {
                m_ulValue[i] = m_ulValue[i + N];
            }
            else
            {
                m_ulValue[i] = 0;
            }
        }
        m_nLength = left;
    }

    bool TestBit(__int64 bits) const
    {
        __int64 wp = bits / 32;
        __int64 bp = bits % 32;

        if (wp >= m_nLength)
            return false;

        __int64 bitMask = 0x1 << bp;

        return (m_ulValue[wp] & bitMask) != 0;
    }

    __int64 GetTotalBits() const
    {
        __int64 c = (m_nLength - 1) * 32;

        unsigned long mv = m_ulValue[m_nLength - 1];
        while (mv)
        {
            mv = mv >> 1;
            c++;
        }

        return c;
    }

    // 数据转换接口
    void GetFromString(const char *str, unsigned int system = HEX)
    {
        ZeroData(MAX_BI_LEN);
        int i = 0;
        int len = (int)strlen(str), k;
        if ((len == 0) || ((len == 1) && (str[0] == '-')))
        {
            return;
        }
        if (str[0] == '-')
            i++;

        while (i < len)
        {
            CBigInt::Mul(*this, system, *this);
            if ((str[i] >= '0') && (str[i] <= '9'))
                k = str[i] - 48;
            else if ((str[i] >= 'A') && (str[i] <= 'F'))
                k = str[i] - 55;
            else if ((str[i] >= 'a') && (str[i] <= 'f'))
                k = str[i] - 87;
            else
                k = 0;
            CBigInt::Add(*this, k, *this);
            i++;
        }
        if (str[0] == '-')
            m_Sign = 1;
    }

    void PutToString(std::string &str, unsigned int system = HEX)
    {
        if ((m_nLength == 1) && (m_ulValue[0] == 0))
        {
            str = "0";
            return;
        }
        str = "";
        const char *t = "0123456789ABCDEF";
        CBigInt value = *this;
        CBigInt sys = system;
        while (value.m_ulValue[value.m_nLength - 1] > 0)
        {
            CBigInt a;
            CBigInt::Div(value, sys, value, a);
            str.insert(str.begin(), t[abs(a.ToInt())]);
        }
        if (m_Sign == 1)
        {
            str.insert(str.begin(), '-');
        }
    }

    void GetLong(unsigned long *values, unsigned int count)
    {
        ZeroData(MAX_BI_LEN);
        m_nLength = count;
        for (unsigned int i = 0; i < count; i++)
            m_ulValue[i] = values[i];
    }

    int PutLong(unsigned long *values, unsigned int bufLen)
    {
        if (m_nLength > bufLen)
            return 0;

        for (unsigned int i = 0; i < m_nLength; i++)
            values[i] = m_ulValue[i];

        return m_nLength;
    }

    int GetFromData(const char *pData, int nBytes) // 返回实际的位数
    {
        int i, j;

        ZeroData(MAX_BI_LEN);
        int n = nBytes / sizeof(unsigned long);
        int r = nBytes % sizeof(unsigned long);
        if (n >= MAX_BI_LEN)
        {
            return -1;
        }

        unsigned long uword = 0;
        unsigned long tmv;
        m_nLength = n;
        if (r != 0)
        {
            m_nLength++;
            for (j = 0; j < r; j++)
            {
                uword <<= 8;
                tmv = *pData++;
                uword |= (tmv & 0x000000FF);
            }
            m_ulValue[m_nLength - 1] = uword;
        }
        for (i = n - 1; i >= 0; i--)
        {
            uword = 0;
            for (j = 0; j < sizeof(unsigned long); j++)
            {
                uword <<= 8;
                tmv = *pData++;
                uword |= (tmv & 0x000000FF);
            }
            m_ulValue[i] = uword;
        }

        while ((m_ulValue[m_nLength - 1] == 0) && (m_nLength > 1))
            m_nLength--;

        return nBytes;
    }

    int PutToData(char *pBuffer, int nBufferBytes) // 返回数据的字节数
    {
        int i, j;

        int len = (m_nLength - 1) * sizeof(unsigned long);
        unsigned long uword = m_ulValue[m_nLength - 1]; /* most significant */
        int r = 0;
        while (uword != 0)
        {
            r++;
            uword >>= 8;
            len++;
        }
        r %= sizeof(unsigned long);

        if (len > nBufferBytes)
        {
            return 0;
        }

        int start = nBufferBytes - len;
        for (i = 0; i < start; i++)
            pBuffer[i] = 0;

        int n = m_nLength - 1;
        if (r != 0)
        {
            uword = m_ulValue[n--];
            for (i = r - 1; i >= 0; i--)
            {
                pBuffer[start + i] = (char)(uword & 0x000000FF);
                uword >>= 8;
            }
        }

        for (i = r; i < len; i += sizeof(unsigned long))
        {
            uword = m_ulValue[n--];
            for (j = sizeof(unsigned long) - 1; j >= 0; j--)
            {
                pBuffer[start + i + j] = (char)(uword & 0x000000FF);
                uword >>= 8;
            }
        }

        return nBufferBytes;
    }

    static CBigInt GenRandomInteger(__int64 bits)

    {
        CBigInt r;

        r.m_Sign = 0;
        r.m_nLength = (unsigned int)(bits / 32);

        for (unsigned int i = 0; i < r.m_nLength; i++)
            r.m_ulValue[i] = rand() * 0x10000 + rand();

        __int64 woff = bits % 32;
        if (woff != 0)
        {
            r.m_nLength++;
            r.m_ulValue[r.m_nLength - 1] = rand() * 0x10000 + rand();
            r.m_ulValue[r.m_nLength - 1] = r.m_ulValue[r.m_nLength - 1] & (0xFFFFFFFF >> (32 - woff));
        }

        return r;
    }

protected:
    void ZeroData(unsigned int length)

    {
        if (length > MAX_BI_LEN)
        {
            length = MAX_BI_LEN;
        }
        m_Sign = 0;
        m_nLength = 1;
        for (unsigned int i = 0; i < length; i++)
            m_ulValue[i] = 0;
    }

    void SetValue(const CBigInt &value)
    {
        m_Sign = value.m_Sign;
        m_nLength = value.m_nLength;
        for (int i = 0; i < MAX_BI_LEN; i++)
            m_ulValue[i] = value.m_ulValue[i];
    }

    void SetValue(__int64 value)
    {
        unsigned __int64 v;
        if (value >= 0)
        {
            m_Sign = 0;
            v = value;
        }
        else
        {
            m_Sign = 1;
            v = -value;
        }
        if (v > 0xffffffff)
        {
            m_nLength = 2;
            m_ulValue[1] = (unsigned long)(v >> 32);
            m_ulValue[0] = (unsigned long)v;
        }
        else
        {
            m_nLength = 1;
            m_ulValue[0] = (unsigned long)v;
        }
        for (int i = m_nLength; i < MAX_BI_LEN; i++)
            m_ulValue[i] = 0;
    }

    int CompareNoSign(const CBigInt &value) const

    {
        if (m_nLength > value.m_nLength)
            return 1;
        if (m_nLength < value.m_nLength)
            return -1;
        for (int i = m_nLength - 1; i >= 0; i--)
        {
            if (m_ulValue[i] > value.m_ulValue[i])
                return 1;
            if (m_ulValue[i] < value.m_ulValue[i])
                return -1;
        }
        return 0;
    }

    static void Add(const CBigInt &value1, const CBigInt &value2, CBigInt &result)
    {
        CBigInt r = value1;

        unsigned carry = 0;
        /*先调整位数对齐*/
        if (r.m_nLength < value2.m_nLength)
            r.m_nLength = value2.m_nLength;
        for (unsigned int i = 0; i < r.m_nLength; i++)
        {
            unsigned __int64 sum = value2.m_ulValue[i];
            sum = sum + r.m_ulValue[i] + carry;
            r.m_ulValue[i] = (unsigned long)sum;
            carry = (unsigned)(sum >> 32);
        }
        // 处理最高位，如果当前最高位进位carry!=0，则需要增加大数的位数
        r.m_ulValue[r.m_nLength] = carry;
        r.m_nLength += carry;

        result = r;
    }

    static void Sub(const CBigInt &value1, const CBigInt &value2, CBigInt &result)
    {
        CBigInt r = value1;

        unsigned int borrow = 0;
        for (unsigned int i = 0; i < r.m_nLength; i++)
        {
            if ((r.m_ulValue[i] > value2.m_ulValue[i]) || ((r.m_ulValue[i] == value2.m_ulValue[i]) && (borrow == 0)))
            {
                r.m_ulValue[i] = r.m_ulValue[i] - borrow - value2.m_ulValue[i];
                borrow = 0;
            }
            else
            {
                unsigned __int64 num = 0x100000000 + r.m_ulValue[i];
                r.m_ulValue[i] = (unsigned long)(num - borrow - value2.m_ulValue[i]);
                borrow = 1;
            }
        }
        while ((r.m_ulValue[r.m_nLength - 1] == 0) && (r.m_nLength > 1))
            r.m_nLength--;

        result = r;
    }

    static void Mul(const CBigInt &value1, const CBigInt &value2, CBigInt &result)
    {
        if (value1.IsZero() || value2.IsZero())
        {
            result = 0;
            return;
        }
        unsigned __int64 carry = 0;
        CBigInt r;
        r.m_nLength = value1.m_nLength + value2.m_nLength - 1; // 初步估算结果的位数
        for (unsigned int i = 0; i < r.m_nLength; i++)
        {
            unsigned __int64 sum = carry;
            carry = 0;
            for (unsigned int j = 0; j < value2.m_nLength; j++)
            {
                if (((i - j) >= 0) && ((i - j) < value1.m_nLength))
                {
                    unsigned __int64 mul = value1.m_ulValue[i - j];
                    mul *= value2.m_ulValue[j];
                    carry += mul >> 32;
                    mul = mul & 0xffffffff;
                    sum += mul;
                }
            }
            carry += sum >> 32;
            r.m_ulValue[i] = (unsigned long)sum;
        }
        if (carry != 0) // 最后仍有进位，则大数位数需要扩大
        {
            r.m_nLength++;
            r.m_ulValue[r.m_nLength - 1] = (unsigned long)carry;
        }
        result = r;
    }
    
    static void Div(const CBigInt &value1, const CBigInt &value2, CBigInt &quotient, CBigInt &remainder)
    {
        CBigInt r = 0;
        CBigInt a = value1;
        while (a.CompareNoSign(value2) >= 0)
        {
            unsigned __int64 div = a.m_ulValue[a.m_nLength - 1];
            unsigned __int64 num = value2.m_ulValue[value2.m_nLength - 1];
            unsigned int len = a.m_nLength - value2.m_nLength;
            if ((div == num) && (len == 0))
            {
                CBigInt::Add(r, CBigInt(1), r);
                CBigInt::Sub(a, value2, a);
                break;
            }
            if ((div <= num) && (len > 0))
            {
                len--;
                div = (div << 32) + a.m_ulValue[a.m_nLength - 2];
            }
            div = div / (num + 1);
            CBigInt multi = div; // 试商的结果
            if (len > 0)
            {
                multi.m_nLength += len;
                unsigned int i;
                for (i = multi.m_nLength - 1; i >= len; i--)
                    multi.m_ulValue[i] = multi.m_ulValue[i - len];
                for (i = 0; i < len; i++)
                    multi.m_ulValue[i] = 0;
            }
            CBigInt tmp;
            CBigInt::Add(r, multi, r);
            CBigInt::Mul(value2, multi, tmp);
            CBigInt::Sub(a, tmp, a);
        }
        quotient = r;
        remainder = a;
    }

    static void Mod(const CBigInt &value1, const CBigInt &value2, CBigInt &result)
    {
        CBigInt a = value1;
        while (a.CompareNoSign(value2) >= 0)
        {
            unsigned __int64 div = a.m_ulValue[a.m_nLength - 1];
            unsigned __int64 num = value2.m_ulValue[value2.m_nLength - 1];
            unsigned int len = a.m_nLength - value2.m_nLength;
            if ((div == num) && (len == 0))
            {
                CBigInt::Sub(a, value2, a);
                break;
            }
            if ((div <= num) && (len > 0))
            {
                len--;
                div = (div << 32) + a.m_ulValue[a.m_nLength - 2];
            }
            div = div / (num + 1);
            CBigInt multi = div;
            CBigInt::Mul(value2, multi, multi);
            if (len > 0)
            {
                multi.m_nLength += len;
                unsigned int i;
                for (i = multi.m_nLength - 1; i >= len; i--)
                    multi.m_ulValue[i] = multi.m_ulValue[i - len];
                for (i = 0; i < len; i++)
                    multi.m_ulValue[i] = 0;
            }
            CBigInt::Sub(a, multi, a);
        }
        result = a;
    }

    static void Power(const CBigInt &value, const CBigInt &n, CBigInt &result)
    {
        result = 1;
        CBigInt t = value;

        for (__int64 i = 0; i < n.GetTotalBits(); i++)
        {
            if (n.TestBit(i))
            {
                CBigInt::Mul(result, t, result);
            }
            CBigInt::Mul(t, t, t);
        }
    }
    static void ShiftLeft(const CBigInt &value, __int64 bits, CBigInt &result)
    {
        __int64 wpos = bits / 32;
        __int64 woff = bits % 32;

        result = value;
        if (woff != 0)
        {
            unsigned long maskRight = 0xFFFFFFFF >> woff;
            unsigned long maskLeft = ~maskRight;
            result.m_ulValue[result.m_nLength] = 0;
            result.m_nLength++;
            for (int i = result.m_nLength - 1; i >= 0; i--)
            {
                unsigned long orp = result.m_ulValue[i] & maskRight;
                if (i > 0)
                {
                    unsigned long nlp = result.m_ulValue[i - 1] & maskLeft;
                    result.m_ulValue[i] = (orp << woff) | (nlp >> (32 - woff));
                }
                else
                {
                    result.m_ulValue[i] = orp << woff;
                }
            }
            if (result.m_ulValue[result.m_nLength - 1] == 0)
                result.m_nLength--;
        }
        if (wpos != 0)
        {
            for (int j = result.m_nLength - 1; j >= 0; j--)
            {
                result.m_ulValue[j + wpos] = result.m_ulValue[j];
            }
            for (int k = 0; k < wpos; k++)
            {
                result.m_ulValue[k] = 0;
            }
            result.m_nLength += (unsigned int)wpos;
        }
    }

    static void ShiftRight(const CBigInt &value, __int64 bits, CBigInt &result)
    {
        if (bits >= value.GetTotalBits())
        {
            result = 0;
            return;
        }

        __int64 wpos = bits / 32;
        __int64 woff = bits % 32;

        result = value;
        if (woff != 0)
        {
            unsigned long maskLeft = 0xFFFFFFFF << woff;
            unsigned long maskRight = ~maskLeft;
            for (unsigned int i = 0; i < result.m_nLength; i++)
            {
                unsigned long olp = result.m_ulValue[i] & maskLeft;
                if (i < (result.m_nLength - 1))
                {
                    unsigned long nrp = result.m_ulValue[i + 1] & maskRight;
                    result.m_ulValue[i] = (olp >> woff) | (nrp << (32 - woff));
                }
                else
                {
                    result.m_ulValue[i] = olp >> woff;
                }
            }
            if (result.m_ulValue[result.m_nLength - 1] == 0)
                result.m_nLength--;
        }
        if (wpos != 0)
        {
            if (result.m_nLength <= wpos)
            {
                result = 0;
            }
            else
            {
                unsigned int j;
                for (j = 0; j < (result.m_nLength - wpos); j++)
                {
                    result.m_ulValue[j] = result.m_ulValue[j + wpos];
                }
                while (j < result.m_nLength)
                {
                    result.m_ulValue[j] = 0;
                    j++;
                }

                result.m_nLength -= (unsigned int)wpos;
            }
        }
    }
};

const int COUNT = 50;
const char *kA[COUNT] =
    {
        "6A16C3C47A7C4F8439004D78E76A858EF81C5E92C5421B8601E2938484DC43BF",
        "AD8C87041A0A6C52C15C300AEA2E6962131A01542406F99683DC6E786244A4AF",
        "8C445E5CB8228EF80DDEC084E58E4F2A3100C97E3A34603E1A7893C6EEFED167",
        "D3A2D540DDF485D86964F06E6192540C4626A74698185B644EEC6F9C30A60457",
        "390A101AAAC4BAFC34D69C6049AA2B40C95C3706F3582DE466B21EAACD1462FF",
        "322AD8268F607BECE9B6E000E83C7D4C9DF063729D94907CCDDC2F40D13A02F3",
        "12F230783BE851C8C0E0AAF8BA32529C5E207B4A786687B0D6082A52E6CC8C07",
        "2E3AEBE65A724922ADC2A144EE140904DBFADBEAC4ECB84CB3D0C8EE14C69C77",
        "6DCA8D4CD7922C1AFAECF2926DA8C20AC00A7122AA5A0610E78491BA12F0692F",
        "EFEC96964F0EDE3C8DA8D298C0F4E9AE8D7E311EA82CC620985C7232D4C0DF63",
        "304CCA0C856CD2E0BA663556D82031B8977EE3D4A248CB8A8372AECCC5941933",
        "9AA2B1C8AE9A90A0088AAC861E180FAAE4DE3E06C3C6BC5468B8D1E2D25C5663",
        "424E83BCC63E55C4C13656708A1072CE05368DBA79B073F0BAC8FA02678EC0CF",
        "B90ED9AADAF8DD62218A9E566E922926100A00923024604CB3BC5CAC6A66F58B",
        "9ECAFF78438039D6782AFB824C8A9CA2DC68F8E60A30AAB43CCAA4A07162F717",
        "49225A764FCADE70918210DEDF7A136CAE5A46E89642A65AAA2484001E1A0137",
        "36D6602E14F0C9D6D54649C6FEE4F7C0BD9AD8DE93F494B03BA406E4FB22997F",
        "E5A2F538C2AE0BB8CB64D4EE3174B9DA3E6236924054A6F4AF4292089DC0FA9B",
        "6A80E21E0D7CD7B48A7AFED8EF7094B2DF086AEEC1FA26B4904EDFD829B6FACF",
        "91C2C646452AFC42BBE247BE280A19849FBA50142024894ED6FC8916720635DB",
        "E2A4590E630200646238ECA46A1490725D729DDE9230CBA8C2723C44B09CB51F",
        "A4E88032FB904B90F85A6CBACA5CF96017A04DF0627C288E1A6629AAF6EC9E07",
        "AE529EC877A462EA2C6A9CA8FFDEE0246EA231E0CF0CD8EA44ECAE805B18E463",
        "57A0A8846AACD84472BCB5208ADA0D68B562E10C70261042DFC8AC7A42F8998F",
        "AB865E2068669B68D412863684DE95DE32C4E5586F76AE1C25B41518741A4D3F",
        "D37EE84CCEF8BA1ACDB226E4BD8075DA15E87EBA78CE82C8A2C4FED0F0A2657B",
        "2AB8EC08700605504918EE46A626C4FAF2F2F266796C3DAEBCD0DDA20B288F5B",
        "830E1212D4D2E07A7B2CFAC85D323F265A24EADA9CD60B1CDC3A8E5E09EC33BB",
        "7BE83444036837C09760C256281CDEB48384CD4274CC983C4BB298CCB4A8F1AB",
        "BEC8C9DA13EC99A03D207BC2237CCB2280C6D24415784424E9A47C8E546C14D3",
        "68566B6809D20FE4D17E1F444E2C1D14B188EF108BEEBC5ACFD21E94779C530F",
        "E634788C7A669ACC5D5242D6B0A0E9840EFE6936476A4EFAE40271247E8A84B3",
        "82BCB90E62BEB4C651905EFE0596389EECDA462C883C115A63FEFA44D8FA16FB",
        "942E4C0E5778E4327EAC634C8354E492F0904B1244F69F0ED30C17A6CA681267",
        "B5A4A7B480140260451C8E3415D83DBAC9563E282C6AEB1678F2939236E26A07",
        "D150F738A6D21B50CFA61246914A0A7AE22C2400B50C51A8F358D4DCECBEC1D7",
        "A86622C2A964E264C8785D148FCCD2B42F3241D4DE386C8481E679F402F8AEDF",
        "D22CA7107024E2E67E12E078CEDC48C22422DE2CFE060840660456107EA05A4B",
        "141229DEE1A873047BA2AE40F1F66136C76E37BE3AE09B20D31EA950FEB8D8AB",
        "E8B08A6E60904ADA76E08A32641063CE0A9C8294BAF4901A7D10FB04F544937B",
        "FB76EE8ABEAEFF36707E4E126DEE7ABE898641E6BA64AB207FEA00D457AE3963",
        "31889C1EB39850A8833EF9A202307B12427C319665C4AFBE29C8DB4AEA14534F",
        "954C744CA81858D0BA94966CD05E48C88634F6DC50408AEEF69AB83225100E3F",
        "D90026CCBB841EBC637CA6D2468C99B0F33855F8A686F05A1950E6BEEE60D8D3",
        "47F0FD1A7AD2E6A8D6864B6EF1069CDA0B409338FE6AD08EC6A213D2AB6CBA67",
        "5148007225D84B5E66B8A46C6860B93E2634755AC01A5C52BE06BA40208A54F3",
        "FC2030AE43FE3E5627F4FF8CC2F2D88630A28DF2F7A0594CBC9A4A76ED90A8FF",
        "ADF413D0B39ACD6886E40452C56A4472C78E76F28E68D37067589858DADE895F",
        "871E06CA43305E36F3A21FFCBC9CA33C539419C08D72D89480600DA8C26E06E7",
        "15505E8CAA4EA800B254C0EE823609E4780C5A480C0CA0C45D4C3804119C2A23"};

const char *kB[COUNT] =
    {
        "15DCD6D07186D2AAB59453C4F0266606C0508B2415E21E36EA8473767842E78B",
        "C99809E85E406F3E590A6514C9D65536CDC4194E76F82EA456886B38C16AAA17",
        "3046EAB0A9BE5804C12E566AD4D2EF2AF39AC42CB044A984D1F2CEAE0D5CB613",
        "F0368824BE10EAEC8F967BCAACA0F71AD952080278766C4E14E44B44F48A9783",
        "A12430C035709A9EF3DA35142ACA800E929CB74A6EF8C93E148408FC22AC6BF7",
        "A2EC32A8F620381660AA2CDEFCC89D102B3A65D4C45AE532BA307EB661D49267",
        "5BDE7074ECFCA27A0F2A7D7A16E03C542FEA2C9E29446DDEA16637EA9EE043CF",
        "475C34244388D9BCF02E3B32509A05E80C3C5B84347A6356B7647D6247487E87",
        "D5860FD8EDC460461D0471D0262C3256541AAE5E3900862AE3D4283AA12C51DF",
        "5A783A1C7904E006883087A40776E0A85154AB72F2C6CD6C61BA30E02DF6DD2B",
        "8A3282801A6EBA40789688C4623022CA25923B40E720967AB05CE82A68D85B5B",
        "1E3025DC04264D4C2902E3CEDEFEBD26025E5E8AC24CCF36A75017D4F450DA9F",
        "FA0E50AE773C85DAE3F8135AB05A85C8B3E08F3A325EC35C3774B81A4F2A36D3",
        "5718177E562C47A8EE5AAE00850E5C0A22BA26DE2C56FDA206702B5633F68D7F",
        "15785BD448A0429EC4F44464B1F65D4A5276D528A76492249936BF62F5A29EE7",
        "2B50A754FDDC9FC05A2E19FA3584B30C86BC97C88D84F232219EE6B6484E26CB",
        "127A367CE4A0EB600982A926C0EC0456A370EAC800C8137AC53C415E88AA3147",
        "7C0885A0A8881DCAE154ECF803DC052AC55679A601D214CEDA8A481CF7B819AF",
        "82FCD6D06EB62C5C1E2C770207543428957421E6D18228EC434244147370ECFB",
        "D444E9FC04F6A03CF388DD0C34A0C5706B58FEA2D2985D1C51E25916BC582CA7",
        "65A82063B14BC16C2E8D4AEF74A26425D584CE8E7A823E4D33299FC855EEA43",
        "C0AE8F3CB5EA02A6309A57DE9B40F0945FFEF92681587974F44A099A6362F39B",
        "1603BB413E2D494478E2CC8ED1E564A56BCD062D0F237EE87601C0A491CDC2B",
        "8E729B745D20A49E9552DB56BFB0F18CA23A84CEFE286F9CB8D041C4F22E17BB",
        "63A028276F05F10FEA86280EC8C17D64AA2802E653C2F624F8E194402A477B7",
        "47D6855AF478550AE982A382FFD67AF6F8D4C7E21EBC42A63FACB1B6E9BA9047",
        "EDEE258C6F6E2B0264940A7C1624086A6F0EC03E920A9EBC8FC4E2703B4EE94F",
        "6D4A4434681449249340160AEC8E775A75ECF2888788CB021AE003DC68AC1113",
        "69E68F1CFAE6724CD1CC3340A91629A0343CDBD0BACE888E39E6059ECBA0744F",
        "A504DA80A392A9A6A0B05002BD0AEC18F498F1265E8E9A6AC6707AB85B90B12F",
        "2204A446CD1AF9A67068CE4C35968870D9CC0D62468E5200B2F43A127CE6F78B",
        "94405A22336666885DD8EA24779EB3DCEC122F58006EC03004582B622778F983",
        "54BAB01E4CF8131CEB84AC5E8BD2AB9CE9605B0428C49720C71EBB7E93245A87",
        "1212779EDAF23EA0A9EE8420E7A8542A8F0057A686A0D2C83076698C19E6AA37",
        "FF986BB8E85C414A37B8A5EAFB5A3F5C754C9AF8643C17B052EE62F4F4C435E3",
        "5366349663E0568E053ECAE0B9D273E6DC0CFC422B7092EE55625BE889E824EB",
        "9B2A92F2C57808F2874E9CF08B46CE22F90237525800B956DE5C89C282B0E057",
        "8C4881FCB264052CB1CEB1D431D405AC1B448FA46940F838F4460378C8F697BB",
        "ACA2ACAE27E08CCCA4C6D50A0886C9BA2FDA2C3C8766FCD2DB18FF787F385BCF",
        "5914D356C4C4AB848526F81E2016531AA978FB02B7CCE81258880A5E2D32F563",
        "58B48616BE0A140CE1FE86B0F9C012D65A620D664E361600AB0C6792EDB0C09B",
        "AE5E3F4646B0092AF13053D24F36228000DA5CD4FDD2A73AC31664F4DD840B3B",
        "91CE5C365EDA02EE66B03ECCD118EC007B0ED5CA9DDEB40A81F62DE2068645D3",
        "390F64CE6F6C0F882B07C9A50742822678E5450C48E5FC2EA5ACEAE27DE9CEB",
        "BA92DCD46B44DAB0B06680A8034052FC68667E9407AC24462B6E95E002883363",
        "BA10EF207CEA14622C0EFCFC8E2490CA27BCDA82E0801B3ADCD6E86286522593",
        "9960D7A46FAABB9E2DA4C0BAD230746069303794890A596A32A4322C56E235AB",
        "5B0E57A012BAAD562206838E0A3684DC800629E61B084E8C622E571C99E67927",
        "2A2E3B24A16CB4B848A61A222674A204807A15783C40D898BDB2E1EC60140A93",
        "3B362CD828B0A390D74400401A92D4A6CF36E54203346AF6A006B7DCCBF25E4F"};

CBigInt Karatsuba(const CBigInt &mul1, const CBigInt &mul2)
{
    // 1位大整数，直接计算，这也是递归的终止条件
    if ((mul1.GetBigNCount() == 1) || (mul2.GetBigNCount() == 1))
        return mul1 * mul2;

    // 问题分解
    CBigInt high1, high2, low1, low2;
    unsigned int k = max(mul1.GetBigNCount(), mul2.GetBigNCount()) / 2;
    high1 = mul1;
    high1.GetRightBigN(k, low1);
    high1.ShiftRightBigN(k);
    high2 = mul2;
    high2.GetRightBigN(k, low2);
    high2.ShiftRightBigN(k);
    CBigInt z0 = Karatsuba(low1, low2);
    CBigInt z1 = Karatsuba((low1 + high1), (low2 + high2));
    CBigInt z2 = Karatsuba(high1, high2);
    // 结果合并
    CBigInt zk = z1 - z2 - z0;
    z2.ShiftLeftBigN(2 * k);
    zk.ShiftLeftBigN(k);

    return (z2 + zk + z0);
}

void TestMul()
{
    CBigInt A, B, r, q;
    for (int i = 0; i < COUNT; i++)
    {
        A.GetFromString(kA[i], 16);
        B.GetFromString(kB[i], 16);
        r = A * B;
        q = Karatsuba(A, B);
        assert(r == q);
    }
}

int main()
{
    TestMul();
    return 0;
}
