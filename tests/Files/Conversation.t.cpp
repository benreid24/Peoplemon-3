#include <Core/Files/Conversation.hpp>
#include <gtest/gtest.h>

using namespace core::file;

namespace tests
{
TEST(Conversation, BasicAppend) {
    Conversation conversation;
    Conversation::Node node;

    node.setType(Conversation::Node::Talk);
    node.next() = 1;

    node.message() = "one";
    conversation.appendNode(node);
    node.message() = "two";
    conversation.appendNode(node);
    node.message() = "three";
    conversation.appendNode(node);

    ASSERT_EQ(conversation.nodes().size(), 3u);
    EXPECT_EQ(conversation.nodes().at(0).message(), "one");
    EXPECT_EQ(conversation.nodes().at(0).next(), 1u);
    EXPECT_EQ(conversation.nodes().at(1).message(), "two");
    EXPECT_EQ(conversation.nodes().at(1).next(), 1u);
    EXPECT_EQ(conversation.nodes().at(2).message(), "three");
    EXPECT_EQ(conversation.nodes().at(2).next(), 1u);
}

TEST(Conversation, BasicDelete) {
    Conversation conversation;
    Conversation::Node node;

    node.setType(Conversation::Node::Talk);
    node.next() = 1;

    node.message() = "one";
    node.next()    = 1;
    conversation.appendNode(node);
    node.message() = "two";
    node.next()    = 2;
    conversation.appendNode(node);
    node.message() = "three";
    node.next()    = 3;
    conversation.appendNode(node);

    ASSERT_EQ(conversation.nodes().size(), 3u);
    EXPECT_EQ(conversation.nodes().at(0).next(), 1u);
    EXPECT_EQ(conversation.nodes().at(1).next(), 2u);
    EXPECT_EQ(conversation.nodes().at(2).next(), 3u);

    conversation.deleteNode(1);

    ASSERT_EQ(conversation.nodes().size(), 2u);
    EXPECT_EQ(conversation.nodes().at(0).message(), "one");
    EXPECT_EQ(conversation.nodes().at(0).next(), 1u);
    EXPECT_EQ(conversation.nodes().at(1).message(), "three");
    EXPECT_EQ(conversation.nodes().at(1).next(), 2u);
}

TEST(Conversation, DeleteConnectJump) {
    Conversation conversation;
    Conversation::Node node;

    node.setType(Conversation::Node::Talk);
    node.next() = 1;

    node.message() = "one";
    node.next()    = 1;
    conversation.appendNode(node);
    node.message() = "two";
    node.next()    = 2;
    conversation.appendNode(node);
    node.message() = "three";
    node.next()    = 3;
    conversation.appendNode(node);
    node.message() = "four";
    node.next()    = 4;
    conversation.appendNode(node);

    node.message() = "one";
    node.next()    = 2;
    conversation.setNode(0, node);

    conversation.deleteNode(2);

    ASSERT_EQ(conversation.nodes().size(), 3u);
    EXPECT_EQ(conversation.nodes().at(0).message(), "one");
    EXPECT_EQ(conversation.nodes().at(0).next(), 2u);
    EXPECT_EQ(conversation.nodes().at(1).message(), "two");
    EXPECT_EQ(conversation.nodes().at(1).next(), 2u);
    EXPECT_EQ(conversation.nodes().at(2).message(), "four");
    EXPECT_EQ(conversation.nodes().at(2).next(), 3u);
}

TEST(Conversation, DeleteInvalidateJump) {
    Conversation conversation;
    Conversation::Node node;

    node.setType(Conversation::Node::Talk);
    node.next() = 1;

    node.message() = "one";
    node.next()    = 1;
    conversation.appendNode(node);
    node.message() = "two";
    node.next()    = 2;
    conversation.appendNode(node);

    node.setType(Conversation::Node::TakeItem);
    node.message() = "three";
    node.next()    = 3;
    conversation.appendNode(node);

    node.setType(Conversation::Node::Talk);
    node.message() = "four";
    node.next()    = 4;
    conversation.appendNode(node);

    node.message() = "one";
    node.next()    = 2;
    conversation.setNode(0, node);

    conversation.deleteNode(2);

    ASSERT_EQ(conversation.nodes().size(), 3u);
    EXPECT_EQ(conversation.nodes().at(0).message(), "one");
    EXPECT_GE(conversation.nodes().at(0).next(), conversation.nodes().size());
    EXPECT_EQ(conversation.nodes().at(1).message(), "two");
    EXPECT_GE(conversation.nodes().at(1).next(), conversation.nodes().size());
    EXPECT_EQ(conversation.nodes().at(2).message(), "four");
    EXPECT_EQ(conversation.nodes().at(2).next(), 3u);
}

} // namespace tests
