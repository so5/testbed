FROM naoso5/test-sshd AS builder

# get OpenPBS and munge source
WORKDIR /root
RUN wget -O openpbs.tgz https://github.com/openpbs/openpbs/archive/refs/tags/v23.06.06.tar.gz &&\
    mkdir src &&\
    tar xfz openpbs.tgz -C src --strip-components 1
RUN wget -O munge.tar.xz https://github.com/dun/munge/releases/download/munge-0.5.16/munge-0.5.16.tar.xz &&\
    mkdir munge &&\
    tar xfJ munge.tar.xz -C munge --strip-components 1

# install dependend packages
RUN apk add\
      gcc\
      g++\
      make\
      autoconf\
      automake\
      libtool\
      hwloc-dev\
      libx11-dev\
      libxt-dev\
      libedit-dev\
      libical-dev\
      ncurses-dev\
      perl\
      postgresql16\
      postgresql16-contrib\
      postgresql16-openrc\
      python3-dev\
      tcl-dev\
      tk-dev\
      swig\
      expat-dev\
      openssl-dev\
      libxext-dev\
      libxft-dev\
      libpq-dev\
      cjson-dev\
      sudo

# build munge from source
WORKDIR /root/munge
RUN ./configure --prefix=/opt/munge --sysconfdir=/etc  --localstatedir=/var --runstatedir=/run &&\
    make &&\
    make install

# build OpenPBS from source
WORKDIR /root/src
COPY execinfo_dummy.h /usr/include/execinfo.h
COPY dummy_functions.h src/include/dummy_functions.h
COPY dummy_functions.c ./

# quick and dirty hack for alpine linux
RUN sed -i /fstab.h/d configure.ac &&\
    sed -i s/eval.h/Python.h/ src/lib/Libpython/pbs_python_external.c &&\
    sed -i 's/pbs_server_bin_LDADD =/pbs_server_bin_LDADD = $(top_builddir)\/dummy_functions.o/' src/server/Makefile.am &&\
    sed -i 's/pbs_rcp_LDADD = @socket_lib@/pbs_rcp_LDADD = @socket_lib@ $(top_builddir)\/dummy_functions.o/' src/mom_rcp/Makefile.am &&\
    sed -i -e 's/pgsql_share_dir="\/usr\/pgsql-${psql_ver}/pgsql_share_dir="\/usr\/share\/postgresql/' \
           -e 's/pgsql_lib_dir="\/usr\/pgsql-${psql_ver}/pgsql_lib_dir="\/usr\/lib\/postgresql16/' \
           -e '/pg_resetxlog/c    cp "$PGSQL_BIN\/pg_resetwal" "$PBS_HOME\/pgsql.forupgrade\/bin\/pg_resetxlog"'\
    src/cmds/scripts/pbs_habitat.in &&\
    sed -i -e '528 c          sleep 5' src/lib/Libdb/pgsql/pbs_db_utility

# actual build process start from here
RUN ./autogen.sh &&\
    CPPFLAGS="-I/usr/include/python-3.12 -DPTHREAD_MUTEX_RECURSIVE_NP=PTHREAD_MUTEX_RECURSIVE" ./configure --prefix=/opt/pbs &&\
    make &&\
    make install


FROM naoso5/test-sshd AS release
EXPOSE 22
VOLUME ["/sys/fs/cgroup"]

COPY --from=builder /opt /opt

RUN apk add\
      hwloc\
      libx11\
      libxt\
      libedit\
      libical\
      ncurses\
      perl\
      postgresql16\
      postgresql16-contrib\
      postgresql16-openrc\
      python3\
      tcl\
      tk\
      swig\
      expat\
      openssl\
      libxext\
      libxft\
      libpq\
      cjson\
      sudo

# set up munge
RUN adduser munge -D -s /sbin/nologin
RUN mkdir -p /etc/munge /var/lib/munge /var/log/munge /run/munge
RUN chown munge:munge /etc/munge /var/lib/munge /var/log/munge /run/munge
RUN chmod 0700  /etc/munge /var/lib/munge /var/log/munge
RUN chmod 0755 /run/munge
RUN sudo -u munge /opt/munge/sbin/mungekey --verbose
COPY munge /etc/init.d/munge
RUN ln -s /opt/munge/lib/libmunge.so /usr/lib/
RUN rc-update add munge

#setup PBS
RUN /opt/pbs/libexec/pbs_postinstall
COPY pbs_init /etc/init.d/pbs

RUN rc-update add pbs

HEALTHCHECK --interval=10s --start-period=5s --retries=5 CMD pgrep pbs_server &&\
                                                          pgrep pbs_comm &&\
                                                          pgrep pbs_mom &&\
                                                          pgrep pbs_sched &&\
                                                          pgrep pbs_ds_monitor &&\
                                                          pgrep munged &&\
                                                          pg_isready -U postgres

USER testuser
RUN echo ". /opt/pbs/etc/pbs.sh" > /home/testuser/profile

USER root
ENTRYPOINT ["/sbin/init"]
