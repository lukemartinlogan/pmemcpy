# SPDX-License-Identifier: GPL-2.0
#
# Makefile for the linux ext4-filesystem routines.
#

KVERSION = $(shell uname -r)
obj-m += pmemulator.o

pmemulator-y	:= balloc.o bitmap.o block_validity.o dir.o ext4_jbd2.o extents.o \
		extents_status.o file.o fsmap.o fsync.o hash.o ialloc.o \
		indirect.o inline.o inode.o ioctl.o mballoc.o migrate.o \
		mmp.o move_extent.o namei.o page-io.o readpage.o resize.o \
		super.o symlink.o sysfs.o xattr.o xattr_trusted.o xattr_user.o
pmemulator-y += acl.o
pmemulator-y += xattr_security.o
pmemulator-y += verity.o

all:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) clean
