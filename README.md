### MSVC DLL转MINGW 导入库

#### MSVC DLL函数符号

由于C++没有同一的ABI接口，故各个编译器厂商生成的库接口不一样，需要转换。

假设现在有**MSVC**生成的**DLL**：`add.dll`，其是由**add.cpp**与**add.h**生成：

```cpp
// add.h
#ifndef ADD_H
#define ADD_H

__declspec(dllexport) int __stdcall add(int a, int b);

#endif // ADD_H
```



```cpp
// add.cpp
#include "add.h"

__declspec(dllexport) int __stdcall add(int a, int b)
{
    return a + b;
}

```

在cmd.exe下使用MSVC6的编译器生成`add.dll`动态库与`add.lib`导入库，并查看生成的**DLL**中的函数符号，命令如下：

```sh
cl.exe /LD add.cpp
dumpbin.exe /exports add.dll
dumpbin.exe /exports add.lib
```

- 第一条语句：生成**DLL**；
- 第二条语句：查看**DLL**中的被编译器修饰后的函数符号。
- 第三条语句：查看**LIB**导入库的函数名（导入库只包含了地址符号表等，确保程序找到对应函数的一些基本地址信息）

此时，从第二条语句的输出可以看到，生成的**DLL**中`add`的函数符号为：`?add@@YGHHH@Z`，关于符号的格式可以google**编译器的符号修饰规则**；

从第三条语句的输出可以看到，**LIB**导入库的函数名为：`?add@@YAHHH@Z (int __stdcall add(int,int))`。

`add.cpp`与`add.h`中**__stdcall**是Windows下的默认调用约定。若将其去掉，然后重新生成**DLL**，再查看函数符号，则此时为：`?add@@YAHHH@Z (int __cdecl add(int,int))`。

由于MSVC与Mingw对C++编译的库接口不一样（函数的符号不一样），不能正确链接，所以一般需要转换为C的ABI接口，因为C语言的ABI接口在各个编译器中都是相同的，不会修饰函数符号。

将C++的函数符号转换为C语言的函数符号也很简单（但这种无法将C++类成员函数转换），只需要将`add.cpp`与`add.h`中的函数定义与声明前加上`extern "C"`：

```cpp
// 当有extern "C"时，有无__declspec(dllexport)都一样
extern "C" __declspec(dllexport) int __stdcall add(int a, int b)
```

即让其按照C语言编译器来编译`add.cpp`，编译后生成的**DLL**的函数符号为：`_add@8`

**__stdcall**会使导出函数名字前面加一个下划线，后面加一个@再加上参数的字节数；若去掉**__stdcall**或改为**__cdecl**，则生成的**DLL**的函数符号为：`add`



#### MSVC使用def文件修改函数符号，生成DLL

有时候，在C++中既希望使用形如：

```c
// 当有extern "C"时，有无__declspec(dllexport)都一样
extern "C" __declspec(dllexport) int __stdcall add(int a, int b)
```

这样的函数声明与定义，又希望其生成的**DLL**的函数符号为**函数的原始声明**（即不对函数符号做修饰，使用**__cdecl**调用约定），则需要使用**def**文件来生成具有指定函数符号的**DLL**：

```
; 这是注释
LIBRARY add.dll
EXPORTS
add
```

在cmd.exe下使用MSVC编译器用以下命令生成具有**函数原始声明**的`add.dll`：

```sh
cl.exe /c /I. add.cpp
link.exe /machine:i386 /def:add.def /out:add.dll add.obj
dumpbin.exe /exports add.dll
dumpbin.exe /exports add.lib
```

第一条语句：仅仅只编译（不链接）`add.cpp`以生成`add.obj`

第二条语句：根据`add.def`文件生成`add.dll`

第三条语句：查看**DLL**中add函数符号，为：`add`

第四条语句：查看**LIB**导入库中的函数名，为：`_add@8`

至此，就为编译后实际被修饰的函数`_addd@8`生成了具有`add`接口的**DLL**



#### 使用Mingw工具生成DLL

```sh
i686-w64-mingw32-g++ -c -m32 -O0 -I. -o add.o add.cppp
dllwrap.exe --def add.def -o add.dll add.o --output-lib libadd.a
lib.exe /machine:i386 /def:add.def /out:add.lib
```

第一条语句：只编译不链接`add.cpp`，生成`add.o`

第二条语句：根据`add.def`文件生成`add.dll`与导入库`libadd.a`

第三条语句：使用**MSVC**工具根据`add.def`文件生成导入库`add.lib`



