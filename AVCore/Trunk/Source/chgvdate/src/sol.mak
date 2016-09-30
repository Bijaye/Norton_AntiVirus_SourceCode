
TARGETS = chgvdate

CCFLAGS = -DSYM_UNIX -DBIG_ENDIAN

OBJS = \
chgvdate.cpp 

chgvdate: $(OBJS)
	gcc $(CCFLAGS) $(INCLUDES) $(OBJS) -g -o $@
	


	
	
	