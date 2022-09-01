#include<iostream>
#include<thread>
#include<list>
#include<mutex>
using namespace std;

//虚假唤醒
//wait 被唤醒 但是队列里面没数据
//所以不能只给wait传一个锁 ，第二个lamda十分重要