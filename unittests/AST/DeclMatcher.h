//===- unittest/AST/DeclMatcher.h - AST unit test support ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_UNITTESTS_AST_DECLMATCHER_H
#define LLVM_CLANG_UNITTESTS_AST_DECLMATCHER_H

#include "clang/ASTMatchers/ASTMatchFinder.h"

namespace clang {
namespace ast_matchers {

enum class DeclMatcherKind { First, Last };

// Matcher class to retrieve the first/last matched node under a given AST.
template <typename NodeType, DeclMatcherKind MatcherKind>
class DeclMatcher : public MatchFinder::MatchCallback {
  NodeType *Node = nullptr;
  void run(const MatchFinder::MatchResult &Result) override {
    if ((MatcherKind == DeclMatcherKind::First && Node == nullptr) ||
        MatcherKind == DeclMatcherKind::Last) {
      Node = const_cast<NodeType *>(Result.Nodes.getNodeAs<NodeType>(""));
    }
  }
public:
  // Returns the first/last matched node under the tree rooted in `D`.
  template <typename MatcherType>
  NodeType *match(const Decl *D, const MatcherType &AMatcher) {
    MatchFinder Finder;
    Finder.addMatcher(AMatcher.bind(""), this);
    Finder.matchAST(D->getASTContext());
    assert(Node);
    return Node;
  }
};
template <typename NodeType>
using LastDeclMatcher = DeclMatcher<NodeType, DeclMatcherKind::Last>;
template <typename NodeType>
using FirstDeclMatcher = DeclMatcher<NodeType, DeclMatcherKind::First>;

template <typename NodeType>
class DeclCounter : public MatchFinder::MatchCallback {
  unsigned count = 0;
  void run(const MatchFinder::MatchResult &Result) override {
      if(Result.Nodes.getNodeAs<NodeType>("")) {
        ++count;
      }
  }
public:
  // Returns the number of matched nodes under the tree rooted in `D`.
  template <typename MatcherType>
  unsigned match(const Decl *D, const MatcherType &AMatcher) {
    MatchFinder Finder;
    Finder.addMatcher(AMatcher.bind(""), this);
    Finder.matchAST(D->getASTContext());
    return count;
  }
};

} // end namespace ast_matchers
} // end namespace clang

#endif
