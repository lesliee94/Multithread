#include<iostream>
#include<thread>
#include<list>
#include<mutex>
using namespace std;

//书接上回 用unique_lock 代替lock_guard;
//但一般推荐lock_guard

std::mutex my_mutex1;
std::mutex my_mutex2;
//lock_guard 是取代了mutex的 lock 和 unlock
//unique_lock 是一个类模板 比lock_guard更灵活
//最基本的使用  基本和 lock_guard一样

//主要是第二个参数
//1.unique_lock<mutex> u_lock(my_mutex1,adopt_lock);  这个lock_guard也有 就是接管 ，因为已经lock了
//2.unique_lock<mutex> u_lock2(my_mutex1,std::try_to_lock);  结合if u_lock2.owns_lock();
//3.unique_lock<mutex> u_lock3(my_mutex1,std::defer_lock);
class A{
public:
    void inMsgRecvQueue(){
        for(int i = 0 ; i < 500; ++i){
            
            std::unique_lock<std::mutex> u_l1(my_mutex1);
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

class B{
public:
    void inMsgRecvQueue(){
        for(int i = 0 ; i < 500; ++i){
            
            std::unique_lock<std::mutex> u_l1(my_mutex1,std::try_to_lock);
            if(u_l1.owns_lock()){
                
                cout<<"inMsgQueue insert an elelment : "<<i<<endl;
                MsgRecvQueue.push_back(i);
            }else{
                cout<<"wait for some time,the lock is not available now"<<endl;
            }
        }

    }
    void outMsgRecvQueue(){
        for(int i = 0 ; i < 500;++i){
            //要访问这个list
            std::unique_lock<std::mutex> u_l2(my_mutex1,std::try_to_lock);
            if(u_l2.owns_lock()){
                std::chrono::milliseconds dura(2000);                       //拿到锁开始睡觉。。。
                std::this_thread::sleep_for(dura);                          //相当于拿着锁 没解锁 搁那睡了2s  所以最好要释放了锁 才睡觉
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
    };
private:
    list<int> MsgRecvQueue;
};
/****************************************************************************************/
class C{                        //介绍unique_lock 自带的成员函数
public:
    void test(){
        unique_lock<mutex> ul(my_mutex2,std::defer_lock);       //没有锁的mutex2  暂时不锁
        ul.lock();          //不用unlock 但是有时候 你需要unlock 也可以自己unlock

        //突然想做什么事情了
        ul.unlock();

        //做完了
        ul.lock();//之后不用解锁
    }

    void test2(){
        unique_lock<mutex> ul(my_mutex2,std::defer_lock);

        if(ul.try_lock()){
            cout<<"now deal with the data."<<endl;
        }else{
            cout<<"didn't get the lock ."<<endl;
        }
    }

private:
    list<int> MsgRecvQueue;
};

class D{
public:
    //release
    void test(){
        unique_lock<mutex> ul(my_mutex2);        //注意这一行如果没有参数，就是要加锁的意思
        mutex* mpx = ul.release();              //现在你有责任自己解锁了 自己去
        cout<<"do sth with the MSG"<<endl;
        mpx->unlock();                           //必须要自己手动解锁
    }
private:
    list<int> MsgRecvQueue;
};

class E{
    //mutex 所有权可以转移
    void test(){
        unique_lock<mutex> ul(my_mutex2);
        unique_lock<mutex> u2(std::move(ul));  //有点像unique_ptr 可以移动语义 ，应该是删除了拷贝构造
    }
    std::unique_lock<std::mutex> get_runtime_u_l(){
        unique_lock<std::mutex> tmp_ul(my_mutex1);
        return tmp_ul;                          //阐释 unique虽然不能拷贝 但是在局部生成的对象可以 通过函数来返回 返回语义
    }
    void test2(){
        auto ul2 = get_runtime_u_l();
        auto ptr = ul2.release();
        ptr->unlock();
    }
};

/**********************************************************************/
int main(){


    return 0;
}