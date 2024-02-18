# C++ implementation of an interpreted programming language

# qd_language
    Imitation from language Lua 
    my first language

### How-To-Build
use cmake:
```
mkdir build
cd build
cmake ..
make
``` 
### How-To-Execute
```
    Generate executable file directory is bin
    ../bin/qd file
    ../bin/qd -i
```

### Compiler type
```
linux g++ 4.8.5
mingw-w64-v8.0.0
msvc-1933  (testing)
```
#### Warning
```
if using g++ version is less than 5.0.0 
is not support printing datetime formatted
```

### Interactive
    [user@localhost] qd -i
    >>> a = 1
    >>> a
     int : 1
    >>> fun add(a,b): \
    : ret a + b \
    pass
    >>> a = add(1,2)
    >>> a
     int : 3

### Tutorial
- [Statements](tutorials/statement.md)
- [Variables](tutorials/variables.md)
- [If Statements]()
- [While Loops]()
- [Functions]()

### Local Lib
- [Base Lib](tutorials/base.md)
- IO Lib  see   test/lib_test/iolib_test
- [Math Lib](tutorials/math.md)
- [String Lib](tutorials/string.md)

