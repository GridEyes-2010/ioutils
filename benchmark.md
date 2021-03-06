# Test environments #

* CPU: Core I7 2200 MHz
* Memory: 16 GB
* Hard drive: Fast SSD drive.
* OS: Darwin Kernel Version 18.2.0
* fd 7.3.0
* find (GNU findutils) 4.6.0
* fast-find (master)

Notes:
* All automated performance benchmark results are collected on a SSD drive with a warm up cache.
* The regular expression is selected using my normal usage pattern so it is definitely biased.
* Both GNU find and fast-find are single thread so the CPU clock does not jump during the test, however, **fd does push all CPU cores to their limits**.
* Our benchmark use the latest version of GNU find which is significantly faster than the system find command.
* We avoid to alter the default options of all tested commands if possible.

# Finding files and folders #

## Big folders ##

### Manual tests ###

#### Find all files ####

This performance benchmark will measure the performance of all commands by finding all valid files and folders in my Macbook Pro.

**GNU find**

``` shell
/usr/bin/time -l find / > 2.log
       57.03 real         2.47 user        35.21 sys
  14012416  maximum resident set size
         0  average shared memory size
         0  average unshared data size
         0  average unshared stack size
      3796  page reclaims
         1  page faults
         0  swaps
         0  block input operations
         0  block output operations
         0  messages sent
         0  messages received
         0  signals received
    175401  voluntary context switches
      8391  involuntary context switches
```

**fd**

``` shell
MacOS:ioutils hdang$ /usr/bin/time -l fd . -H --no-ignore / > 3.log
       19.08 real        12.35 user        62.09 sys
 291508224  maximum resident set size
         0  average shared memory size
         0  average unshared data size
         0  average unshared stack size
     73009  page reclaims
       244  page faults
         0  swaps
         0  block input operations
         0  block output operations
         0  messages sent
         0  messages received
         0  signals received
    166536  voluntary context switches
    770471  involuntary context switches
```

**fd with one thread**

``` shell
MacOS:ioutils hdang$ /usr/bin/time -l fd . -H --no-ignore / -j1 > 3.log
       36.66 real         8.42 user        28.17 sys
 180875264  maximum resident set size
         0  average shared memory size
         0  average unshared data size
         0  average unshared stack size
     45311  page reclaims
         0  page faults
         0  swaps
         0  block input operations
         0  block output operations
         0  messages sent
         0  messages received
         0  signals received
    107988  voluntary context switches
    227620  involuntary context switches
```

**fast-find**

``` shell
/usr/bin/time -l fast-find --donot-ignore-git / > 1.log
       24.96 real         1.62 user        13.39 sys
   1843200  maximum resident set size
         0  average shared memory size
         0  average unshared data size
         0  average unshared stack size
       426  page reclaims
        77  page faults
         0  swaps
         0  block input operations
         0  block output operations
         0  messages sent
         0  messages received
         0  signals received
     93079  voluntary context switches
      4006  involuntary context switches
```

**Analysis**

* Both GNU find and fast-find output more paths than fd.
* fd is the fastest command in this performance benchmark. It is 30% faster than fast-find, however, fd uses 4x more CPU resources and 150x more memory than that of fast-find.
* If we use fd with only one thread then it is 46% slower than fast-find and uses 100x more memory.
* GNU find is 2x slower than fast-find and it is also 3x slower than fd.

#### Find files using given regular expression ####

This benchmark will try to simulate a regular user workflow i.e match the whole path with the given regular expression instead of matching to the stem only. fast-find command does support this option by default, however, we do need to specify a **--full-path** option so fd can match the whole path. **Note that the gap between fast-find and other commands will be increased with the complexity of the input regular expression pattern.**

**fd**

``` shell
MacOS:commands hdang$ /usr/bin/time -lp fd --full-path "zstd/.*doc/README[.]md$" /
/Users/hdang/working/zstd/doc/README.md
/Users/hdang/working/contribs/zstd/doc/README.md
/Users/hdang/working/3p/src/zstd/doc/README.md
/Users/hdang/working/backup/projects/projects/others/coverage/3p/zstd/doc/README.md
real        19.89
user        16.80
sys         93.30
 226848768  maximum resident set size
         0  average shared memory size
         0  average unshared data size
         0  average unshared stack size
     55889  page reclaims
       308  page faults
         0  swaps
         0  block input operations
         0  block output operations
         0  messages sent
         0  messages received
         0  signals received
    169989  voluntary context switches
   1730498  involuntary context switches
```

**fast-find**
*Note that we need to use --ignore-error flag to suppress all error related to file/folder permissions*.

