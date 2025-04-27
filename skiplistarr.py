import random, bisect

class Node:
    def __init__(self, arr, level,max_array_size=2):
        # 每个节点包含一个键值对和指向不同层的下一个节点的指针数组
        self.array = arr
        self.max_array_size = max_array_size
        # forward数组存储该节点在每一层的下一个节点
        self.forward = [None] * (level + 1)
    def __str__(self) -> str:
        return str(self.array)
    def Key(self):
        return self.array[0]
    def insert_into_array(self, key):
        # 使用bisect模块保持数组有序
        index = bisect.bisect_left(self.array, key)
        self.array.insert(index, key)
        return True
    def search_in_array(self, key):
        # 在数组中查找键
        index = bisect.bisect_left(self.array, key)
        if index < len(self.array) and self.array[index] == key:
            return key
        return None
    def delete_from_array(self, key):
        # 从数组中删除键
        index = bisect.bisect_left(self.array, key)
        if index < len(self.array) and self.array[index] == key:
            self.array.pop(index)
            return True
        return False
    def is_full(self):
        return len(self.array) >= self.max_array_size
    
            
class SkipList:
    def __init__(self, max_level=16, p=0.5):
        # 最大层数
        self.max_level = max_level
        # 用于随机层数的概率因子
        self.p = p
        # 当前跳表的最大层数
        self.level = 0
        # 头节点，不存储实际数据
        self.header = Node([float('-inf')], max_level)
    
    # 随机生成层数
    def random_level(self):
        level = 0
        while random.random() < self.p and level < self.max_level:
            level += 1
        return level
    
    # 插入键值对
    def insert(self, key):
        # 用于记录每一层需要更新的节点
        update = [None] * (self.max_level + 1)
        current = self.header
        
        # 从最高层开始搜索
        for i in range(self.level, -1, -1):
            while current.forward[i] and current.forward[i].Key() <= key:
                current = current.forward[i]
            update[i] = current
        
        # 到达第0层, 恰好array[0] < key, 而forward > key 或者为None
        # 恰好满, 而且还在范围内
        if current and current != self.header and current.is_full() and current.array[-1] > key:
            replace = current.array.pop()
            current.insert_into_array(key)
            key = replace
        if current and current != self.header and (not current.is_full()):
            current.insert_into_array(key)
        else:
            #并且向前移动, 如果非空且未满,说明也可以插入在最前面
            current = current.forward[0]
            if current and current != self.header and (not current.is_full()):
                current.insert_into_array(key)
                return
            # 下面的需要生成新节点
            # 生成一个随机层数
            new_level = self.random_level()
            
            # 如果新层数大于当前最大层数，需要更新头节点
            if new_level > self.level:
                for i in range(self.level + 1, new_level + 1):
                    update[i] = self.header
                self.level = new_level
            
            # 创建新节点
            new_node = Node([key], new_level)
            
            # 更新每一层的指针
            for i in range(new_level + 1):
                new_node.forward[i] = update[i].forward[i]
                update[i].forward[i] = new_node
    
    # 查找键对应的值
    def search(self, key):
        current = self.header
        
        # 从最高层开始搜索
        for i in range(self.level, -1, -1):
            while current.forward[i] and current.forward[i].Key() <= key:
                current = current.forward[i]
        # search from current or search from forward?
        # 到达第0层，并且向前移动
        # current -> forward 节点比key 大
        return current.search_in_array(key) 
    
    # 删除键值对
    def delete(self, key):
        update = [None] * (self.max_level + 1)
        current = self.header
        
        # 从最高层开始搜索
        for i in range(self.level, -1, -1):
            while current.forward[i] and current.forward[i].array[-1] < key:
                current = current.forward[i]
            update[i] = current
        # 到达第0层，并且向前移动
        current = current.forward[0]
        if current == None:
            return False
        else:
            res = current.delete_from_array(key)
            if not res:
                return False
        # 已经成功删除
        if len(current.array) == 0:
            # 更新每一层的指针
            for i in range(self.level + 1):
                if update[i].forward[i] != current:
                    break
                update[i].forward[i] = current.forward[i]
            
            # 更新最大层数
            while self.level > 0 and self.header.forward[self.level] is None:
                self.level -= 1
        return True

    # 打印跳表内容（用于调试）
    def display(self):
        for level in range(self.level, -1, -1):
            print(f"Level {level}: ", end="")
            node = self.header.forward[level]
            while node:
                print(f"({node.array}) ", end="")
                node = node.forward[level]
            print()
if __name__ == "__main__":
    # 创建跳表
    sl = SkipList()

    # 插入一些键值对
    sl.insert(3)
    sl.insert(6)
    sl.insert(7)
    sl.insert(9)
    sl.insert(12)
    sl.insert(19)
    sl.insert(17)
    sl.insert(26)
    sl.insert(21)
    sl.insert(25)
    sl.insert(5)
    sl.insert(4)
    # 显示跳表内容
    print("初始跳表:")
    sl.display()
    print("\n查找键7:", sl.search(9))
    print("\n查找键17:", sl.search(17))
    print("\n查找键7:", sl.search(7))
    print("\n before delete")
    sl.display()
    print("\n删除键6:", sl.delete(6))
    # sl.delete(7)
    sl.display()
    sl.delete(5)
    sl.delete(12)
    sl.delete(17)
    print("show after delete")
    sl.display()
    print("\n删除键7,5,12,17后:", sl.search(7))

    sl.display()
    sl = SkipList()

    # 插入一些键值对
    sl.insert(3)
    sl.insert(18)
    sl.insert(9)
    sl.insert(4)

    # 显示跳表内容
    print("初始跳表:")
    sl.display()