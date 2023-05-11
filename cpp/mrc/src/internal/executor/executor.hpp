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

#pragma once

#include "internal/service.hpp"
#include "internal/system/system_provider.hpp"

#include <memory>

namespace mrc {
class Options;
}  // namespace mrc
namespace mrc::pipeline {
class IPipeline;
class Manager;
}  // namespace mrc::pipeline
namespace mrc::resources {
class Manager;
}  // namespace mrc::resources
namespace mrc::system {
class ThreadingResources;
}  // namespace mrc::system

namespace mrc::executor {

/**
 * @brief Common Executor code used by both the Standalone and Architect Executors
 *
 * Issues #149 will begin to separate some of the functionality of ExeuctorBase into individual components.
 */
class Executor : public Service, public system::SystemProvider
{
  public:
    Executor(std::shared_ptr<Options> options);
    Executor(std::unique_ptr<system::ThreadingResources> resources);
    ~Executor() override;

    void register_pipeline(std::unique_ptr<pipeline::IPipeline> ipipeline);

  private:
    void do_service_start() final;
    void do_service_stop() final;
    void do_service_kill() final;
    void do_service_await_live() final;
    void do_service_await_join() final;

    std::unique_ptr<resources::Manager> m_resources_manager;
    std::unique_ptr<pipeline::Manager> m_pipeline_manager;
};

std::unique_ptr<Executor> make_executor(std::shared_ptr<Options> options);

std::unique_ptr<Executor> make_executor(std::unique_ptr<system::ThreadingResources> resources);

}  // namespace mrc::executor
