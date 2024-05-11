
#ifndef SKIPLIST
#define SKIPLIST
#define MAX_LEVEL 10
#include<vector>
#include<memory>
#include<iostream>
#include<fstream>
#include<string>
#include<cmath>
#include<algorithm>
/*template<typename K,typename V>
class Node;*/
#define STORE_FILE "./store/user"


template<typename K, typename V>
class Node;
template<typename K, typename V>
using NodePtr=std::shared_ptr<Node<K,V>>;
template<typename K, typename V>
class Node{
public:
    Node(){}
    //key,value,节点层级
    Node(K k,V v, int level);
    ~Node(){
        forwards.reserve(0);
        //std::cout<<key<<"已销毁"<<std::endl;
    }
    //返回节点key
    K getKey() const;
    //返回节点value
    V getValue() const;
    //设置节点value
    void setValue(V v);
public:
    int node_level;
    std::vector<std::shared_ptr<Node<K,V>>> forwards;
private:
    K key;
    V value;
};

//template<typename K, typename V>
//using NodePtr=std::shared_ptr<Node<K,V>>;

template<typename K, typename V>
class SkipList{
public:
    //跳表最大level
    SkipList(int maxlevel=MAX_LEVEL);//构造函数
    ~SkipList(){}    //析构函数
    NodePtr<K,V> createNode(K key, V value,int level);//创建节点
    int getRandomLevel();    //获得节点层级
    int insertNode(K k,V v);    //插入节点
    bool searchKey(K key);    //搜索节点
    void deleteNode(K key);   //删除节点
    void showList();    //展示节点
    void dumpFile();    //持久化文件，即存入磁盘
    void loadFile();    //从磁盘文件加载数据
    void clear();  //清空跳表
    int size();    //获得当前节点个数
private:  
    //寻找key，current返回目标位置的上一个位置
    void search(NodePtr<K, V>& current, K key);
    //同上，left保存每一层的上一个位置
    void search(NodePtr<K, V>& current, K key, std::vector<NodePtr<K,V>>&left);
    //判断传入字符串是否有效
    bool isValidString(const std::string& str, const char& delimiter = ':');
    //从传入字符串获取key和value
    void getKeyValueFromString(const std::string& str, std::string& key,std::string&value,const char& delimiter=':');
    NodePtr<K,V> head;
    //最大高度
    int max_level;
    //当前高度
    int skiplist_level;
    //跳表元素个数
    int elem_count;
    
    std::ofstream file_writer;
    std::ifstream file_reader;
};

template <typename K, typename V>
Node<K, V>::Node(K k, V v, int level)
    : key(k), value(v), node_level(level)
{
    forwards.resize(level,nullptr);
    /*for (int i = 0; i < level; ++i)
    {
        forwards.push_back(nullptr);
    }*/
}

/*template<typename K,typename V>
Node<K,V>::~Node(){
        for(int i=0;i<node_level;i++){
            delete forwards[i];
            forwards[i]=nullptr;
        }
}*/

template <typename K, typename V>
K Node<K, V>::getKey() const
{
    return key;
}
template <typename K, typename V>
V Node<K, V>::getValue() const
{
    return value;
}

template <typename K, typename V>
void Node<K, V>::setValue(V v)
{
    value = v;
}

template <typename K, typename V>
int SkipList<K, V>::getRandomLevel()
{
    int level = 1;  
    while (rand() % 2 && level <= max_level)
    {
        ++level;
    }
    return max_level > level ? level : max_level;
}

template <typename K, typename V>
SkipList<K, V>::SkipList(int maxlevel) : max_level(maxlevel), skiplist_level(0), elem_count(0)
{
    K k=K();
    V v=V();
    // 头结点层级设置为最大
    head = std::make_shared<Node<K, V>>(k,v,max_level);
}

template <typename K, typename V>
NodePtr<K, V> SkipList<K, V>::createNode(K key, V value,int level)
{
    //NodePtr<K,V> new_node=std::make_shared<Node<K,V>>(key,value,level);
    return std::make_shared<Node<K,V>>(key,value,level);
    //return new_node;
}

/*template <typename K, typename V>
SkipList<K, V>::~SkipList()
{
}*/
template <typename K, typename V>
void SkipList<K, V>::search(NodePtr<K, V>& current, K key){
    //跳表是有序的，逐层查找，如果本层的下一个节点大于当前要查找的，则进入下一层，
    //如果本层下一个节点小于要查找的，则查找本层下一个节点
    //本层被查找完也会进入下一层
    for(int i=skiplist_level;i>=0;i--){
        while(current->forwards[i]&&current->forwards[i]->getKey()<key){
            //移动到当前层下一个节点
            current=current->forwards[i];
        }
        //本层为空或者本层的下一个节点大于找查找的key，下沉
    }
}

//寻找key，保存每一层的前置节点
template <typename K, typename V>
void SkipList<K, V>::search(NodePtr<K, V>& current, K key, std::vector<NodePtr<K, V>>& left) {
    //跳表是有序的，逐层查找，如果本层的下一个节点大于当前要查找的，则进入下一层，
    //如果本层下一个节点小于要查找的，则查找本层下一个节点
    //本层被查找完也会进入下一层
    //left.resize(skiplist_level+1,nullptr);
    for (int i = skiplist_level; i >= 0; i--) {
        while (current->forwards[i]&& current->forwards[i]->getKey() < key) {
            //移动到当前层下一个节点
            current = current->forwards[i];
        }
        left[i]=current;
        //left.push_back(current);
        //本层为空或者本层的下一个节点大于找查找的key，下沉
    }
    //reverse(left.begin(), left.end());
}

