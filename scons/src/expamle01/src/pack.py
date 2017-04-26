#!/usr/bin/env python
# -*-coding:UTF-8-*-
#=========================================================================

import os, sys
import tempfile, datetime
import getopt 
#import struct
from struct import pack
import cStringIO

def usage(code):
    print """
Usage: %s [options] inputfile ....
  -h  --help             Display this usage information.
  -o  --output filename  Write output to file.
  -v  --verbose          Print verbose messages.
  -s  --system osid      Set target operating system type:
                           1-vxWorks,
                           2-Linux(default)
  -c  --chip chiptype    Set target chip type:
                           1-arm9(default),
                           2-powerpc,
                           3-ti,
                           4-adi,
                           5-equator,
                           6-hisilicon
  -p  --product prdid    Set target product type, check products.xls
  -g  --guard certid     Set package ceritfication type:
                           0-none(default),
                           1-simply add,
                           2-xor checksum,
                           3-crc checksum,
                           4-md5 checksum,
                           5-encrypted md5 checksum
  -r  --revision version Set package version mark
  -z  --gzip             gzip compress items
""" % os.path.basename(sys.argv[0])
    sys.exit(code)
    
def fill0(pattern):
    return 32*'\0'

def addsum(pattern):
    sum = 0
    for ch in pattern:
        sum += ord(ch)
    sum &= 0xFF
    return pack('B', sum) + 31*'\0'

def xorsum():
    sum = 0
    for ch in pattern:
        sum ^= ord(ch)
    return pack('B', sum) + 31*'\0'


sumFunctions = [ fill0, addsum, xorsum ]

def exint(candiate):
    try:
        try: return int(candiate)
        except ValueError: return int(candiate, 16)
    except:
        usage(2)
        
class kjpack:
    def __init__(self):
        self.compress = False
        self.osType = 2
        self.chipType = 1
        self.certType = 1
        self.version = 0
        self.fileName = ''
        self.files = []

    def pack(self):
        self.__openPackFile()
        self.__procPackInfo()
        self.__procPackItems()        
        self.__closePackFile()

    def __openPackFile(self):
        try:            
            self.packFile = open(self.fileName, 'w')
        except StandardError, e:
            print str(e)
            usage(2)
        
    def __procPackInfo(self):
        try:
            self.packInfo = cStringIO.StringIO()
            
            self.packInfo.write(pack('cccc', 'K', 'J', 'B', 'N'))
            self.packInfo.write(pack('<L', self.osType))
            self.packInfo.write(pack('<L', self.chipType))
            self.packInfo.write(pack('<L', self.devCode))
            self.packInfo.write(pack('>L', self.version))
            self.packInfo.write(pack('L', 0)) # hardware revision
            self.packInfo.write(pack('<L', len(self.files)))
            self.packInfo.write(pack('<L', self.certType))

            # compute checksum            
            #self.packInfo.write(32*'\0')
            pattern = self.packInfo.getvalue()[0:32]
            sumfunc = sumFunctions[self.certType]
            self.packInfo.write(sumfunc(pattern))
                
        except StandardError, e:
            self.packInfo.close()
            print str(e)
            usage(2)
            
    def __copyItemIntoPack(self, itemName):
        try:
            self.packFile.seek(self.packOffset)
            
            readFile = open(itemName, 'r')

            while True:
                content = readFile.read(16384)
                self.packFile.write(content)
                if (16384 != len(content)):
                    n = len(content) & 3
                    if 0 != n: self.packFile.write((4-n)*'\0')
                    break
                
        except:
            readFile.close()
        else:
            count = 0
            for ch in itemName:
                self.packInfo.write(ch)
                count += 1
                if count >= 16:
                    break
            else:
                assert(count < 16), 'unexpect case'
                self.packInfo.write((16-count)*'\0')

            packLen = readFile.tell()
            self.packInfo.write(pack('<L', self.packOffset))
            self.packInfo.write(pack('<L', packLen))
            self.packInfo.write(pack('<L', self.compress))
            self.packInfo.write(4*'\0') # reserved

            packLen = (packLen + 3) >> 2
            
            self.packOffset += (packLen << 2)
            readFile.close()
        
    def __procPackItems(self):
        self.packOffset = 64 + 32*len(self.files)
        try:
            for item in self.files:
                self.__copyItemIntoPack(item)
        except StandardError, e:
            print str(e)
            
    def __closePackFile(self):
        self.packFile.seek(0)
        self.packFile.write(self.packInfo.getvalue())
        self.packFile.close()
        self.packInfo.close()
    
if __name__=='__main__':
    if (1 == len(sys.argv)):
        usage(1)
        
    try:
        opts, args = getopt.getopt(sys.argv[1:], "ho:vs:c:p:g:r:z", \
            ["help", "output=", "verbose", "system=", "chip=", \
             "product=", "guard=", "revision=", "gzip"])
    except getopt.GetoptError, err:
        # print help information and exit:
        print str(err) # will print something like "option -a not recognized"
        usage(2)

    mypack = kjpack()
    verbose = False
    
    for o, a in opts:
        if o in ("-v", "--verbose"):
            verbose = True
        elif o in ("-h", "--help"):
            usage(0)
        elif o in ("-o", "--output"):
            mypack.fileName = a
        elif o in ("-s", "--system"):
            mypack.osType = exint(a)
        elif o in ("-c", "--chip"):
            mypack.chipType = exint(a)
        elif o in ("-p", "--product"):
            mypack.devCode = exint(a)
        elif o in ("-g", "--guard"):
            mypack.certType = exint(a)
        elif o in ("-r", "--revision"):
            tempver = a.split('.')
            tempver.extend(['0','0','0','0'])
            mypack.version = int(tempver[0])
            mypack.version <<= 8
            mypack.version |= int(tempver[1])
            mypack.version <<= 8
            mypack.version |= int(tempver[2])
            mypack.version <<= 8
            mypack.version |= int(tempver[3])
            
        elif o in ("-z", "--gzip"):
            mypack.compress = True
        else:
            assert False, "unhandled option"

    if len(args) <= 0:
        print 'no file pack!'
        usage(2)
        
    mypack.files = args[0:]

    mypack.pack()
    
else:
    print '%s not support import' % os.path.basename(sys.argv[0])
