add_clang_tool(clang-torch
  ClangTorch.cpp
  )

clang_target_link_libraries(clang-torch
  clangTooling
  )
