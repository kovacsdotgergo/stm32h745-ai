# list of objects
objects := $(patsubst $(root_dir)%,$(build_dir)%,$(c_sources:.c=.o))
# list of c++ objects
objects += $(patsubst $(root_dir)%,$(build_dir)%,$(cxx_sources:.cc=.o))
# list of ASM program objects
objects += $(patsubst $(root_dir)%,$(build_dir)%,$(asm_sources:.s=.o))
objects += $(patsubst $(root_dir)%,$(build_dir)%,$(asmm_sources:.S=.o))

# Generate dependency information (on every build)
# MMD is the flag to generate the dependency infromation
# MP generates a phony target for the header files, the rules don't fail then if the header is removed (like this: `header.h: `)
# MF is to specify the name of the dependency file
generate_dependency = -MMD -MP -MF"$(@:%.o=%.d)"
# Generate a disassebly output file
# Wa: is the flag to pass arguments to assembler
# d: Inlcude debugging information in the listing file
# a: Produce a listing file
# l: Include an assembly listing of the source code
# m: Include macro expansions in the listing
# s: Include symbol table information in the listing
generate_disassembly = -Wa,-a,-ad,-alms=$(@:.o=.lst)

$(build_dir)/%.o: $(root_dir)/%.c Makefile
	@mkdir -p $(dir $@)
	$(log) compiling $(patsubst $(root_dir)/%,%,$@)
	$(q)$(CC) -c $(CFLAGS) $(generate_dependency) $(generate_disassembly) $< -o $@

$(build_dir)/%.o: $(root_dir)/%.cc Makefile
	@mkdir -p $(dir $@)
	$(log) compiling $(patsubst $(root_dir)/%,%,$@)
	$(q)$(CXX) -c $(CXXFLAGS) $(generate_dependency) $(generate_disassembly) $< -o $@

$(build_dir)/%.o: $(root_dir)/%.s Makefile
	@mkdir -p $(dir $@)
	$(log) compiling $(patsubst $(root_dir)/%,%,$@)
	$(q)$(AS) -c $(ASFLAGS) $< -o $@
$(build_dir)/%.o: $(root_dir)/%.S Makefile
	@mkdir -p $(dir $@)
	$(log) compiling $(patsubst $(root_dir)/%,%,$@)
	$(q)$(AS) -c $(ASFLAGS) $< -o $@

$(build_dir)/$(target).elf: $(objects) $(tflite_lib) Makefile
	@mkdir -p $(dir $@)
	$(log) linking $(patsubst $(root_dir)/%,%,$@)
	$(q)$(CXX) $(objects) $(LDFLAGS) $(LDLIBS) -o $@
	$(SZ) $@

$(build_dir)/%.hex: $(build_dir)/%.elf
	$(HEX) $< $@
	
$(build_dir)/%.bin: $(build_dir)/%.elf
	$(BIN) $< $@	

$(build_dir)/%.list: $(build_dir)/%.elf
	$(DP) -h -S $< > $@
