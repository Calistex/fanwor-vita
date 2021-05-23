
.text


; Joystickroutine installieren
.globl _joyinit
_joyinit:
        move.w  #34,-(sp)
        trap    #14
        addq.l  #2,sp
        move.l  d0,a0
        lea     $18(a0),a0
        move.l  (a0),oldjoyvec
        move.l  #joyvec,(a0)
        rts

; alte Joystickroutine restaurieren
.globl _joyrstor
_joyrstor:
        move.w  #34,-(sp)
        trap    #14
        addq.l  #2,sp
        move.l  d0,a0
        lea     $18(a0),a0
        move.l  oldjoyvec,(a0)
        rts

; Eigener Joystick-Vektor
joyvec:
        cmp.b   #$ff,(a0)
        bne     nojoy
        move.b  2(a0),_joystate
nojoy:  rts


.bss
oldjoyvec: ds.l 1
