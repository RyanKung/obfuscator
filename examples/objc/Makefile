obf_build:
	${CC} -Xclang -load -Xclang \
	../../build/Obfuscation/LLVMObfuscation.dylib  \
	 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.15.sdk \
	-fobjc-arc ./main.m \
	-g

build:
	${CC} \
	-fobjc-arc ./main.m
