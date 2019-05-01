﻿#include <QQuickView>
#include <QGuiApplication>
#include "Logger.h"

#include <iostream>
#include <chrono>
#include <vector>
#include <atomic>
#include <sstream>
#include <thread>

using namespace  Logger;
using namespace std;
const int N = 8;

//原子计数器。不要用普通的int，多线程情况下不准确。
static atomic_int gCount(0);
static QString currentThreadId()
{

    stringstream ss;
    ss << this_thread::get_id();
    return QString::fromStdString(ss.str());
}
//输出log的函数，随便打印一点信息
static void showSomeLogger(int index)
{
    //总共8个线程，0号线程睡8毫秒，1号线程睡7毫秒，依次类推
    //8个线程同时睡醒。
    this_thread::sleep_for(chrono::milliseconds(N - index));
    //在文件本身的编码是utf-8的前提下，以下三种方式都可以直接输出中文。
    //u8是c++11标准支持的字符串字面量写法，可以参考https://zh.cppreference.com/w/cpp/language/string_literal
    //QStringLiteral是Qt特有的宏，用来在编译期生成字符串字面量
    //QString::fromLocal8Bit可以在运行过程中，动态处理中文字符串。
    for (int i = 0; i < 1000; ++i)
    {
        gCount++;
        int count = gCount;
        LOG_DEBUG << currentThreadId() << u8"山有木兮木有枝，心悦君兮君不知。";
        LOG_DEBUG << currentThreadId() << QStringLiteral("黄河远上白云间，一片孤城万仞山。");
        LOG_DEBUG << currentThreadId() << QString::fromLocal8Bit("人生若只如初见，何事秋风悲画扇。");
        LOG_INFO << currentThreadId() << u8"玲珑骰子安红豆，入骨相思知不知。";
        LOG_WARN << currentThreadId() << u8"此情可待成追忆，只是当时已惘然。";
        LOG_CRIT << currentThreadId() << u8"严重的事情发生了，股票跌了!" << count;

//        this_thread::sleep_for(chrono::milliseconds(20));

        qApp->processEvents();
    }
}
int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    //app创建之后，调一下initLog就行了
    initLog();

    LOG_INFO << currentThreadId() << u8"故事就是从这里开始的";
    QQuickView view;
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.resize(800, 600);
    view.setSource(QUrl(QStringLiteral("qrc:/Qml/main.qml")));
    view.show();
    vector<std::thread> threads;
    //创建N个线程
    for (int i = 0; i < N; ++i) {
        threads.emplace_back(thread(showSomeLogger, i));
    }
    //启动N个线程
    for (int i = 0; i < N; ++i)
    {
        threads[i].join();
    }


    LOG_INFO << currentThreadId() << u8"故事到这就结束了，总共输出log" << gCount;
    app.exec();
}
