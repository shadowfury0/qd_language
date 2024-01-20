### BASE 

#### print 
- @brief 
output to console
```
>>> sys.print('hello world')  
hello world>>> 
>>> a = 10     
>>> b = 2.1
>>> c = 3
>>> d = 'qwe'
>>> sys.print(a,b,c,d)
102.13qwe>>> 
```

#### println
- @brief 
is the same as print but with the addition of character '\n'
```
>>> sys.println('hello world')
hello world
```

#### type
- @brief 
show the type of input 
```
>>> sys.type(a)
int
>>> sys.type(d) 
string
```

#### error
- @brief 
terminal the program
```
>>> sys.error('error')
system error !!! error
(- err   -)  <id 2> [ 2024-01-20 20:13:07 ] system call error
(- err   -)  <id 3> [ 2024-01-20 20:13:07 ] analyse lib expression error
>>>
```

#### assert
- @brief 
only first arg is true or number is 0 call error 
```
>>> sys.assert(true)
(- err   -)  <id 1> [ 2024-01-20 20:13:48 ] system call error
(- err   -)  <id 2> [ 2024-01-20 20:13:48 ] analyse lib expression error
>>> sys.assert(false)   
>>> sys.assert(1) 
>>> sys.assert(0) 
(- err   -)  <id 3> [ 2024-01-20 20:15:43 ] system call error
(- err   -)  <id 4> [ 2024-01-20 20:15:43 ] analyse lib expression error
```

#### input
- @brief 
accept console input
- @return -> string
```
>>> sys.input()
hello world
>>> str = sys.input() 
hello world
>>> str
 string : hello world 
>>>
```