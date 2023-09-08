/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "common.hpp"

#include "internal/control_plane/client.hpp"
#include "internal/control_plane/client/connections_manager.hpp"
#include "internal/control_plane/client/instance.hpp"
#include "internal/control_plane/server.hpp"
#include "internal/network/network_resources.hpp"
#include "internal/resources/manager.hpp"
#include "internal/resources/partition_resources.hpp"
#include "internal/runnable/runnable_resources.hpp"
#include "internal/runtime/partition.hpp"
#include "internal/runtime/runtime.hpp"
#include "internal/system/partitions.hpp"
#include "internal/system/system.hpp"
#include "internal/system/system_provider.hpp"

#include "mrc/codable/fundamental_types.hpp"  // IWYU pragma: keep
#include "mrc/core/task_queue.hpp"
#include "mrc/core/userspace_threads.hpp"
#include "mrc/memory/literals.hpp"
#include "mrc/options/options.hpp"
#include "mrc/options/placement.hpp"
#include "mrc/options/topology.hpp"
#include "mrc/pubsub/api.hpp"
#include "mrc/pubsub/forward.hpp"
#include "mrc/pubsub/publisher.hpp"
#include "mrc/pubsub/subscriber.hpp"
#include "mrc/types.hpp"

#include <boost/fiber/future/future.hpp>
#include <gtest/internal/gtest-internal.h>

#include <cassert>
#include <chrono>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <ostream>
#include <thread>
#include <utility>
#include <vector>

using namespace mrc;
using namespace mrc::pubsub;
using namespace mrc::memory::literals;

static auto make_runtime(std::function<void(Options& options)> options_lambda = [](Options& options) {})
{
    auto resources = std::make_unique<resources::Manager>(
        system::SystemProvider(tests::make_system([&](Options& options) {
            options.topology().user_cpuset("0-3");
            options.topology().restrict_gpus(true);
            options.placement().resources_strategy(PlacementResources::Dedicated);
            options.placement().cpu_strategy(PlacementStrategy::PerMachine);
            options_lambda(options);
        })));

    return std::make_unique<runtime::Runtime>(std::move(resources));
}

void single_client_connect_disconnect()
{
    auto sr     = make_runtime();
    auto server = std::make_unique<control_plane::Server>(sr->partition(0).resources().runnable());

    server->service_start();
    server->service_await_live();

    auto cr = make_runtime([](Options& options) {
        options.architect_url("localhost:13337");
        options.topology().user_cpuset("4");
    });

    // the total number of partition is system dependent
    auto expected_partitions = cr->resources().system().partitions().flattened().size();
    auto actual_partitions =
        cr->partition(0).resources().network()->control_plane().client().connections().instance_ids().size();
    assert(actual_partitions == expected_partitions);

    // destroying the resources should gracefully shutdown the data plane and the control plane.
    cr.reset();

    server->service_stop();
    server->service_await_join();
}

int main(int argc, char** argv)
{
    std::cout << "Promise<bool> : " << sizeof(Promise<bool>) << std::endl;
    std::cout << "Promise<bool>* : " << sizeof(Promise<bool>*) << std::endl;

    std::cout << "Running single_client_connect_disconnect()" << std::endl;
    single_client_connect_disconnect();
    std::cout << "Done" << std::endl;
    return 0;
}
