obf_build:
	${CC} -Xclang -load -Xclang \
	../../build/Obfuscation/LLVMObfuscation.dylib  \
	 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.15.sdk \
	--debug=funcobf \
	-fobjc-arc ./main.m \

build:
	${CC} \
	-fobjc-arc ./main.m

ir:
	${CC} -S -emit-llvm \
	 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.15.sdk \
	./main.m \
	-o main.ll

debug:
	opt -load ../../build/Obfuscation/LLVMObfuscation.dylib -func_name < main.ll > /dev/null
