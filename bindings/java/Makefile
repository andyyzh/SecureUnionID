CC = gcc
CFLAGS = -fPIC
SWIG = swig
SWIG_FLAGS = -java
SWIG_INTERFACE_NAME = SecureUnionID
SWIG_FILE = $(SWIG_INTERFACE_NAME).i
SWIG_WRAP_NAME = $(SWIG_INTERFACE_NAME)_wrap

MAC_OS_LDFLAGS = -dynamiclib -undefined suppress -flat_namespace

UNAME_S := $(shell uname)
ifeq ($(UNAME_S),Linux)
# jni.h path
JAVA_HOME = /usr/lib/jvm/java-8-openjdk-amd64
# jni_md.h
JNI_CFLAG = -I"$(JAVA_HOME)/include" -I"$(JAVA_HOME)/include/linux"
endif

ifeq ($(UNAME_S),Darwin)
JAVA_HOME:=`/usr/libexec/java_home`
JNI_CFLAG = -I"$(JAVA_HOME)/include" -I"$(JAVA_HOME)/include/darwin"
endif


ECC_DIR = ../../src/crypto
CORE_DIR = ../../src/psi
OBJ_DIR = objs

ECC_SRCS := $(wildcard $(ECC_DIR)/*.c)
CORE_SRCS := $(wildcard $(CORE_DIR)/*.c)


OBJS := $(ECC_SRCS:$(ECC_DIR)/%.c=$(OBJ_DIR)/%.o)
OBJS += $(CORE_SRCS:$(CORE_DIR)/%.c=$(OBJ_DIR)/%.o)
OBJS += $(OBJ_DIR)/$(SWIG_WRAP_NAME).o

# build all share library
all: $(OBJS)
ifeq ($(UNAME_S),Linux)
	$(CC) -shared -o lib$(SWIG_INTERFACE_NAME).so $^ $(CFLAGS)
endif
ifeq ($(UNAME_S),Darwin)
	$(CC) $(MAC_OS_LDFLAGS) -o lib$(SWIG_INTERFACE_NAME).dylib $^ $(CFLAGS)
endif


$(OBJ_DIR)/%.o: $(ECC_DIR)/%.c $(OBJ_DIR)
	$(CC) -c $< -o $@  $(CFLAGS)

$(OBJ_DIR)/%.o: $(CORE_DIR)/%.c $(OBJ_DIR)
	$(CC) -c $< -o $@  $(CFLAGS)


$(OBJ_DIR):
	mkdir objs

# clean the library files
clean:
	rm -f *.so *.class *.dylib
	rm -rf $(OBJ_DIR)

#$(SWIG_WRAP_NAME).c: $(SWIG_FILE)
	#$(SWIG) $(SWIG_FLAGS) $(SWIG_FILE)


$(OBJ_DIR)/$(SWIG_WRAP_NAME).o: $(SWIG_WRAP_NAME).c
	gcc -c $(SWIG_WRAP_NAME).c -fPIC $(JNI_CFLAG) -o $(OBJ_DIR)/$(SWIG_WRAP_NAME).o

# compile java source code into classes
build:
	javac *.java

# run the java demo
run:
	java -Djava.library.path=. Demo

# re-genereate the swig wrapper files. This is only used by the developers.
gen:
	rm -f SecureUnionID*.java *.c
	$(SWIG) $(SWIG_FLAGS) $(SWIG_FILE)

# clean all the files. This is only used by the developers.
distclean:
	rm -f *.so *.class *.dylib SecureUnionID*.java *.c
	rm -rf $(OBJ_DIR)