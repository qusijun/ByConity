#pragma once

#include <Common/config.h>

#if USE_AWS_S3
#include <Storages/StorageS3Settings.h>
#include <common/types.h>
#include <aws/s3/S3Client.h>
#include <Common/ErrorCodes.h>
#include <aws/s3/S3Errors.h>
#include <aws/s3/model/GetObjectResult.h>
#include <aws/s3/model/HeadObjectResult.h>
#include <aws/s3/model/HeadObjectRequest.h>

namespace DB::S3
{

struct ObjectInfo
{
    size_t size = 0;
    time_t last_modification_time = 0;

    std::map<String, String> metadata; /// Set only if getObjectInfo() is called with `with_metadata = true`.
};

ObjectInfo getObjectInfo(
    const Aws::S3::S3Client & client,
    const String & bucket,
    const String & key,
    const String & version_id = {},
    const S3Settings::ReadWriteSettings & request_settings = {},
    bool with_metadata = false,
    bool for_disk_s3 = false,
    bool throw_on_error = true);

size_t getObjectSize(
    const Aws::S3::S3Client & client,
    const String & bucket,
    const String & key,
    const String & version_id = {},
    const S3Settings::ReadWriteSettings & request_settings = {},
    bool for_disk_s3 = false,
    bool throw_on_error = true);

bool objectExists(
    const Aws::S3::S3Client & client,
    const String & bucket,
    const String & key,
    const String & version_id = {},
    const S3Settings::ReadWriteSettings & request_settings = {},
    bool for_disk_s3 = false);

/// Throws an exception if a specified object doesn't exist. `description` is used as a part of the error message.
void checkObjectExists(
    const Aws::S3::S3Client & client,
    const String & bucket,
    const String & key,
    const String & version_id = {},
    const S3Settings::ReadWriteSettings & request_settings = {},
    bool for_disk_s3 = false,
    std::string_view description = {});

bool isNotFoundError(Aws::S3::S3Errors error);

}

#endif
