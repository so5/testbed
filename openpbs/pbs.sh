export PATH=/opt/pbs/bin:/opt/pbs/sbin:/opt/munge/bin:$PATH

if [ "$PWD" = "/" ]; then
  cd $HOME
fi
