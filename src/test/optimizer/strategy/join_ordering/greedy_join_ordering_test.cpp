#include "join_reordering_base_test.hpp"

#include "optimizer/strategy/join_ordering/greedy_join_ordering.hpp"

namespace opossum {

class GreedyJoinOrderingTest : public JoinReorderingBaseTest {
 public:
};

TEST_F(GreedyJoinOrderingTest, BasicChainGraph) {
  /**
   * The chain equi-JoinGraph {C, D, E} should result in this JoinPlan:
   *
   *         ___Join (D.a == E.a)___
   *        /                       \
   *   ___Join (D.a == C.a)___       E
   *  /                       \
   * D                        C
   *
   * Reasoning: D is the smallest table and has the same overlapping range with C and E, but C is way smaller than E,
   * that is why C is added secondly and E afterwards
   */

  auto plan = GreedyJoinOrdering(_join_graph_cde_chain).run();

  ASSERT_INNER_JOIN_NODE(plan, ScanType::OpEquals, ColumnID{0}, ColumnID{0});
  ASSERT_INNER_JOIN_NODE(plan->left_child(), ScanType::OpEquals, ColumnID{0}, ColumnID{0});
  ASSERT_EQ(plan->right_child(), _table_node_e);
  ASSERT_EQ(plan->left_child()->left_child(), _table_node_d);
  ASSERT_EQ(plan->left_child()->right_child(), _table_node_c);
}

TEST_F(GreedyJoinOrderingTest, BasicCliqueGraph) {
  /**
   * The clique equi-JoinGraph {B, C, D} should result in this JoinPlan:
   *
   *          Predicate(? == B.a)
   *                   |
   *         ___Join (? == B.a)___
   *        /                     \
   *   ___Join (D.a == C.a)___     B
   *  /                       \
   * D                        C
   *
   * Reasoning: D is the smallest table and has the same overlapping range with C and E, but C is way smaller than E,
   * that is why it is added secondly and B afterwards.
   * For merging B there will be 2 candidate edges whose predicates both need to be fullfilled. In order not to rely too
   * much on details of the cost model, we're not testing which edge is fulfilled by the Predicate and which by the
   * Join, but just that both are fulfilled
   */

  auto plan = GreedyJoinOrdering(_join_graph_bcd_clique).run();

  plan->print();

  ASSERT_INNER_JOIN_NODE(plan, ScanType::OpEquals, ColumnID{0}, ColumnID{0});
  ASSERT_INNER_JOIN_NODE(plan->left_child(), ScanType::OpEquals, ColumnID{0}, ColumnID{0});
  ASSERT_EQ(plan->right_child(), _table_node_e);
  ASSERT_EQ(plan->left_child()->left_child(), _table_node_d);
  ASSERT_EQ(plan->left_child()->right_child(), _table_node_c);
}
}