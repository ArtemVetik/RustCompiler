FromFPUToEaxEdx Macro
	.data
		align 4
		temp real8 0.0
	.code
	FSTP temp
	push dword ptr [temp+4]
	push dword ptr [temp]
	pop eax
	pop edx
endm