#### 使用Mingw工具生成MSVC的DLL的导入库

此处的`add.dll`为上面使用MSVC编译器生成的，函数符号为`_add@8`。

在bash.exe中使用以下命令：

```
pexports.exe add.dll > add.def
```

生成`def`文件，内容如下：

```
LIBRARY add.dll
EXPORTS
_add@8
```

修改`add.def`为：

```
LIBRARY add.dll
EXPORTS
add
```

然后再在bash.exe中执行：

```
dlltool.exe -D add.dll -d add.def -l libadd.a
```

以使用`add.dll`根据`add.def`文件生成`libadd.a`

注意，`add.dll`是32bit系统生成的，需要指定使用**i686**版本的版本的`pexports.exe`和`dlltool.exe`工具



### 实例应用

#### 使用吉阳光电GY8507的上位机DLL替换ZLG CANTest的DLL

首先，在cmd.exe中使用以下命令分析GY8507上位机的`VCI_CAN.dll`的函数符号与CANTest的`ControlCAN.dll`的函数符号：

```sh
dumpbin.exe /exports VCI_CAN.dll
dumpbin.exe /exports ControlCAN.dll
```

对比后发现，`ControlCAN.dll`中的函数符号，在`VCI_CAN.dll`中都可以找到，其函数调用者所暴露的名称都一样，**为未加修饰的函数名**。

然后，再使用以下命令分析两个**DLL**的导入库`VCI_CAN.lib`与ControlCAN.lib`：

```sh
dumpbin.exe /all VCI_CAN.lib
dumpbin.exe /all ControlCAN.lib
```

对比后发现相同的函数名有些不同，这里以`VCI_Transmit`这个**API**为例：

**`VCI_CAN.lib`的`VCI_Transmit`函数的信息：**

```
Archive member name at 1D18: VCI_CAN.dll/
5E0B081F time/date Tue Dec 31 16:34:39 2019
         uid
         gid
       0 mode
      31 size
correct header end

  Version      : 0
  Machine      : 14C (i386)
  TimeDateStamp: 5E0B081F Tue Dec 31 16:34:39 2019
  SizeOfData   : 0000001D
  DLL name     : VCI_CAN.dll
  Symbol name  : _VCI_Transmit@16
  Type         : code
  Name type    : ordinal
  Ordinal      : 15
```

**`ControlCAN.lib`中`VCI_Transmit`函数的信息：**

```
Archive member name at 11AC: ControlCAN.dll/
5C8B6E7A time/date Fri Mar 15 17:20:58 2019
         uid
         gid
       0 mode
      34 size
correct header end

  Version      : 0
  Machine      : 14C (i386)
  TimeDateStamp: 5C8B6E7A Fri Mar 15 17:20:58 2019
  SizeOfData   : 00000020
  DLL name     : ControlCAN.dll
  Symbol name  : _VCI_Transmit@20
  Type         : code
  Name type    : undecorate
  Hint         : 13
  Name         : VCI_Transmit
```



从上面的`VCI_Transmit`函数的信息例子可以发现，两个**DLL**中的`VCI_Transmit`函数符号格式都是`_VCI_Transmit@n`的格式，其中，@符号后跟的是函数参数所占字节数，因此，两个**DLL**中，API接口虽然函数名称相同，但参数个数不同。这说明其函数原型都遵循**C语言**的**__stdcall**调用约定，即函数声明形如：

```cpp
extern "C" return_type __stdcall function_name(type1 arg1, type2, arg2)
```

另外，注意到上面的**Name type**信息行，`VCI_CAN.dll`中为**ordinal**，`ControlCAN.dll`中为**undecorate**。

最后，再对比两个**DLL**给出的头文件，`VCI_CAN.h`与`ControlCAN.h`（这里只看`VCI_Transmit`函数的声明）：

```cpp
// VCI_CAN.h
extern "C" {
	DWORD __stdcall VCI_Transmit(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd, 				PVCI_CAN_OBJ pSend);
}

