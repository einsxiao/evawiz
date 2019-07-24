#!/bin/sh
if ! [ `id -u` == '0' ]; then
    echo "You should run this script as root.";
    exit 1;
fi
    
if ! [ -d /opt/evawiz ];then
  useradd -d /opt/evawiz evawiz
  chmod 755 /opt/evawiz
  echo 'source /opt/evawiz/evawiz.source' >> /opt/evawiz/.bashrc
  su -c 'mkdir /opt/evawiz/evawiz' evawiz
  su -c 'mkdir /opt/evawiz/evacc' evawiz
  su -c 'mkdir /opt/evawiz/programs' evawiz
fi

## install packages
updatedb
yum -y install vim-enhanced
yum -y install xorg-x11-xauth
yum -y install wget
yum -y install rsync
yum -y install crypto-utils
yum -y install libtiff-devel
yum -y install libtiff
yum -y install libtiff-tools
yum -y install mlocate
yum -y install gcc
yum -y install compat-libtiff3
yum -y install libpng12-devel
yum -y install libXpm-devel
yum -y install libXaw-devel
yum -y install libXrender-devel
yum -y install libXft
yum -y install libXft-devel
yum -y install libXinerama-devel
yum -y install dejavu-*
yum -y remove libpng12-devel
yum -y install gtk2
yum -y install gtk2-devel
yum -y install net-tools
yum -y install kernel-headers
yum -y install emacs
yum -y install x11vnc
yum -y install libudev-devel 
yum -y install libpam-devel



export RSYNC_PASSWORD=evawiz_for_update
server=evawiz@115.154.191.100:9573
sdir_nodel(){
    echo -e "\n\nupdate $1"
    rsync -avz rsync://$server/evawiz/$1/ /opt/evawiz/$1/
}
sdir(){
    echo -e "\n\nupdate $1"
    rsync -avz --delete-after rsync://$server/evawiz/$1/ /opt/evawiz/$1/
}
sfile(){
    echo -e "\n\nupdate $1"
    rsync -avz --delete-after rsync://$server/evawiz/$1 /opt/evawiz/$2
}
sfile evawiz.source.local.ori evawiz.source.local.ori
sfile evawiz.source evawiz.source
sdir evawiz
sdir bin
sdir etc
sdir_nodel python
sdir_nodel python3
sdir evawizd
sdir gdb
sdir gcc
sdir cuda-7.0
sdir openmpi
sdir evacc
sdir base
sdir emacs
sdir wxWidgets
sdir codeblocks

ln -sf /opt/evawiz/bin/config-eva /usr/bin/config-eva
ln -sf /opt/evawiz/bin/update-eva /usr/bin/update-eva
ln -sf /opt/evawiz/bin/eva /usr/bin/eva
ln -sf /opt/evawiz/bin/update-eva-no-output /etc/cron.hourly/update-eva

if ! [ -f /opt/evawiz/evawiz.source.local ]; then
  mv /opt/evawiz/evawiz.source.local.ori /opt/evawiz/evawiz.source.local
fi

version=`uname -r |awk -F'.' '{print $1"."$2}'`
if [ $version == '3.10' ]; then
  ln -sf /opt/evawiz/etc/evawiz/evawizd.service.centos7 /lib/systemd/system/evawizd.service
  systemctl enable evawizd.service
elif [ $version == '2.6' ];then
  ln -sf /opt/evawiz/etc/evawiz/evawizd.service.centos6 /etc/init.d/evawizd
  chkconfig --add evawizd
  chkconfig evawizd on
else
    echo "Linux distribution is not supported yet."
    exit 1;
fi


exit 0
