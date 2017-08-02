; Visual Studio doesn't support inline x64 assembly, so this file is needed.
; SSE does some strange stack tricks on x64 as well, so we need to align it.

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
