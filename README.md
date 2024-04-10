# ECODE APP
## Clone:
`git clone --recurse-submodules git@github.com:ghosh-inspire/ecodeapp.git`

## Build
### Build will need CMake 3.22 and  a recent gcc version installed
## Commands
```
$ cd build
$ cmake ..
$ make
```
## Example Test Commands
```
$ ./ecodecpu1 /cpu1-tests/cpu1-cb-write-only --single
$ ./ecodecpu1 /cpu1-tests/cpu1-cb-write-only --param cpu1-cb-write-params 55
$ ./ecodecpu1 /cpu1-tests/cpu1-cb-show-only

$ ./ecodecpu2 /cpu2-tests/cpu2-cb-read-only --single
$ ./ecodecpu2 /cpu2-tests/cpu2-cb-show-only
```

## Randomised Test Commands
```
$ ./ecodecpu1 /cpu1-tests/cpu1-cb-write-ofst-test --iterations 100
$ ./ecodecpu2 /cpu2-tests/cpu2-cb-read-ofst-test --iterations 100
```

## Buffer Clear Commands
```
$ ./ecodecpu1 /cpu1-tests/cpu1-cb-show-clear
$ ./ecodecpu2 /cpu2-tests/cpu2-cb-show-clear
```
