# Logging into os-class server:

ssh flip.engr.oregonstate.edu
ssh os-class.engr.oregonstate.edu

# Creating team directory:

cd /
cd scratch
cd spring2017
cd 10-01
ls
cd ..
mkdir 12-06
cd 12-06

# Cloning yocto repository and checking out 3.14 tag:

git clone git://git.yoctoproject.org/linux-yocto-3.14
cd linux-yocto-3.14
git checkout -b v3.14.26

# Sourcing proper profile:

source /scratch/opt/environment-setup-i586-poky-linux

# Copying default config:

cp /scratch/spring2017/files/config-3.14.26-yocto-qemu .config

# menuconfig, changing the version string to our team:

make menuconfig

# Building the kernel:

make -j4 all

# Running gdb in terminal 2:

source /scratch/opt/environment-setup-i586-poky-linux
cd /scratch/spring2017/12-06
gdb

# Copying sample bzImage and filesystem:

cp /scratch/spring2017/files/bzImage-qemux86.bin .
cp /scratch/spring2017/files/core-image-lsb-sdk-qemux86.ext3 .

# Running sample kernel build:

qemu-system-i386 -gdb tcp::5726 -S -nographic -kernel bzImage-qemux86.bin -drive file=core-image-lsb-sdk-qemux86.ext3,if=virtio -enable-kvm -net none -usb -localtime --no-reboot --append "root=/dev/vda rw console=ttyS0 debug"

# Running our kernel build:

qemu-system-i386 -gdb tcp::5726 -S -nographic -kernel linux-yocto-3.14/arch/x86/boot/bzImage -drive file=core-image-lsb-sdk-qemux86.ext3,if=virtio -enable-kvm -net none -usb -localtime --no-reboot --append "root=/dev/vda rw console=ttyS0 debug"

# Logging out:

exit
