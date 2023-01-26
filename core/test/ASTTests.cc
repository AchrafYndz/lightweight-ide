#include "doctest/doctest.h"

#include "../src/parser/ASTree.h"
#include "lr.h"

#include <iostream>

#include <fstream>
#include <sstream>

TEST_SUITE("ASTTests") {
    TEST_CASE("[ASTTests] load1") {
        // root
        auto root = new ASTNode<Value>(new Value("+", true), {});

        ASTree asTree(root);

        const std::ifstream expectedFile("test/res/expected/ASTTest1.txt");
        std::stringstream expected;
        expected << expectedFile.rdbuf();

        std::string actual = asTree.getContent();

        CHECK_EQ(expected.str(), actual);

    }
    TEST_CASE("[ASTTests] load2") {
        // nesting 1
        auto leftNode = new ASTNode<Value>(new Value("1", true), {});
        auto rightNode = new ASTNode<Value>(new Value("2", true), {});

        auto root = new ASTNode<Value>(new Value("+", true), {std::shared_ptr<ASTNode<Value>>(leftNode), std::shared_ptr<ASTNode<Value>>(rightNode)});

        ASTree asTree(root);

        const std::ifstream expectedFile("test/res/expected/ASTTest2.txt");
        std::stringstream expected;
        expected << expectedFile.rdbuf();

        std::string actual = asTree.getContent();

        CHECK_EQ(expected.str(), actual);

    }
    TEST_CASE("[ASTTests] load3") {

        // nesting 2
        auto n1 = new ASTNode<Value>(new Value("(", true), {});
        auto n2 = new ASTNode<Value>(new Value("1", true), {std::shared_ptr<ASTNode<Value>>(n1)});

        auto n3 = new ASTNode<Value>(new Value(")", true), {});
        auto n4 = new ASTNode<Value>(new Value("2", true), {std::shared_ptr<ASTNode<Value>>(n3)});

        auto root = new ASTNode<Value>(new Value("+", true), {std::shared_ptr<ASTNode<Value>>(n2), std::shared_ptr<ASTNode<Value>>(n4)});

        ASTree asTree(root);

        const std::ifstream expectedFile("test/res/expected/ASTTest3.txt");
        std::stringstream expected;
        expected << expectedFile.rdbuf();

        std::string actual = asTree.getContent();

        CHECK_EQ(expected.str(), actual);

    }
    TEST_CASE("[ASTTests] load") {

        // nesting 3

        auto* n1 = new ASTNode<Value>(new Value("(", true), {});
        auto* nn2 = new ASTNode<Value>(new Value("(", true), {});
        auto* nn3 = new ASTNode<Value>(new Value("3", true), {});
        auto* nn4 = new ASTNode<Value>(new Value("4", true), {});
        auto* nn5 = new ASTNode<Value>(new Value(")", true), {});
        auto* nn1 = new ASTNode<Value>(new Value("*", true), {std::shared_ptr<ASTNode<Value>>(nn2), std::shared_ptr<ASTNode<Value>>(nn3), std::shared_ptr<ASTNode<Value>>(nn4), std::shared_ptr<ASTNode<Value>>(nn5)});
        auto* n2 = new ASTNode<Value>(new Value("1", true), {std::shared_ptr<ASTNode<Value>>(nn1)});
        auto* n3 = new ASTNode<Value>(new Value("2", true), {});
        auto* n4 = new ASTNode<Value>(new Value(")", true), {});
        auto* root = new ASTNode<Value>(new Value("+", true), {std::shared_ptr<ASTNode<Value>>(n1), std::shared_ptr<ASTNode<Value>>(n2), std::shared_ptr<ASTNode<Value>>(n3), std::shared_ptr<ASTNode<Value>>(n4)});

        ASTree asTree(root);

        const std::ifstream expectedFile("test/res/expected/ASTTest4.txt");
        std::stringstream expected;
        expected << expectedFile.rdbuf();

        std::string actual = asTree.getContent();

        CHECK_EQ(expected.str(), actual);

    }

    TEST_CASE("[ASTTests] syntax1") {
        nlohmann::json result{};
        const CFG cfg = CFG::parse_ebnf("res/grammar/cminmin.gram");
        LR lr(cfg);

        const auto parse_result = lr.parse(StreamReader("test/res/input/Syntax1.cmm"));

        const std::ifstream expectedFile("test/res/expected/Syntax1.txt");
        std::stringstream expected;
        expected << expectedFile.rdbuf();

        CHECK(parse_result.success);
        CHECK(parse_result.tree.has_value());

        if (parse_result.tree.has_value()) {
            const auto& root = parse_result.tree.value()->getRoot();
            root->inorderVisit(root, result["bounds"]);
        } else {
            result["bounds"] = {};
        }

        CHECK_EQ(expected.str(), (std::string) result["bounds"].dump());

    }

    TEST_CASE("[ASTTests] syntax2") {
        nlohmann::json result{};
        const CFG cfg = CFG::parse_ebnf("res/grammar/cminmin.gram");
        LR lr(cfg);

        const auto parse_result = lr.parse(StreamReader("test/res/input/Syntax2.cmm"));

        const std::ifstream expectedFile("test/res/expected/Syntax2.txt");
        std::stringstream expected;
        expected << expectedFile.rdbuf();

        CHECK(parse_result.success);
        CHECK(parse_result.tree.has_value());

        if (parse_result.tree.has_value()) {
            const auto& root = parse_result.tree.value()->getRoot();
            root->inorderVisit(root, result["bounds"]);
        } else {
            result["bounds"] = {};
        }

        CHECK_EQ(expected.str(), (std::string) result["bounds"].dump());

    }
}
