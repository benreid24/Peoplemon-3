#include <Core/AI/Conversation.hpp>
#include <Core/Systems/Systems.hpp>
#include <SystemsHelper.hpp>
#include <gtest/gtest.h>

using namespace core;

namespace tests
{
// TODO - test jump updates on insert and delete

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

} // namespace tests
