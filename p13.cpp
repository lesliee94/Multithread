#include<iostream>
#include<thread>
#include<list>
#include<vector>
#include<mutex>
#include<condition_variable>
#include<future>
using namespace std;

//thread 和async的区别，想要返回值，async ，async不一定创建线程 deferred ，就在主程序执行
//系统资源限制，thread 创建异常，程序会崩溃
//对于async 系统资源紧张它不报错或者异常，而是后续谁调用了result.get() 系统自动选择deferred 参数
//就用主线程去运行
//如果一定要创建新线程 async 选择 std::launch::async
//经验 线程数不要超过两百 可以测试

//如何解决async 不可预料的问题的解决
// 不加参数的情况 可能不创建的问题
//问题焦点在于，异步任务有没有被推迟执行   就是看std::future 的wait_for 函数，去查看他的std::future_status::ready  还是 timeout  还是 deferred

//std::chrono::seconds dura(5)  ==  5s

int func(){
    this_thread::sleep_for(3s);
    return 666;

}

int main(){
    thread th(func);
    std::future<int> fufu = async(std::launch::deferred,func);

    std::future_status state;

    if(state == std::future_status::ready){
        cout<<"thread has returned.";
        cout<<fufu.get()<<endl;             //直接获取
    }else if(state == std::future_status::deferred){
        cout<<"thread has been deferred."<<endl;
        cout<<fufu.get()<<endl;            //让他开始

    }else if(state == std::future_status::timeout){
        cout<<"thread is executing ,has not fisished yet."<<endl;
        cout<<fufu.get()<<endl;             //干等
    }



    return 0;
}