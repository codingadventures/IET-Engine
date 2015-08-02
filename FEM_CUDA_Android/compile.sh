 GCC=$(NDK_ROOT)/toolchains/arm-linux-androideabi-4.6/gen_standalone/linux-x86_64/bin/arm-linux-androideabi-g++ -L$(LIBS_ANDROID) -landroid  -lc -lstdc++ -lm -lgcc -L$(LIBS_ARM) -lgnustl_shared  -L$(LIBS_IET) -lsoil -lassimp -O3

$(GCC) -o program main.cpp -I"/home/superg/IET-Engine/IETEngine" -I"/home/superg/IET-Engine/IETEngine/include" -DSOFA_DEVICE_CUDA -DNO_OPENGL -DANDROID -L$(LIB_CUDA) -lcudart_static -lcudadevrt libcudaFem.o
