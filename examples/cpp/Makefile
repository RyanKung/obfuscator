obf:
	${CXX} -Xclang \
	-load -Xclang \
	../../build/Obfuscation/LLVMObfuscation.dylib \
	-isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.15.sdk \
	./main.cpp

noobf:
	${CXX} -std=c++14 \
	-isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.15.sdk \
	./main.cpp


asm:
	${CC} -Xclang \
	-load -Xclang \
	../../build/Obfuscation/LLVMObfuscation.dylib \
	-isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.15.sdk \
	-fobjc-arc -S ./main.m -o /dev/stdout


	${CC}  \
	-isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.15.sdk \
	-fobjc-arc ./obf.bc -o a.out
build:
	${CC} \
	-fobjc-arc ./main.m

ir:
	${CC} -S -emit-llvm \
	 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.15.sdk \
	./main.cpp \
	-o main.ll

debug:
	/usr/local/bin/opt -load ../../build/Obfuscation/LLVMObfuscation.dylib -func_name < main.ll > obf.bc
