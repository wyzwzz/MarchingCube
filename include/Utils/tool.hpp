//
// Created by wyz on 2021/5/28.
//

#ifndef MARCHINGCUBES_TOOL_HPP
#define MARCHINGCUBES_TOOL_HPP

#include<chrono>

#define START_CPU_TIMER \
{auto _start=std::chrono::steady_clock::now();

#define END_CPU_TIMER \
auto _end=std::chrono::steady_clock::now();\
auto _t=std::chrono::duration_cast<std::chrono::milliseconds>(_end-_start);\
std::cout<<"CPU cost time : "<<_t.count()<<"ms"<<std::endl;}



#endif //MARCHINGCUBES_TOOL_HPP
