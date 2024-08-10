# Bad Programming Language
A very simple language i created in my mind. Frankly the semantics came to me in a dream so sorry for the pain,
# Goal
My goal is to be able to compile this:
```
fn foo, start
call putl "Bar"
fn foo end


fn main arr<str> uint8, start
@x uint8 9
@y uint8 60
@z uint8 $ add @^x @^y
@s str $ concat "Funny number: " @^z
call putl @^s
call foo
fn main end
```
# Syntax
Its the most intuitive thing in the world
- @[varname] [type] [data | eval sign] [eval stuff]
- call [func name] [params]
When referencing a variable use the @[varname] or @^[varname] (this just means read only)
- No comments as they are for the weak
# STD lib funcs
- putl - 1*_any_t, ret _void, Prints the line
- add - 2*_uint8_t|_uint32_t, ret _uint8_t|_uint32_t, Adds the two args together
- concat - n*_any_t, ret _str_t, Not implemented; Concats all data into a singular str 