``` shell
MacOS:commands hdang$ /usr/bin/time -lp ./fast-find --ignore-error -e "zstd/.*doc/README[.]md$" /
/Users/hdang/working/backup/projects/projects/others/coverage/3p/zstd/doc/README.md
/Users/hdang/working/zstd/doc/README.md
/Users/hdang/working/contribs/zstd/doc/README.md
/Users/hdang/working/3p/src/zstd/doc/README.md
real        24.62
user         2.16
sys         13.46
   3973120  maximum resident set size
         0  average shared memory size
         0  average unshared data size
         0  average unshared stack size
      1005  page reclaims
         0  page faults
         0  swaps
         0  block input operations
         0  block output operations
         0  messages sent
         0  messages received
         0  signals received
     87541  voluntary context switches
      4743  involuntary context switches
```

**Analysis**

* The performance gap between fd and fast-find is decreased to about 25%, however, fd uses 6x times more system resources than that of fast-find. 

## Small and medium folders with 50K and 200K files and folders ##

### Correctness tests ###

**Results**

*A medium folder with about 200K of files and folders*

``` shell
MacOS:commands hdang$ ../unittests/verify.sh ../../3p/src
Find all files using GNU find
        2.18 real         0.48 user         1.54 sys
Find all files using fast-find
        1.26 real         0.16 user         1.09 sys
Find all files using fd
        1.63 real         1.48 user         8.48 sys
\n==== Verify the output of fast-find ====
1d0
< ../../3p/src
\n==== Verify the output of fd ====
1d0
< ../../3p/src
```

*A small folder with about 50K of files and folders*

``` shell
MacOS:commands hdang$ ../unittests/verify.sh ../../3p/src/boost
Find all files using GNU find
        0.63 real         0.16 user         0.46 sys
Find all files using fast-find
        0.47 real         0.06 user         0.40 sys
Find all files using fd
        0.79 real         0.51 user         3.03 sys
\n==== Verify the output of fast-find ====
1d0
< ../../3p/src/boost
\n==== Verify the output of fd ====
1d0
< ../../3p/src/boost
```

**Analysis**

* All three commands produce the same output. Note that the output of GNU find is off by one because it includes a given search path.
* The output of fast-find and fd are identical.
* Both fd and fast-find trim the slash at the end, however, GNU find does not.
* The benchmark results show that fast-find is the fastest command and it also uses less system resources than both find and fd.

### Manual tests ###

**GNU find**

``` shell
MacOS:benchmark hdang$ /usr/bin/time -lp gfind ../../3p/src/boost/ | wc
real         0.62
user         0.15
sys          0.45
   1904640  maximum resident set size
         0  average shared memory size
         0  average unshared data size
         0  average unshared stack size
       474  page reclaims
         0  page faults
         0  swaps
         0  block input operations
         0  block output operations
         0  messages sent
         0  messages received
         0  signals received
         2  voluntary context switches
        13  involuntary context switches
   69341   69343 4695375
```

**fd**

``` shell
MacOS:benchmark hdang$ /usr/bin/time -lp fd . ../../3p/src/boost/ -H --no-ignore | wc
real         0.43
user         0.44
sys          2.20
  10924032  maximum resident set size
         0  average shared memory size
         0  average unshared data size
         0  average unshared stack size
      2644  page reclaims
        35  page faults
         0  swaps
         0  block input operations
         0  block output operations
         0  messages sent
         0  messages received
         0  signals received
      4079  voluntary context switches
     30266  involuntary context switches
   59457   59460 3959539
```

**faast-find**

``` shell
MacOS:benchmark hdang$ /usr/bin/time -lp fast-find --donot-ignore-git ../../3p/src/boost/ | wc
real         0.38
user         0.05
sys          0.32
   1413120  maximum resident set size
         0  average shared memory size
         0  average unshared data size
         0  average unshared stack size
       356  page reclaims
         0  page faults
         0  swaps
         0  block input operations
         0  block output operations
         0  messages sent
         0  messages received
         0  signals received
         4  voluntary context switches
        44  involuntary context switches
   59457   59460 3959539
```

**Analysis**

* Both GNU find and fast-find use the similar amount of memory, however, fd uses 7x more memory than both GNU find and fast-find.
* fast-find is 60% faster than GNU find and 10% faster than fd in our manual tests.
* fast-find uses less system resources than GNU find i.e 3x.
* fd use 6x more system resource than that of fast-find.

### Search for all files ###

**Benchmark results**

