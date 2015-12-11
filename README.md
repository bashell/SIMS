## SIMS
A student information management system, realized in C.

#### 1. **Functionality**: 
(Used for Student information and Account information)
- Add
- Delete
- Update
- Search

For Administrator: (Own all rights)
```
1. search  student  information
2. add     student  information
3. delete  student  information
4. update  student  information
5. add     user     account
6. update  user     account
7. delete  user     account
8. search  user     account
9. exit
```

For Common User: (Can only query student information)
```
1. search  student  information
9. exit
```

#### 2. Usage
```
$ git clone https://github.com/bashell/sims.git
$ cd sims/src
$ gcc -o sims sims.c
$ ./sims
```
