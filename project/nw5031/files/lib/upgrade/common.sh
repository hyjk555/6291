#!/bin/sh

RAM_ROOT=/tmp/root

ldd() { LD_TRACE_LOADED_OBJECTS=1 $*; }
libs() { ldd $* | awk '{print $3}'; }

install_file() { # <file> [ <file> ... ]
	for file in "$@"; do
		dest="$RAM_ROOT/$file"
		[ -f $file -a ! -f $dest ] && {
			dir="$(dirname $dest)"
			mkdir -p "$dir"
			cp $file $dest
		}
	done
}

install_bin() { # <file> [ <symlink> ... ]
	src=$1
	files=$1
	[ -x "$src" ] && files="$src $(libs $src)"
	install_file $files
	[ -e /lib/ld.so.1 ] && {
		install_file /lib/ld.so.1
	}
	shift
	for link in "$@"; do {
		dest="$RAM_ROOT/$link"
		dir="$(dirname $dest)"
		mkdir -p "$dir"
		[ -f "$dest" ] || ln -s $src $dest
	}; done
}

supivot() { # <new_root> <old_root>
	/bin/mount | grep "on $1 type" 2>&- 1>&- || /bin/mount -o bind $1 $1
	mkdir -p $1$2 $1/proc $1/sys $1/dev $1/tmp $1/overlay && \
	/bin/mount -o noatime,move /proc $1/proc && \
	pivot_root $1 $1$2 || {
		/bin/umount -l $1 $1
		return 1
	}

	/bin/mount -o noatime,move $2/sys /sys
	/bin/mount -o noatime,move $2/dev /dev
	/bin/mount -o noatime,move $2/tmp /tmp
	/bin/mount -o noatime,move $2/overlay /overlay 2>&-
	return 0
}

run_ramfs() { # <command> [...]
	install_bin /bin/busybox /bin/ash /bin/sh /bin/mount /bin/umount	\
		/sbin/pivot_root /usr/bin/wget /sbin/reboot /bin/sync /bin/dd	\
		/bin/grep /bin/cp /bin/mv /bin/tar /usr/bin/md5sum "/usr/bin/["	\
		/bin/dd /bin/vi /bin/ls /bin/cat /usr/bin/awk /usr/bin/hexdump	\
		/bin/sleep /bin/zcat /usr/bin/bzcat /usr/bin/printf /usr/bin/wc \
		/bin/cut /usr/bin/printf /bin/sync /bin/mkdir /bin/rmdir	\
		/bin/rm /usr/bin/basename /bin/kill /bin/chmod /usr/bin/awk /usr/sbin/set_sys_flag

	install_bin /sbin/mtd
	install_bin /sbin/ubi
	install_bin /sbin/mount_root
	install_bin /sbin/snapshot
	install_bin /sbin/snapshot_tool
	install_bin /usr/sbin/ubiupdatevol
	install_bin /usr/sbin/ubiattach
	install_bin /usr/sbin/ubiblock
	install_bin /usr/sbin/ubiformat
	install_bin /usr/sbin/ubidetach
	install_bin /usr/sbin/ubirsvol
	install_bin /usr/sbin/ubirmvol
	install_bin /usr/sbin/ubimkvol
	install_bin /usr/bin/tr
	install_bin /usr/mips/cgi-bin/sysupdate
	install_bin /usr/sbin/updatefw

	for file in $RAMFS_COPY_BIN; do
		install_bin ${file//:/ }
	done
	install_file /etc/resolv.conf /lib/*.sh /lib/functions/*.sh /lib/upgrade/*.sh $RAMFS_COPY_DATA

	[ -L "/lib64" ] && ln -s /lib $RAM_ROOT/lib64

	supivot $RAM_ROOT /mnt || {
		echo "Failed to switch over to ramfs. Please reboot."
		exit 1
	}

	/bin/mount -o remount,ro /mnt
	/bin/umount -l /mnt

	grep /overlay /proc/mounts > /dev/null && {
		/bin/mount -o noatime,remount,ro /overlay
		/bin/umount -l /overlay
	}

	# spawn a new shell from ramdisk to reduce the probability of cache issues
	exec /bin/busybox ash -c "$*"
}

kill_remaining() { # [ <signal> ]
	local sig="${1:-TERM}"
	echo -n "Sending $sig to remaining processes ... "

	local my_pid=$$
	local my_ppid=$(cut -d' ' -f4  /proc/$my_pid/stat)
	local my_ppisupgraded=
	grep -q upgraded /proc/$my_ppid/cmdline >/dev/null && {
		local my_ppisupgraded=1
	}
	
	local stat
	for stat in /proc/[0-9]*/stat; do
		[ -f "$stat" ] || continue

		local pid name state ppid rest
		read pid name state ppid rest < $stat
		name="${name#(}"; name="${name%)}"

		local cmdline
		read cmdline < /proc/$pid/cmdline

		# Skip kernel threads
		[ -n "$cmdline" ] || continue

		if [ $$ -eq 1 ] || [ $my_ppid -eq 1 ] && [ -n "$my_ppisupgraded" ]; then
			# Running as init process, kill everything except me
			if [ $pid -ne $$ ] && [ $pid -ne $my_ppid ]; then
				echo -n "$name "
				kill -$sig $pid 2>/dev/null
			fi
		else 
			case "$name" in
				# Skip essential services
				*procd*|*ash*|*init*|*watchdog*|*ssh*|*dropbear*|*telnet*|*login*|*hostapd*|*wpa_supplicant*|*nas*) : ;;

				# Killable process
				*)
					if [ $pid -ne $$ ] && [ $ppid -ne $$ ]; then
						echo -n "$name "
						kill -$sig $pid 2>/dev/null
					fi
				;;
			esac
		fi
	done
	echo ""
}

