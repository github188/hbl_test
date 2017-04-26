#!/usr/bin/env python
# -*-coding:UTF-8-*-
#=========================================================================
import os, sys
import cStringIO, tempfile

content = """
/usr/bin
[ [[ arping awk basename clear cmp comm cut dirname dos2unix du env expr find fold 
free ftpget ftpput hexdump ipcrm ipcs killall last length less logger logname md5sum 
nc nohup nslookup passwd renice seq sort strings tail telnet test tftp time top tr 
traceroute tty uniq unzip uptime wc wget which who whoami xargs yes

/usr/sbin
in.ftpd inetd telnetd


/bin
addgroup adduser ash busybox cat chmod chown cp crypt date dd df dmesg echo egrep 
false fgrep getopt grep gunzip gzip hostname ip ipcalc kill ln login ls lsattr mkdir 
mknod mktemp more mount mv netstat nice pidof ping ps pwd rm rmdir run-parts sed sh
sleep stat stty su sync tar touch true umount uname usleep watch


/sbin
fdisk getty ifconfig ifdown ifup init insmod iptables klogd losetup lsmod mkswap 
modprobe mkfs.ext2 nameif reboot rmmod route swapoff swapon sysctl syslogd
"""

# ramdisk size in KB
ramdiskSize = 8192
tarballs = [ 'default/App/null.tar.gz' ]
commBalls = []
workingDir = '.'
busyboxDir = '.'
mountDir = 'mnt'
hardware = 'dm8127'
product_g = 'xcs'
            
def commandsLink(tmpdir):
    buf = cStringIO.StringIO()
    buf.write(content)
    buf.seek(0)

    cmds = {}
    list = []
    
    for _line in buf.readlines():
        _line = _line.strip()
        if not _line:
            continue
        if _line.startswith('/'):
            if list:
                cmds[list[0]] = list[1:]
            list = [ _line ]
        else:
            list.extend(_line.split())
    if list:
        cmds[list[0]] = list[1:]
        
    for _k, _list in cmds.iteritems():
        for _ in _list:
            os.system('ln -s %s/bin/busybox %s/%s/%s' % (tmpdir, tmpdir, _k[1:], _))
            
def prepareTarget():
    global workingDir, mountDir, ramdiskSize
    os.system('dd if=/dev/zero of=%s/ramdisk bs=1024 count=%d' % (workingDir, ramdiskSize))
    os.system('mke2fs -F -v -m0 %s/ramdisk' % workingDir)
    mountDir = tempfile.mkdtemp()
    os.system('mount -o loop %s/ramdisk %s' % (workingDir, mountDir))
#    os.system('mkdir bin dev etc lib proc sbin tmp usr var')
#    os.system('chmod 1777 tmp')
#    os.system('mkdir usr/bin usr/lib usr/sbin')
#    os.system('mkdir var/lib var/lock var/log var/run var/tmp')
#    os.system('chmod 1777 var/tmp')

    # copy
#    os.system('cp -f %s/busybox %s/bin' % (busyboxDir, mountDir))
#    os.system('ln -s %s/bin/busybox %s/linuxrc' % (tmpdir, mountDir))
    # and libs...
    
    return mountDir

def setupTarballs():
    global workingDir
    curdir = os.getcwd()
    os.chdir(mountDir)
    for _ in tarballs:
        sCmp = _.find('update')
        if sCmp > 0:
            sCmp = product_g.find('-G')
            if sCmp > 0:
                _='default/App/update_g.tar.gz'
                
        print 'decompress', _, 'to ramdisk...'
        _file = '%s/%s' %(curdir, _)
        if os.path.exists(_file):
            os.system('tar --overwrite -zxf %s' %(_file))
            sCmp = _.find('init')
            if sCmp == 12:
                _tmpFile = '%s/%s' %(curdir, 'default/App/device.sh')
                os.system('cp %s %s/etc/ -f' %(_tmpFile, mountDir))
        else:
            print 'MISSING:', _
            
    for _ in commBalls:
        len = curdir.rfind('/')
        commFileDir = curdir[:len]
        _file = '%s/%s' %(commFileDir, _)
        if os.path.exists(_file):
            print 'decompress', _, 'to ramdisk...'
            os.system('tar --overwrite -zxf %s/%s' %(commFileDir, _))
        else:
            print 'MISSING:', _

    os.chdir(curdir)
    
def gzipRamdisk(tmpdir):
    os.system('umount %s' % tmpdir)
    os.system('gzip -9 %s/ramdisk' % workingDir)

def convertRamdisk():
    address = { 'davinci': '0x80800000', 'arm-linux': '0x800000', 'dm6467': '0x80800000', 'dm8127': '0x82800000' }
    _addr = address[hardware]
    os.chdir(workingDir)
    os.system('mkimage -A arm -O linux -T ramdisk -C gzip -a %s -e %s \
           	-n "ramdisk file system" -d ramdisk.gz uRamdisk' % (_addr, _addr))

def prepareCopy(tmpdir=''):
    if not tmpdir:
        tmpdir = tempfile.mkdtemp()
    os.system('gunzip %s/ramdisk.gz' % workingDir)
    os.system('mount -o loop %s/ramdisk %s' % (workingDir, tmpdir))

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