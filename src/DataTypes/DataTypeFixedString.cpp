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

#include <Columns/ColumnFixedString.h>
#include <Columns/ColumnConst.h>

#include <Formats/FormatSettings.h>
#include <DataTypes/DataTypeFixedString.h>
#include <DataTypes/DataTypeFactory.h>
#include <DataTypes/Serializations/SerializationFixedString.h>

#include <IO/WriteBuffer.h>
#include <IO/ReadHelpers.h>
#include <IO/WriteHelpers.h>
#include <IO/ReadBufferFromString.h>
#include <IO/VarInt.h>

#include <Parsers/IAST.h>
#include <Parsers/ASTLiteral.h>

#include <Common/typeid_cast.h>
#include <Common/assert_cast.h>


namespace DB
{

namespace ErrorCodes
{
    extern const int NUMBER_OF_ARGUMENTS_DOESNT_MATCH;
    extern const int UNEXPECTED_AST_STRUCTURE;
}


std::string DataTypeFixedString::doGetName() const
{
    return "FixedString(" + toString(n) + ")";
}

MutableColumnPtr DataTypeFixedString::createColumn() const
{
    return ColumnFixedString::create(n);
}

Field DataTypeFixedString::getDefault() const
{
    return String();
}

bool DataTypeFixedString::equals(const IDataType & rhs) const
{
    return typeid(rhs) == typeid(*this) && n == static_cast<const DataTypeFixedString &>(rhs).n;
}

SerializationPtr DataTypeFixedString::doGetDefaultSerialization() const
{
    return std::make_shared<SerializationFixedString>(n);
}


static DataTypePtr create(const ASTPtr & arguments)
{
    if (!arguments || arguments->children.size() != 1)
        throw Exception("FixedString data type family must have exactly one argument - size in bytes", ErrorCodes::NUMBER_OF_ARGUMENTS_DOESNT_MATCH);

    const auto * argument = arguments->children[0]->as<ASTLiteral>();
    if (!argument || argument->value.getType() != Field::Types::UInt64 || argument->value.get<UInt64>() == 0)
        throw Exception("FixedString data type family must have a number (positive integer) as its argument", ErrorCodes::UNEXPECTED_AST_STRUCTURE);

    return std::make_shared<DataTypeFixedString>(argument->value.get<UInt64>());
}


void registerDataTypeFixedString(DataTypeFactory & factory)
{
    factory.registerDataType("FixedString", create);

    /// Compatibility alias.
    factory.registerAlias("BINARY", "FixedString", DataTypeFactory::CaseInsensitive);
}

Field DataTypeFixedString::stringToVisitorField(const String & ins) const
{
    return stringToVisitorString(ins);
}

String DataTypeFixedString::stringToVisitorString(const String & ins) const
{
    // This routine assume ins was generated by FieldVisitorToString, and should be Quoted.
    ReadBufferFromString rb(ins);

    String res;
    readQuoted(res, rb);
    return res;
}

}
