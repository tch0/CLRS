#pragma once

#include <functional>
#include <iterator>
#include <memory>
#include <queue>
#include <type_traits>
#include <concepts>

template<typename Key, typename Value, typename KeyOfValue, bool Multi = false, typename Compare = std::less<Key>>
class RbTree
{
/*
attributes of red-black tree:
1. every node is either red or black
2. root node is always black
3. every leave node (NIL) is black
4. if a node is red, its children will be both black
5. for every node, the path to every offspring leave nodes contains the same number of black nodes.

Every inserted node is initialized to red, so only 2 and 4 may be broken, and it's either 2 or 4, not both.
breaking of 2: if inserted node is root.
breaking of 4: if the parent of inserted node is red.

implementation detail: Let a Nil node that painted to black be root's parent and leave nodes.
*/
private:
    enum NodeColor : unsigned char{ RED, BLACK };
    struct TreeNode
    {
        TreeNode(const Value& _data) : data(_data)
        {
        }
        TreeNode(Value&& _data) : data(std::move(_data))
        {
        }
        template<typename... Args>
        TreeNode(Args&&... args) : data(std::forward<Args>(args)...)
        {
        }
        TreeNode* left = nullptr;
        TreeNode* right = nullptr;
        TreeNode* parent = nullptr;
        Value data;
        NodeColor color = RED;
    };

