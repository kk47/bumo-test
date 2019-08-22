CC = g++ 
CXXFLAGS = -O -Wno-unused-parameter -std=c++11 -fPIC -Wno-unused-variable
INCLUDEFLAGS = -I./secp256k1-zkp/include 
LDLIBS = -L./secp256k1-zkp
LDLIBS += -lsecp256k1
SRC_FILES := main.cpp
#SRC_FILES := $(wildcard *.cpp)
OBJS = $(patsubst %.cpp, %.o,$(SRC_FILES))
TARGETS = mwtest 

all: secp256k1-zkp $(TARGETS)

secp256k1-zkp:
	make -C ./secp256k1-zkp
.PHONY: secp256k1-zkp

$(TARGETS): $(OBJS)
	$(CC) -o $@ $^ $(LDLIBS) $(CXXFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CXXFLAGS) $(LDLIBS) $(INCLUDEFLAGS)

%.d:%.c
	@set -e; rm -f $@; $(CC) -MM $< $(INCLUDEFLAGS) > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; rm -f $@.$$$$

-include $(OBJS:.o=.d)

.PHONY: clean 
clean:
	rm -f $(TARGETS) *.o *.d *.d.*

