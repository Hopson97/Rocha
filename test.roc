func:
    push 4
    push 3
    ret
test:
    call func
    call add
    call print
    push 6
    push 6
    push 6
    call add
    call add
    call print
    ret
