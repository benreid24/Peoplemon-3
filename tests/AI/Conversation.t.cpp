#include <Core/AI/Conversation.hpp>
#include <Core/Systems/Systems.hpp>
#include <SystemsHelper.hpp>
#include <gtest/gtest.h>

using namespace core;

#ifndef ON_CI

namespace tests
{
TEST(Conversation, Empty) {
    bl::logging::Config::configureOutput(std::cout, bl::logging::Config::Error);

    file::Conversation conversation;
    SystemsHelper systems;
    ai::Conversation runner(systems.systems());
    runner.setConversation(conversation);
    EXPECT_TRUE(runner.finished());
}

TEST(Conversation, Linear) {
    bl::logging::Config::configureOutput(std::cout, bl::logging::Config::Error);

    file::Conversation conversation;
    file::Conversation::Node node;
    node.setType(file::Conversation::Node::Talk);

    node.next()    = 1;
    node.message() = "one";
    conversation.appendNode(node);

    // intentionally let next be set by append
    node.message() = "two";
    conversation.insertNode(1, node); // should call append

    node.next()    = 3;
    node.message() = "three";
    conversation.appendNode(node);

    SystemsHelper systems;
    ai::Conversation runner(systems.systems());
    runner.setConversation(conversation);

    ASSERT_FALSE(runner.finished());
    EXPECT_EQ(runner.currentNode().message(), "one");
    runner.notifyNext();

    ASSERT_FALSE(runner.finished());
    EXPECT_EQ(runner.currentNode().message(), "two");
    runner.notifyNext();

    ASSERT_FALSE(runner.finished());
    EXPECT_EQ(runner.currentNode().message(), "three");
    runner.notifyNext();
    EXPECT_TRUE(runner.finished());
}

TEST(Conversation, CheckPassFail) {
    bl::logging::Config::configureOutput(std::cout, bl::logging::Config::Error);

    file::Conversation conversation;
    file::Conversation::Node node;
    node.setType(file::Conversation::Node::Talk);

    node.next()    = 1;
    node.message() = "one";
    conversation.appendNode(node);

    node.setType(file::Conversation::Node::TakeItem);
    node.message()      = "check";
    node.nextOnPass()   = 2;
    node.nextOnReject() = 4;
    conversation.appendNode(node);

    node.next()    = 3;
    node.message() = "three";
    conversation.appendNode(node);

    SystemsHelper systems;
    ai::Conversation runner(systems.systems());
    runner.setConversation(conversation);

    ASSERT_FALSE(runner.finished());
    EXPECT_EQ(runner.currentNode().message(), "one");
    runner.notifyNext();

    ASSERT_FALSE(runner.finished());
    EXPECT_EQ(runner.currentNode().message(), "check");
    runner.notifyCheckPassed();

    ASSERT_FALSE(runner.finished());
    EXPECT_EQ(runner.currentNode().message(), "three");
    runner.notifyNext();
    EXPECT_TRUE(runner.finished());

    // reset and check fail
    runner.setConversation(conversation);

    ASSERT_FALSE(runner.finished());
    EXPECT_EQ(runner.currentNode().message(), "one");
    runner.notifyNext();

    ASSERT_FALSE(runner.finished());
    EXPECT_EQ(runner.currentNode().message(), "check");
    runner.notifyCheckFailed();

    ASSERT_TRUE(runner.finished());
}

TEST(Conversation, Choices) {
    bl::logging::Config::configureOutput(std::cout, bl::logging::Config::Error);

    file::Conversation conversation;
    file::Conversation::Node node;

    node.setType(file::Conversation::Node::Prompt);
    node.message() = "question";
    node.choices() = {std::make_pair("yes", 1), std::make_pair("no", 2)};
    conversation.appendNode(node);

    node.setType(file::Conversation::Node::Talk);
    node.message() = "cont";
    node.next()    = 2;
    conversation.appendNode(node);

    SystemsHelper systems;
    ai::Conversation runner(systems.systems());
    runner.setConversation(conversation);

    ASSERT_FALSE(runner.finished());
    EXPECT_EQ(runner.currentNode().message(), "question");

    runner.notifyChoiceMade("yes");
    ASSERT_FALSE(runner.finished());
    EXPECT_EQ(runner.currentNode().message(), "cont");

    runner.notifyNext();
    EXPECT_TRUE(runner.finished());

    // reset and test other choice
    runner.setConversation(conversation);
    ASSERT_FALSE(runner.finished());
    EXPECT_EQ(runner.currentNode().message(), "question");

    runner.notifyChoiceMade("no");
    EXPECT_TRUE(runner.finished());
}

} // namespace tests

#endif
