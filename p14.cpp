#include<iostream>
#include<thread>
#include<list>
#include<mutex>
using namespace std;
//讲了了一下 windows的临界区 与mutex的区别，mutex 不能多次加锁  Windows的可以多次加锁解锁

/****************************************************************************************/
//2.自动析构技术  meitaida意义 Windows的 没用
//std::lock_guard 
//c++11怎么处理这个问题
//利用recursive_mutex  相比于std::mutex 独占锁
//recursive_mutex 允许同一个线程 同一个锁 多次lock unlock
//如果是recursive_mutex 是否有优化代码的空间 ，用这个mutex 效率很低，

std::mutex my_mutex1;
std::mutex my_mutex2;
/**************************************************************************************/


std::recursive_mutex re_mutex;


/**************************************************************************************/

std::timed_mutex t_mutex;  


class A{
public:
    void inMsgRecvQueue(){
        for(int i = 0 ; i < 500; ++i){
            
            //std::unique_lock<std::mutex> u_l1(my_mutex1);
            std::unique_lock<std::recursive_mutex> locker(re_mutex);     //递归次数有限制 不能太多次貌似
            std::unique_lock<std::recursive_mutex> locker2(re_mutex);
            std::unique_lock<std::recursive_mutex> locker3(re_mutex);
            if(t_mutex.try_lock_for(100ms)){                            //等待100ms
                cout<<"hehe"<<endl;
                t_mutex.unlock();
            }else{
                this_thread::sleep_for(1000ms);                         //dengdao 某个时间点
            }
            if(t_mutex.try_lock_until(std::chrono::steady_clock::now() + 1000ms)){
                t_mutex.unlock();
            }else{
                this_thread::sleep_for(1000ms);
            }

            std::chrono::milliseconds dura(2000);
            std::this_thread::sleep_for(dura);                          //相当于拿着锁 没解锁 搁那睡了2s  所以最好要释放了锁 才睡觉
            cout<<"inMsgQueue insert an elelment : "<<i<<endl;
            MsgRecvQueue.push_back(i);

            
        }

    }
    void outMsgRecvQueue(){
        for(int i = 0 ; i < 500;++i){
            //要访问这个list
            std::unique_lock<std::mutex> u_l1(my_mutex1);
            if(!MsgRecvQueue.empty()){
                int command = MsgRecvQueue.front();
                cout<<"get command : "<<command<<endl;
                MsgRecvQueue.pop_front();
                //怎么处理是业务逻辑了
            }else{
                cout<<"Waiting for resourses..."<<endl;
            }
        }
    }
private:
    list<int> MsgRecvQueue;
};

int main(){

    A my_obj;
    std::thread myOutObj(&A::outMsgRecvQueue,&my_obj);
    std::thread myInObj(&A::inMsgRecvQueue,&my_obj);

    myOutObj.join();
    myInObj.join();






    return 0;
}