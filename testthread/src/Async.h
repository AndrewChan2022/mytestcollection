//
// Copyright 2023-2024 Chen Kai
//
// This program is free software: you can redistribute it and/or modify it under the terms 
// of the GNU General Public License as published by the Free Software Foundation, either 
// version 3 of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with this program. 
// If not, see <https://www.gnu.org/licenses/>.
//

//
// Implement 
//


#pragma once


#include <functional>


namespace feynman::Utils {

/// async execute task with creating new thread
/// after finish job, delete the thread
void asyncWithNewThread(const std::function<void(void)>& op);

/// async execute task with pool of threads
void async(const std::function<void(void)>& op);

/// async execute task with one background thread
void asyncWithBackgroundQueue(const std::function<void(void)>& op);


} // namespace feynman