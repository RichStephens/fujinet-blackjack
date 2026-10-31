; Colors the table between the top border and the status row
.export _dli, _tableColor

.include "atari.inc"

.data
_tableColor: .byte $B4

.code
_dli:
        pha
        lda VCOUNT
        cmp #64
        bcs @status
        lda _tableColor
        sta WSYNC
        sta COLBK
        pla
        rti
@status:
        lda #0
        sta WSYNC
        sta COLBK
        pla
        rti
