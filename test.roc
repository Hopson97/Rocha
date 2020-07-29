;   This programme demonstres how Rocha works
;   This creates an "Adder"  object, which is user-defined in the C++ code
;   It then calls some methods on it, prints out etc

;   Functions are called by first pushing some numbers onto the stack
;   and then actually calling the function
;   The functions consume the stack

;   For example, the following will push 50 onto the stack, 
;   and then call the "setA" function, a member of "some_object", which will pop 50 off the stack 
;   when it is used
;       push 50
;       call some_object setA     
;  


func:         
    push 4  ;   Push 4 onto the stack
    push 5  ;   Push 5 onto the stack
    ret     ;   Return to caller


;   Called from C++
test:
    make Adder some_object      ;   Creates an "Adder" object called "some_object"
    push 50                     ;   Push 50 onto the stack
    call some_object setA       ;   Calls "setA" on "some_object", setting some_object.a to 50
    call func                   ;   Calls "func", so jumps to the func label above 
    call add                    ;   Calls global function "add", adding the top-two stack items, then pushing 9 onto the stack
    call print                  ;   Prints the top of the stack (9)
    push 100                    ;   Push 100 onto the stack
    call some_object setB       ;   Calls "setB" on "some_object", setting some_object.b to 100
    call some_object add        ;   Calls "add" on "some_object", which does a + b and then pushes result onto the stack
    call print                  ;   Prints the top of the stack (9)
    ret


