add_llvm_component_library(LLVMTypegraphPasses
		RelatedWorkPass.cpp
		TypegraphPass.cpp
		TypegraphPass.h
		CleanupPass.cpp
		CleanupPass.h
		TyproLibraryFixes.cpp
		TyproLibraryFixes.h

		ADDITIONAL_HEADER_DIRS
		#${LLVM_MAIN_INCLUDE_DIR}/llvm/ExecutionEngine/Orc

		#DEPENDS
		#intrinsics_gen
		)

target_link_libraries(LLVMTypegraphPasses PRIVATE
		LLVMCore
		LLVMIRReader
		LLVMBitWriter
		LLVMObject
		LLVMSupport
		LLVMTypegraph
		)

#target_link_libraries(LLVMOrcJIT
#		PRIVATE
#		LLVMAnalysis
#		LLVMBitReader
#		LLVMBitWriter
#		LLVMPasses
#		)
