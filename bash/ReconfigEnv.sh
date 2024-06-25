#!/bin/sh
set -e
#python
apt-get update
apt install python3.8
export PATH=/usr/local/neuware/bin:/usr/local/go/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
echo "export PATH=/usr/local/neuware/bin:/usr/local/go/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin" >> ~/.bashrc
update-alternatives --install /usr/local/bin/python python /usr/bin/python3 10
update-alternatives --install /usr/local/bin/python3 python3 /usr/bin/python3 10
#gdb
bash -c "$(curl -fsSL https://gef.blah.cat/sh)"
# vim
cd /Workspace/vim/src
make && make install
rm /usr/bin/vi && ln -s /usr/local/bin/vim /usr/bin/vi
cd -
# ctags
cd /Workspace/ctags
bash ./autogen.sh && bash ./configure
make && make install
cd -
# vimconfig
cd /Workspace/vimconfig
bash ./install.sh
cd -
