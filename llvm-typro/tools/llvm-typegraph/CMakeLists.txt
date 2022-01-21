set(LLVM_LINK_COMPONENTS
		BitReader
		Core
		IRReader
		Linker
		Object
		Support
		Typegraph
		)

add_llvm_tool(llvm-typegraph
		llvm-typegraph.cpp
		)