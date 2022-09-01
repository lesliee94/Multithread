#include<iostream>
#include<thread>
#include<list>
#include<mutex>
using namespace std;

//设计模式概谈  大项目的模块划分经验
//单例设计模式使用比较多
//整个项目中，有某个或者某些类，只能创建一个对象
//好的方法最好是主线程创建好单例对象
//如果不行 就用call_once
std::mutex my_mutex1;
std::mutex my_mutex2;

std::once_flag g_flag;

class A{            //单例类
public:
    static void CreateInstance(){
        if(!only_instance){
            only_instance = new A();
        }
    }
    static A* GetInstance2(){           //替代下面的方案
        std::call_once(g_flag,CreateInstance);
        return only_instance;
    }

    static A* GetInstance(){
        //正确但是会被吊的写法
    //  用ul
    //双锁是有问题的   这段代码是有问题的  再议
        if(!only_instance){
            std::unique_lock<std::mutex> ul(myMutex);
            if(!only_instance){
                only_instance = new A();
            }
        }
        return only_instance;
    }
    void func(){
        std::cout<<"test...\n";
    }

    class release{          //类中套类，析构
        public:
        ~release(){
            if(A::only_instance){
                delete only_instance;
                A::only_instance = nullptr;
            }
        }
    };
private:
    A(){ }          //私有化构造函数
    static A* only_instance;
};

//初始化静态成员变量
//以上 讲了单例类的创建使用
//一定要在主线程，在最开始先创建出来
//后面数据方便
/******************************************************************/

//但是并不是所有场景都是你可以在主线程中创建，有可能你需要在子线程调用getInstance 
std::mutex myMutex; 
A* A::only_instance = nullptr;

void thread_func(){
    //讲解 子线程getinstance的情况

    
    std::cout<<"start thread ,trying to init only_instance .\n";
    A* thread_p = A::GetInstance();
    thread_p->func();
    std::cout<<"end thread.\n";
}


/*******************************************************************/

// std::call_once();   保证某个函数只被调用一次  结合 once_flag
int main(){
    A* p_a = A::GetInstance();
    p_a->func();




    return 0;
}