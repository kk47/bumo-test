CXXFLAGS = -Wno-unused-parameter -std=c++11 -fPIC -Wno-unused-variable
CXXFLAGS += -L./zsl/snark 

LDLIBS += -lgmpxx -lgmp -lboost_system -fopenmp -lcrypto -lzsl
SUBDIR = ./zsl/snark

all:
	#cd $(SUBDIR) && make
	g++ $(CXXFLAGS) -c utils/util.cpp -o utils/util.o
	g++ $(CXXFLAGS) -c utils/sha256.cpp -o utils/sha256.o
	g++ $(CXXFLAGS) -c merkle_tree.cpp -o merkle_tree.o
	g++ $(CXXFLAGS) -c main.cpp -o main.o

	g++ utils/util.o utils/sha256.o merkle_tree.o main.o -o zsltest $(CXXFLAGS) $(LDLIBS)
clean:
	rm -f utils/*.o *.o
	#rm -f zsltest ./zsl/snark/libsnark.a
