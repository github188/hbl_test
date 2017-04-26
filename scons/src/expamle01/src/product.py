#!/usr/bin/env python
# -*-coding:UTF-8-*-
#=========================================================================
import os, sys
from urllib import urlencode
import httplib

def genProductScript(info):
    pass
    
def genProductInfofile(info):
    ofile = open(info['PRODUCT_DIR'] + os.sep + 'product.inf', 'w')
    
    
    print >>ofile, "#"
    print >>ofile, "# 产品规格文件"
    print >>ofile, "#"
    print >>ofile
    print >>ofile, "#"
    print >>ofile, "# 指定内核运行版本，主要是频率不同"
    print >>ofile, "UIMAGE_RATE=uImage"
    print >>ofile
    print >>ofile, "# 产品软件版本"
    print >>ofile, "# 至多四个数字,用 '.' 分割"
    print >>ofile, "SOFTWARE_VERSION=0"
    print >>ofile
    print >>ofile, "# ver 命令显示产品软件版本时打印的特殊标识"
    print >>ofile, "# 如果不需要, 可设置为空或者注释掉"
    print >>ofile, "SOFTWARE_MARK="
    print >>ofile
    
    print >>ofile, "# 产品 ramdisk 尺寸"
    print >>ofile, "RAMDISK_SIZE=12000"
    print >>ofile
    print >>ofile, "# 产品组件, 存放在 default/App 下的 tar 包, "
    print >>ofile, "# 只需填写名称, 以空格分开"
    print >>ofile, "PRODUCT_TARBALLS=null init dsp config app update nview"
    print >>ofile
    print >>ofile, "# 额外打包内容, 以空格分开"
    print >>ofile, "#EXTRA_PACK=uBoot"
    print >>ofile
    
    ofile.close()

def genProductMakefile(info):
    ofile = open(info['PRODUCT_DIR'] + os.sep + 'Makefile', 'w')
    
    print >>ofile, ".PHONY : shell update version ramdisk package release clean"
    print >>ofile
    print >>ofile, "all: version"
    print >>ofile, "\tcd .. && scons product=%(Name)s" % info
    print >>ofile
    print >>ofile, "version:"
    print >>ofile, "\tcd .. && scons product=%(Name)s target=spec -c && scons product=%(Name)s target=spec" % info
    print >>ofile
    print >>ofile, "ramdisk:"
    print >>ofile, "\tcd .. && scons product=%(Name)s target=ramdisk" % info
    print >>ofile
    print >>ofile, "package:"
    print >>ofile, "\tcd .. && scons product=%(Name)s target=package" % info
    print >>ofile
    print >>ofile, "release:"
    print >>ofile, "\tcd .. && scons product=%(Name)s target=commit" % info
    print >>ofile
    print >>ofile, "clean:"
    print >>ofile, "\tcd .. && scons product=%(Name)s -c " % info
    print >>ofile, "\trm -f $(ALL_OBJ)/*.o"
    print >>ofile, "\trm -f $(EXEC)"

    ofile.close()

    
def genProductVersion(info):
    ofile = open(info['PRODUCT_DIR'] + os.sep + 'vernum.c', 'w')             
        
    print >>ofile, "/* automatic generated version file, don't edit it manually */"
    print >>ofile, 'char version_string[] = { \\'
    print >>ofile, '   "%(Declare)s Version: %(VersionString)s\\n" \\' % info
    print >>ofile, '   "Build: %(BuildNumber)d " __DATE__ " " __TIME__ \\' % info
    print >>ofile, '};'
    print >>ofile, 'const unsigned int version_int = %(VersionInt)d;' % info
    print >>ofile, 'const unsigned int device_code = %(DeviceCode)s;' % info
    print >>ofile
    ofile.close()

def genProductEnviron(info):
    ofile = open(info['PRODUCT_DIR'] + os.sep + 'default/App/device.sh', 'w')
    
    print >>ofile, "#!/bin/sh"
    print >>ofile, "hostname", info['G_PRODUCT_NAME']
    print >>ofile, 'export DEVICE="{ \\"ID\\" : %d,' % int(info['DeviceCode'], 16),
    print >>ofile, '\\"Name\\" : \\"%(G_PRODUCT_NAME)s\\",' % info,
    print >>ofile, '\\"VideoIn\\" :%s, \\"VideoOut\\":%s,' % tuple(info['Video'].split('|')),
    print >>ofile, '\\"AudioIn\\":%s, \\"AudioOut\\":%s,' % tuple(info['Audio'].split('|')),
    print >>ofile, '\\"AlarmIn\\":%s, \\"AlarmOut\\":%s }"' % tuple(info['Alarm'].split('|'))   
    ofile.close()
        
if __name__=='__main__':
    if len(sys.argv) != 2:
        print 'Usage product.py <Internal Name of Product>'
        sys.exit(0)
        
    _product = sys.argv[1].upper()
    _list = [_product, 'default', 'App']
    for i, _ in enumerate(_list):
        try:
            os.mkdir(os.sep.join(_list[0:i+1]))
        except:
            pass
                                
    try:
        info = {}
        info['PRODUCT_DIR'] = _product
        info['PRODUCT_NAME'] = _product

        client = httplib.HTTPConnection('172.16.65.120:9408')
        data = { 'name': info['PRODUCT_NAME'] }
        client.request("GET", '/getinfo?' + urlencode(data))
        content = client.getresponse()
        client.close()
                
        _info = eval(content.read())
        print info['PRODUCT_NAME'], 'info response:' , _info
        info.update(_info.items())
        info['BuilderTag'] = _info['BuilderTag']
#        info['toolchain'] = toolchains[_info['BuilderTag']]
        print 'after request info is', info

        genProductEnviron(info)
        genProductMakefile(info)
        genProductInfofile(info)
#        genProductScript(info)

    except Exception, e:
        raise e
                    
           	    