``` shell
MacOS:benchmark hdang$ ./fast-find -g all
Celero
Timer resolution: 0.001000 us
-----------------------------------------------------------------------------------------------------------------------------------------------
     Group      |   Experiment    |   Prob. Space   |     Samples     |   Iterations    |    Baseline     |  us/Iteration   | Iterations/sec  |
-----------------------------------------------------------------------------------------------------------------------------------------------
all             | find            |               0 |              10 |               1 |         1.00000 |    509638.00000 |            1.96 |
all             | fd_noignore     |               0 |              10 |               1 |         1.10995 |    565671.00000 |            1.77 |
all             | fast_find       |               0 |              10 |               1 |         0.75489 |    384719.00000 |            2.60 |
all             | fast_find_bfs   |               0 |              10 |               1 |         0.77074 |    392799.00000 |            2.55 |
Complete.
```

**Analysis**

* If we do not ignore the **.git** folder then fast-find is about 30% faster than GNU find and 47% faster than fd.
* fast-find is faster than both commands because it does not use less system calls.

### Skip .git folder ###

**Benchmark results**

``` shell
MacOS:benchmark hdang$ ./fast-find -g ignore_git
Celero
Timer resolution: 0.001000 us
-----------------------------------------------------------------------------------------------------------------------------------------------
     Group      |   Experiment    |   Prob. Space   |     Samples     |   Iterations    |    Baseline     |  us/Iteration   | Iterations/sec  |
-----------------------------------------------------------------------------------------------------------------------------------------------
ignore_git      | find            |               0 |              10 |               1 |         1.00000 |    509479.00000 |            1.96 |
ignore_git      | fd              |               0 |              10 |               1 |         1.09373 |    557231.00000 |            1.79 |
ignore_git      | fast_find_defau |               0 |              10 |               1 |         0.50422 |    256892.00000 |            3.89 |
ignore_git      | fast_find_bfs   |               0 |              10 |               1 |         0.49767 |    253551.00000 |            3.94 |
Complete.
```

**Analysis**

* Our performance benchmark results has shown that GNU find is still faster than fd even though fd has ignored all git related files.
* fast-find is 2x faster than both GNU find and fd if it can ignore **.git** folder. Note that the performance gap between fast-find and fd might change if for folder with millions of files.

### Use regex to find files ###

**Benchmark results**

``` shell
MacOS:benchmark hdang$ ./fast-find -g regex
Celero
Timer resolution: 0.001000 us
-----------------------------------------------------------------------------------------------------------------------------------------------
     Group      |   Experiment    |   Prob. Space   |     Samples     |   Iterations    |    Baseline     |  us/Iteration   | Iterations/sec  |
-----------------------------------------------------------------------------------------------------------------------------------------------
regex           | find            |               0 |              10 |               1 |         1.00000 |    502403.00000 |            1.99 |
regex           | fd              |               0 |              10 |               1 |         2.82621 |   1419896.00000 |            0.70 |
regex           | fast_find       |               0 |              10 |               1 |         0.53596 |    269268.00000 |            3.71 |
regex           | fast_find_bfs   |               0 |              10 |               1 |         0.53349 |    268029.00000 |            3.73 |
Complete.
```

**Analysis**

* fd is about 3x slower than GNU find and 5x slower than fast-find in this benchmark.
* The performance gap between fast-find and fd will be dereased by the number of files and folders since fast-find is a single thread command.

# Locate files and folders #

## fast-updatedb vs GNU updatedb ##

**Runtime**

From the output of the time command we can easily see that fast-update is about 4x faster than the latest version of GNU updatedb i.e gupdatedb. The 4x improvement comes from below factors:
* fast-update uses fast-find and gupdate uses GNU find to build the database. Our previous becnhmark results have shown that fast-find is at least 1.5x faster than that of GNU find.
* The way fast-updatedb store file information database is optimized for the read/write performance.

**fast-updatedb**

``` shell
MacOS:benchmark hdang$ time fast-updatedb / -d .all
fast-find: '/.Spotlight-V100': Operation not permitted
...

real    0m24.638s
user    0m1.619s
sys     0m13.225s
```

**GNU updatedb**

``` shell
MacOS:benchmark hdang$ time gupdatedb --localpaths="/" --output=all.db
/usr/local/Cellar/findutils/4.6.0/bin/gfind: '/usr/sbin/authserver': Permission denied
/usr/local/Cellar/findutils/4.6.0/bin/gfind: '/.Spotlight-V100': Operation not permitted
...

real    1m32.091s
user    0m27.843s
sys     1m0.713s
```

**Correctness**

A simple analysis show that fast-updatedb store more paths than GNU updatedb and we can easily see that if fast-locate is 2.5x faster than GNU locate if we want to show all paths stored in the indexed databases.

