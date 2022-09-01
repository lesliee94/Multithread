#include<iostream>
#include<thread>
#include<list>
#include<mutex>
using namespace std;

//保护共享数据
//使用mutex 是一个类对象
//lock函数返回了，就说明锁住了，不然会阻塞等着锁 lock
//死锁了 张三在北京喊  李四不来北京 我等他一辈子  李四在深圳喊，如果张三不来深圳，我在深圳等他一辈子
//死锁必须要有俩锁  
//线程A 把金锁锁了 要去锁银锁
//上下文切换  
//线程b 把银锁锁了 要去锁金锁 ，这时候 两个线程都阻塞了，尽管后面又解锁的代码，但都阻塞在那了 互等
mutex mymutex;
mutex goldMutex;
mutex silverMutex;


class A{
public:
    void inMsgRecvQueue(){
        for(int i = 0 ; i < 500; ++i){
            mymutex.lock();

            cout<<"inMsgQueue insert an elelment : "<<i<<endl;
            MsgRecvQueue.push_back(i);

            mymutex.unlock();
        }

    }

    void outMsgRecvQueue(){
        for(int i = 0 ; i < 500;++i){
            //要访问这个list
            mymutex.lock();         //注意有if else 分支 每个分支都得unlock
                                    //为了避免忘记unlock std::lock_guard;   用了这个就不能使用lock unlock;
           // std::lock_guard<std::mutex> my_guard(mymutex);  还是一个类模板 用了这个 lock unlock  都别用
           //出作用域就会解锁 ，感觉这个东西还是有点影响
           //所以想让lock_guard 提前结束 可以直接套一个作用域 不咋常用
            if(!MsgRecvQueue.empty()){
                int command = MsgRecvQueue.front();
                cout<<"get command : "<<command<<endl;
                MsgRecvQueue.pop_front();
                //怎么处理是业务逻辑了

                mymutex.unlock();
            }else{
                cout<<"Waiting for resourses..."<<endl;

                mymutex.unlock();
            }
        }
    }
private:
    list<int> MsgRecvQueue;
};

class deadLockTest{
    void t1(){
        goldMutex.lock();
        cout<<"thread 1 has locked goldlock ,trying to lock sliver..."<<endl;
        silverMutex.lock();
        silverMutex.unlock();
        goldMutex.unlock();
    }

    void t2(){
        silverMutex.lock();
        cout<<"thread 2 has locked silverlock ,trying to lock gold..."<<endl;
        goldMutex.lock();
        
        
        
        goldMutex.unlock();
        silverMutex.unlock();
    }

};

//1.死锁的解决方案 保证两个互斥量上锁的顺序一致  就不会死锁
//2.用std::lock() 要么锁俩  要么一个也没锁住   std::lock(mutexGold,mutexSilver);   但是得分开 各自解锁
//3.先用

class Solution{
public:
    void func(){
        std::lock_guard<std::mutex> g1(silverMutex);
        std::lock_guard<std::mutex> g2(goldMutex);
        //在另外线程 也这个顺序写
        //如果不用guard 也可以用 my_mutex.lock() 只要保证顺序一致就好了
        
    }

};
class Solution2{
public:
    void func(){
        std::lock(goldMutex,silverMutex);
        //do sth

        goldMutex.unlock();
        silverMutex.unlock();
    }
};
class Solution3{
public:
    void func(){
        std::lock(goldMutex,silverMutex);           //一次锁住多个  不太建议使用，还是建议一个个锁
        std::lock_guard<std::mutex> g1(silverMutex,std::adopt_lock);
        std::lock_guard<std::mutex> g1(goldMutex,std::adopt_lock);
    }

};

    


int main(){

    A a;
    thread outbj(&A::outMsgRecvQueue,&a);
    thread inobj(&A::inMsgRecvQueue,ref(a));


    outbj.join();
    inobj.join();

    return 0;
}