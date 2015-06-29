# use 'make IPOD=1' to build for iPod, 'make' for native build
# For convenience, iPod builds of libmikmod.a and libpthread.a are supplied

ifneq ($(IPOD),)

# iPod build
CC= arm-elf-gcc
# -I . to bring in mikmod includes
CFLAGS= -D__IPOD__ -I.
LDFLAGS= -elf2flt libmikmod.a libpthread.a -lm
ADD_OBJS= fb_ipod.o

else

# native build
CC= gcc
CFLAGS= `sdl-config --cflags`
LDFLAGS = -lm `sdl-config --static-libs` -lmikmod
ADD_OBJS= fb_sdl.o
endif

CFLAGS+=\
	-Wall -g \

PF_OBJS= podfather.o rotazoom.o shadeplasma.o text_printer.o sprite.o clovermap.o copper.o vectorballs.o bobs.o tunnel.o rubber.o end.o

PF_GFX= aluminium_font.h ducks.h ipod.h podfather_title.h

all: podfather

podfather: $(PF_OBJS) $(ADD_OBJS) $(PF_GFX) Makefile
	$(CC) $(PF_OBJS) $(ADD_OBJS) -o podfather $(CFLAGS) $(LDFLAGS)

clean: 
	$(RM) *.o *~ *.gdb podfather
