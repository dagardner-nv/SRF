/**
 * SPDX-FileCopyrightText: Copyright (c) 2021-2022, NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#pragma once

#include "internal/resources/partition_resources_base.hpp"

#include <memory>

namespace mrc::internal::network {
class Resources;
}  // namespace mrc::internal::network

namespace mrc::internal::control_plane {

class Client;
class Server;

class Resources final : private resources::PartitionResourceBase
{
  public:
    Resources(resources::PartitionResourceBase& base);
    ~Resources() final;

    Client& client()
    {
        return *m_client;
    }

  private:
    std::unique_ptr<Server> m_server;
    const std::unique_ptr<Client> m_client;

    friend network::Resources;
};

}  // namespace mrc::internal::control_plane
