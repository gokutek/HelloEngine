`_BitScanForward`由低到高搜索哪一位是`1`，返回索引值。`_BitScanReverse`与之相反。
另外还有64位的版本`_BitScanForward64`。

`CommandContext`有`GraphicsContext`和`ComputeContext`两个子类，基类的析构函数也不
是`virtual`的。但子类中都不含任何成员变量。所以也是安全的。

---

ModelViewer::RenderScene的主要流程
创建一个GraphicsContext，gfxContext

graphic command list
dynamic heap alloc

Resource是如何与hlsl中的寄存器建立关联的？
