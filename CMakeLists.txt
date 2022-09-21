set( LLVM_LINK_COMPONENTS
		${LLVM_TARGETS_TO_BUILD}
		Option
		Support
		)


add_clang_executable(DA
		DA.cpp
		DARecursiveASTVisitor.cpp
		DAASTFrontedAction.cpp
		DAContext.cpp
		DAFileManager.cpp
		DAPPCallbacks.cpp
		SymbolTable.cpp
		Util.cpp
		DAASTConsumer.cpp
		DASignatureGenerator.cpp
		)

target_link_libraries(
		DA
		PRIVATE
		clangAST
		clangBasic
		clangDriver
		clangFrontend
		clangRewriteFrontend
		clangTooling
	)