run_hooks() {
	local arg="$1"; shift
	for func in "$@"; do
		eval "$func $arg"
	done
}

ask_bool() {
	local default="$1"; shift;
	local answer="$default"

	[ "$INTERACTIVE" -eq 1 ] && {
		case "$default" in
			0) echo -n "$* (y/N): ";;
			*) echo -n "$* (Y/n): ";;
		esac
		read answer
		case "$answer" in
			y*) answer=1;;
			n*) answer=0;;
			*) answer="$default";;
		esac
	}
	[ "$answer" -gt 0 ]
}

v() {
	[ "$VERBOSE" -ge 1 ] && echo "$@"
}

rootfs_type() {
	/bin/mount | awk '($3 ~ /^\/$/) && ($5 !~ /rootfs/) { print $5 }'
}

get_image() { # <source> [ <command> ]
	local from="$1"
	local conc="$2"
	local cmd

	case "$from" in
		http://*|ftp://*) cmd="wget -O- -q";;
		*) cmd="cat";;
	esac
	if [ -z "$conc" ]; then
		local magic="$(eval $cmd $from 2>/dev/null | dd bs=2 count=1 2>/dev/null | hexdump -n 2 -e '1/1 "%02x"')"
		case "$magic" in
			1f8b) conc="zcat";;
			425a) conc="bzcat";;
		esac
	fi

	eval "$cmd $from 2>/dev/null ${conc:+| $conc}"
}

get_magic_word() {
	(get_image "$@" | dd bs=2 count=1 | hexdump -v -n 2 -e '1/1 "%02x"') 2>/dev/null
}

get_magic_long() {
	(get_image "$@" | dd bs=4 count=1 | hexdump -v -n 4 -e '1/1 "%02x"') 2>/dev/null
}

jffs2_copy_config() {
	if grep rootfs_data /proc/mtd >/dev/null; then
		# squashfs+jffs2
		mtd -e rootfs_data jffs2write "$CONF_TAR" rootfs_data
	else
		# jffs2
		mtd jffs2write "$CONF_TAR" rootfs
	fi
}

