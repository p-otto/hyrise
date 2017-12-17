#pragma once

#include <memory>

#include "base_table_scan_impl.hpp"

#include "types.hpp"

namespace opossum {

class Table;

/**
 * @brief Compares a column and a subselect to each other
 *
 * TODO: comment
 *
 */
class SubselectTableScanImpl : public BaseTableScanImpl {
 public:
  SubselectTableScanImpl(std::shared_ptr<const Table> in_table, const ColumnID left_column_id,
                                const ScanType& scan_type, std::shared_ptr<AbstractLQPNode> subselect_node);

  PosList scan_chunk(ChunkID chunk_id) override;

 private:
  std::shared_ptr<AbstractLQPNode> _subselect_node;
};

}  // namespace opossum
