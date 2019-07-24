CXXFLAGS = -Wno-unused-parameter -std=c++11 -fPIC -Wno-unused-variable
CXXFLAGS += -L./zsl/snark 

LDLIBS += -lgmpxx -lgmp -lboost_system -fopenmp -lcrypto -lzsl
SUBDIR = ./zsl/snark

all:
	cd $(SUBDIR) && make
	g++ main.cpp utils/util.cpp utils/sha256.cpp -o zsltest $(CXXFLAGS) $(LDLIBS)
clean:
	rm -f zsltest ./zsl/snark/libsnark.a