default_do_upgrade() {
	sync
	if [ "$SAVE_CONFIG" -eq 1 ]; then
		get_image "$1" | mtd $MTD_CONFIG_ARGS -j "$CONF_TAR" write - "${PART_NAME:-image}"
	else
		get_image "$1" | mtd write - "${PART_NAME:-image}"
	fi
}

do_upgrade() {
	v "Performing system upgrade..."
#	tr '\000' '\377' < /dev/zero | dd of=/dev/mmcblk0 bs=1024 seek=3072 count=54272
#	tr '\000' '\377' < /dev/zero | dd of=/tmp/FF53MB.bin count=108544
#	dd if=/tmp/FF53MB.bin of=/dev/mmcblk0 bs=1M seek=3
#	sync
	
	sys_flag=`hexdump -s 0x40000 -n 5 -C /dev/mmcblk0 | awk '{print $5}' | awk 'NR==1{print}'`
	boot_flag=`hexdump -s 0x40000 -n 5 -C /dev/mmcblk0 | awk '{print $4}' | awk 'NR==1{print}'`
	
	echo sys_flag=$sys_flag boot_flag=$boot_flag
	
	if [ $boot_flag = "04" ];then
		v "in backup system"
		if [ $sys_flag = "00" ]; then
			v "upgrade the first system..."
			dd if=/tmp/fwupgrade of=/dev/mmcblk0 bs=1M seek=3 count=5 conv=fsync
			[ $? -ne 0 ] && {
				v "Upgrade failed"
				exit
			}
			sync;sync;sync
			dd if=/tmp/fwupgrade of=/dev/mmcblk0 bs=1M skip=5 seek=13 count=48 conv=fsync
			[ $? -ne 0 ] && {
				v "Upgrade failed"
				exit
			}
			sync;sleep 1;sync;sleep 1;sync
		else
			v "upgrade the second system..."
			dd if=/tmp/fwupgrade of=/dev/mmcblk0 bs=1M seek=8 count=5 conv=fsync
			[ $? -ne 0 ] && {
				v "Upgrade failed"
				exit
			}
			sync;sync;sync
			dd if=/tmp/fwupgrade of=/dev/mmcblk0 bs=1M skip=5 seek=70 count=48 conv=fsync
			[ $? -ne 0 ] && {
				v "Upgrade failed"
				exit
			}
			sync;sleep 1;sync;sleep 1;sync
		fi		
	else
		v "in normal system"
		if [ $sys_flag = "00" ]; then
			v "upgrade the second system..."
			dd if=/tmp/fwupgrade of=/dev/mmcblk0 bs=1M seek=8 count=5 conv=fsync
			[ $? -ne 0 ] && {
				v "Upgrade failed"
				exit
			}
			sync;sync;sync
			dd if=/tmp/fwupgrade of=/dev/mmcblk0 bs=1M skip=5 seek=70 count=48 conv=fsync
			[ $? -ne 0 ] && {
				v "Upgrade failed"
				exit
			}
			sync;sleep 1;sync;sleep 1;sync
		else
			v "upgrade the first system..."
			dd if=/tmp/fwupgrade of=/dev/mmcblk0 bs=1M seek=3 count=5 conv=fsync
			[ $? -ne 0 ] && {
				v "Upgrade failed"
				exit
			}
			sync;sync;sync
			dd if=/tmp/fwupgrade of=/dev/mmcblk0 bs=1M skip=5 seek=13 count=48 conv=fsync
			[ $? -ne 0 ] && {
				v "Upgrade failed"
				exit
			}
			sync;sleep 1;sync;sleep 1;sync
		fi
	fi
	
	v "set system flag auto..."
	/tmp/set_sys_flag auto

	v "Upgrade completed"
	/tmp/uart_client_test -s 9
	reboot -f
	
	[ "$1" = "reboot" ] && {
		v "Rebooting system..."
		reboot -f
		sleep 5
		echo b 2>/dev/null >/proc/sysrq-trigger
	}
}
