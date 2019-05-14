/** @file

   Red/Black tree.

   The primary purpose of this class is to provide access to structural changes in the
   tree so that subtree data can be maintained.
*/
/* Licensed to the Apache Software Foundation (ASF) under one or more contributor license agreements.
   See the NOTICE file distributed with this work for additional information regarding copyright
   ownership.  The ASF licenses this file to you under the Apache License, Version 2.0 (the
   "License"); you may not use this file except in compliance with the License.  You may obtain a
   copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software distributed under the License
   is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
   or implied. See the License for the specific language governing permissions and limitations under
   the License.
 */

#pragma once

namespace swoc
{
namespace detail
{
  /** A node in a red/black tree.

      This class provides only the basic tree operations. The container must provide the search and
      decision logic. This enables this class to be a base class for templated nodes with much less
      code duplication.
  */
  struct RBNode {
    using self_type = RBNode; ///< self reference type

    /// Node colors
    enum class Color {
      RED,
      BLACK,
    };

    /// Directional constants
    enum class Direction {
      NONE,
      LEFT,
      RIGHT,
    };

    /// Default constructor.
    RBNode() = default;

    /// Force virtual destructor.
    virtual ~RBNode() {}

    /** Get the child in direction @a dir.
     *
     * @param d Child direction.
     * @return A pointer to the child node, or @c nullptr if no child in that direction.
     */
    self_type *child_at(Direction d) const;

    /** Get the direction of a child node @a n.
     *
     * @param n Child node.
     * @return Direction of the child if @a n is a child, @c Direction::NONE if not.
     */
    Direction direction_of(self_type *const &n) const;

    /// @return The color of the node.
    Color color() const;

    self_type *left_most_descendant() const;

    /** Reverse a direction
        @return @c LEFT if @a d is @c RIGHT, @c RIGHT if @a d is @c LEFT,
        @c NONE otherwise.
    */
    Direction flip(Direction d);

    /** Perform internal validation checks.
        @return 0 on failure, black height of the tree on success.
    */
    int validate();

    /** Rotate the subtree rooted at this node in the direction @a dir.
     *
     * @param dir Direction of rotatoin.
     * @return The root node after rotation.
     *
     * @a this node is rotated in to the position of its @a dir child and the child in the other
     * direction becomes the new root node for the subtree. This operation is a no-op if there
     * is no other child.
     *
     * If @a this node has a parent then that parent's child pointer is updated as needed.
     */
    self_type *rotate(Direction dir);

    /** Set the @a dir child of @a this node to @a child.
     *
     * @param child Node to set as the child.
     * @param dir Direction of the child location.
     * @return @a child
     *
     * The internal pointers in both @a this and @a child are updated as needed. It is an error
     * to call this if the specified child is already set.
     */
    self_type *set_child(self_type *child, Direction dir);

    /** Remove this node from the tree.
        The tree is rebalanced after removal.
        @return The new root node.
    */
    self_type *remove();

    /** Remove the @a dir child.
     *
     * @param dir Direction of child to remove.
     *
     * The pointers in @a this and the child are updated as needed. It is not an error if there
     * is no child in the direction @a dir.
     */
    void clear_child(Direction dir);

    /** @name Subclass hook methods */
    //@{
    /** Structural change notification.

        This method is called if the structure of the subtree rooted at this node was changed. This
        makes it possible to keep subtree information nodes synchronized with the state of the tree
        efficiently.

        This is intended a hook. The base method is empty so that subclasses are not required to
        override.

        @internal This is the primary reason for this class.
    */
    virtual void
    structure_fixup()
    {
    }

    /** Called from @c validate to perform any additional validation checks.
        Clients should chain this if they wish to perform additional checks.
        @return @c true if the validation is successful, @c false otherwise.
    */
    virtual bool structure_validate();
    //@}

    /** Replace @a this node with @a n.
     *
     * @param n Replacement node.
     *
     * Invariant: this is a non-order modifying replacement.
     */
    void replace_with(self_type *n);

    //! Rebalance the tree starting at this node
    /** The tree is rebalanced so that all of the invariants are
        true. The (potentially new) root of the tree is returned.

        @return The root node of the tree after the rebalance.
    */
    self_type *rebalance_after_insert();

    /** Rebalance the tree after a deletion.
        Called on the lowest modified node.
        @return The new root of the tree.
    */
    /** Rebalance the tree root at @a this node after a removal.
     *
     * @param c The color of the removed node.
     * @param d The direction of the removed node from @a this.
     * @return The new root node.
     *
     * Invariant: @a this is the parent of the removed node.
     */
    self_type *rebalance_after_remove(Color c, Direction dir);

    /** Invoke @c structure_fixup on @a this node and its parents to the root.
     *
     * @return @a The root node.
     *
     * @note This is used by the base class to inform subclasses of structure changes, starting
     * from a leaf node up to the root node.
     */
    self_type *ripple_structure_fixup();

    Color _color{Color::RED};    ///< node color
    self_type *_parent{nullptr}; ///< parent node (needed for rotations)
    self_type *_left{nullptr};   ///< left child
    self_type *_right{nullptr};  ///< right child
    self_type *_next{nullptr};   ///< Next node.
    self_type *_prev{nullptr};   ///< Previous node.
  };

  // --- Implementation ---

  inline auto
  RBNode::direction_of(self_type *const &n) const -> Direction
  {
    return (n == _left) ? Direction::LEFT : (n == _right) ? Direction::RIGHT : Direction::NONE;
  }

  inline RBNode::Color
  RBNode::color() const
  {
    return _color;
  }

  inline RBNode::Direction
  RBNode::flip(RBNode::Direction d)
  {
    return Direction::LEFT == d ? Direction::RIGHT : Direction::RIGHT == d ? Direction::LEFT : Direction::NONE;
  }

  inline void
  RBNode::clear_child(RBNode::Direction dir)
  {
    if (Direction::LEFT == dir)
      _left = nullptr;
    else if (Direction::RIGHT == dir)
      _right = nullptr;
  }

  inline bool
  RBNode::structure_validate()
  {
    return true;
  }

} /* namespace detail */

} // namespace swoc