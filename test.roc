func:
    push 4
    push 3
    ret
test:
    make Vector2 a
    call func
    call add
    call print
    push 6
    push 6
    push 6
    call add
    call add
    call print
    push 2
    push 3
    call multiplyTest
    call print
    ret
