# preparation
before using this project, you need to install the following packages:
## In Unbuntu
```bash 
sudo apt install build-essential
```
## In Macos
```bash
brew install make, gcc, g++
```
# build
```bash
make    
```

# test performance
comparison between skip list and SCSL and 
```bash
./test  
```
# test cache miss (linux)
1. test scsl cache miss
```bash
perf stat -e cache-references,cache-misses ./test1
```
2. test standard skip list cache miss

```
perf stat -e cache-references,cache-misses ./test2
```
# conclusion
1. The performance of SCSL is better than standard skip list. almost 1.2~3 times faster
2. The cache miss of SCSL is better than standard skip list.