#include<thread>
#include<mutex>
#include<functional>
using namespace std;
//可调用对象的三种
//第一种
//函数
void print(){
    std::cout<<"normal function.\n";
}
//thread t(print);

/**********************************/
class TA{
    void operator()(){
        std::cout<<" class type .\n";
    }
    void thread_work(int num){
        cout<<this<<" id: "<<this_thread::get_id()<<endl;
    }
};
//调用就用 TA ta;
//         thread t(ta);
/*************************************/

//lamda
//

/**************************************/
// function
using f_int = function<int,(int)>;
typedef function<double(double)> fd;


/***************************************/

//bind  mem_fn ? 


auto mylamda = [](){
    std::cout<<" this is lamda method.\n";
}
int main(){
    thread t(mylamda);
    t.detach();
    if(t.joinable()){
        t.join();
    }
    A oj;
    thread mytobj(&A::thread_work,oj,10);//  取函数地址，传入对象，参数 可以detach 因为拷贝构造
    thread mytobj(&A::thread_work,std::ref(oj),10);// 绝不能detach  & oj 也不会拷贝构造
    
    // 用括号 是不用函数名字的地址的，你也没有，但是普通函数 函数名字的地址 对象 或者对象引用  参数列表
    mytobj.join();
    return 0;
}


//detach 慎用 ，比如主线程的东西 是个引用，传进去子线程，主线程先结束了，那么主线程的局部变量已经销毁
// 不可预知
//为什么主线程结束了，子线程这个利用主线程创建的类还能调用构造了 ，比如ta已经销毁，实际上是因为 ta是被复制过去的
//实际上是调用的拷贝构造 所以detach之后 ，控制台看到的析构 析构的是主线程的哪个对象，子线程去后台析构了 
//所以如果这个ta没有指针什么的 就不会产生问题 
//如果用的join 那就是先析构子线程的拷贝构造 再析构主线程的构造
