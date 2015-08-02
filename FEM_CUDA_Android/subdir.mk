
OBJS += \
	CudaBarycentricMapping.o \
	CudaFixedConstraint.o \
	CudaMechanicalObject.o \
	CudaMergedKernels.o \
	CudaPlaneForceField.o \
	CudaSphereForceField.o \
	CudaTetrahedronFEMForceField.o \
	CudaUniformMass.o \
	CudaVisualModel.o \
	mycuda.o \
 
%.o: %.cu
	$(NVCC) $(CFLAGS) $(EXTRA_CFLAGS) -c -o "$@" "$<"
 
