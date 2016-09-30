# unix.mak
# Unix makefile for the macroheur library.

TARGETS = libmacroheu.a
include ../unixmake.inc

CCFLAGS += -DNDEBUG $(CCSHAREDFLAGS)

OBJS = \
        control.o    \
        env.o        \
        eval.o       \
        exec.o       \
        external.o   \
        for.o        \
        goto.o       \
        gsm.o        \
        ident.o      \
        if.o         \
        modenv.o     \
        select.o     \
        state.o      \
        var.o        \
        wbutil.o     \
        while.o      \
        o97call.o    \
        o97ctrl.o    \
        o97doop.o    \
        o97doop0.o   \
        o97doop1.o   \
        o97doop2.o   \
        o97doop3.o   \
        o97doop4.o   \
        o97doop5.o   \
        o97doop6.o   \
        o97doop7.o   \
        o97doop8.o   \
        o97doop9.o   \
        o97doopa.o   \
        o97doopb.o   \
        o97doopc.o   \
        o97doopd.o   \
        o97doope.o   \
        o97doopf.o   \
        o97eval.o    \
        o97env.o     \
        o97exec.o    \
        o97modev.o   \
        o97param.o   \
        o97str.o     \
        o97var.o  

libmacroheu.a: $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)
