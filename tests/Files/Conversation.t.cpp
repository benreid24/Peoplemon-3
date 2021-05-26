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

    ASSERT_EQ(conversation.nodes().size(), 3);
    EXPECT_EQ(conversation.nodes().at(0).message(), "one");
    EXPECT_EQ(conversation.nodes().at(0).next(), 1);
    EXPECT_EQ(conversation.nodes().at(1).message(), "two");
    EXPECT_EQ(conversation.nodes().at(1).next(), 2);
    EXPECT_EQ(conversation.nodes().at(2).message(), "three");
    EXPECT_EQ(conversation.nodes().at(2).next(), 1);
}

TEST(Conversation, BasicInsert) {
    Conversation conversation;
    Conversation::Node node;

    node.setType(Conversation::Node::Talk);
    node.next() = 1;

    node.message() = "one";
    conversation.appendNode(node);
    node.message() = "two";
    conversation.appendNode(node);
    node.message() = "three";
    node.next()    = 3;
    conversation.appendNode(node);

    node.message() = "inserted";
    node.next()    = 2;
    conversation.insertNode(1, node);

    ASSERT_EQ(conversation.nodes().size(), 4);
    EXPECT_EQ(conversation.nodes().at(0).message(), "one");
    EXPECT_EQ(conversation.nodes().at(0).next(), 1);

    EXPECT_EQ(conversation.nodes().at(1).message(), "inserted");
    EXPECT_EQ(conversation.nodes().at(1).next(), 2);

    EXPECT_EQ(conversation.nodes().at(2).message(), "two");
    EXPECT_EQ(conversation.nodes().at(2).next(), 3);
    EXPECT_EQ(conversation.nodes().at(3).message(), "three");
    EXPECT_EQ(conversation.nodes().at(3).next(), 4);
}

TEST(Conversation, InsertWithJump) {
    Conversation conversation;
    Conversation::Node node;

    node.setType(Conversation::Node::Talk);

    node.message() = "one";
    conversation.appendNode(node);
    node.message() = "two";
    conversation.appendNode(node);
    node.message() = "three";
    node.next()    = 3;
    conversation.appendNode(node);

    node.message() = "one";
    node.next()    = 2;
    conversation.setNode(0, node);

    node.message() = "inserted";
    node.next()    = 2;
    conversation.insertNode(1, node);

    ASSERT_EQ(conversation.nodes().size(), 4);
    EXPECT_EQ(conversation.nodes().at(0).message(), "one");
    EXPECT_EQ(conversation.nodes().at(0).next(), 3);

    EXPECT_EQ(conversation.nodes().at(1).message(), "inserted");
    EXPECT_EQ(conversation.nodes().at(1).next(), 2);

    EXPECT_EQ(conversation.nodes().at(2).message(), "two");
    EXPECT_EQ(conversation.nodes().at(2).next(), 3);
    EXPECT_EQ(conversation.nodes().at(3).message(), "three");
    EXPECT_EQ(conversation.nodes().at(3).next(), 4);
}

TEST(Conversation, BasicDelete) {
    Conversation conversation;
    Conversation::Node node;

    node.setType(Conversation::Node::Talk);
    node.next() = 1;

    node.message() = "one";
    conversation.appendNode(node);
    node.message() = "two";
    conversation.appendNode(node);
    node.message() = "three";
    node.next()    = 3;
    conversation.appendNode(node);

    ASSERT_EQ(conversation.nodes().size(), 3);
    EXPECT_EQ(conversation.nodes().at(0).next(), 1);
    EXPECT_EQ(conversation.nodes().at(1).next(), 2);
    EXPECT_EQ(conversation.nodes().at(2).next(), 3);

    conversation.deleteNode(1);

    ASSERT_EQ(conversation.nodes().size(), 2);
    EXPECT_EQ(conversation.nodes().at(0).message(), "one");
    EXPECT_EQ(conversation.nodes().at(0).next(), 1);
    EXPECT_EQ(conversation.nodes().at(1).message(), "three");
    EXPECT_EQ(conversation.nodes().at(1).next(), 2);
}

TEST(Conversation, DeleteConnectJump) {
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
    node.message() = "four";
    node.next()    = 4;
    conversation.appendNode(node);

    node.message() = "one";
    node.next()    = 2;
    conversation.setNode(0, node);

    conversation.deleteNode(2);

    ASSERT_EQ(conversation.nodes().size(), 3);
    EXPECT_EQ(conversation.nodes().at(0).message(), "one");
    EXPECT_EQ(conversation.nodes().at(0).next(), 2);
    EXPECT_EQ(conversation.nodes().at(1).message(), "two");
    EXPECT_EQ(conversation.nodes().at(1).next(), 2);
    EXPECT_EQ(conversation.nodes().at(2).message(), "four");
    EXPECT_EQ(conversation.nodes().at(2).next(), 3);
}

TEST(Conversation, DeleteInvalidateJump) {
    Conversation conversation;
    Conversation::Node node;

    node.setType(Conversation::Node::Talk);
    node.next() = 1;

    node.message() = "one";
    conversation.appendNode(node);
    node.message() = "two";
    conversation.appendNode(node);

    node.setType(Conversation::Node::TakeItem);
    node.message() = "three";
    conversation.appendNode(node);

    node.setType(Conversation::Node::Talk);
    node.message() = "four";
    node.next()    = 4;
    conversation.appendNode(node);

    node.message() = "one";
    node.next()    = 2;
    conversation.setNode(0, node);

    conversation.deleteNode(2);

    ASSERT_EQ(conversation.nodes().size(), 3);
    EXPECT_EQ(conversation.nodes().at(0).message(), "one");
    EXPECT_GE(conversation.nodes().at(0).next(), conversation.nodes().size());
    EXPECT_EQ(conversation.nodes().at(1).message(), "two");
    EXPECT_GE(conversation.nodes().at(1).next(), conversation.nodes().size());
    EXPECT_EQ(conversation.nodes().at(2).message(), "four");
    EXPECT_EQ(conversation.nodes().at(2).next(), 3);
}

} // namespace tests
