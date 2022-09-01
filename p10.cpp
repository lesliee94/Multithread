#include<iostream>
#include<thread>
#include<list>
#include<mutex>
#include<condition_variable>
using namespace std;

//讲解条件变量之类 condition_variable 
 
class A{
public:
    void inMsgRecvQueue(){
        // for(int i = 0 ; i < 500; ++i){
            
        //     std::unique_lock<std::mutex> u_l1(my_mutex1);
        //     std::chrono::milliseconds dura(2000);
        //     std::this_thread::sleep_for(dura);                          //相当于拿着锁 没解锁 搁那睡了2s  所以最好要释放了锁 才睡觉
        //     cout<<"inMsgQueue insert an elelment : "<<i<<endl;
        //     MsgRecvQueue.push_back(i);

            
        // }

        /*******************************************************/
        for( int i = 0 ; i < 1000;++i){
            cout<<"produce element : "<<i<<endl;            // I/O了 让出内核
            std::unique_lock<mutex> ul(my_mutex1);
            MsgRecvQueue.push_back(i);
        }
        my_condition.notify_one();                          //要注意 也不见得总能唤醒，要看另外一个线程是不是在wait 搞不好就在转其他业务 你唤醒也没用
        //my_condition.notify_all();                        //所有等待的都唤醒，看谁抢到锁
        return;
    }
    void outMsgRecvQueue(){
        // for(int i = 0 ; i < 500;++i){
        //     //要访问这个list
        //     std::unique_lock<std::mutex> u_l1(my_mutex1);
        //     if(!MsgRecvQueue.empty()){                                      //不好的地方 不停的加锁，效率比较低，改成生产者消费者模型
        //         int command = MsgRecvQueue.front();
        //         cout<<"get command : "<<command<<endl;
        //         MsgRecvQueue.pop_front();
        //         //怎么处理是业务逻辑了
        //     }else{
        //         cout<<"Waiting for resourses..."<<endl;
        //     }
        // }
        int command = 0;
        while(true){
            std::unique_lock<mutex> ul(my_mutex1);      //如果不加第二个变量，就直接阻塞，等待notify
            my_condition.wait(ul,[this](){              //如果空 返回false ,那么条件变量会解锁掉互斥量，并且阻塞在本行，直到某个线程notify
                if(MsgRecvQueue.empty()){               //如果返回true 那么就往下走了，不阻塞 接着执行下面的代码
                    return true;
                }
                return false;
            });
            command = MsgRecvQueue.front();
            MsgRecvQueue.pop_front();
            ul.unlock();                                //u l 的好处 可以提前解锁
        }
    }
private:
    list<int> MsgRecvQueue;
    mutex my_mutex1;
    condition_variable my_condition;            //条件变量对象
};



int main(){

    A myobj;
    thread t_out(&A::outMsgRecvQueue,&myobj);
    thread t_in(&A::inMsgRecvQueue,ref(myobj));


    t_out.join();
    t_in.join();




    return 0;
}