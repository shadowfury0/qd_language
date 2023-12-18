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
### Compiler type
```
linux g++ 4.8.5
mingw-w64-v8.0.0
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

