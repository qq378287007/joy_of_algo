#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <random>
#include <ctime>
#include <chrono>
#include <iomanip>
using namespace std;

using sys_clock = chrono::system_clock;

atomic<bool> quit_mark(false);
default_random_engine re(time(nullptr));
// default_random_engine re(sys_clock::now().time_since_epoch().count());

class Bucket
{
public:
    Bucket() = delete;
    Bucket(int capacity)
    {
        m_capacity = capacity;
        m_qty = 0;
    }
    bool Put(void *data, int size)
    {
        lock_guard<mutex> lock(m_mtp);
        if ((m_qty + size) > m_capacity)
            return false;

        m_qty += size;
        return true;
    }
    int Take(void *data_buf, int size)
    {
        lock_guard<mutex> lock(m_mtp);
        if (m_qty > size)
        {
            m_qty -= size;
            return size;
        }
        else
        {
            int rms = m_qty;
            m_qty = 0;
            return rms;
        }
    }

protected:
    mutex m_mtp;
    int m_qty;
    int m_capacity;
};

void producer(Bucket &bkt)
{
    while (!quit_mark)
    {
        this_thread::sleep_for(chrono::milliseconds(500));
        uniform_int_distribution<unsigned int> ud(300, 650);
        char *data = nullptr; // 数据，为了演示算法简单，Bucket 不处理数据，这里就简单设为 nullptr
        int psize = ud(re);
        if (!bkt.Put(data, psize))
            cout << "thread: " << this_thread::get_id() << " fail to put " << psize << " data to bucket" << endl;
    }
}

void SendInterface(void *pData, int count)
{
}
// 发送限制每秒 1K Bytes
void sender(Bucket &bkt)
{
    char data[200];
    while (!quit_mark)
    {
        this_thread::sleep_for(chrono::milliseconds(200)); //  200ms 发送一次，每次 200
        time_t curTime = sys_clock::to_time_t(sys_clock::now());
        int tsize = bkt.Take(data, 200); // 从桶中漏出数据，存在 data 中
        cout << "thread: " << this_thread::get_id() << " send out " << tsize
             << " data at: " << put_time(localtime(&curTime), "%F, %T") << endl;

        // 向外部接口发送数据
        SendInterface(data, tsize);
    }
}

void demo_stop()
{
    this_thread::sleep_for(chrono::seconds(10));
    quit_mark = true;
}

int main()
{
    Bucket bucket(50000);
    thread st(sender, ref(bucket));
    thread pt1(producer, ref(bucket));
    thread pt2(producer, ref(bucket));
    thread stop_demo(demo_stop);

    st.join();
    pt1.join();
    pt2.join();
    stop_demo.join();

    cout << "Hello World!\n";
    return 0;
}
