# test OpenPBS
test server of OpenPBS

** please do NOT use this container on public network **

user: testuser

password: passw0rd

privatekey: /home/testuser/.ssh/id_rsa

publickey: /home/testuser/.ssh/id_rsa.pub


publickey is already registered to authorized_keys


# OpenPBS

installed version
- v23.06.06 Release

you can use OpenPBS commands such as qsub qdel... after login to container as `testuser`

## How to use
### bootup

```
docker run --rm -d  --name CONTAINER_NAME --cap-add SYS_RESOURCE naoso5/test-openpbs
```
you can use any other container name for `CONTAINER_NAME`

## customize server
After container become healty state, you can use qmgr as follows.

```
docker exec CONTAINER_NAME /opt/pbs/bin/qmgr -c "set server job_history_enable=True"
```

container state can be checked by `docker ps`. you can find following result

```
CONTAINER ID   IMAGE                 COMMAND        CREATED          STATUS                    PORTS     NAMES
40db725c4ee3   naoso5/test-openpbs   "/sbin/init"   17 seconds ago   Up 17 seconds (healthy)   22/tcp    CONTAINER_NAME
```

If STATUS colum have `starting` instead of `healthy`, OpenPBS is in the process of boot up.
So, wait until it change to healthy and then issue qmgr or any other commands of OpenPBS.


## limitations
- `pbs_iff` does not work (segmentaion fault)
- `backtrace`, `backtrace_symbols`, and `backtrace_symbols_fd` are not supported on alpine linux (and any other musl system). we add stub for this functions. It may cause some trouble.
- `ruserok` and `rcmd` are also not supported on alpine linux. It may also cause some trouble.
- we add 5sec sleep just after PBS startup script boot up postgresql to avoid create db too fast.
  In some environments, 5 seconds may not be enough.
  If you find postgresql can not start, increase the time on line 528 of  `/opt/pbs/libexec/pbs_db_utility`


## see also
please check base container's document

https://hub.docker.com/_/alpine
https://hub.docker.com/repository/docker/naoso5/test-sshd/general
