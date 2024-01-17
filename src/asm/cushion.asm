.model flat

hook_inline PROTO C
hook_inline_unsafe PROTO C

.data
MBCONTEXT  STRUCT
    EFlags  DWORD   ?
    _Edi    DWORD   ?
    _Esi    DWORD   ?
    _Ebp    DWORD   ?
    _Esp    DWORD   ?
    _Ebx    DWORD   ?
    _Edx    DWORD   ?
    _Ecx    DWORD   ?
    _Eax    DWORD   ?
    _Eip    DWORD   ?
MBCONTEXT  ENDS

.code 
_hook_inline_cushion proc
    pushad
    pushfd
    sub (MBCONTEXT PTR [esp])._Eip, 05h
    add (MBCONTEXT PTR [esp])._Esp, 04h
    push esp
    call hook_inline
    add esp, 04h
    popfd
    popad
    ret
_hook_inline_cushion endp
_hook_inline_cushion_unsafe proc
    pushad
    pushfd
    sub (MBCONTEXT PTR [esp])._Eip, 05h
    add (MBCONTEXT PTR [esp])._Esp, 04h
    push esp
    call hook_inline_unsafe
    add esp, 04h
    popfd
    popad
    ret
_hook_inline_cushion_unsafe endp
_hook_return_cushion proc
    ret
_hook_return_cushion endp
end