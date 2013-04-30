	.thumb
	.global edit_user_pc
	.global reset_stack
	
; Modification de l'adresse de retour de l'interruption, situé sur la stack
; Voir Cortex-M4 Devices Generic User Guide - 2.3.7. Exception entry and return
edit_user_pc:
   STR R0, [SP, #32]
   BX  R14
   
; Vide la pile en déplaçant le pointeur de pile
reset_stack:
   MOV SP, R0
   BX  R14