#pragma once

#include <optional>
#include <memory>

#include "storage/encoded_columns/column_encoding_type.hpp"
#include "types.hpp"

namespace opossum {

class Table;

class TableGenerator {
 public:
  std::shared_ptr<Table> generate_table(const ChunkID chunk_size, std::optional<EncodingType> encoding_type = std::nullopt);

 protected:
  const size_t _num_columns = 10;
  const size_t _num_rows = 5 * 1000;
  const int _max_different_value = 1000;
};

}  // namespace opossum
