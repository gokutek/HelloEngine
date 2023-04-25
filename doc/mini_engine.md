## 移植问题
`CommandContext`有`GraphicsContext`和`ComputeContext`两个子类，子类中都不含任何成员
变量。基类的析构函数也不是`virtual`的。
