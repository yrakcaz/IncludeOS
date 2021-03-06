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
#ifndef VIRTIO_BLOCK_HPP
#define VIRTIO_BLOCK_HPP

#include <common>
#include <hw/disk_device.hpp>
#include <hw/pci_device.hpp>
#include "virtio.hpp"
//#include <delegate>

/** Virtio-net device driver.  */
class VirtioBlk : public Virtio, public hw::IDiskDevice
{
public:
  static constexpr size_t SECTOR_SIZE = 512;
  
  /** Human readable name. */
  virtual const char* name() const noexcept override
  {
    return "VirtioBlk";
  }
  
  // returns the optimal block size for this device
  virtual block_t block_size() const noexcept override
  {
    return SECTOR_SIZE; // some multiple of sector size
  }
  
  virtual void read(block_t blk, on_read_func func) override;
  
  virtual void read(block_t, block_t, on_read_func cb) override
  {
    cb(buffer_t());
  }
  
  virtual buffer_t read_sync(block_t blk) override;
  
  virtual block_t size() const noexcept override
  {
    return config.capacity;
  }
  
  /** Constructor. @param pcidev an initialized PCI device. */
  VirtioBlk(hw::PCI_Device& pcidev);
  
private:
  struct virtio_blk_geometry_t
  {
    uint16_t cyls;
    uint8_t  heds;
    uint8_t  sect;
  } __attribute__((packed));
  
  struct virtio_blk_config_t
  {
    uint64_t capacity;
    uint32_t size_max;
    uint32_t seg_max;
    virtio_blk_geometry_t geometry;
    uint32_t blk_size;           // Block size of device
    uint8_t physical_block_exp;  // Exponent for physical block per logical block
    uint8_t alignment_offset;    // Alignment offset in logical blocks
    uint16_t min_io_size;        // Minimum I/O size without performance penalty in logical blocks
    uint32_t opt_io_size;        // Optimal sustained I/O size in logical blocks    
  } __attribute__((packed));
  
  struct scsi_header_t
  {
    uint32_t type;
    uint32_t ioprio;
    uint64_t sector;
    /// SCSI ///
    //char* cmd = nullptr;
  } __attribute__((packed));
  struct blk_data_t
  {
    uint8_t sector[512];
    uint32_t stuff1;
    on_read_func* handler;
    uint32_t stuff2;
    uint8_t status;
  } __attribute__((packed));
  
  struct request_t
  {
    scsi_header_t hdr;
    blk_data_t    data;
  } __attribute__((packed));
  
  /** Get virtio PCI config. @see Virtio::get_config.*/
  void get_config();
  
  /** Service the RX Queue. 
      Push incoming data up to linklayer, dequeue RX buffers. */
  void service_RX();
  
  /** Service the TX Queue 
      Dequeue used TX buffers. @note: This function does not take any 
      responsibility for memory management. */
  void service_TX();
  
  /** Handle device IRQ. 
      
      Will look for config. changes and service RX/TX queues as necessary.*/
  void irq_handler();
  
  Virtio::Queue req;
  
  // configuration as read from paravirtual PCI device
  virtio_blk_config_t config;
  uint16_t request_counter;
};

#endif
