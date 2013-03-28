	.thumb
	.global vsqrtf_stellaris
	
vsqrtf_stellaris:
	VSQRT.F32       S0, S0
	BX              R14