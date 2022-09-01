#include<iostream>
#include<thread>
#include<list>
#include<mutex>
#include<atomic>
using namespace std;

//虚假唤醒
//wait 被唤醒 但是队列里面没数据
//所以不能只给wait传一个锁 ，第二个lamda十分重要
//找一个知名的代码 看看人家是怎么工作的
atomic<int> g_count(0);
class A{
public:
atomic<int> my_atm;
A(){
    my_atm.store(0);                        //my_atm = 0;
}

void in(){
    while(true){

    }
}

void out(){



}
private:    
list<int> msgQueue;   
};