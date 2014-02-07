# Compile: create object files from C++ source files.
%.o: $(ARDUINO)/%.cpp
	$(CXX) -c $(ALL_CXXFLAGS) $< -o $@

%.o: $(LIB)/%.cpp
	$(CXX) -c $(ALL_CXXFLAGS) $< -o $@

%.o: %.cpp
	$(CXX) -c $(ALL_CXXFLAGS) $< -o $@

# Compile: create object files from C source files.
%.o: $(ARDUINO)/%.c
	$(CC) -c $(ALL_CFLAGS) $< -o $@

%.o: %.c
	$(CC) -c $(ALL_CFLAGS) $< -o $@

# Compile: create assembler files from C source files.
.cpp.s:
	$(CC) -S $(ALL_CFLAGS) $< -o $@

.c.s:
	$(CC) -S $(ALL_CFLAGS) $< -o $@


# Assemble: create object files from assembler source files.
.S.o:
	$(CC) -c $(ALL_ASFLAGS) $< -o $@
