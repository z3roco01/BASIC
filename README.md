# BASIC
Learning how to make a BASIC interpreter so I can implement one in my OS BubblesOS

## Todo
- [X] Varriables
- [X] Math
- [X] Built-ins
- [ ] Strings

## Docs
## Commands
There are 3 commands `LIST`, `LISTD` and `RUN`</br>
`LIST` will list you program as you entered it.</br>
`LISTD` will list the tokenized version of your input, this is for debugging.</br>
`RUN` will run your program.</br>

### Variables
You can set a variable with `A=5`.
Variable names can be any capital letter.
You can also set a variable to another variable and you can do a sismple one math or bitwise operation when assigning variables like `C=A&B`.

### FOR Loops
In a `FOR` loop you can set a variable to a value and set the TO value it will stop at, You also need a `NEXT` statement sometime after to to signify the end.

``` purebasic
10 FOR A=0 TO 5
20 PRINT A
30 NEXT A
```

output

```
0
1
2
3
4
5
```

### IF Statements
In an if statment you can have a simple check then limited code after it.

``` purebasic
10 A=5
20 IF A>1 THEN PRINT "A is greater than 1 !"
```

output:
```
A is greater than 1 !
```

