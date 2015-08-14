#environment setup
INCLUDES="/home/superg/IET-Engine/IETEngine/include","/home/superg/IET-Engine/IETEngine"
LIBS_ANDROID="/home/superg/NVPACK_3/android-ndk-r10c/platforms/android-19/arch-arm/usr/lib"
LIBS_IET="/home/superg/IET-Engine/IETEngine/androidlib"
LIBS_ARM="/home/superg/NVPACK_3/android-ndk-r10c/sources/cxx-stl/gnu-libstdc++/4.6/libs/armeabi-v7a"
LIB_OUT="/home/superg/IET-Engine/FEM_CUDA_Android"
LIB_CUDA="/home/superg/NVPACK_3/cuda-6.0/targets/armv7-linux-androideabi/lib"
GCC=$(NDK_ROOT)/toolchains/arm-linux-androideabi-4.6/gen_standalone/linux-x86_64/bin/arm-linux-androideabi-g++  -L$(LIBS_ANDROID) -landroid  -lc -lstdc++ -lm -lgcc -L$(LIBS_ARM) -lgnustl_shared  -L$(LIBS_IET) -lsoil -lassimp -O3
NVCC=$(CUDA_TOOLKIT_ROOT)/bin/nvcc -ccbin $(GCC) -I$(INCLUDES) -DSOFA_DEVICE_CUDA -DNO_OPENGL -DANDROID --library-path $(LIBS_ANDROID),$(LIBS_IET),$(LIBS_ARM) --library android  -target-cpu-arch=ARM -m32 -arch=sm_32 -Xptxas '-dlcm=ca' -target-os-variant=Android 
 
CFLAGS += $(addprefix -I, $(INCLUDES))

-include subdir.mk


program: libcudaFem.a
	$(GCC) -std=c++11 -o program main.cpp -I"/home/superg/IET-Engine/IETEngine" -I"/home/superg/IET-Engine/IETEngine/include" -DSOFA_DEVICE_CUDA -DNO_OPENGL -DANDROID -L$(LIB_CUDA) -lcudart -lcudadevrt -O3 -fomit-frame-pointer -fexceptions -mfloat-abi=softfp libcudaFem.a


libcudaFem.a: $(OBJS)
	$(NVCC) -cudart static -lib -o "$@" $(OBJS)

clean:
	rm -rf *.a $(OBJS)
 
