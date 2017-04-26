#!/usr/bin/env python
# -*-coding:UTF-8-*-
#=========================================================================
import os, sys
import cStringIO, tempfile

# ramdisk size in KB
ramdiskSize = 8192
tarballs = []
workingDir = '.'
busyboxDir = '.'
mountDir = 'mnt'
hardware = 'davinci'

def prepareTarget():
    global workingDir, mountDir, ramdiskSize
    os.system('dd if=/dev/zero of=%s/exapprd bs=1024 count=%d' % (workingDir, ramdiskSize))
    os.system('mke2fs -F -v -m0 %s/exapprd' % workingDir)
    mountDir = tempfile.mkdtemp()
    os.system('mount -o loop %s/exapprd %s' % (workingDir, mountDir))
    return mountDir

def setupTarballs():
    global workingDir
    curdir = os.getcwd()
    os.chdir(mountDir)
    for _ in tarballs:
        len = curdir.rfind('/')
        commFileDir = curdir[:len]
        _file = '%s/%s' %(commFileDir, _)
        if os.path.exists(_file):
            print 'decompress', _, 'to exapprd...'
            os.system('tar --overwrite -zxf %s/%s' %(commFileDir, _))
        else:
            print 'MISSING:', _
    os.chdir(curdir)
    
def gzipRamdisk(tmpdir):
    os.system('umount %s' % tmpdir)
    os.system('gzip -9 %s/exapprd' % workingDir)

def convertRamdisk():
    address = { 'davinci': '0x80800000', 'arm-linux': '0x800000', 'dm6467': '0x80800000' }
    _addr = address[hardware]
    os.chdir(workingDir)
    os.system('mkimage -A arm -O linux -T ramdisk -C gzip -a %s -e %s \
           	-n "ramdisk file system" -d exapprd.gz uExappRd' % (_addr, _addr))

def prepareCopy(tmpdir=''):
    if not tmpdir:
        tmpdir = tempfile.mkdtemp()
    os.system('gunzip %s/exapprd.gz' % workingDir)
    os.system('mount -o loop %s/exapprd %s' % (workingDir, tmpdir))

    return tmpdir

if __name__=='__main__':
    if (1 == len(sys.argv)):
        print 'ramdisk.py <directory>'
        sys.exit(0)
    os.chdir(sys.argv[1])

    _info = open('product.inf')
    _lines = [ tuple(_l.strip().split('=', 1)) for _l in _info.readlines() if _l.strip() and _l[0] != '#' ]
    _info.close()

    info = {}
    info.update(_lines)

    if info.has_key('PRODUCT_TARBALLS') and info['PRODUCT_TARBALLS']:
        tarballs = [ 'default/App/%s.tar.gz' % _ for _ in info['PRODUCT_TARBALLS'].split() ]

    if info.has_key('RAMDISK_SIZE') and info['RAMDISK_SIZE']:
        ramdiskSize = int(info['RAMDISK_SIZE'])      
                  
    prepareTarget()
    setupTarballs()
    gzipRamdisk(mountDir)
    convertRamdisk()
    os.rmdir(mountDir)
    mountDir = 'anything'    