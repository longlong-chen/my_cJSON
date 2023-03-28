## JSON 启程及处理逻辑值

JSON例子：
~~~js
{
    "title": "Design Patterns",
    "subtitle": "Elements of Reusable Object-Oriented Software",
    "author": [
        "Erich Gamma",
        "Richard Helm",
        "Ralph Johnson",
        "John Vlissides"
    ],
    "year": 2009,
    "weight": 1.8,
    "hardcover": true,
    "publisher": {
        "Company": "Pearson Education",
        "Country": "India"
    },
    "website": null
}
~~~

从例子中可以看出JSON包含以下6中数据类型：

* 数字 (整数或浮点数)
* 字符串（在双引号中）
* 逻辑值（true或false）
* 数组 （在中括号中）
* 对象（在大括号中）
* null

JSON语法集：

~~~
JSON-text = ws value ws
ws = *(%x20 / %x09 / %x0A / %x0D)
value = null / false / true / string / number / object / array
null  = "null"
false = "false"
true  = "true"
~~~

注：%x20（空格）、%x09（制表符\t）、%x0A（换行符\n，当前位置移到下一行开头）、%x0D（回车\r，当前位置移到本行开头）；有一种做法把小于32的看作无关字符(跳过)

## 细节记录与说明

### 防卫式声明

~~~c
#ifndef cJSON__h
#define cJSON__h

#endif
~~~

### C 和 C++ 兼容编译方式

~~~c
#ifdef __cplusplus
extern "C"
{
#endif

pass

#ifdef __cplusplus
}
#endif
~~~

