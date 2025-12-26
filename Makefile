mkfile_path := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
project_dir := $(abspath $(mkfile_path))

clean: windows-mingw-clean arch-clean ubuntu24-clean windows-native-clean ;

windows-mingw:
	$(MAKE) -f ci/windows-mingw.Makefile battery version artifact

windows-mingw-clean:
	$(MAKE) -f ci/windows-mingw.Makefile clean

arch:
	$(MAKE) -f ci/arch.Makefile battery version artifact

arch-clean:
	$(MAKE) -f ci/arch.Makefile clean

ubuntu24: ;

ubuntu24-clean: ;

windows-native: ;

windows-native-clean: ;
