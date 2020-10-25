CXX = arm-himix200-linux-g++
CC = arm-himix200-linux-gcc
AR = arm-himix200-linux-ar cqs

TOP_DIR = .

TARGET_EXE = unit_test

#CXX_SOURCE += $(TOP_DIR)/src/unit_test_conv.cpp
CXX_SOURCE += $(TOP_DIR)/optimize_gemm/test_matrix_multiply.cpp

####################head file###################
DEP_INC_DIR += -I$(TOP_DIR)/include
DEP_INC_DIR += -I$(TOP_DIR)/include/opencv2.4.9 
DEP_INC_DIR += -I$(TOP_DIR)/include/opencv2.4.9/opencv 
DEP_INC_DIR += -I$(TOP_DIR)/include/opencv2.4.9/opencv2 
DEP_INC_DIR += -I$(TOP_DIR)/optimize_gemm
DEP_INC_DIR += -I$(TOP_DIR)/src


####################ld flag###################
#LDFLAGS += ./libboxfilter.a

LDFLAGS+= -L$(TOP_DIR)/library/opencv2.4.9
LDFLAGS+= -lopencv_highgui -lopencv_imgproc -lopencv_core -lz
LDFLAGS+= -L$(TOP_DIR)/library/jpeg
LDFLAGS+= -ljpeg  -lm -pthread -lrt -fopenmp -ldl
LDFLAGS+= -lz

OBJ_CPP := $(patsubst %.cpp, %.o, $(CXX_SOURCE))
OBJ_C := $(patsubst %.c, %.o, $(C_SOURCE))

DEFINE_MACRO := 

CPPFLAGS := -Wall -O3 -fpermissive -fopenmp -mfloat-abi=softfp -mfpu=neon -march=armv7-a -std=c++11 
CPPFLAGS += $(DEP_INC_DIR) $(DEFINE_MACRO) 
CFLAGS += $(DEP_INC_DIR) $(DEFINE_MACRO) 

#$(TARGET_SHARED_LIB):$(OBJ_CPP) $(OBJ_C)
#	$(CXX) -o $@ $^ $(LDFLAGS)

$(TARGET_EXE):$(OBJ_CPP)$(OBJ_C)
	$(CXX) -o $@ $^ $(LDFLAGS)

# $(TARGET_LIB):$(OBJ_CPP) $(OBJ_C)
# 	$(AR) -o $@ $^ $(LDFLAGS)

%.o : %.cpp
	$(CXX) $(CPPFLAGS) -o $@ -c $< 

%.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $< 

clean:
	rm -rf $(OBJ_CPP) $(OBJ_C) $(TARGET_EXE)