// ControlCAN.h
extern "C" ULONG __stdcall VCI_Transmit(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd, 	PVCI_CAN_OBJ pSend, ULONG Len);
```

这和上面的分析也吻合，函数名相同，调用约定相同，但参数个数不同。



#### 使用MinGW编译器二次封装VCI_CAN.dll

从上面分析发现，`ControllCAN.dll`与`VCI_CAN.dll`的函数同名，但参数个数不同，因此**不能**简单的将`VCI_CAN.dll`**重命名**后直接替换**CANTest**软件中的`ControllCAN.dll`。

因此，需要想一个方法，使**CANTest**软件通过`ControllCAN.dll`去调用`VCI_CAN.dll`中的函数才行，方案如下：

```
新的ControllCAN.dll ----> 中间转换静态库 ----> VCI_CAN.dll
```

此处的`ControllCAN.dll`是新封装的DLL，不是**CANTest**软件下的`ControllCAN.dll`，其内容不同，但其函数接口都一样，之后只需要用这个新的`ControllCAN.dll`去替换**CANTest**下的原始`ControllCAN.dll`即可；当然，还需要将`VCI_CAN.dll`赋值到CANTest软件目录下。



#### 创建中间转换静态库

新建**GY8507_LIB_Wrap**文件夹，将**GY8507提供的二次开发包**中的头文件`ControllCAN.h`拷贝到此目录，并在此文件夹中新建**warp.c**与**warp.h**文件（下面仍然以`VCI_transmit`函数为例）：

**原始GY8507提供的二次开发包的`ControllCAN.h`：**

```c
#if !defined(AFX_CONTROLCAN_H__A09B0BED_2018_4536_89DA_E8A9BD2F74FB__INCLUDED_)
#define AFX_CONTROLCAN_H__A09B0BED_2018_4536_89DA_E8A9BD2F74FB__INCLUDED_
#include <afxtempl.h>

typedef CMap<DWORD ,DWORD,CString,CString> DEVICESN_MAP;

....

//专为打开NET_CAN设计
extern __declspec(   dllexport   ) DWORD VCI_OpenDevice(DWORD DeviceType,DWORD DeviceIP,DWORD DevicePort,DWORD HostPort);

extern "C"
{	
	DWORD __stdcall VCI_OpenDevice(DWORD DeviceType,DWORD DeviceInd,DWORD Reserved);
    ...
	...
	DWORD __stdcall VCI_Transmit(DWORD DeviceType,DWORD DeviceInd,DWORD 						CANInd,PVCI_CAN_OBJ pSend);
	....
}

#endif
```



**修改后的GY8507的`ControllCAN.h`：**

```c
#ifndef CONTROLL_CAN_H
#deifne CONTROLL_CAN_H

#include <windows.h>

DWORD __stdcall VCI_OpenDevice(DWORD DeviceType,DWORD DeviceInd,DWORD Reserved);
...
DWORD __stdcall VCI_Transmit(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd,PVCI_CAN_OBJ 	pSend);
....

#endif
```



```c
// wrap.h
#ifndef WRAP_H
#define WRAP_H

#include "ControlCAN.h"

#ifdef __cplusplus
extern "C"{
#endif

typdef struct {
    ULONG (*pfn_VCI_Transmit)(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd, 					PVCI_CAN_OBJ pSend);
} GY8507_funcs_t;

extern GY8507_funcs_t* GY8507_funcs_table_get(void);

#ifdef __cplusplus
}
#endif

#endif /* WRAP_H */
```



```c
// wrap.c
#include "wrap.h"

#if __cplusplus
extern "C" {
#endif

static GY8507_funcs_t g_GY8507_funcs_table = {
    NULL
};

GY8507_funcs_t* GY8507_funcs_table_get(void)
{
    HMODULE dll_module = LoadLibrary(TEXT("VCI_CAN.dll"));
    if (dll_module == NULL)
    {
        return NULL;
    }
    
    g_GY8507_funcs_table.pfn_VCI_Transmit = (ULONG (*) (DWORD, DWORD, DWORD, 					PVCI_CAN_OBJ))GetProcAddress(dll_module, "VCI_Transmit");
    
    return &g_GY8507_funcs_table;
}

#if __cplusplus
}
#endif
```



然后在**bash.exe**中使用以下命令生成中间静态库**`libGY8507_LIB_WARP.a`**：

```sh
i686-w64-mingw32-gcc -c -m32 -g -O0 -I. -o ./warp.o ./warp.c

ar -cr ./libGY8507_LIB_WARP.a ./warp.o
```



#### 创建新的ControllCAN.dll库

新建**GY8507_DLL_to_ZLG_DLL**目录，将**ZLG提供的二次开发包**中的`ControllCAN.h`拷贝到此目录下，然后新建`ControlCAN.cpp`与`ControlCAN.def`文件：

注意，在ControlCAN.h中开头添加一行`#include <windows.h>`。

