#
#  Portions Copyright (C) 2003 Ben Goodwin
#
#  This program is free software; you can redistribute it and/or modify it
#  under the terms of the OSML - Open Source Modification License v1.0 as
#  described in the file COPYING which is distributed with this source
#  code.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.


# Set the environment variable TA to something if you wish to build
# the TA version ...


CC=cc
CFLAGS=

CFILES = $(wildcard *.c)
EXCLUDE = g_rankings.c bg_lib.c
FILES = $(filter-out $(EXCLUDE),$(CFILES))
OBJS = $(FILES:%.c=%.o)
DEPS = $(OBJS:.o=.d)

ifdef TA
OUTDIR = OBJS/TA
CPPFLAGS= -pipe -DNDEBUG -O6 -ffast-math -fstrength-reduce -DMISSIONPACK -DQAGAME -fPIC
else
OUTDIR=OBJS/Q3
CPPFLAGS= -pipe -DNDEBUG -O6 -ffast-math -fstrength-reduce -DQAGAME -fPIC
endif

vpath %.o $(OUTDIR)
vpath %.so $(OUTDIR)

define makedir
[ -d $(OUTDIR) ] || mkdir -p $(OUTDIR)
endef

define fixdep
cp -f $(OUTDIR)/$*.d $(OUTDIR)/$*.d.tmp 2> /dev/null || mv -f $*.d $(OUTDIR)/$*.d.tmp
sed -e 's/.*://' -e 's/\\$$//' < $(OUTDIR)/$*.d.tmp | fmt -1 | \
  sed -e 's/^ *//' -e 's/$$/:/' >> $(OUTDIR)/$*.d
rm -f $(OUTDIR)/$*.d.tmp
endef

qagamei386.so : $(OBJS)
	cd $(OUTDIR) && $(CC) $(CPPFLAGS) -shared -o qagamei386.so $(OBJS) -lm

%.o: %.c
	@$(makedir)
	$(CC) -c -MMD $(CPPFLAGS) $(CFLAGS) $*.c -o $(OUTDIR)/$*.o
	@$(fixdep)

-include $(DEPS)

.PHONY : clean
clean:
	cd $(OUTDIR) && rm -f *.o *.so *.d

