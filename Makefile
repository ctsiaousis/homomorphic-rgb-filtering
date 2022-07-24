CXX ?= g++

CXXFLAGS += -c -Wall $(shell pkg-config --cflags opencv4)
LDFLAGS += /lib64/libopencv_core.so /lib64/libopencv_highgui.so /lib64/libopencv_features2d.so /lib64/libopencv_xfeatures2d.so /lib64/libopencv_imgcodecs.so /lib64/libopencv_imgproc.so

all: rgb_homomorphic_example

rgb_homomorphic_example: main.o; $(CXX) $< -o $@ $(LDFLAGS)

%.o: %.cpp; $(CXX) $< -o $@ $(CXXFLAGS)

clean: ; rm -f main.o rgb_homomorphic_example

