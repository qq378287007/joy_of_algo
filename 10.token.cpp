#include <iostream>
#include <mutex>
#include <chrono>
#include <algorithm>
#include <thread>
using namespace std::chrono;

class CRateLimiter
{
    mutable std::mutex m_mutex;
    double m_stableIntervalMicros;
    double m_maxPermits;
    double m_storedPermits;
    unsigned long long m_nextFreeTicketMicros;

public:
    // CRateLimiter() = delete;
    CRateLimiter(double permitsPerSecond, double capacity = 0.0)
    {
        if (permitsPerSecond <= 0.0)
            throw std::runtime_error("CRateLimiter: permitsPerSecond param is invalid for CRateLimiter()");

        m_maxPermits = capacity;
        m_stableIntervalMicros = 1000000.0 / permitsPerSecond;
        m_storedPermits = capacity; // 0.0
        m_nextFreeTicketMicros = 0;
    }

    double Aquire(int permits = 1)
    {
        if (permits < 0)
            throw std::runtime_error("CRateLimiter: permits param is invalid for Aquire()");

        auto wait_time = reserve(permits);
        std::this_thread::sleep_for(microseconds(wait_time));

        return wait_time / 1000000.0;
    }

    bool TryAquire(int permits, int timeouts = 0)
    {
        auto now = duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();

        if (m_nextFreeTicketMicros > now + timeouts * 1000.0)
            return false;
        else
            Aquire(permits);

        return true;
    }

    double GetPermitsRate() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return 1000000.0 / m_stableIntervalMicros;
    }

    void SetPermitsRate(double permitsPerSecond)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (permitsPerSecond <= 0.0)
            throw std::runtime_error("CRateLimiter: permitsPerSecond param is invalid for SetRate()");

        m_stableIntervalMicros = 1000000.0 / permitsPerSecond;
    }

protected:
    virtual void resync(unsigned long long nowMicros)
    {
        if (nowMicros > m_nextFreeTicketMicros)
        {
            double new_permits = (nowMicros - m_nextFreeTicketMicros) / m_stableIntervalMicros; // 本次新发放令牌数
            m_storedPermits = std::min(m_maxPermits, m_storedPermits + new_permits);
            m_nextFreeTicketMicros = nowMicros;
        }
    }

    virtual unsigned long long reserve(double permits)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        // 转为微秒为单位
        unsigned long long nowMicros = duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
        resync(nowMicros);

        unsigned long long waitMicros = m_nextFreeTicketMicros - nowMicros; // 如果令牌发放后积累令牌数够用，wait 就是 0，否则本次就要 wait

        double storedPermitsToSpend = std::min(permits, m_storedPermits); // 本次消费的令牌数
        double freshPermits = permits - storedPermitsToSpend;             // 是否需要透支消费

        unsigned long long freshPermitsMicros = (unsigned long long)(freshPermits * m_stableIntervalMicros); // 根据透支消费的令牌数反算需要的时间

        m_nextFreeTicketMicros += freshPermitsMicros; // 如果这次有超前消费，则需要将下次发放令牌时间向后延到将来的某个时间
        m_storedPermits -= storedPermitsToSpend;      // 消费指定数量的令牌

        return std::max(waitMicros, 0ULL);
    }
};

int main()
{
    /*
        CRateLimiter limiter(1000, 1000);

        //std::this_thread::sleep_for(std::chrono::seconds(1));

        double wait = limiter.Aquire(700);
        std::cout << "0: Aquire 700: need_sleep: " << wait << std::endl;
        wait = limiter.Aquire(500);
        std::cout << "1: Aquire 500: need_sleep: " << wait << std::endl;
        //std::this_thread::sleep_for(std::chrono::seconds(1));
        wait = limiter.Aquire(700);
        std::cout << "2: Aquire 700: need_sleep: " << wait << std::endl;
        wait = limiter.Aquire(1000);
        std::cout << "3: Aquire 1000: need_sleep: " << wait << std::endl;
    */

    CRateLimiter limiter(1);

    double wait = limiter.Aquire(1);
    std::cout << "0: Aquire 1: need_sleep: " << wait << std::endl;
    wait = limiter.Aquire(1);
    std::cout << "1: Aquire 1: need_sleep: " << wait << std::endl;
    wait = limiter.Aquire(1);
    std::cout << "2: Aquire 1: need_sleep: " << wait << std::endl;
    wait = limiter.Aquire(1);
    std::cout << "3: Aquire 1: need_sleep: " << wait << std::endl;

    return 0;
}
