	file	"assist.s"

	text
	even

#
#	specialized bitblt routine to keep tex3b1 running at an
#	acceptable speed
#
	global	bitblt
bitblt:
	link.w	%a6,&0
	movm.l	&0x0f08,-(%sp)

	mov.l	&15,%d7
	mov.l	28(%fp),%d6
	add.l	%d7,%d6
	asr.l	&4,%d6
	mov.l	12(%fp),%d0
	mov.l	%d0,%d1
	and.l	%d7,%d0
	sub.l	%d0,%d7
	addq.l	&1,%d7

	mov.l	8(%fp),%a4
	lsr.l	&4,%d1
	lsl.l	&1,%d1
	add.l	%d1,%a4
	mov.w	18(%fp),%d0
	mulu.w	22(%fp),%d0
	add.l	%d0,%a4
	mov.l	24(%fp),%a1

	mov.w	34(%fp),%d1
	subq.w	&1,%d6
	mov.w	22(%fp),%d4
L%1:
	mov.l	%a4,%a0
	mov.w	%d6,%d5
L%2:
	mov.l	&0,%d0
	mov.w	(%a1)+,%d0
	ror.l	%d7,%d0
	or.l	%d0,(%a0)
	addq.w	&2,%a0
	dbra	%d5,L%2

	add.w	%d4,%a4
	dbra	%d1,L%1

	movm.l	(%sp)+,&0x10f0
	unlk	%a6
	rts

#
#	for some reason the 3b1 has the bits backwards from everyone else
#
	global	rev
rev:
	mov.l	&0,%d0
	mov.w	6(%sp),%d1
	mov.l	%d2,%a0
	mov.l	&15,%d2
L%0:
	roxr.w	&1,%d1
	roxl.w	&1,%d0
	dbra	%d2,L%0

	mov.l	%a0,%d2
	rts
