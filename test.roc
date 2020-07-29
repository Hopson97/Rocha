func:
    push 4
    push 5
    ret
test:
    make Adder some_object
    push 50
    call some_object setA
    call func
    call add
    call print
    push 100
    call some_object setB
    call some_object add
    call print
    ret
