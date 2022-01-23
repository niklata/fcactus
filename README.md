# fcactus

Copyright 2015-2017 Nicholas J. Kain.

License: Two-clause BSD.

## Introduction

fcactus is a counterpart to ncron.  It invokes processes in response to
changes to directories or files on local (no sshfs/cifs), non-pseudo
filesystems (no procfs/devfs/sysfs).  It is dependent on the inotify
API that is specific to Linux and is not portable to other systems.

fcactus is not part of ncron itself because it's fundamentally different
for three main technical reasons.  First, ncron uses mostly portable
facilities for its core functionality.  Second, ncron doesn't spend its
time waiting on file descriptors; it waits for time intervals to complete.
Lastly, ncron must keep track of the time when events were last performed,
and fcactus has the luxury of being stateless.

The configuration file format of fcactus is of a very similar flavor to
ncron and is mostly different because it is not necessary to assign unique
numerical identifiers to jobs for mapping them to run history state.

fcactus is intended to run from a process supervisor like runit or s6.
I don't provide any inbuilt facility for forking into the background,
writing pidfiles, logging to syslog, or anything like that.  ncron
supports these things because it's older and I don't like removing
features.

## Motivation

Since ndhc by design does not run scripts, another program must perform
any actions that are necessary when the DHCP-assigned IP address changes.

fcactus is intended to perform this task by watching for fmodify events
on the `/etc/ndhc/LEASE-interface` file.  `debounce_rise` should also be
used with a reasonable value (around `2000`-`4000`ms).

Since there's no reason other than laziness to write a specific tool
for a task when a general tool could also be written to perform the task
with a little bit more effort, I have written a general purpose tool.

## Requirements

