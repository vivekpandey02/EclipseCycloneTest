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

int main() {
    int result = 0;
    try
    {
        /** A domain participant and topic are created identically as in
         the ::publisher */
        dds::domain::DomainParticipant dp(domain::default_id());
        dds::topic::qos::TopicQos topicQos = dp.default_topic_qos()       
                                                    << dds::core::policy::Reliability::Reliable()
						    << dds::core::policy::Durability::TransientLocal();
        dds::topic::Topic<TestCommand::TestCommandMsg> topic(dp, "TestCommand_msg", topicQos);

        /** A dds::sub::Subscriber is created on the domain participant. */
        std::string name = "Command Partition";
        dds::sub::qos::SubscriberQos subQos
            = dp.default_subscriber_qos()
                << dds::core::policy::Partition(name);
        dds::sub::Subscriber sub(dp, subQos);

        /** The dds::sub::qos::DataReaderQos are derived from the topic qos */
        dds::sub::qos::DataReaderQos drqos = topic.qos();

        /** A dds::sub::DataReader is created on the Subscriber & Topic with the DataReaderQos. */
        dds::sub::DataReader<TestCommand::TestCommandMsg> dr(sub, topic, drqos);

        /** An attempt to take samples is made repeatedly until it succeeds,
         * or sixty seconds have elapsed. */
        bool sampleReceived = false;
        int count = 0;
        do
        {
            dds::sub::LoanedSamples<TestCommand::TestCommandMsg> samples = dr.take();
            for (dds::sub::LoanedSamples<TestCommand::TestCommandMsg>::const_iterator sample = samples.begin();
                 sample < samples.end();
                 ++sample)
            {
                if (sample->info().valid())
                {
                    std::cout << "=== [Subscriber] message received :" << std::endl;
                    std::cout << "    ID  : " << sample->data().command_Id() << std::endl;
                    sampleReceived = true;
                }
            }
            sleep(1);
            ++count;
        }
        while (!sampleReceived && count < 600);

        if (!sampleReceived)
        {
            std::cerr << "ERROR: Waited for 60 seconds but no sample received" << std::endl;
            result = 1;
        }
    }
    catch (const dds::core::Exception& e)
    {
        std::cerr << "ERROR: Exception: " << e.what() << std::endl;
        result = 1;
    }
    return result;
}
