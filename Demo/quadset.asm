;
; Fills a region of memory using 32-bit stores
;

.MODEL FLAT,C
.CODE
PUBLIC QuadSet

QuadSet PROC C dest:DWORD, data:DWORD, count:DWORD

.586

cld
les di, dest
mov ecx, count
mov eax, data
rep stosd

ret

QuadSet ENDP

END
