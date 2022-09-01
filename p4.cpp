#include<iostream>
#include<thread>
#include<mutex>
#include<vector>
using namespace std;

void print(int num){
    cout<<" check num = "<<num<<endl;
}
//注意 std容器与线程的结合
//这节课主要处理 要写的数据，
int main( ){
    vector<thread>threads{};
    for(int i = 0 ; i < 10 ;++i){
        threads.push_back(thread(print,i));
    }
    for(auto iter = threads.begin();iter != threads.end(); ++iter){
        iter->join();
    }
    return 0;

}