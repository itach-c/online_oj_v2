  #pragma once
  #include <string>
  #include <map>

  //change 
  struct question{

        std::string qid; //题目编号
        std::string title;//题目的标题
        std::string level;//难度
        int cpu_limit; //单位秒
        int mem_limit; //单位kb
        std::string desc;//题目描述
        // std::string header;//预留代码
        std::map<std::string,std::string> header;//预留代码 change 
        std::map<std::string,std::string> tail;//测试提交编译器的

        // std::string tail; //测试提交编译器的
    };