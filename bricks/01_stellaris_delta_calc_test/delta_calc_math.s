	.thumb
	.global vsqrtf_stellaris

; Utilise l'instruction racine carr�e de la plateforme 	
vsqrtf_stellaris:
	VSQRT.F32       S0, S0
	BX              R14