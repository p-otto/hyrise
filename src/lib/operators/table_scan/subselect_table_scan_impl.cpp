#include "subselect_table_scan_impl.hpp"

#include "single_column_table_scan_impl.hpp"

#include <memory>
#include <string>
#include <type_traits>

#include "storage/chunk.hpp"
#include "storage/iterables/create_iterable_from_column.hpp"
#include "storage/table.hpp"

#include "utils/assert.hpp"

#include <logical_query_plan/lqp_translator.hpp>
#include <scheduler/operator_task.hpp>

#include "resolve_type.hpp"
#include "type_comparison.hpp"

namespace opossum {

std::shared_ptr<OperatorTask> schedule_query_and_return_task(std::shared_ptr<AbstractOperator> op) {
  auto tasks = OperatorTask::make_tasks_from_operator(op);
  for (auto& task : tasks) {
    task->schedule();
  }
  return tasks.back();
}

SubselectTableScanImpl::SubselectTableScanImpl(std::shared_ptr<const Table> in_table,
                                                             const ColumnID left_column_id, const ScanType& scan_type,
                                                             std::shared_ptr<AbstractLQPNode> subselect_node)
    : BaseTableScanImpl{in_table, left_column_id, scan_type}, _subselect_node{subselect_node} {}

PosList SubselectTableScanImpl::scan_chunk(ChunkID chunk_id) {
    std::cout << "TableScan on subselect is executed..." << std::endl;

    LQPTranslator translator;
    auto subselect_result = translator.translate_node(_subselect_node);

    auto result_table = schedule_query_and_return_task(subselect_result)->get_operator()->get_output();

    // TODO: this could be slow and only works with floats for now
    const auto compare_value = result_table->get_value<float>(_left_column_id, 0);

    SingleColumnTableScanImpl impl(_in_table, _left_column_id, _scan_type, AllTypeVariant(compare_value));
    return impl.scan_chunk(chunk_id);
}

}  // namespace opossum
