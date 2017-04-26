#!/usr/bin/env python
# -*-coding:UTF-8-*-
#=========================================================================
import os, sys, re
import tempfile, datetime
import cStringIO

workingDir = '.'
versionTag = '0-0-0-0'
productName = ('TEST', 'TEST')
commentTxt = '这家伙很懒，什么也没留下'
buildNumber = 1
svnrepo = 'svn://172.16.65.220/Product'

# 貌似不好使
def removeNode(target):
    for _root, _dirs, _files in os.walk(target, topdown=False):
        for _name in _files:
            os.remove(os.path.join(_root, _name))
        for _name in _dirs:
            os.rmdir(os.path.join(_root, _name))
            
def takeComment():
    global commentTxt
    buf = cStringIO.StringIO()
    print "输入该版本所做的修改(按Ctrl-D结束)"
    while True:
        try:
            _line = raw_input()
            _line = _line.strip()
            buf.write(_line + '\r\n')
        except EOFError:
            break

    fp = open('update.txt', 'w')
    commentTxt = buf.getvalue().strip()
    fp.write(commentTxt)
    fp.close()


def makeRarCommit():
    rarname = productName[0].replace('-','')+'_v'+versionTag
    os.system('md5sum -b %s.pkg  >release.md5' % productName[1])
    os.system('rar a %s.rar %s.pkg release.md5' % (rarname, productName[1]))
    os.system('rar c %s.rar <update.txt' % rarname)

    # subversion checkout
    _repo = svnrepo + '/trunk/Device/' + productName[0]
#    os.system('svn checkout --username ? --password ? ' + _repo)
    os.system('svn checkout ' + _repo)
    os.chdir(productName[0])
    os.system('cp -fv ../%s.rar .' % rarname)
    
    # modify ChangeLog
    fp = open('ChangeLog.txt', 'a')
    print >>fp, 49*'='
    print >>fp, datetime.datetime.today(), 'release', versionTag, 'build', buildNumber
    print >>fp, 49*'-'
    print >>fp, commentTxt
    print >>fp
    fp.close()

    # and commit        
    os.system('svn add %s.rar' % rarname)
    os.system('svn commit --file ../update.txt')
