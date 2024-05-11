# skiplist
一个基于跳表的轻量级kv存储引擎

本项目基于C++11实现，使用智能指针、STL。

项目实现了跳表的插入、删除、查找、持久化与读取、数据库大小等。

在terminal输入start_stress_test.sh进行压力测试

# 项目中文件
* main.cpp 压力测试代码

* skiplist.h 跳表实现

* README.md 项目介绍

# 文件接口
* createNode(K key, V value,int level);//创建节点

* getRandomLevel();    //获得节点层级

* insertNode(K k,V v);    //插入节点

* searchKey(K key);    //搜索节点

* deleteNode(K key);   //删除节点

* showList();    //展示节点

* dumpFile();    //持久化文件，即存入磁盘

* loadFile();    //从磁盘文件加载数据

* clear();  //清空跳表

* size();    //获得当前节点个数

# 压力测试结果

|插入数据规模（万条）|耗时|
|---|---|
|10|0.194361|10|
|50|1.29573|10|
|100|3.08242|100|

QPS：32.44w

|查找数据规模（万条）|耗时|
|---|---|
|10|0.111841|10|
|50|0.935836|50|
|100|2.47119|100|

QPS：40.47w

# 待优化
* 七月之前实现分布式存储
* 应用到自己的Webserver服务器中
