/*
 * This file may have been modified by Bytedance Ltd. and/or its affiliates (“ Bytedance's Modifications”).
 * All Bytedance's Modifications are Copyright (2023) Bytedance Ltd. and/or its affiliates.
 */

// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#pragma once

#include <cstddef>
#include <cstdint>

#include <Storages/IndexFile/Iterator.h>

namespace DB::IndexFile
{

struct BlockContents;
class Comparator;

// Decodes the blocks generated by block_builder.cc.
class Block
{
public:
    // Initialize the block with the specified contents.
    explicit Block(const BlockContents & contents);

    Block(const Block &) = delete;
    Block & operator=(const Block &) = delete;

    ~Block();

    size_t size() const { return size_; }
    Iterator * NewIterator(const Comparator * comparator);

private:
    class Iter;

    uint32_t NumRestarts() const;

    const char * data_;
    size_t size_;
    uint32_t restart_offset_; // Offset in data_ of restart array
    bool owned_; // Block owns data_[]
};

}
