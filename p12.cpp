#include<iostream>
#include<thread>
#include<list>
#include<vector>
#include<mutex>
#include<condition_variable>
#include<future>
using namespace std;

int mythread(){

    cout<<"thread_id : "<<this_thread::get_id()<<endl;
    std::chrono::milliseconds dura(5000);               //5000 毫秒
    std::this_thread::sleep_for(dura);
    return 5;
}
int mythread2(std::future<int>& tmp){
    cout<<"thread 2 starts , thread ID = "<<this_thread::get_id()<<endl;
    auto res = tmp.get();                               //只能get一次 因为是移动语义
    return res;
}

int mythread3(shared_future<int>& tmp2){
    cout<<"thread 3 for shared_future has started ,thread_ID = "<<this_thread::get_id()<<endl;
    auto res = tmp2.get();
    tmp2.get();                         //随便get 不影响
    return res;
}

std::atomic<int> g_count(0);
void atomic_test(){
    for(int i = 0 ; i < 10000; ++i){
        g_count++;                     // g_count = g_count +1; 这种操作不支持 没用 有用的是 ++ --  += -= |= 之类的
    }
}
int main(){

    future<int> fufu = async(mythread);                 //带回thread 函数值

    std::future_status state = fufu.wait_for(std::chrono::seconds(1));
    //int res = fufu.get();
    int res;
    if(state == std::future_status::timeout){
        //说明线程还没执行完 等一秒 没返回 那就是超时
        cout<<"thread has not return yet."<<endl;
    }else if(state == std::future_status::ready){
        cout<<"thread has returned "<<endl;
        res = fufu.get();
    }else if(state == std::future_status::deferred){
        cout<<"thread has been deferred ."<<endl;
        cout<<"thread will be init in main thread by using get"<<endl;
        res = fufu.get();
    }

/********************************************************************************/
//知识点2  std::shared_future
//作用 一个future get 了就移动掉了，如果你想获取多个线程的结果，那就得用shared_future  这个不是移动语义 是复制语义

    std::packaged_task<int()> pk(mythread);                 //打包第一个thread      这个只是封装了一下 并没有去跑这个线程
    thread t1(ref(pk));
    std::packaged_task<int()> pk2(mythread);
    future<int> f3 = pk2.get_future();                      //这样才开始跑这个线程
    //if(!f3.valid()){
        shared_future<int> f_s(std::move(f3));                  //这里就给f_s托管了
    //也可以直接创建
    shared_future<int> f_s2 (pk.get_future());
        //shared_future<int> f_s(f3.share());                    这样写也可以
    //}
    //然后创建第二个线程
    f_s.get();                                                  //第一个线程的结果可以用这个shared_future获得很多次
    f_s.get();
    f_s.get();
    thread t_for_share(mythread3,ref(f_s));                      //子线程里面也获得了很多次
    

/************************************************************************************************/
//知识点3 原子操作 ，我寻思这玩意不就是机器指令吗 
//atomic 更加高效一点  一种无锁技术  粒度更小一点 ，多行代码害得用互斥量
//也是类模板 封装某个类型的值
    thread t_a(atomic_test);
    thread t_b(atomic_test);
    t_a.join();
    t_b.join();
    return 0;
}