``` shell
MacOS:benchmark hdang$ time glocate -d all.db . | wc
 1548086 2129057 154803280

real    0m1.479s
user    0m1.973s
sys     0m0.102s
MacOS:benchmark hdang$ time fast-locate -d .all | wc
 1725875 2399258 168627045

real    0m0.593s
user    0m0.558s
sys     0m0.117s
```

Now let pipe outputs to files and figure out the differences bettween these two databases.

``` shell
glocate -d all.db . | sort -s > 2.log
fast-locate -d .all | sort -s > 1.log
MacOS:benchmark hdang$ diff 1.log 2.log | grep '<' | wc
  177831  448078 14183329
MacOS:benchmark hdang$ diff 1.log 2.log | grep '>' | wc
      42      88    3986
```

Below are the observations:
* GNU updatedb does not index some files without user's permission. It does ask for me for my permission during the process of generating test data.
* fast-updatedb indexes all files and folders **without asking for user permission**.
* fast-updatedb have about 177K paths more than that of GNU updatedb.

## fast-locate vs GNU locate ##

### Manual benchmark ###

``` shell
MacOS:benchmark hdang$ /usr/bin/time -lp fast-locate -d .database_big 'zstd/.*doc/README[.]md$'
/Users/hdang/working/backup/projects/projects/others/coverage/3p/zstd/doc/README.md
/Users/hdang/working/zstd/doc/README.md
/Users/hdang/working/contribs/zstd/doc/README.md
/Users/hdang/working/3p/src/zstd/doc/README.md
real         0.16
user         0.12
sys          0.03
   3526656  maximum resident set size
         0  average shared memory size
         0  average unshared data size
         0  average unshared stack size
       872  page reclaims
         0  page faults
         0  swaps
         0  block input operations
         0  block output operations
         0  messages sent
         0  messages received
         0  signals received
         2  voluntary context switches
         7  involuntary context switches
MacOS:benchmark hdang$ /usr/bin/time -lp glocate -d locate_db_big --regex 'zstd/.*doc/README[.]md$'
/Users/hdang/working/3p/src/zstd/doc/README.md
/Users/hdang/working/backup/projects/projects/others/coverage/3p/zstd/doc/README.md
/Users/hdang/working/contribs/zstd/doc/README.md
/Users/hdang/working/zstd/doc/README.md
real         5.35
user         5.29
sys          0.02
   1212416  maximum resident set size
         0  average shared memory size
         0  average unshared data size
         0  average unshared stack size
       305  page reclaims
         0  page faults
         0  swaps
         0  block input operations
         0  block output operations
         0  messages sent
         0  messages received
         0  signals received
         2  voluntary context switches
      1557  involuntary context switches
```

**Analysis**

* fast-locate is 30x faster than GNU locate in tests that try to locate files in my Macbook Pro machine.
* The memory used by fast-locate is about 3x more than that of GNU locate. It is expected because fast-locate is compiled using heavily templatized code.

### Automated benchmark results using Celero ###

In this benchmark we try to look for files that match **zstd/.*doc/README[.]md$** pattern using two folders
1. A mid-size folder which has about 50K files and folders.
2. A big folder which has more than 1.5 million files and folders.

**Mid-size folder**

``` shell
./locate_benchmark
Celero
Timer resolution: 0.001000 us
-----------------------------------------------------------------------------------------------------------------------------------------------
     Group      |   Experiment    |   Prob. Space   |     Samples     |   Iterations    |    Baseline     |  us/Iteration   | Iterations/sec  |
-----------------------------------------------------------------------------------------------------------------------------------------------
mid             | gnu_locate      |               0 |              10 |               1 |         1.00000 |    669958.00000 |            1.49 |
mid             | fast_locate     |               0 |              10 |               1 |         0.05769 |     38652.00000 |           25.87 |
Complete.
```

**Big folder**

``` shell
./locate_benchmark
Celero
Timer resolution: 0.001000 us
-----------------------------------------------------------------------------------------------------------------------------------------------
     Group      |   Experiment    |   Prob. Space   |     Samples     |   Iterations    |    Baseline     |  us/Iteration   | Iterations/sec  |
-----------------------------------------------------------------------------------------------------------------------------------------------
big             | gnu_locate      |               0 |              10 |               1 |         1.00000 |   5124920.00000 |            0.20 |
big             | fast_locate     |               0 |              10 |               1 |         0.03302 |    169206.00000 |            5.91 |
Complete.
```

**Analysis**

It is obvious that fast-locate is 10x faster than GNU locate command and the differences come from below factors:
1. fast-locate can quickly read the file information database from the database faster.
2. fast-locate regular expression matching algorithm is much faster that that that of GNU locate.
3. fast-locate source code is heavily templatized and many logics or code paths have been dispatched at compile time.
