<!--
                 file : css.md
        creation date : 27-03-2017 19:31
           created by : Hubert Li
        last modified : Mon 03 Apr 2017 11:56:43 AM CST
            copyright : 2015-2019 by Hubert Li
                             Strictly Confidential
                             All rights reserved.
                       No part of this hardware description, either
                       material or conceptual may be copied or distributed,
                       transmitted, transcribed, stored in a retrieval system
                       or translated into any human or computer language in
                       any form by any means, electronic, mechanical, manual
                       or otherwise, or disclosed to third parties without
                       the express written permission of Hubert Li
-->


# 三、css
------------

## 1.基本概念
* 层叠样式单 CSS
   * 一组格式化指令，格式化指令被称为样式规则，能够同时控制众多HTML页面的外观
   * 样式规则，包括一个或多个样式属性及其值 
   * 指定
      * 字体
      * 字间距
      * 行间距
      * 页面边距
      * 边框
      * 颜色
      * 背景
   * 可以直接存储与HTML网页或者单数的样式单文件
* 样式规则
   * 样式规则 以 “层叠”的方式，应用与HTML文档
      * 更具体的样式覆盖通用样式

## 2.语法
* 每个样式规则都以";"分号计数

### 使用CSS样式单
* HTML文档链接样式单
   * 在文档```<head> </head>```部分包含一个```<link/>```标签
   * 举例
   ```
      <link rel = "stylesheet" type="text/css" href="/folder/styles.css"/>
   ```

### 样式属性
* 用于控制字体/颜色/对齐方式/边距
* 分为2大类
   * 布局属性
      * 由影响网页中元素位置的属性(边距/填充/对齐等)组成
   * 格式化属性
      * 由影响网页中元素的视觉显示的属性(字体类型/大小/颜色等)组成
* 格式
   * 属性:值

#### 布局属性
* 决定内容在网页上如何排列

##### 1. display 属性
* 有4个可能的取值
   * block
      * 元素显示在下一行，作为一个新段落
   * list-item
      * 元素显示在下一行，前面有列表元素标记(项目符号)
   * inline
      * 元素显示在当前段落内
   * none
      * 元素隐藏不显示
* display 依赖于相对位置的概念
   * 相对位置，指元素相对于网页中其他元素进行定位
   * 绝对位置，将元素放在网页中特定的位置，而不考虑其他元素

##### 2.width/height 属性
* 控制元素矩形区域的大小
* 值可使用不同的度量单位
   * in(英寸)
   * cm(厘米)
   * mm(毫米)
   * px(像素)
   * pt(点)
* 在同一个样式单中，可以使用不同的度量单位，但类似的样式属性最好使用一致的单位

#### 格式化属性
* 用于控制网页内容的外观

##### 1. border属性
* 用于在元素四周建立一个可见的方框或部分边框
* 有下列具体的属性
   * border-width : 边框的宽度
   * border-color : 边框的颜色
   * border-style : 边框的样式
      * 值/含义
         * solid  : 单实线
         * double : 双实线
         * dashed : 虚线
         * dotted : 点线
         * groove : 沟线
         * ridge  : 脊线
         * inset  : 内陷边框
         * outset : 外凸边框
         * none   : 无边框
      * 默认值:none
         * 例外，图像放在```<a>```标签内最为链接图像时，默认实线边框
   * border-left  : 边框的左边
   * border-right : 边框的右边
   * border-top   : 边框的顶部
   * border-bottom: 边框的底部
   * border       : 边框的所有边
      * 值：border-width border-style border-color

##### 2. color / background-color 属性
* color 设置元素中文本的颜色(前景色)
* background-color 设置文本后面的背景色
* 值：
   * 预定义的颜色
      * black / orange / gray / silver(浅灰) / green / purple（紫色) / yellow / red / white 
   * 自定义颜色
      * 通过制定十六进制或RGB(红、绿、蓝)十进制值
      * 十六进制 
      ```
         #xxxxxx   , x->[0,f]
      ```
      * rgb(x,x,x), x->[0,255]
      
##### 3. text-align(对齐)
* 值
   * left   : 向左对齐
   * right  : 向右对齐
   * center : 向中对齐

##### 4.text-indent(缩进)属性
* 

##### 5. font 字体属性
* font-family  : 字体集
   * 制定一个有优先级的字体集合名称列表，而不是一个值
* font-size    : 字体大小
   * in(英寸)
   * cm(厘米)
   * mm(毫米)
   * px(像素)
   * pt(点)
   * 通常使用pot 
* font-style   : 字体样式(正常或斜体等)
   * italic
* font-weight  : 字体的粗细(细、中等、粗等)
   * bold
   * medium
* font
   * 一次指定许多字体属性

##### 6. line-height 文本行高度
* 通常以点为单位

##### 7. text-decoration 链接下划线
* 值
   * underline
   * italic
   * line-through
   * none
      * 用于删除链接的下划线

##### 8. padding 内边距
* 在元素的左/右/上/下添加填充(内边距)
* 填充的单位可以为页宽的百分比
* padding-top     : 上填充
* padding-bottom  : 下填充
* padding-left    : 左填充
* padding-right   : 右填充


### 样式类
* 一组自定义的格式化说明


