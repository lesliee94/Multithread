#include<iostream>
#include<thread>
#include<mutex>
#include<algorithm>
using namespace std;
//线程信息同步的问题
//最简单的崩溃处理 读的时候别写 写的时候别读


class A{
public:
    void operator()(int &a){
        a+=1;
        cout<<"first thread a = "<<a<<endl;
        return;
    }
    void do_work(int &a){
        a+= 5;
        cout<<"second thread a ="<<a<<endl;
        return;
    }
    void do_work2(string &s){
        int index = 0 ; 
        string tmp;
        int len = s.size();
        while(index <len){
            if(s[index] != ' '){
                tmp += s[index];
            }
            index++;
        }
        sort(tmp.begin(),tmp.end());
        len = (len +1)/2;
        s[0] = tmp[0];
        for(int i = 1; i < len ;++i){
            s[2*i] = tmp[i];
        }
        cout<<s<<"in thread."<<endl;
        return ;
    }

};

int main(){
    A a;
    int par = 3;
    thread mytobj(a,ref(par));
    mytobj.join();

    thread mytobj2(&A::do_work,a,ref(par));
    mytobj2.join();
    string s{"1 4 2 5 1 "};
    thread mytobj3(&A::do_work2,a,ref(s));
    mytobj2.join();
    cout<<"in main para = :"<<par<<endl;;
    return 0;
}