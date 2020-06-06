CC=g++
INCFLAG=-Ilib/luceneplusplus-3.0.7/include/lucene++
LIBFLAG=-Llib/luceneplusplus-3.0.7/lib
CFLAGS=-std=gnu++11 -g
LIBS=-lpthread -lboost_system -llucene++ -llucene++-contrib

all:
	mkdir -p bin
	$(CC) $(INCFLAG) $(LIBFLAG) $(CFLAGS) -o bin/XSearchData.exe \
		src/XSearchNew.cpp $(LIBS)
	$(CC) $(INCFLAG) $(LIBFLAG) $(CFLAGS) -o bin/XSearchMeta.exe \
		src/XSearchMeta.cpp $(LIBS)

clean:
	$(RM) -r bin/*.exe
