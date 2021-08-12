# setcap
Linux: 如何使用capabilities机制实现进程权限管理?

**问题：**

**1. 编写的daemon不需要root用户的所有权限，独立账号运行安全性更高**

**2. 更改为普通用户拉起daemon，需要读取部分root权限才可以正常运行业务**

**解决方法：**

**使用linux capabilitie 管理进程的权限，保障最小权限正常运行业务**

# capabilities简介
传统的UNIX通过区别：特权进程(user ID 0 root进程)和非特权进程，来达到权限检查的目的。特权进程可以通过内核所有的权限检查，但是非特权进程基于（有效的UID，有效GID和补充组列表）进行完全完全权限检查。

从内核2.2版本开始，Linux划分特权进程的权限进行更详细的划分，也就是capabilities，每一个单元的权限都可以独立启用和禁用。

# 实现方案

1. 进程启动过程中设置uid
2. 进程启动过程中设置capability(eip)
3. 封装接口为so，方便后续进程降权直接调用
4. capability使用配置文件进行设置，方便动态调整

# 最终实现效果
```shell
[root@ink src]# ps aux | grep demo
test     14369  0.0  0.0  12668   144 ?        Ss   23:33   0:00 ./demo
root     14400  0.0  0.0 112708   980 pts/0    R+   23:34   0:00 grep --color=auto demo
[root@ink src]# getpcaps 14369
Capabilities for `14369': = cap_chown,cap_dac_override,cap_dac_read_search,cap_fowner,cap_fsetid,cap_kill+eip
```
