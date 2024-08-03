### MATH 

#### abs 
- @brief 
    the absolute value of num
- @return -> int or double
```
>>> a = 10
>>> b = -10
>>> c = -10.10
>>> val = math.abs(a)
>>> val
 int : 10 
>>> val = math.abs(b) 
>>> val
 int : 10 
>>> val = math.abs(c) 
>>> val
 double : 10.1 
 >>> val = math.abs('asd')
(- err   -)  <id 5> [ 2024-01-20 20:23:46 ] system call error
(- err   -)  <id 6> [ 2024-01-20 20:23:46 ] analyse lib expression error
```

#### fabs 
- @brief 
    is the same as abs

#### ceil 
- @brief round Up
- @return -> int or double
```
>>> a = 10.12
>>> b = -10.31
>>> val = math.ceil(a)
>>> val
 double : 11 
>>> val = math.ceil(b) 
>>> val
 double : -10 
```

#### floor 
- @brief round down
- @return -> int or double
```
>>> a = 10.12
>>> b = -10.31
>>> val = math.floor(a) 
>>> val
 double : 10 
>>> val = math.floor(b) 
>>> val
 double : -11 
```

#### rand
- @brief 
random num
- @param 
generate data within the range of [0, X) X is the first input value
- @return -> int or double
```
>>> ran
>>> ran = math.rand()
>>> ran
 int : 1719156529 
>>> ran = math.rand(20) 
>>> ran
 int : 15 
>>> ran = math.rand(20)
>>> ran
 int : 11 
>>> ran = math.rand(20)
>>> ran
 int : 2 
>>> ran = math.rand(20)
>>> ran
 int : 7 
>>> ran = math.rand(20)
>>> ran
 int : 7 
>>> ran = math.rand(20)
>>> ran
 int : 8 
>>> ran = math.rand(20)   
>>> ran
 int : 6 
```

Other Functions
```
sqrt
cbrt
round
log
log2
exp
pow
//三角函数
sin
cos
tan
asin
acos
atan
//双曲函数
sinh
cosh
tanh
asinh
acosh
atanh
```