* Linux kernel
* GCC or Clang
* CMake
* [Ragel](https://www.colm.net/open-source/ragel)

## Installation

```
$ mkdir build
$ cd build
$ cmake ..
$ make
$ su
$ cp fcactus /usr/local/bin
$ chmod 755 /usr/local/bin/fcactus
```

## Command Options
```
LONG OPTION     SHORT OPTION        ACTION
--config FILE   -c FILE             Loads the specified configure file
                                    the default is '/etc/fcactus.actions'
--help          -h                  Prints command line argument help
--version       -v                  Prints the program copyright and version
```

## Configuration

The canonical reference here is to read cfg.rl and look at the Ragel
state machine that defines the parser (search for `machine factions`).

The configuration file is essentially a list of files or directories to
watch for certain changes, and corresponding programs to execute when
these changes happen.  Keywords are one-per-line, of the form `keyword`
or `keyword = value`, and the end of a job is signified by placing a
`;;` marker on a line after all of the keywords that describe the job.

A few keywords are required for a job to be valid; if these keywords are
not all present, then the malformed job will be ignored.  These keywords
are:

`watch = FILE`

Defines the full pathname to the file or dir that will be watched for
inotify events.

Note that a particular file or directory can only be watched one time
by a single fcactus process; this is a limitation of inotify; so if you
need to run two or more programs in reaction to an event, have fcactus
run a script that does what you need instead.

`command = FILE ARGS...`

Defines the program that will be invoked as a subprocess when one of
the specified events is raised for the watched file or directory.

One extra bit of syntax that is not evident in the Ragel parser is the
substitution syntax that is shared with incron for command arguments
(judging from the man page; the details may differ as I've not used
incron or read its source code):

|  SYMBOL         |  IS REPLACED WITH
|-----------------|----------------------------------------------------------
|  `$#`           | if the watched filepath is a directory and the event was triggered by an event on a file in that directory, then this symbol will be replaced with the name of that file; otherwise it will be empty
|  `$@`           | the path to the watched file or directory that raised the event
|  `$%`           | the symbolic names of the inotify flags set in the event, delimited by commas
|  `$&`           | the numerical value of the inotify flags set in the event
|  `$$`           | `$`

Additionally, at least one of the following event flag keywords must be
present; after all, a job that watches no events is equivalent to no job.
These flags directly correspond to inotify flags; I suggest looking at
`man 7 inotify` for details.

|  EVENT KEYWORD   | CORRESPONDING INOTIFY FLAG
|------------------|---------------------------
|  `faccess`       | `IN_ACCESS`
|  `fattrib`       | `IN_ATTRIB`
|  `fclosewrite`   | `IN_CLOSE_WRITE`
|  `fclosenowrite` | `IN_CLOSE_NOWRITE`
|  `fclose`        | `IN_CLOSE`
|  `fcreate`       | `IN_CREATE`
|  `fdelete`       | `IN_DELETE`
|  `fdeleteself`   | `IN_DELETE_SELF`
|  `fmodify`       | `IN_MODIFY`
|  `fmoveself`     | `IN_MOVE_SELF`
|  `fmovefrom`     | `IN_MOVED_FROM`
|  `fmoveto`       | `IN_MOVED_TO`
|  `fopen`         | `IN_OPEN`
|  `fmove`         | `IN_MOVE`

There are optional keywords that allow the process properties of the
job to be limited.

`chroot = DIRECTORY_PATH`

Restricts the subprocess of the job to the directory at `DIRECTORY_PATH`.

`user = USER_NAME_OR_UID`

Runs the subprocess as the specified user.

`group = GROUP_NAME_OR_GID`

Runs the subprocess as the specified group.

`path = PATH_LIST`

Sets the `PATH` variable in environment of the spawned process to
`PATH_LIST`.  The format of `PATH_LIST` should be identical of that used
by shells (eg, `path1:path2:path3`).

`debounce_rise = TIME_IN_MILLISECONDS`

Instructs fcactus to wait until at least `debounce_rise` milliseconds have
passed after the event before invoking the job.  If another event comes
in before `debounce_rise` milliseconds have passed, the time to wait will
be updated.  This means that the event could possibly be deferred forever.

`debounce_fall = TIME_IN_MILLISECONDS`

Instructs fcactus that after running the job, all events for the same
job should be ignored until `debounce_fall` milliseconds have passed.
This means that if the events keep firing with an arbitrarily short
interval, jobs will be run every `debounce_fall` milliseconds; they will
not be deferred forever.

The following keywords all enforce rlimits and have the same syntax;
each of these keywords should be of the form `keyword = NUMBER0` or
`keyword = NUMBER0, NUMBER1`.  `NUMBER0` represents the soft limit of
the corresponding resource, and `NUMBER1` represents the hard limit; if
`NUMBER0` is not present, then `NUMBER1` will implicitly be the same as
`NUMBER0`.

```
 lim_cpu, lim_fsize, lim_data, lim_stack,
 lim_core, lim_rss, lim_nproc, lim_nofile, lim_memlock, lim_as,
 lim_msgqueue, lim_nice, lim_rttime, lim_rtprio, lim_sigpending
```
For more information on what these keywords do, I suggest reading `man
2 setrlimit` and looking at the corresponding `RLIMIT_*` defines.

The keywords for a job MUST be followed by a line containing `;;` as
the only content of the line.  This symbol is the job terminator and
must be present even if there is only a single job.

## Signals

fcactus has no special signal behavior; it will terminate itself
if it is sent `SIGHUP`, `SIGTERM`, or `SIGINT`.  It has no facility
for loading a new configure file; if you wish to do so, simply
restart the fcactus process.

## Randomness

fcactus does not consume or emit entropy, except that implicitly consumed by
its dependent libraries (such as the libc or kernel ELF loader).

## Run as non-root

fcactus, like ncron, can be run as a non-root user.  Without root, however,
several of the keywords (such as user, group, or chroot) will not properly
work and will cause the job to fail if present.  Additionally, rlimit
keywords will be of limited use, as they can only further restrict whatever
limits the user is already subject to.

Other than these restrictions, fcactus should function normally, subject
to the permission restrictions of the user.

## Downloads

* [GitLab](https://gitlab.com/niklata/fcactus)
* [BitBucket](https://bitbucket.com/niklata/fcactus)
* [GitHub](https://github.com/niklata/fcactus)

