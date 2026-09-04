; --- DATA TYPES ---
; i1 - bool
; i8 - char
; i32
; i64
; float
; double
; ptr - pointer
; void

; --- VALUES and VARIABLES ---

; immutable register
%immutable_var = add i32 0, 67

; mutable values (stack allocation)
%x = alloca i32                     ; allocate place for i32 in stack
store i32 10, ptr %x                ; store value in x
store i32 20, ptr %x                ; store new value in x
%current_val = load i32, ptr %x     ; load it so it can be used

; --- MATH OPERATIONS ---
; integer math
; add -> +
; sub -> -
; mul -> *
; sdiv -> /
; udiv -> / (positive only)
; srem -> %
; urem -> % (positive  only)

; float math
; fadd -> +
; fsub -> -
; fmul -> *
; fdiv -> /
; frem -> %

; bitwise logic
; and -> &
; or -> |
; xor -> ^
; shl -> << (shift left)
; lshr -> >> (shift right)


; --- CONDITIONS ---

; %is_greater = icmp sgt i32 67, 0    ; icmp -> int compare

; eq -> ==
; ne -> !=
; slt -> <
; sle -> <=
; sge -> >=

; --- BRANCHING ---

; br i1 %is_greater, label %label1, label %label2

; --- WHILE loop ---

define void @loop_foo_func(i32 %start_val){     ; every code needs to live inside a function
entry:
    %x = alloca i32
    store i32 %start_val, ptr %x
    br label %loop_condition

loop_condition:
    ; checks if loop should continue
    %current = load i32, ptr %x
    %is_greater = icmp sgt i32 %current, 0  ; Is x > 0?

    ; If true, jump to the body. If false, break out of the loop!
    br i1 %is_greater, label %loop_body, label %loop_exit

loop_body:
    ; This runs every iteration
    %old_x = load i32, ptr %x
    %new_x = sub i32 %old_x, 1      ; x = x - 1
    store i32 %new_x, ptr %x

    br label %loop_condition        ; JUMP BACK TO THE TOP - This makes it a loop.

loop_exit:
    ; the loop is finished
    ret void    ; return
}