//寻找key是否存在
template <typename K, typename V>
bool SkipList<K, V>::searchKey(K key)
{
    //头结点
    NodePtr<K,V> current=head;
    /*//跳表是有序的，逐层查找，如果本层的下一个节点大于当前要查找的，则进入下一层，
    //如果本层下一个节点小于要查找的，则查找本层下一个节点
    //本层被查找完也会进入下一层
    for(int i=level;i>=0;i--){
        while(current->forward[i]&&current->forward[i]->getKey()<key){
            //移动到当前层下一个节点
            current=current->forward[i];
        }
        //本层为空或者本层的下一个节点大于找查找的key，下沉
    }*/
    search(current,key);
    //检查本层下一个节点是否为要查找的key
    if(current->forwards[0]&&current->forwards[0]->getKey()==key){
        return true;
    }
    return false;
}

//插入新节点
template<typename K, typename V>
int SkipList<K, V>::insertNode(K k, V v) {
    NodePtr<K, V> current = head;
    //用来保存新节点的前置节点
    std::vector<NodePtr<K, V>>left(skiplist_level+1,nullptr);

    //寻找前置节点位置
    search(current, k, left);
    current=current->forwards[0];
    //找到key
    
    if (current && current->getKey() == k) {
        current->setValue(v);
        return 0;
    }
    //没找到key
    int new_level = getRandomLevel();
    if (current == nullptr || current->getKey() != k) {
        //新节点的高度比跳表之前的高度更高
        if (new_level > skiplist_level) {
            for (int i = skiplist_level + 1; i <= new_level; i++) {
                //增加头结点高度
                left.resize(new_level+1,head);
                //left.emplace_back(head);
            }
            skiplist_level = new_level;
        }
    }

    //创建新节点
    NodePtr<K, V> new_node = createNode(k, v, new_level);
    for (int i = 0; i < new_level; i++) {
        //新插入节点，节点在第i层位置之前已经保存了
        new_node->forwards[i] = left[i]->forwards[i];
        left[i]->forwards[i] = new_node;
    }
    ++elem_count;
    return 1;
}


template<typename K, typename V>
void SkipList<K, V>::deleteNode(K key){
    NodePtr<K,V> current=head;
    std::vector<NodePtr<K,V>> left(skiplist_level,nullptr);
    search(current,key,left);
    //没找到，直接返回
    if(current->forwards[0]==nullptr){
        return;
    }
    //如果找到了
    if(current->forwards[0]->getKey()==key){
        //保存该节点层级
        int key_level=current->forwards[0]->node_level;
        //保存找到的节点
        current=current->forwards[0];
        for(int i=key_level-1;i>=0;i--){
            //把目标节点从所有层次上摘下来
            left[i]->forwards[i]=current->forwards[i];
            //删除本层的目标节点
            current->forwards[i]=nullptr;
        }
        //彻底删除目标节点
        current=nullptr;
    }
    elem_count--;
}

template<typename K, typename V>
void SkipList<K,V>::showList(){
    for(int i=skiplist_level-1;i>=0;i--){
        //保存头结点
        NodePtr<K,V> current=head;
        std::cout<<"Level "<<i<<":";
        while(current->forwards[i]!=nullptr){
            //依次获得本层节点
            std::cout<<current->forwards[i]->getKey()<<":"<<current->forwards[i]->getValue()<<";";
            //指向本层下一个
            current=current->forwards[i];
        }
        std::cout<<std::endl;
    }
}

template<typename K,typename V>
void SkipList<K,V>::dumpFile(){
    file_writer.open(STORE_FILE);
    NodePtr<K,V> current=head->forward[0];
    while(current!=nullptr){
        file_writer<<current->getKey()<<":"<<current->getValue()<<"\n";
        current=current->forwards[0];
    }
    file_writer.flush();
    file_writer.close();
}

template<typename K,typename V>
bool SkipList<K,V>::isValidString(const std::string& str, const char& delimiter){
    return !str.empty()&&str.find(delimiter)!=std::string::npos;
}

template<typename K,typename V>
void SkipList<K,V>::getKeyValueFromString(const std::string& str, std::string& key,std::string&value,const char& delimiter){
    if(!isValidString(str))return;
    key=str.substr(0,str.find(delimiter));
    value=str.substr(str.find(delimiter)+1);
}
template<typename K,typename V>
void SkipList<K,V>::loadFile(){
    file_reader.open(STORE_FILE);
    std::string line;
    std::string key="";
    std::string value="";
    while(getline(file_reader,line)){
        getKeyValueFromString(line,key,value);
        if(key.empty()||value.empty())continue;
        insertNode(stoi(key),stoi(value));
    }
}

template<typename K,typename V>
int SkipList<K,V>::size(){
    return elem_count;
}

template<typename K,typename V>
void SkipList<K,V>::clear(){
    NodePtr<K,V>current=head;
    if(current->forwards[0]==nullptr)return;
    current=current->forwards[0];
    while(current){
        K key=current->getKey();
        current=current->forwards[0];
        deleteNode(key);
    }
}
#endif