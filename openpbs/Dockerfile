FROM alpine:3.20.3 AS builder

# get OpenPBS and munge source
WORKDIR /root
USER root
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
RUN echo "#define _PATH_FSTAB \"/etc/fstab\"" > /usr/include/fstab.h
RUN echo "struct fstab { char *fs_spec; char *fs_file; char *fs_vfstype; char *fs_mntops; char *fs_type; int fs_freq; int fs_passno; };" >> /usr/include/fstab.h
RUN echo "struct fstab *getfsent(void) { return 0; }" >> /usr/include/fstab.h
RUN echo "struct fstab *getfsspec(const char *p) { return 0; }" >> /usr/include/fstab.h
RUN echo "struct fstab *getfsfile(const char *p) { return 0; }" >> /usr/include/fstab.h
RUN echo "int setfsent(void) { return 1; }" >> /usr/include/fstab.h
RUN echo "void endfsent(void) {}" >> /usr/include/fstab.h

COPY execinfo_dummy.h /usr/include/execinfo.h
COPY dummy_functions.h src/include/dummy_functions.h
COPY dummy_functions.c ./

# quick and dirty hack for alpine linux
RUN sed -i s/eval.h/Python.h/ src/lib/Libpython/pbs_python_external.c &&\
    sed -i 's/pbs_server_bin_LDADD =/pbs_server_bin_LDADD = $(top_builddir)\/dummy_functions.o/' src/server/Makefile.am &&\
    sed -i 's/pbs_mom_LDADD =/pbs_mom_LDADD = $(top_builddir)\/dummy_functions.o/' src/resmom/Makefile.am &&\
    sed -i 's/pbs_mom_bin_LDADD =/pbs_mom_bin_LDADD = $(top_builddir)\/dummy_functions.o/' src/resmom/Makefile.am &&\
    sed -i 's/pbs_rcp_LDADD = @socket_lib@/pbs_rcp_LDADD = @socket_lib@ $(top_builddir)\/dummy_functions.o/' src/mom_rcp/Makefile.am &&\
    sed -i -e 's/pgsql_share_dir="\/usr\/pgsql-${psql_ver}/pgsql_share_dir="\/usr\/share\/postgresql16/' \
           -e 's/pgsql_lib_dir="\/usr\/pgsql-${psql_ver}/pgsql_lib_dir="\/usr\/lib\/postgresql16/' \
           -e 's/pgsql_bin_dir="\/usr\/pgsql-${psql_ver}/pgsql_bin_dir="\/usr\/libexec\/postgresql16/' \
           -e 's/pgsql_share_dir="\/usr\/share\/postgresql\/share"/pgsql_share_dir="\/usr\/share\/postgresql16"/' \
           -e 's/pgsql_lib_dir="\/usr\/lib\/postgresql16\/lib"/pgsql_lib_dir="\/usr\/lib\/postgresql16"/' \
           -e '/pg_resetxlog/c    cp "\/usr\/libexec\/postgresql16\/pg_resetwal" "$PBS_HOME\/pgsql.forupgrade\/bin\/pg_resetxlog"'\
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
USER root

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

# make pbs and munge commands available for non-interactive ssh
RUN ln -s /opt/pbs/bin/* /usr/bin/ && \
    ln -s /opt/pbs/sbin/* /usr/sbin/ && \
    ln -s /opt/munge/bin/* /usr/bin/ && \
    ln -s /opt/munge/sbin/* /usr/sbin/

#setup PBS
RUN /opt/pbs/libexec/pbs_postinstall
COPY pbs_init /etc/init.d/pbs
COPY pbs.sh /etc/profile.d/pbs.sh

RUN rc-update add pbs

HEALTHCHECK --interval=10s --start-period=5s --retries=5 CMD pgrep pbs_server &&\
                                                          pgrep pbs_comm &&\
                                                          pgrep pbs_mom &&\
                                                          pgrep pbs_sched &&\
                                                          pgrep pbs_ds_monitor &&\
                                                          pgrep munged &&\
                                                          pg_isready -U postgres

USER root
WORKDIR /home/testuser
ENTRYPOINT ["/sbin/init"]