```cpp
// ControlCAN.cpp
#include "ControlCAN.h"

// 有ZLG的函数接口转到调用GY8507的函数接口时需要使用
typedef struct _GY8507_VCI_INIT_CONFIG {
    DWORD AccCode;
    DWORD AccMask;
    DWORD Reserved;
    UCHAR Filter;   // 过滤方式, 0: 单滤波, 1: 双滤波
    UCHAR kCanBaud; // CAN波特率索引号
    UCHAR Timing0;
    UCHAR Timing1;
    UCHAR Mode;
    UCHAR CanRx_IER;
} GY8507_VCI_INIT_CONFIG, *GY8507_PVCI_INIT_CONFIG;

// 有ZLG的函数接口转到调用GY8507的函数接口时需要使用
typedef struct _GY8507_VCI_CAN_OBJ
{
    BYTE ID[4];
    UINT TimeStamp;	 //保留不用
    BYTE TimeFlag;	 //保留不用
    BYTE SendType;	 //保留不用
    BYTE RemoteFlag; //是否是远程帧
    BYTE ExternFlag; //是否是扩展帧
    BYTE DataLen;
    BYTE Data[8];
    BYTE Reserved[3];
} GY8507_VCI_CAN_OBJ, *GY8507_PVCI_CAN_OBJ;

typedef struct {
    ...
    ULONG (*pfn_VCI_Transmit)(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd, 					GY8507_PVCI_CAN_OBJ pSend);
    ...
} GY8507_funcs_t;

extern "C" GY8507_funcs_t* GY8507_funcs_table_get(void);

static GY8507_funcs_t *g_GY8507_funcs = nullptr;

ULONG STDCALL VCI_Transmit(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd, PVCI_CAN_OBJ pSend, ULONG Len)
{
    ULONG rc = 0;
    ULONG ret = 0;
    GY8507_PVCI_CAN_OBJ pbuf;
    VCI_CAN_OBJ send_buf = {0};

    if (!g_GY8507_funcs)
    {
        g_GY8507_funcs = GY8507_funcs_table_get();
    }

    if (2 < DeviceType && DeviceType < 5)
    {
        DeviceType = 2;
    }

    for (ULONG i = 0; i < Len; ++i)
    {
        send_buf = pSend[i];

        // ZLG CAN-ID ==> GY8507 CAN-ID
        // ZLG 右对齐, GY8507 左对齐
        if (pSend[i].ExternFlag == 1)
        {
            send_buf.ID = 0
                | (((pSend[i].ID >>  0) & 0x1F) << 27)  // GY8507 29bits CAN-ID [ 4: 0]bits
                | (((pSend[i].ID >>  5) & 0xFF) << 16)  // GY8507 29bits CAN-ID [12: 5]bits
                | (((pSend[i].ID >> 13) & 0xFF) <<  8)  // GY8507 29bits CAN-ID [20:13]bits
                | (((pSend[i].ID >> 21) & 0xFF) <<  0); // GY8507 29bits CAN-ID [28:21]bits
        }
        else
        {
            send_buf.ID = 0
                | (((pSend[i].ID >> 0) & 0x07) << 13)  // GY8507 11bits CAN-ID [ 2:0]bits
                | (((pSend[i].ID >> 3) & 0xFF) <<  0); // GY8507 11bits CAN-ID [10:3]bits
        }

        pbuf = (GY8507_PVCI_CAN_OBJ)&send_buf;

        ret = g_GY8507_funcs->pfn_VCI_Transmit(DeviceType, DeviceInd, CANInd, pbuf);
        if (ret == 1)
        {
            rc++;
        }
        else if (ret == -1 || ret > 1)
        {
            break;
        }
    }

    return rc;
}

```



```
// ControllCAN.def

LIBRARY "ControlCAN.dll"
EXPORTS
VCI_Transmit
```

注意，上面的ControlCAN.def文件中，只添加了一个函数`VCI_Transmit`。

最后在bash.exe中使用以下命令生成新的`ControlCAN.dll`：

```sh
i686-w64-mingw32-g++ -c -m32 -g -O0 -Isrc -o ./ControlCAN.o ./ControlCAN.cpp

dllwrap.exe --def ./ControlCAN.def -o ./ControlCAN.dll ./ControlCAN.o -LGY8507_LIB_WARP -lGY8507_LIB_WARP --output-lib ./ControlCAN.lib
```

至此，新的`ControlCAN.dll`就生成了，然后将此`ControlCAN.dll`与`VCI_CAN.dll`，以及`VCI_CAN.dll`的依赖`SiUSBXp.dll`拷贝到**CANTest**软件的目录下，就可以使用**CANTest连接GY8507**了。
