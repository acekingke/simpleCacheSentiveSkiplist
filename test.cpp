#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>
#include <limits>
// 常量定义
const float P = 0.5f;     // 升级概率
const int MAX_LEVEL = 16; // SkipList 最大层数
const int MAX_LEN = 128 ;
// SkipList 节点定义
class SkipListNode {
public:
    int value;
    // forward[i] 表示当前节点在 i 层的下一个节点
    std::vector<SkipListNode*> forward;

    SkipListNode(int val, int level) : value(val), forward(level + 1, nullptr) {}
};

// SkipList 实现
class SkipList {
private:
    int level;                // 当前最高层数
    SkipListNode* header;     // 哨兵节点
    std::mt19937 rng;         // 随机数生成器
    std::uniform_real_distribution<float> dist; // [0, 1) 均匀分布

public:
    SkipList() : level(0), header(new SkipListNode(-1, MAX_LEVEL)), 
                 rng(std::random_device{}()), dist(0.0f, 1.0f) {}

    ~SkipList() {
        // 释放所有节点
        SkipListNode* node = header;
        while (node) {
            SkipListNode* next = node->forward[0];
            delete node;
            node = next;
        }
    }

    // 随机生成层数
    int randomLevel() {
        int lvl = 0;
        while (dist(rng) < P && lvl < MAX_LEVEL)
            ++lvl;
        return lvl;
    }

    // 插入操作
    void insert(int value) {
        std::vector<SkipListNode*> update(MAX_LEVEL + 1, nullptr);
        SkipListNode* x = header;

        // 从最高层开始查找插入位置
        for (int i = level; i >= 0; --i) {
            while (x->forward[i] != nullptr && x->forward[i]->value < value) {
                x = x->forward[i];
            }
            update[i] = x;
        }
        x = x->forward[0];

        // 随机生成新节点层数
        int lvl = randomLevel();
        if (lvl > level) {
            // 对于新增加的层，初始化 update 为 header
            for (int i = level + 1; i <= lvl; ++i)
                update[i] = header;
            level = lvl;
        }
        // 创建新节点，并将各层指针接入链表
        SkipListNode* newNode = new SkipListNode(value, lvl);
        for (int i = 0; i <= lvl; ++i) {
            newNode->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = newNode;
        }
    }
    int search(int key) {
        SkipListNode* current = header;
        // 从最高层开始查找
        for (int i = level - 1; i >= 0; i--) {
            while (current->forward[i] != nullptr && current->forward[i]->value < key)
                current = current->forward[i];
        }
        // 到达底层后，当前节点的下一个节点可能是目标
        current = current->forward[0];
        // 如果找到目标 key，则返回该节点，否则返回 nullptr
        if (current != nullptr && current->value == key)
            return current->value;
        return -1;
    }
};


// 使用二分查找在有序数组中插入（std::vector 实现）
void arrayInsert(std::vector<int>& arr, int value) {
    auto pos = std::lower_bound(arr.begin(), arr.end(), value);
    arr.insert(pos, value);
}

int arraySearch(const std::vector<int>& arr, int value) {
    auto pos = std::lower_bound(arr.begin(), arr.end(), value);
    if (pos != arr.end() && *pos == value) {
        return value;
    }
    return -1;
}

