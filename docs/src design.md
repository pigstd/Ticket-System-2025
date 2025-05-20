大概设计：

src 中各个文件的作用：

vector.hpp 实现 sjtu::vector

hashmap 简单的实现一个 hashmap，用于缓存优化

cache.hpp 实现缓存优化

MemoryRiver 没加缓存优化的文件读写

MR_with_cache.hpp 加了缓存优化的文件读写

database.hpp 基于 B+ 树的数据库

initial.hpp 全局变量（数据库用全局变量存）的定义，初始化

utility.hpp 提供一些实用的内容，包括

- sjtu::pair
- 字符串的分割，处理

user.hpp 用户类的设计以及和用户有关的操作

train.hpp 火车类的设计以及和火车有关的操作

ticket.hpp 与购票有关的操作

operator.hpp 对输入进行不同操作的分类