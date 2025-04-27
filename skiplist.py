import random

class Node:
    def __init__(self, key, value, level):
        # 每个节点包含一个键值对和指向不同层的下一个节点的指针数组
        self.key = key
        self.value = value
        # forward数组存储该节点在每一层的下一个节点
        self.forward = [None] * (level + 1)

class SkipList:
    def __init__(self, max_level=16, p=0.5):
        # 最大层数
        self.max_level = max_level
        # 用于随机层数的概率因子
        self.p = p
        # 当前跳表的最大层数
        self.level = 0
        # 头节点，不存储实际数据
        self.header = Node(None, None, max_level)
    
    # 随机生成层数
    def random_level(self):
        level = 0
        while random.random() < self.p and level < self.max_level:
            level += 1
        return level
    
    # 插入键值对
    def insert(self, key, value):
        # 用于记录每一层需要更新的节点
        update = [None] * (self.max_level + 1)
        current = self.header
        
        # 从最高层开始搜索
        for i in range(self.level, -1, -1):
            while current.forward[i] and current.forward[i].key < key:
                current = current.forward[i]
            update[i] = current
        
        # 到达第0层，并且向前移动
        current = current.forward[0]
        
        # 如果键已存在，则更新值
        if current and current.key == key:
            current.value = value
        else:
            # 生成一个随机层数
            new_level = self.random_level()
            
            # 如果新层数大于当前最大层数，需要更新头节点
            if new_level > self.level:
                for i in range(self.level + 1, new_level + 1):
                    update[i] = self.header
                self.level = new_level
            
            # 创建新节点
            new_node = Node(key, value, new_level)
            
            # 更新每一层的指针
            for i in range(new_level + 1):
                new_node.forward[i] = update[i].forward[i]
                update[i].forward[i] = new_node
    
    # 查找键对应的值
    def search(self, key):
        current = self.header
        
        # 从最高层开始搜索
        for i in range(self.level, -1, -1):
            while current.forward[i] and current.forward[i].key < key:
                current = current.forward[i]
        
        # 到达第0层，并且向前移动
        current = current.forward[0]
        
        # 如果找到键，则返回对应的值
        if current and current.key == key:
            return current.value
        return None
    
    # 删除键值对
    def delete(self, key):
        update = [None] * (self.max_level + 1)
        current = self.header
        
        # 从最高层开始搜索
        for i in range(self.level, -1, -1):
            while current.forward[i] and current.forward[i].key < key:
                current = current.forward[i]
            update[i] = current
        
        # 到达第0层，并且向前移动
        current = current.forward[0]
        
        # 如果找到键，则删除对应的节点
        if current and current.key == key:
            # 更新每一层的指针
            for i in range(self.level + 1):
                if update[i].forward[i] != current:
                    break
                update[i].forward[i] = current.forward[i]
            
            # 更新最大层数
            while self.level > 0 and self.header.forward[self.level] is None:
                self.level -= 1
            return True
        return False
    
    # 打印跳表内容（用于调试）
    def display(self):
        for level in range(self.level, -1, -1):
            print(f"Level {level}: ", end="")
            node = self.header.forward[level]
            while node:
                print(f"({node.key}:{node.value}) ", end="")
                node = node.forward[level]
            print()
if __name__ == "__main__":
    # 创建跳表
    sl = SkipList()

    # 插入一些键值对
    sl.insert(3, "值3")
    sl.insert(6, "值6")
    sl.insert(7, "值7")
    sl.insert(9, "值9")
    sl.insert(12, "值12")
    sl.insert(19, "值19")
    sl.insert(17, "值17")
    sl.insert(26, "值26")
    sl.insert(21, "值21")
    sl.insert(25, "值25")

    # 显示跳表内容
    print("初始跳表:")
    sl.display()

    # 查找操作
    print("\n查找键17的值:", sl.search(17))
    print("查找键100的值:", sl.search(100))

    # 删除操作
    print("\n删除键19:", sl.delete(19))
    print("删除后的跳表:")
    sl.display()

    # 更新操作
    print("\n更新键17的值:")
    sl.insert(17, "新值17")
    print("查找键17的新值:", sl.search(17))