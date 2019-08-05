CC = g++ 
CXXFLAGS = -O -Wno-unused-parameter -std=c++11 -fPIC -Wno-unused-variable
INCLUDEFLAGS = 
LDLIBS = -L./zsl
LDLIBS += -lgmpxx -lgmp -lboost_system -fopenmp -lcrypto -lzsl
SRC_FILES := $(wildcard utils/*.cpp)
SRC_FILES += $(wildcard *.cpp)
OBJS = $(patsubst %.cpp, %.o,$(SRC_FILES))
TARGETS = zsltest 


.PHONY: all
all: zsl $(TARGETS)

.PHONY: zsl
zsl:
	make -C $@
	
zsltest: $(OBJS)
	$(CC) -o $@ $^ $(LDLIBS) $(CXXFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CXXFLAGS) $(LDLIBS) $(INCLUDEFLAGS)

%.d:%.c
	@set -e; rm -f $@; $(CC) -MM $< $(INCLUDEFLAGS) > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; rm -f $@.$$$$

-include $(OBJS:.o=.d)

.PHONY: clean 
clean:
	cd zsl && make clean
	rm -f $(TARGETS) *.o *.d *.d.* utils/*.d utils/*.o

