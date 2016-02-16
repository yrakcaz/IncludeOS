// This file is a part of the IncludeOS unikernel - www.includeos.org
//
// Copyright 2015 Oslo and Akershus University College of Applied Sciences
// and Alfred Bratterud
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once
#ifndef FS_MEMDISK_HPP
#define FS_MEMDISK_HPP

#include <cstdint>
#include "disk_device.hpp"

namespace fs
{
  class MemDisk : public IDiskDevice
  {
  public:
    static const size_t SECTOR_SIZE = 512;
    
    MemDisk();
    
    virtual void 
    read_sector(block_t blk, on_read_func func) override;
    virtual void 
    read_sectors(block_t start, block_t cnt, on_read_func) override;
    
    virtual uint64_t size() const override;
    
  private:
    void*  image_start;
    void*  image_end;
  };
  
}

#endif