// 使用给定随机数序列测试 SkipList 插入，计时单位为微秒
long long benchmarkSkipList(const std::vector<int>& values) {
    SkipList skiplist;
    auto start = std::chrono::high_resolution_clock::now();
    for (int v : values) {
        skiplist.insert(v);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    return elapsed;
}

class Node {
private:
    std::vector<int> array;
    int max_array_size;

public:
    std::vector<Node*> forward;

    Node(const std::vector<int>& arr, int level, int max_array_size = MAX_LEN) 
        : array(arr), max_array_size(max_array_size) {
        forward.resize(level + 1, nullptr);
    }

    int First() const {
        return array[0];
    }

    int Last() const {
        return array.back();
    }
    void Pop() {
        array.pop_back();
    }
    bool insert_into_array(int key) {
        // Find position to insert while maintaining sorted order
        auto pos = std::lower_bound(array.begin(), array.end(), key);
        array.insert(pos, key);
        return true;
    }

    int search_in_array(int key) const {
        auto pos = std::lower_bound(array.begin(), array.end(), key);
        if (pos != array.end() && *pos == key) {
            return key;
        }
        return -1; // Not found, using -1 as "null" equivalent
    }

    bool delete_from_array(int key) {
        auto pos = std::lower_bound(array.begin(), array.end(), key);
        if (pos != array.end() && *pos == key) {
            array.erase(pos);
            return true;
        }
        return false;
    }

    bool is_full() const {
        return array.size() >= max_array_size;
    }
    bool is_empty() const {
        return array.empty();
    }
    friend std::ostream& operator<<(std::ostream& os, const Node& node) {
        os << "(";
        for (size_t i = 0; i < node.array.size(); ++i) {
            os << node.array[i];
            if (i < node.array.size() - 1) {
                os << ", ";
            }
        }
        os << ")";
        return os;
    }
};

class SkipListArr {
private:
    int max_level;
    double p;
    int level;
    Node* header;
    std::mt19937 gen;
    std::uniform_real_distribution<> dis;

public:
    SkipListArr(int max_level = MAX_LEVEL, double p = 0.5) 
        : max_level(max_level), p(p), level(0), gen(std::random_device()()), dis(0.0, 1.0) {
        std::vector<int> headerKey = { std::numeric_limits<int>::min() }; // Using INT_MIN as negative infinity
        header = new Node(headerKey, max_level);
    }

    ~SkipListArr() {
        Node* current = header;
        Node* next;

        while (current) {
            next = current->forward[0];
            delete current;
            current = next;
        }
    }

    int random_level() {
        int lvl = 0;
        while (dis(gen) < p && lvl < max_level) {
            lvl++;
        }
        return lvl;
    }

    void insert(int key) {
        std::vector<Node*> update(max_level + 1, nullptr);
        Node* current = header;

        for (int i = level; i >= 0; i--) {
            while (current->forward[i] && current->forward[i]->First() <= key) {
                current = current->forward[i];
            }
            update[i] = current;
        }

        if (current && current != header && current->is_full() && current->Last() > key) {
           
            int replace = current->Last();
            current->Pop();
            current->insert_into_array(key);
            key = replace;
        }

        if (current && current != header && !current->is_full()) {
            current->insert_into_array(key);
        } else {
            current = current->forward[0];
            if (current && current != header && !current->is_full()) {
                current->insert_into_array(key);
                return;
            }

            int new_level = random_level();

            if (new_level > level) {
                for (int i = level + 1; i <= new_level; i++) {
                    update[i] = header;
                }
                level = new_level;
                
            }

            Node* new_node = new Node({key}, new_level);

            for (int i = 0; i <= new_level; i++) {
                new_node->forward[i] = update[i]->forward[i];
                update[i]->forward[i] = new_node;
            }
        }
    }

    int search(int key) const {
        Node* current = header;

        for (int i = level; i >= 0; i--) {
            while (current->forward[i] && current->forward[i]->First() <= key) {
                current = current->forward[i];
            }
        }

        return current->search_in_array(key);
    }

    bool delete_key(int key) {
        std::vector<Node*> update(max_level + 1, nullptr);
        Node* current = header;

        for (int i = level; i >= 0; i--) {
            while (current->forward[i] && current->forward[i]->First() < key) {
                current = current->forward[i];
            }
            update[i] = current;
        }

        current = current->forward[0];
        if (!current) {
            return false;
        }

        bool res = current->delete_from_array(key);
        if (!res) {
            return false;
        }

        if (current->is_empty()) {
            for (int i = 0; i <= level; i++) {
                if (update[i]->forward[i] != current) {
                    break;
                }
                update[i]->forward[i] = current->forward[i];
            }

            delete current;

            while (level > 0 && header->forward[level] == nullptr) {
                level--;
            }
        }
        return true;
    }

    void display() const {
        for (int i = level; i >= 0; i--) {
            std::cout << "Level " << i << ": ";
            Node* node = header->forward[i];
            while (node) {
                std::cout << *node << " ";
                node = node->forward[i];
            }
            std::cout << std::endl;
        }
    }
};


long long benchmarkSkipListArr(const std::vector<int>& values) {
    SkipListArr skiplist;
    auto start = std::chrono::high_resolution_clock::now();
    for (int v : values) {
        skiplist.insert(v);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    return elapsed;
}


// 使用给定随机数序列测试数组插入，计时单位为微秒
long long benchmarkArray(const std::vector<int>& values) {
    std::vector<int> arr;
    arr.reserve(values.size());
    auto start = std::chrono::high_resolution_clock::now();
    for (int v : values) {
        arrayInsert(arr, v);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    return elapsed;
}

// benchmark search
long long benchmarkSearchSkipList(const std::vector<int>& values) {
    SkipList skiplist;
    for (int v : values) {
        skiplist.insert(v);
    }
    std::vector<int> newVec(values);
    std::random_device rd;  // 随机设备（用于获取真正的随机数种子）
    std::mt19937 g(rd());   // 以随机数种子初始化 Mersenne Twister 引擎
    std::shuffle(newVec.begin(), newVec.end(), g);
    auto start = std::chrono::high_resolution_clock::now();
    for (int v : newVec) {
        skiplist.search(v);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    return elapsed;
}

long long benchmarkSearchArray(const std::vector<int>& values) {
    std::vector<int> arr;
    arr.reserve(values.size());
    for (int v : values) {
        arrayInsert(arr, v);
    }
    std::vector<int> newVec(values);
    std::random_device rd;  // 随机设备（用于获取真正的随机数种子）
    std::mt19937 g(rd());   // 以随机数种子初始化 Mersenne Twister 引擎
    std::shuffle(newVec.begin(), newVec.end(), g);
    auto start = std::chrono::high_resolution_clock::now();
    for (int v : newVec) {
        arraySearch(arr, v);
    }
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

long long benchmarkSearchSkipListArr(const std::vector<int>& values) {
    std::vector<int> newVec(values);
    SkipListArr skiplist;
 
    for (int v : values) {
        skiplist.insert(v);
    }
    std::vector<int> arr;
    std::random_device rd;  // 随机设备（用于获取真正的随机数种子）
    std::mt19937 g(rd());   // 以随机数种子初始化 Mersenne Twister 引擎
    std::shuffle(newVec.begin(), newVec.end(), g);
    auto start = std::chrono::high_resolution_clock::now();
    for (int v : newVec) {
        skiplist.search(v);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    return elapsed;
}


int main() {
    // 测试不同规模：例如 100, 1000, 5000, 10000, 20000 个插入
    std::vector<int> sizes = {100, 1000, 5000, 10000, 20000, 20000,50000, 200000, 250000, 300000};
    std::cout<<"insert"<<std::endl;
    std::cout << "规模\tSkipList(us)\tSkipList2(us)\t数组插入耗时(us)" << std::endl;

    for (int n : sizes) {
        // 生成随机数序列（保证相同种子，保证两组测试数据一致）
        std::vector<int> values(n);
        std::mt19937 rng(42); // 固定种子
        std::uniform_int_distribution<int> dist(0, n * 10);
        for (int i = 0; i < n; ++i) {
            values[i] = dist(rng);
        }
        long long t_skip2 = benchmarkSkipListArr(values);
        long long t_skip = benchmarkSkipList(values);
        long long t_array = benchmarkArray(values);
        
        std::cout << n << "\t" << t_skip << "\t\t"<< t_skip2 <<"\t\t"  <<t_array<< std::endl;
    }
    std::cout<<"search"<<std::endl;
     std::cout << "规模\tSkipList(us)\tSkipList2(us)\t数组查询耗时(us)" << std::endl;
    for (int n : sizes) {
        std::vector<int> values(n);
        std::mt19937 rng(42);
        std::uniform_int_distribution<int> dist(0, n * 10);
        for (int i = 0; i < n; ++i) {
            values[i] = dist(rng);
        }
        long long t_skip2 = benchmarkSearchSkipListArr(values);
        long long t_skip = benchmarkSearchSkipList(values);
        long long t_array = benchmarkSearchArray(values);
        std::cout << n << "\t" << t_skip << "\t\t"<< t_skip2 <<"\t\t"  <<t_array<< std::endl;
    }

    return 0;
}
int main2(){
    // Create skip list
    SkipList sl;

    // Insert some key-value pairs
    sl.insert(3);
    sl.insert(6);
    sl.insert(7);
    sl.insert(9);
    sl.insert(12);
    sl.insert(19);
    sl.insert(17);
    sl.insert(26);
    sl.insert(21);
    sl.insert(25);
    sl.insert(5);
    sl.insert(4);
    
    // Display skip list content
    std::cout << "Initial skip list:" << std::endl;
    std::cout << "\nSearch for key 17: " << sl.search(17) << std::endl;
    std::cout << "Search for key 7: " << sl.search(7) << std::endl;
    
    return 0;
}