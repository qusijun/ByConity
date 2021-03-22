#pragma once

#include <Core/Block.h>
#include <DataStreams/IBlockOutputStream.h>
#include <Core/ExternalResultDescription.h>
#include <Parsers/IdentifierQuotingStyle.h>
#include <nanodbc/nanodbc.h>


namespace DB
{

class ODBCBlockOutputStream : public IBlockOutputStream
{

public:
    ODBCBlockOutputStream(
            nanodbc::connection & connection_,
            const std::string & remote_database_name_,
            const std::string & remote_table_name_,
            const Block & sample_block_,
            Context & context_,
            IdentifierQuotingStyle quoting);

    Block getHeader() const override;
    void write(const Block & block) override;

private:
    Poco::Logger * log;

    nanodbc::connection & connection;
    std::string db_name;
    std::string table_name;
    Block sample_block;
    Context & context;
    IdentifierQuotingStyle quoting;

    ExternalResultDescription description;
};

}
