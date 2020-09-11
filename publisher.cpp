/*
 * Copyright(c) 2006 to 2020 ADLINK Technology Limited and others
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License v. 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0, or the Eclipse Distribution License
 * v. 1.0 which is available at
 * http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * SPDX-License-Identifier: EPL-2.0 OR BSD-3-Clause
 */
#include <cstdlib>
#include <iostream>
#include <chrono>
#include <thread>

/* Include the C++ DDS API. */
#include "dds/dds.hpp"

/* Include data type and specific traits to be used with the C++ DDS API. */
#include "TestCommand_DCPS.hpp"

using namespace org::eclipse::cyclonedds;

int main()
{
    int result = 0;
    try
    {
        /** A dds::domain::DomainParticipant is created for the default domain. */
        dds::domain::DomainParticipant dp(domain::default_id());

        /** The Durability::Transient policy is specified as a dds::topic::qos::TopicQos
         * so that even if the subscriber does not join until after the sample is written
         * then the DDS will still retain the sample for it. The Reliability::Reliable
         * policy is also specified to guarantee delivery. */
        dds::topic::qos::TopicQos topicQos
             = dp.default_topic_qos()
                << dds::core::policy::Reliability::Reliable()
                << dds::core::policy::Durability::TransientLocal();
                

        /** A dds::topic::Topic is created for our sample type on the domain participant. */
        dds::topic::Topic<TestCommand::TestCommandMsg> topic(dp, "TestCommand_msg", topicQos);

        /** A dds::pub::Publisher is created on the domain participant. */
        std::string name = "Command Partition";
        dds::pub::qos::PublisherQos pubQos
            = dp.default_publisher_qos()
                << dds::core::policy::Partition(name);
        dds::pub::Publisher pub(dp, pubQos);

        /** The dds::pub::qos::DataWriterQos is derived from the topic qos and the
         * WriterDataLifecycle::ManuallyDisposeUnregisteredInstances policy is
         * specified as an addition. This is so the publisher can optionally be run (and
         * exit) before the subscriber. It prevents the middleware default 'clean up' of
         * the topic instance after the writer deletion, this deletion implicitly performs
         * DataWriter::unregister_instance */
        dds::pub::qos::DataWriterQos dwqos = topic.qos();
       // dwqos << dds::core::policy::WriterDataLifecycle::ManuallyDisposeUnregisteredInstances();

        /** A dds::pub::DataWriter is created on the Publisher & Topic with the modififed Qos. */
        dds::pub::DataWriter<TestCommand::TestCommandMsg> dw(pub, topic, dwqos);

        /** A sample is created and then written. */
	TestCommand::KeyValue kv;
	kv.keyval("Storage");
	std::string str="This is my first string";
	kv.value().sValue(str);
	kv.value()._d(TestCommand::ValueKind::VALUEKIND_STRING);
        std::vector<TestCommand::KeyValue> vkv;
	vkv.push_back(kv);
        TestCommand::TestCommandMsg cmdInstance;
	cmdInstance.command_Id(1);
	cmdInstance.kind()._d(TestCommand::CommandKind::ADD_COMMAND);
	cmdInstance.kind().addCmd(vkv);

        dw << cmdInstance;

        std::cout << "=== [Publisher] writing a message containing :" << std::endl;
        /* A short sleep ensures time is allowed for the sample to be written to the network.
        If the example is running in *Single Process Mode* exiting immediately might
        otherwise shutdown the domain services before this could occur */
        sleep(100);
    }
    catch (const dds::core::Exception& e)
    {
        std::cerr << "ERROR: Exception: " << e.what() << std::endl;
        result = 1;
    }
    return result;
}
