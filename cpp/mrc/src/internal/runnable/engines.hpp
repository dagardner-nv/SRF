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

#include "mrc/runnable/engine.hpp"  // IWYU pragma: export
#include "mrc/runnable/launch_options.hpp"

#include <cstddef>
#include <memory>
#include <vector>

namespace mrc::runnable {
enum class EngineType;
}  // namespace mrc::runnable

namespace mrc::internal::runnable {

using ::mrc::runnable::EngineType;
using ::mrc::runnable::LaunchOptions;

class Engines : public ::mrc::runnable::Engines
{
  public:
    Engines(LaunchOptions launch_options);
    ~Engines() override;

    const std::vector<std::shared_ptr<::mrc::runnable::Engine>>& launchers() const final;
    const LaunchOptions& launch_options() const final;
    std::size_t size() const final;

  protected:
    void add_launcher(std::shared_ptr<::mrc::runnable::Engine> launcher);
    void clear_launchers();

  private:
    LaunchOptions m_launch_options;
    std::vector<std::shared_ptr<::mrc::runnable::Engine>> m_launchers;
};

}  // namespace mrc::internal::runnable
