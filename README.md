# AutoBackup for Linux
Automatically backs up files that are deleted, modified, rename and moved by applications. 
This is done transparently by system call api redirection.
A dynamic loaded shared library is automatically attached to the executed application and redirect openxxx, renamexxx, unlinkxxx
calls and when required. The shared library is loaded via LD_PRELOAD mechanism
Files are backed up under /tmp/back

TO BE DONE:
-Testing, especially multiple processes access the same files. For the moment we use O_EXCL when, creating a to be backed up, file.
I hope it should do the trick. This also has the advantage that original version of the file is backed up and not the successive 
potentials modifications
-Exclude file list: files or directories that do not need to be backed up
-Clean the code
-Configuration: backup location,...
-User defined heuristics for when and how to backup files

example:
LD_PRELOAD=/tmp/autobackup.so yum erase strace

results in the following backup tree:
/tmp/back/
/tmp/back/usr
/tmp/back/usr/bin
/tmp/back/usr/bin/strace
/tmp/back/usr/bin/strace-log-merge
/tmp/back/usr/share
/tmp/back/usr/share/doc
/tmp/back/usr/share/doc/strace
/tmp/back/usr/share/doc/strace/COPYING
/tmp/back/usr/share/doc/strace/CREDITS
/tmp/back/usr/share/doc/strace/ChangeLog
/tmp/back/usr/share/doc/strace/ChangeLog-CVS
/tmp/back/usr/share/doc/strace/NEWS
/tmp/back/usr/share/doc/strace/README
/tmp/back/usr/share/man
/tmp/back/usr/share/man/man1
/tmp/back/usr/share/man/man1/strace.1.gz
/tmp/back/tmp
/tmp/back/tmp/tmpxU8tCZ
/tmp/back/tmp/yum_save_tx.2015-01-22.22-29.nBtkbN.yumtx
/tmp/back/tmp/tmpzSpw5R
/tmp/back/tmp/DzWKA8
/tmp/back/run
/tmp/back/run/yum.pid
/tmp/back/var
/tmp/back/var/lib
/tmp/back/var/lib/yum
/tmp/back/var/lib/yum/yumdb
/tmp/back/var/lib/yum/yumdb/s
/tmp/back/var/lib/yum/yumdb/s/573e6386682aaf9dc0d05de4efa58ba23b8b08a6-strace-4.9-3.fc21-x86_64
/tmp/back/var/lib/yum/yumdb/s/573e6386682aaf9dc0d05de4efa58ba23b8b08a6-strace-4.9-3.fc21-x86_64/checksum_data
/tmp/back/var/lib/yum/yumdb/s/573e6386682aaf9dc0d05de4efa58ba23b8b08a6-strace-4.9-3.fc21-x86_64/from_repo_revision
/tmp/back/var/lib/yum/yumdb/s/573e6386682aaf9dc0d05de4efa58ba23b8b08a6-strace-4.9-3.fc21-x86_64/reason
/tmp/back/var/lib/yum/yumdb/s/573e6386682aaf9dc0d05de4efa58ba23b8b08a6-strace-4.9-3.fc21-x86_64/from_repo_timestamp
/tmp/back/var/lib/yum/yumdb/s/573e6386682aaf9dc0d05de4efa58ba23b8b08a6-strace-4.9-3.fc21-x86_64/from_repo
/tmp/back/var/lib/yum/yumdb/s/573e6386682aaf9dc0d05de4efa58ba23b8b08a6-strace-4.9-3.fc21-x86_64/installed_by
/tmp/back/var/lib/yum/yumdb/s/573e6386682aaf9dc0d05de4efa58ba23b8b08a6-strace-4.9-3.fc21-x86_64/command_line
/tmp/back/var/lib/yum/yumdb/s/573e6386682aaf9dc0d05de4efa58ba23b8b08a6-strace-4.9-3.fc21-x86_64/origin_url
/tmp/back/var/lib/yum/yumdb/s/573e6386682aaf9dc0d05de4efa58ba23b8b08a6-strace-4.9-3.fc21-x86_64/releasever
/tmp/back/var/lib/yum/yumdb/s/573e6386682aaf9dc0d05de4efa58ba23b8b08a6-strace-4.9-3.fc21-x86_64/checksum_type
/tmp/back/var/lib/yum/transaction-done.2015-01-22.22:29.17
/tmp/back/var/lib/yum/transaction-all.2015-01-22.22:29.17
/tmp/back/var/lib/yum/rpmdb-indexes
/tmp/back/var/lib/yum/rpmdb-indexes/version
/tmp/back/var/lib/yum/rpmdb-indexes/version.tmp
/tmp/back/var/lib/yum/rpmdb-indexes/pkgtups-checksums
/tmp/back/var/lib/yum/rpmdb-indexes/pkgtups-checksums.tmp
/tmp/back/var/lib/yum/rpmdb-indexes/file-requires
/tmp/back/var/lib/yum/rpmdb-indexes/file-requires.tmp
/tmp/back/var/lib/yum/rpmdb-indexes/obsoletes
/tmp/back/var/lib/yum/rpmdb-indexes/obsoletes.tmp
/tmp/back/var/lib/rpm
/tmp/back/var/lib/rpm/Sha1header
/tmp/back/var/lib/rpm/Sigmd5
/tmp/back/var/lib/rpm/Installtid
/tmp/back/var/lib/rpm/Dirnames
/tmp/back/var/lib/rpm/Triggername
/tmp/back/var/lib/rpm/Obsoletename
/tmp/back/var/lib/rpm/Conflictname
/tmp/back/var/lib/rpm/Providename
/tmp/back/var/lib/rpm/Requirename
/tmp/back/var/lib/rpm/Group
/tmp/back/var/lib/rpm/Basenames
/tmp/back/var/lib/rpm/Name
/tmp/back/var/lib/rpm/Packages
/tmp/back/var/lib/rpm/.rpm.lock
/tmp/back/var/lib/rpm/__db.003
/tmp/back/var/lib/rpm/__db.002
/tmp/back/var/lib/rpm/__db.001
/tmp/back/var/lib/rpm/.dbenv.lock

