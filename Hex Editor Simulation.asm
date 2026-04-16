.model small
.stack 100h
.data
    msg1 db 'Enter address (0-15): $'
    msg2 db 0Dh, 0Ah, 'Enter hex value (00-FF): $'
    msg3 db 0Dh, 0Ah, 'Value at address '
    addr db '  is: $'
    hex_table db '0123456789ABCDEF'
    address db ?
    value db ?
.code
main proc
    mov ax, @data
    mov ds, ax

    ; Prompt for address
    mov dx, offset msg1
    mov ah, 9
    int 21h
    call get_hex_digit
    cmp al, 0Fh
    ja error
    mov address, al

    ; Prompt for hex value
    mov dx, offset msg2
    mov ah, 9
    int 21h
    call get_hex_byte
    cmp al, 0FFh
    ja error
    mov value, al

    ; Display value at address
    mov dx, offset msg3
    mov ah, 9
    int 21h
    mov al, address
    call print_hex_byte
    mov dx, offset addr
    mov ah, 9
    int 21h
    mov al, value
    call print_hex_byte

    mov ah, 4ch
    int 21h
main endp

get_hex_digit proc
    mov ah, 1
    int 21h
    cmp al, '0'
    jl error
    cmp al, '9'
    jle digit
    cmp al, 'A'
    jl error
    cmp al, 'F'
    jle letter
    jmp error
digit:
    sub al, '0'
    ret
letter:
    sub al, 'A'
    add al, 10
    ret
error:
    mov ah, 4ch
    int 21h
get_hex_digit endp

get_hex_byte proc
    call get_hex_digit
    mov bl, al
    shl bl, 4
    call get_hex_digit
    add bl, al
    mov al, bl
    ret
get_hex_byte endp

print_hex_byte proc
    push ax
    mov ah, 0
    shr al, 4
    mov bx, offset hex_table
    xlat
    mov dl, al
    mov ah, 2
    int 21h
    pop ax
    and al, 0Fh
    mov bx, offset hex_table
    xlat
    mov dl, al
    mov ah, 2
    int 21h
    ret
print_hex_byte endp
end main