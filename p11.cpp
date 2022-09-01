#include<iostream>
#include<thread>
#include<list>
#include<vector>
#include<mutex>
#include<condition_variable>
#include<future>
using namespace std;

//讲解 async  future  async是一个函数模板  ，启动一个异步任务之后，返回一个std:: future 对象
//启动一个异步任务，就是hi自动创建一个线程并开始执行相应的线程入口函数，返回一个future对象，返回的这个future对象里面就有线程入口函数的结果
//使用future对象的get 结果
//就是其实就是我们之前用的是void 然后引用传参 ，然后传出，现在可以把这个结果直接传出来

int mythread(){

    cout<<"thread_id : "<<this_thread::get_id()<<endl;
    std::chrono::milliseconds dura(5000);               //5000 毫秒
    std::this_thread::sleep_for(dura);
    return 5;
}

class A{

public:
    int func(int a ){
        cout<<"thread_id : "<<this_thread::get_id()<<endl;
        std::chrono::milliseconds dura(5000);               //5000 毫秒
        std::this_thread::sleep_for(dura);
        return ++a;
    }
};

//知识点3 packaged_task  也是个类模板 模板参数是各种可调用对象，包装各种可调用对象，方便作为线程入口函数调用


//4 promise

void promise_test(std::promise<int>&tmp,int calc){
    //各种才足以
    calc *= 10;
    calc -= 1;
    std::chrono::milliseconds dura(3000);
    std::this_thread::sleep_for(dura);
    int res = calc;
    tmp.set_value(res);
    return;
}


int main(){



/*************************************************************************************/

    cout<<"main t_id"<<this_thread::get_id()<<endl;
    std::future<int> my_f = std::async(mythread);
    A obj;
    std::future<int> my_f2 = std::async(&A::func,&obj,4);   //第二个参数用的对象引用，不用会额外创建一个对象，消耗比较大
    int def = 0 ; 
    //async 后面参数可以调用std::launch 的枚举类型
    std::future<int> my_f3 = std::async(std::launch::deferred,&A::func,&obj,4);     //必须要等到调用他的wait 或者get 他才会延迟创建线程
    std::future<int> my_f4 = std::async(std::launch::async,&A::func,&obj,4);        //立即执行 ，感觉就是不输入，然后相当于默认值

    //my_f.wait();                                          //阻塞等不返回值
    cout<<my_f.get()<<endl;                                 //阻塞等待  不返回就一直等待 最好get一下 不然不太安全
    cout<<my_f2.get()<<endl;
    my_f3.wait();                                           //并没用创建新线程，而是在主线程中调用了


/*****************************************************************************************/
    std::packaged_task<int(int)> my_task(&A::func);
    int a = 6;
    std::packaged_task<int(int)> my_task2([](int a){
        cout<<"thread_id : "<<this_thread::get_id()<<endl;
        return ++a;
    });
    std::future<int> res = my_task.get_future();            //这样就不用调用async 了

    future<int> res2 = my_task2.get_future();               //另外就是可以get_future 然后就能出来值

    

    cout<<res.get()<<endl;;                             
    cout<<res2.get()<<endl;

    std::thread thread_1(my_task,1);                        //本身可以直接调用         
    thread_1.join();

    /********************************************************************************************/

    vector<packaged_task<int(int)> > vec;

    vec.push_back(std::move(my_task));
    vec.push_back(std::move(my_task2));                    //这个task 就空了，移动到了vec里面

    //vec使用就正常 ，你也可以移动回来
    auto iter = vec.begin();
    my_task = std::move(*iter);                             //移动回来
    vec.erase(iter);
    //end packaged_task
    /**********************************************************************************************/    
    //promise 
    std::promise<int> my_prom;
    std::thread t4(promise_test,std::ref(my_prom),180);
    t4.join();
    //然后获得返回值，感觉这个没有上面好用
    //绕来绕去 就是想获得返回值，现在又把这个promise的值 搞出来而已，目的就是这个
    std::future<int> fu = my_prom.get_future();

    int ress = fu.get();

    return 0;
}