CC              = g++ -std=c++14
CC_OBJ_FLAGS    = -c -fPIC
CC_Shared_FLAGS = -shared -Wl,-soname,libvfTDC.so
ROOT_CFLAGS     = $(shell ${ROOTSYS}/bin/root-config --cflags)
ROOT_LIBS       = $(shell ${ROOTSYS}/bin/root-config --libs)
libvfTDC       = libvfTDC

all:		Measurements.o CalibDoubleMeasure.o
		mkdir -p lib; rm -f lib/*.so
		$(CC) $(CC_Shared_FLAGS) -o lib/${libvfTDC}.so.1.0.1 $^
		cd lib;\
		ln -sf ${libvfTDC}.so.1.0.1 ${libvfTDC}.so.1; ln -sf ${libvfTDC}.so.1.0.1 ${libvfTDC}.so
	
	
Measurements.o:	src/Measurement.cc include/Measurement.h
		 $(CC) $(CC_OBJ_FLAGS) src/Measurement.cc -o $@ $(ROOT_CFLAGS) -I ./include
	
CalibDoubleMeasure.o:	src/CalibDoubleMeasure.cc include/CalibDoubleMeasure.h
			$(CC) $(CC_OBJ_FLAGS) src/CalibDoubleMeasure.cc -o $@ $(ROOT_CFLAGS) -I ./include
	
	
clean:
		rm -f lib/*.so *.o
