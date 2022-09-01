#include<thread>
#include<iostream>
#include<memory>
using namespace std;

void print(const int& i , char*mybuf){//这里的引用时假的引用 就是还是拷贝了一个 骗子 所以detach 第一个参数安全
    cout<<i<<endl;                    //但是第二个参数 指针地址一样 绝对会出问题
    cout<<mybuf<<endl;
}
//但是以上的还是有bug 因为有可能往下走了，你调用了thread 但是 主程序走的快
//然后bufInmain 已经回收了

//测试类
class A{
public:
    int m_i;
    A(int a):m_i(a){cout<<"A::A(int a )ctor"<<endl;}
    A(const A &a):m_i(a.m_i){cout<<"A::A(const A&) copyctor"<<endl;}
    ~A(){ cout<<"dtor"<<endl;}
}
void print_2(const int& i,const A& a){      //这里想要传进去这个a 如果使用& 前面必须加const
    cout<<&a<<endl;                         //但是就很蠢 ，所以需要std:: ref,我就是要修改这个值
    return;
}

void print_3(unique_ptr<int>p){
    //do sth 
}
int main(){

    int var =1;
    int &refvar = var;
    char bufInMain = "this is a game.";
    //thread myobj(print,var,bufInMain);//buggy
    // thread myobj(print,var,string(bufInmain));//这样就没问题
    // myobj.join();
    int i = 12;
    //thread myobj(print_2,var,i); 这样只能join

    //如果使用join 没有问题
    //顺序时先构造一个A的对象 ，传递给线程，线程再拷贝构造一个a' 用a'
    //但如果时detach 就会出问题，构造出a后，有可能还没来得及拷贝出一个a'
    //就会结束程序，然后a被析构了 
    //所以如果要使用detach
    //thread myobj(print_2,var,A(i));//这样能够保证拷贝构造执行成功 主线程 构造 拷贝构造 
    A obj(i);
    thread myobj(prnt_2,std::ref(obj));//我就是要改变这个值 不再调用拷贝构造了
    //这样可以detach
    myobj.detach();


    //知识点2  线程ID 也是一样的 就是只要你不是在主线程用了A（i） 就会到子线程才开始构造
    //detach就会出问题
    std::this_thread::get_id();
    //std::this_thread :: yield()   / sleep_until();  sleep_for();
    unique_ptr<int> up1(new int(100));
    std::thread t2(print_3,std::move(up1));//让出控制权给另一个up
    //一定要join
    t2.join();
    return 0;
}