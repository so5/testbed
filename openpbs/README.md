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

For OpenPBS and OpenRC to manage services and track job processes correctly, you **must** use `--privileged`, `--cap-add SYS_RESOURCE`, and mount the cgroup volume. This path is universal across Mac, Windows (WSL2), and Linux.

```bash
docker run --rm -d \
  --name CONTAINER_NAME \
  --privileged \
  --cap-add SYS_RESOURCE \
  -v /sys/fs/cgroup:/sys/fs/cgroup:ro \
  naoso5/test-openpbs
```

### Docker Compose
Alternatively, use the following `compose.yml`:

```yaml
services:
  openpbs:
    image: naoso5/test-openpbs
    container_name: test-pbs
    privileged: true
    cap_add:
      - SYS_RESOURCE
    volumes:
      - /sys/fs/cgroup:/sys/fs/cgroup:ro
    ports:
      - "2222:22"
```
Run with `docker compose up -d`.

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
