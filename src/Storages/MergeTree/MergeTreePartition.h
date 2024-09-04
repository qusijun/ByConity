/*
 * Copyright 2016-2023 ClickHouse, Inc.
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


/*
 * This file may have been modified by Bytedance Ltd. and/or its affiliates (“ Bytedance's Modifications”).
 * All Bytedance's Modifications are Copyright (2023) Bytedance Ltd. and/or its affiliates.
 */

#pragma once

#include <common/types.h>
#include <Disks/IDisk.h>
#include <IO/WriteBuffer.h>
#include <Storages/KeyDescription.h>
#include <Core/Field.h>

namespace DB
{

class Block;
class MergeTreeMetaBase;
struct FormatSettings;
struct MergeTreeDataPartChecksums;
struct StorageInMemoryMetadata;

using StorageMetadataPtr = std::shared_ptr<const StorageInMemoryMetadata>;

/// This class represents a partition value of a single part and encapsulates its loading/storing logic.
struct MergeTreePartition
{
    Row value;

public:
    MergeTreePartition() = default;

    explicit MergeTreePartition(Row value_) : value(std::move(value_)) {}

    /// For month-based partitioning.
    explicit MergeTreePartition(UInt32 yyyymm) : value(1, yyyymm) {}

    String getID(const MergeTreeMetaBase & storage) const;
    String getID(const Block & partition_key_sample, bool extract_nullable_date_value) const;

    void serializeText(const MergeTreeMetaBase & storage, WriteBuffer & out, const FormatSettings & format_settings) const;

    void load(const MergeTreeMetaBase & storage, const DiskPtr & disk, const String & part_path);
    void load(const MergeTreeMetaBase & storage, ReadBuffer & buf);
    void store(const MergeTreeMetaBase & storage, const DiskPtr & disk, const String & part_path, MergeTreeDataPartChecksums & checksums) const;
    void store(const Block & partition_key_sample, const DiskPtr & disk, const String & part_path, MergeTreeDataPartChecksums & checksums) const;
    void store(const MergeTreeMetaBase & storage, WriteBuffer & buf) const;

    void assign(const MergeTreePartition & other) { value = other.value; }

    void create(const StorageMetadataPtr & metadata_snapshot, Block block, size_t row, ContextPtr context);

    /// Adjust partition key and execute its expression on block. Return sample block according to used expression.
    static NamesAndTypesList executePartitionByExpression(const StorageMetadataPtr & metadata_snapshot, Block & block, ContextPtr context);

    /// Make a modified partition key with substitution from modulo to moduloLegacy. Used in paritionPruner.
    static KeyDescription adjustPartitionKey(const StorageMetadataPtr & metadata_snapshot, ContextPtr context);

    /** ----------------------- COMPATIBLE CODE BEGIN-------------------------- */
    /*  compatible with old metastore. remove this later  */
    void read(const MergeTreeMetaBase & storage, ReadBuffer & buffer);
    /*  -----------------------  COMPATIBLE CODE END -------------------------- */
};

}
