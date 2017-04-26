.PHONY : shell update version ramdisk package release clean

all: version
	cd .. && scons product=CYC500XKW4C gProduct=CYC500X-KW4C

version:
	cd .. && scons product=CYC500XKW4C target=spec -c gProduct=CYC500X-KW4C && scons product=CYC500XKW4C target=spec gProduct=CYC500X-KW4C

ramdisk:
	cd .. && scons product=CYC500XKW4C target=ramdisk gProduct=CYC500X-KW4C

packageall:
	cd .. && scons product=CYC500XKW4C target=packageAll gProduct=CYC500X-KW4C

package:
	cd .. && scons product=CYC500XKW4C target=package gProduct=CYC500X-KW4C

packfpga:
	cd .. && scons product=CYC500XKW4C target=packFpga gProduct=CYC500X-KW4C

release:
	cd .. && scons product=CYC500XKW4C target=commit gProduct=CYC500X-KW4C
	
exappRD:
	cd .. && scons product=CYC500XKW4C target=exappRD gProduct=CYC500X-KW4C
	
packexapp:
	cd .. && scons product=CYC500XKW4C target=packExapp gProduct=CYC500X-KW4C
	
update:
    cd .. && scons product=CYC500XKW4C target=update gProduct=CYC500X-KW4C hardVer=1
    
veraddno:
	cd .. && scons product=CYC500XKW4C target=updateVer gProduct=CYC500X-KW4C

clean:
	cd .. && scons product=CYC500XKW4C -c gProduct=CYC500X-KW4C
	rm -f $(ALL_OBJ)/*.o
	rm -f $(EXEC)