    template<bool isConst>
    struct RbTreeIterator
    {
        friend class RbTree<Key, Value, KeyOfValue, Multi, Compare>;
    private:
        using BaseTreeType = RbTree<Key, Value, KeyOfValue, Multi, Compare>;
        using TreeType = std::conditional_t<isConst, const BaseTreeType, BaseTreeType>;
        using IterReference = std::conditional_t<isConst, Value&, const Value&>;
        using IterPointer = std::conditional_t<isConst, Value*, const Value*>;
    public:
        RbTreeIterator(TreeType* _tree = nullptr, TreeNode* _node = nullptr) : tree(_tree), node(_node)
        {
        }
        RbTreeIterator(const RbTreeIterator& other) : tree(other.tree), node(other.node)
        {
        }
        RbTreeIterator(const RbTreeIterator<false>& other) requires (isConst) // convert iterator to const_iterator
            : tree(other.tree)
            , node(other.node)
        {
        }
        IterReference operator*() const
        {
            return node->data;
        }
        IterPointer operator->() const
        {
            return &(node->data);
        }
        RbTreeIterator& operator++()
        {
            node = tree->successor(node);
            return *this;
        }
        RbTreeIterator operator++(int)
        {
            RbTreeIterator res(tree, node);
            ++*this;
            return res;
        }
        RbTreeIterator& operator--()
        {
            node = tree->predecessor(node);
            return *this;
        }
        RbTreeIterator operator--(int)
        {
            RbTreeIterator res(tree, node);
            --*this;
            return res;
        }
        bool operator==(const RbTreeIterator& other) const
        {
            return tree == other.tree && node == other.node;
        }
        RbTreeIterator& operator=(const RbTreeIterator& other)
        {
            tree = other.tree;
            node = other.node;
            return *this;
        }
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = Value;
        using size_type = std::size_t;
        using reference = IterReference;
        using pointer = IterPointer;
    private:
        TreeType* tree;
        TreeNode* node;
    };
public:
    using iterator = RbTreeIterator<false>;
    using const_iterator = RbTreeIterator<true>;
    static_assert(std::bidirectional_iterator<iterator>);
    static_assert(std::bidirectional_iterator<const_iterator>);
private:
    Compare m_keyCompare;
    TreeNode* m_root = nullptr;
    TreeNode* Nil = nullptr;
    std::size_t m_nodeCount = 0;
private:
    // node construction and destruction
    template<typename... Args>
    [[nodiscard]] TreeNode* constructNode(Args&&... args)
    {
        TreeNode* p = new TreeNode(std::forward<Args>(args)...);
        p->left = p->right = p->parent = Nil;
        return p;
    }
    void destroyNode(TreeNode* node)
    {
        if (node)
        {
            delete node;
        }
    }
    // every tree has an unique Nil node
    void initNil()
    {
        Nil = static_cast<TreeNode*>(operator new(sizeof(TreeNode)));
        Nil->left = Nil->right = Nil->parent = nullptr;
        Nil->color = BLACK;
    }
    void freeNil()
    {
        operator delete(Nil);
    }
    // inner operations, make sure input node is not nullptr
    static Key& key(TreeNode* x)
    {
        static KeyOfValue keyOfValue;
        return keyOfValue(x->data);
    }
    TreeNode* leftMost(TreeNode* node) const
    {
        while (node->left != Nil)
        {
            node = node->left;
        }
        return node;
    }
    TreeNode* rightMost(TreeNode* node) const
    {
        while (node->right != Nil)
        {
            node = node->right;
        }
        return node;
    }
    TreeNode* successor(TreeNode* node) const
    {
        if (node->right != Nil)
        {
            return leftMost(node->right);
        }
        TreeNode* par = node->parent;
        while (par != Nil && node == par->right)
        {
            node = par;
            par = par->parent;
        }
        return par; // Nil if node is the right most
    }
    TreeNode* predecessor(TreeNode* node) const
    {
        if (node == Nil)
        {
            return rightMost(m_root);
        }
        if (node->left != Nil)
        {
            return rightMost(node->left);
        }
        TreeNode* par = node->parent;
        while (par != Nil && node == par->left)
        {
            node = par;
            par = par->parent;
        }
        return par; // Nil if node is the left most
    }
    TreeNode* search(TreeNode* node, const Key& val) const
    {
        while (node != Nil)
        {
            if (m_keyCompare(key(node), val))
            {
                node = node->right;
            }
            else if (m_keyCompare(val, key(node)))
            {
                node = node->left;
            }
            else // val == key(node)
            {
                return node;
            }
        }
        return Nil;
    }
    // rotate
    // left rotate: make sure node is not Nil and node has right child
    //  node  to   y
    //   \        /
    //    y      node
    void leftRotate(TreeNode* node)
    {
        TreeNode* y = node->right;
        node->right = y->left;
        if (y->left != Nil)
        {
            y->left->parent = node;
        }
        y->parent = node->parent;
        if (node->parent == Nil) // node is root
        {
            m_root = y;
        }
        else if (node == node->parent->left)
        {
            node->parent->left = y;
        }
        else
        {
            node->parent->right = y;
        }
        y->left = node;
        node->parent = y;
    }
    // right rotate: make sure node is not Nil and node has left child
    //  node  to  y
    //  /          \_
    // y           node
    void rightRotate(TreeNode* node)
    {
        TreeNode* y = node->left;
        node->left = y->right;
        if (y->right != Nil)
        {
            y->right->parent = node;
        }
        y->parent = node->parent;
        if (node->parent == Nil) // node is root
        {
            m_root = y;
        }
        else if (node == node->parent->left)
        {
            node->parent->left = y;
        }
        else
        {
            node->parent->right = y;
        }
        y->right = node;
        node->parent = y;
    }
    // insert a new node, return existed node with same key or the input new node
    TreeNode* insertNode(TreeNode* newNode)
    {
        TreeNode* node = m_root;
        TreeNode* prev = Nil;
        while (node != Nil)
        {
            prev = node;
            if (m_keyCompare(key(node), key(newNode)))
            {
                node = node->right;
            }
            else if (m_keyCompare(key(newNode), key(node)))
            {
                node = node->left;
            }
            else // key(newNode) == key(node)
            {
                if constexpr (Multi)
                {
                    node = node->right;
                }
                else
                {
                    destroyNode(newNode);
                    return node;
                }
            }
        }
        newNode->parent = prev;
        if (prev == Nil) // root is Nil, the tree is empty
        {
            m_root = newNode;
        }
        else if (m_keyCompare(key(newNode), key(prev)))
        {
            prev->left = newNode;
        }
        else
        {
            prev->right = newNode;
        }
        newNode->color = RED;
        newNode->left = newNode->right = Nil;
        insertFixUp(newNode);
        m_root->parent = Nil;   // insert process may change m_root, m_root->parent should keep Nil all the time.
        m_nodeCount++;
        return newNode;
    }
    // fix up the attributes of Red-Black tree after inserting: make sure node is not nullptr or Nil
    void insertFixUp(TreeNode* node)
    {
        while (node->parent->color == RED) // root is always black, so node->parent is not root and has a parent for sure.
        {
            if (node->parent == node->parent->parent->left)
            {
                TreeNode* uncle = node->parent->parent->right; // node's uncle
                if (uncle->color == RED) // case 1: node's uncle is red
                {
                    // paint it's parent and uncle to black, paint it's grandparent to red, then go up to it's grandfather
                    node->parent->color = BLACK;
                    uncle->color = BLACK;
                    node->parent->parent->color = RED;
                    node = node->parent->parent;
                }
                else if (node == node->parent->right) // case 2: node's uncle is black, and node is a right child
                {
                    // go up to it's parent and left rotate (possible to become case 3)
                    node = node->parent;
                    leftRotate(node);
                }
                else // case 3: node's uncle is black, and node is a left child
                {
                    // if parent is not root, paint parent to black, paint grandparent to red, then right rotate
                    node->parent->color = BLACK;
                    node->parent->parent->color = RED;
                    rightRotate(node->parent->parent);
                }
            }
            else // just change the direction
            {
                TreeNode* uncle = node->parent->parent->left;
                if (uncle->color == RED) // case 1
                {
                    node->parent->color = BLACK;
                    uncle->color = BLACK;
                    node->parent->parent->color = RED;
                    node = node->parent->parent;
                }
                else if (node == node->parent->left) // case 2
                {
                    node = node->parent;
                    rightRotate(node);
                }
                else // case 3
                {
                    node->parent->color = BLACK;
                    node->parent->parent->color = RED;
                    leftRotate(node->parent->parent);
                }
            }
        }
        m_root->color = BLACK;
    }
    // auxiliary function: replace node with newNode, make sure node is not Nil or nullptr
    void transplant(TreeNode* node, TreeNode* newNode)
    {
        if (node->parent == Nil) // node is root
        {
            m_root = newNode;
        }
        else if (node == node->parent->left) // node is left child of it's parent
        {
            node->parent->left = newNode;
        }
        else // node is right child
        {
            node->parent->right = newNode;
        }
        newNode->parent = node->parent; // set parent even if newNode is Nil. (this is a key point !!!)
    }
    // remove specified node: make sure node is not Nil or nullptr
    TreeNode* removeNode(TreeNode* node)
    {
        TreeNode* ret = successor(node);
        TreeNode* y = node;
        TreeNode* x = Nil;
        NodeColor yOriginalColor = y->color;
        if (node->left == Nil) // node has no left child, (include the case of no child)
        {
            x = node->right;
            transplant(node, node->right);
        }
        else if (node->right == Nil) // node has no right child
        {
            x = node->left;
            transplant(node, node->left);
        }
        else // node has both left and right child
        {
            y = leftMost(node->right); // y has no left child for sure
            yOriginalColor = y->color;
            x = y->right;
            if (y->parent == node) // y is right child of node
            {
                x->parent = y; // set parent even if x is Nil.
            }
            else
            {
                transplant(y, y->right);
                y->right = node->right;
                y->right->parent = y;
            }
            transplant(node, y);
            y->left = node->left;
            y->left->parent = y;
            y->color = node->color;
        }
        destroyNode(node);
        if (yOriginalColor == BLACK)
        {
            // if node has 0 or 1 child, x is current node in node's original location
            // if node has 2 children, x is current node in node's successor's original location
            // x could be Nil in both two cases, in those cases, x->parent will point to it's parent (see transplant), not Nil.
            // general speaking, x is the location that break the attributes of red-black tree.
            removeFixUp(x);
        }
        m_root->parent = Nil; // delete process may change m_root, m_root->parent should keep Nil all the time (include the case m_root is Nil).
        m_nodeCount--;
        return ret;
    }
    // fix up the attributes of Red-Black tree after inserting: make sure node is not nullptr or Nil
    void removeFixUp(TreeNode* node)
    {
        // attribute 2, 4, 5 could be broken
        while (node != m_root && node->color == BLACK)
        {
            if (node == node->parent->left)
            {
                // w is node's sibling
                // and w is not Nil for sure, because the path from w to Nil(exclude Nil itself) definitely has at least one black node.
                TreeNode* w = node->parent->right;
                if (w->color == RED) // case 1
                {
                    // case 1: w is red, so w has two black non-Nil children for sure.
                    w->color = BLACK;
                    node->parent->color = RED;
                    leftRotate(node->parent);
                    w = node->parent->right; // set w to node's sibling, then become case 2,3,4
                }
                if (w->left->color == BLACK && w->right->color == BLACK) // case 2
                {
                    // case 2: w is black, and w has two black chilren
                    // change w to red is enough, and node become it's parent, node's parent break attribute 5 now.
                    w->color = RED;
                    node = node->parent;
                }
                else if (w->right->color == BLACK) // case 3
                {
                    // case 3: w is black, and w has a red left child and a black right child.
                    // convert it to case 4.
                    w->left->color = BLACK;
                    w->color = RED;
                    rightRotate(w);
                    w = node->parent->right;
                }
                else // case 4
                {
                    // case 4: w is black, and w has a red right child.
                    // repaint and rotate node's parent to fulfill attribute 5, jump out loop next.
                    w->color = node->parent->color;
                    node->parent->color = BLACK;
                    w->right->color = BLACK;
                    leftRotate(node->parent);
                    node = m_root;
                }
            }
            else // node is right child of its parent
            {
                TreeNode* w = node->parent->left;
                if (w->color == RED) // case 1
                {
                    w->color = BLACK;
                    node->parent->color = RED;
                    rightRotate(node->parent);
                    w = node->parent->left;
                }
                if (w->right->color == BLACK && w->left->color == BLACK) // case 2
                {
                    w->color = RED;
                    node = node->parent;
                }
                else if (w->left->color == BLACK) // case 3
                {
                    w->right->color = BLACK;
                    w->color = RED;
                    leftRotate(w);
                    w = node->parent->left;
                }
                else // case 4
                {
                    w->color = node->parent->color;
                    node->parent->color = BLACK;
                    w->left->color = BLACK;
                    rightRotate(node->parent);
                    node = m_root;
                }
            }
        }
        node->color = BLACK;
    }
public:
    RbTree(Compare keyComp = Compare())
        : m_keyCompare(keyComp)
        , m_root(nullptr)
        , m_nodeCount(0)
    {
        initNil();
        m_root = Nil;
    }
    template<std::input_iterator InputIterator>
    RbTree(InputIterator first, InputIterator last, Compare keyComp = Compare())
        : m_keyCompare(keyComp)
        , m_root(nullptr)
        , m_nodeCount(0)
    {
        initNil();
        m_root = Nil;
        insert(first, last);
    }
    RbTree(const RbTree& other)
        : m_keyCompare(other.m_keyCompare)
        , m_root(nullptr)
        , m_nodeCount(0)
    {
        initNil();
        m_root = Nil;
        insert(other.begin(), other.end()); // better option is copy every node
    }
    RbTree(RbTree&& other)
        : m_keyCompare(std::move(other.m_keyCompare))
        , m_root(other.m_root)
        , Nil(other.Nil)
        , m_nodeCount(other.m_nodeCount)
    {
        other.initNil();
        other.m_root = other.Nil;
        other.m_nodeCount = 0;
    }
    RbTree& operator=(const RbTree& other)
    {
        clear();
        insert(other.begin(), other.end()); // better option is copy every node
        return *this;
    }
    RbTree& operator=(RbTree&& other)
    {
        clear();
        freeNil();
        Nil = other.Nil;
        m_root = other.m_root;
        m_nodeCount = other.m_nodeCount;
        m_keyCompare = std::move(other.m_keyCompare);
        other.initNil();
        other.m_root = other.Nil;
        other.m_nodeCount = 0;
        return *this;
    }
    ~RbTree()
    {
        clear();
        freeNil();
    }
    // better option, record size
    std::size_t size() const
    {
        return m_nodeCount;
    }
    bool empty() const
    {
        return m_root == Nil;
    }
    iterator begin()
    {
        return { this, m_root == Nil ? Nil : leftMost(m_root) };
    }
    iterator end()
    {
        return iterator(this, Nil);
    }
    const_iterator begin() const
    {
        return { this, m_root == Nil ? Nil : leftMost(m_root) };
    }
    const_iterator end() const
    {
        return const_iterator(this, Nil);
    }
    template<std::input_iterator InputIterator>
    void insert(InputIterator first, InputIterator last)
    {
        for (; first != last; ++first)
        {
            insertNode(constructNode(*first));
        }
    }
    iterator insert(const Value& value)
    {
        return { this, insertNode(constructNode(value)) };
    }
    iterator insert(Value&& value)
    {
        return { this, insertNode(constructNode(std::move(value))) };
    }
    template<typename... Args>
    iterator emplace(Args&&... args)
    {
        return { this, insertNode(constructNode(std::forward<Args>(args)...)) };
    }
    // UB if iter is end()
    iterator erase(const_iterator iter)
    {
        return { this, removeNode(iter.node) };
    }
    iterator find(const Key& key)
    {
        return { this, search(m_root, key) };
    }
    const_iterator find(const Key& key) const
    {
        return { this, search(m_root, key) };
    }
    void clear()
    {
        std::queue<TreeNode*> q;
        if (m_root != Nil)
        {
            q.push(m_root);
        }
        while (!q.empty())
        {
            TreeNode* node = q.front();
            if (node->left != Nil)
            {
                q.push(node->left);
            }
            if (node->right != Nil)
            {
                q.push(node->right);
            }
            destroyNode(node);
            q.pop();
        }
        m_root = Nil;
        m_nodeCount = 0;
    }
};
