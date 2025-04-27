#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <climits>

class Node {
private:
    std::vector<int> array;
    int max_array_size;

public:
    std::vector<Node*> forward;

    Node(const std::vector<int>& arr, int level, int max_array_size = 2) 
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
    SkipListArr(int max_level = 16, double p = 0.5) 
        : max_level(max_level), p(p), level(0), gen(std::random_device()()), dis(0.0, 1.0) {
        std::vector<int> headerKey = {INT_MIN}; // Using INT_MIN as negative infinity
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
            while (current->forward[i] && current->forward[i]->Last() < key) {
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

int main() {
    // Create skip list
    SkipListArr sl;

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
    sl.display();
    
    std::cout << "\nSearch for key 17: " << sl.search(17) << std::endl;
    std::cout << "Search for key 7: " << sl.search(7) << std::endl;
    
    sl.delete_key(7);
    std::cout << "Show after delete" << std::endl;
    sl.delete_key(5);
    sl.delete_key(12);
    sl.delete_key(17);
    
    std::cout << "Show after delete" << std::endl;
    sl.display();
    
    std::cout << "\nAfter deleting keys 7,5,12,17: " << sl.search(7) << std::endl;
    
    sl.display();
    
    SkipListArr sl2;
    
    // Insert some key-value pairs
    sl2.insert(3);
    sl2.insert(18);
    sl2.insert(9);
    sl2.insert(4);
    
    // Display skip list content
    std::cout << "Initial skip list:" << std::endl;
    sl2.display();
    
    return 0;
}