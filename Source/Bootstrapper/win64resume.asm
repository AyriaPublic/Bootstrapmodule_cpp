; Visual Studio / CL doesn't support inline x64 assembly, so this file is needed.
; This is because a lot of games need some stack-alignment.

_TEXT SEGMENT

; Set by PECallback.cpp
EXTERN Entrypoint:qword

; Called from PECallback.cpp
PUBLIC Resumeprogram
Resumeprogram PROC

and rsp, 0FFFFFFFFFFFFFFF0h
mov rax, 0DEADDEADDEADDEADh
push rax

mov rax, qword ptr [Entrypoint]
jmp rax

Resumeprogram ENDP
_TEXT ENDS
END
