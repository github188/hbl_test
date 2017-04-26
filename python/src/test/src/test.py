#!/usr/bin/python
# _*_ coding:UTF-8 _*_    # 在头部指定编码
from package.runoob1 import runoob1
from package import runoob2

print "Hello,world!"
print "你好，世界!"

name = raw_input("Input the name\n")
age = raw_input("Input your age\n")

print "name is %s, age = %d\n"%(name,25)

if name == 'python':        # 0 or null 为false, else true
   print 'welcome boss'
elif name == 'liyh':
   print 'welcome liyh'
else:
   print name

   print '缩进错误'

count = 0
while (count < 9):
   print 'The count is :',count
   count = count + 1
else:
   print count,'is more than 8'


prime = [2,3,5,7]

for i in range(1,10):
   for j in prime:
      if i == j:
         break
   else:
      print i,'is not a prime num'


import time

print "当前时间戳:",time.time()

print "本地时间:",time.localtime(time.time())

print "格式化本地时间:",time.asctime(time.localtime(time.time()))

# 格式化成2016-03-20 11:45:39形式
print time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()) 

# 格式化成Sat Mar 28 22:24:24 2016形式
print time.strftime("%a %b %d %H:%M:%S %Y", time.localtime()) 
  
# 将格式字符串转换为时间戳
a = "Sat Mar 28 22:24:24 2016"
print time.mktime(time.strptime(a,"%a %b %d %H:%M:%S %Y"))


import calendar
print "输出2017年4月份日历:",calendar.month(2017,4)

# 传不可变对象 string,tuple,numbers
def printme(str):
   "打印字符串"
   print str
   return

# 传可变对象list,dict
def changeme(mylist):
   "修改传入的列表"
   mylist.append('liyh so handsome a boy!')
   print "函数内：",mylist
   return

printme("liyh handsome boy!\n")

list1=[1,2,3,4]
print "函数外，函数前：",list1
changeme(list1)
print "函数外，函数后：",list1

runoob2.runoob2()
runoob1()

try:
   fh = open("testfile", "r")
   fh.write("这是一个测试文件，用于测试异常!!")
except IOError:
   print "Error: 没有找到文件或读取文件失败"
else:
   print "内容写入文件成功"
   fh.close()

raw_input("\n\nPress the enter key to exit")
