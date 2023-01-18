#pragma once

#include <functional>
#include <iterator>
#include <memory>
#include <queue>
#include <type_traits>
#include <concepts>
#include <TreeImpl_RbTree.hpp>

template<typename Key, typename Value, typename KeyOfValue, bool Multi = false, typename Compare = std::less<Key>>
class BsTree
{
/*
normal binary search tree
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
    struct BsTreeIterator
    {
        friend class BsTree<Key, Value, KeyOfValue, Multi, Compare>;
    private:
        using BaseTreeType = BsTree<Key, Value, KeyOfValue, Multi, Compare>;
        using TreeType = std::conditional_t<isConst, const BaseTreeType, BaseTreeType>;
        using IterReference = std::conditional_t<isConst, Value&, const Value&>;
        using IterPointer = std::conditional_t<isConst, Value*, const Value*>;
    public:
        BsTreeIterator(TreeType* _tree = nullptr, TreeNode* _node = nullptr) : tree(_tree), node(_node)
        {
        }
        BsTreeIterator(const BsTreeIterator& other) : tree(other.tree), node(other.node)
        {
        }
        BsTreeIterator(const BsTreeIterator<false>& other) requires (isConst) // convert iterator to const_iterator
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
        BsTreeIterator& operator++()
        {
            node = tree->successor(node);
            return *this;
        }
        BsTreeIterator operator++(int)
        {
            BsTreeIterator res(tree, node);
            ++*this;
            return res;
        }
        BsTreeIterator& operator--()
        {
            node = tree->predecessor(node);
            return *this;
        }
        BsTreeIterator operator--(int)
        {
            BsTreeIterator res(tree, node);
            --*this;
            return res;
        }
        bool operator==(const BsTreeIterator& other) const
        {
            return tree == other.tree && node == other.node;
        }
        BsTreeIterator& operator=(const BsTreeIterator& other)
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
    using iterator = BsTreeIterator<false>;
    using const_iterator = BsTreeIterator<true>;
    static_assert(std::bidirectional_iterator<iterator>);
    static_assert(std::bidirectional_iterator<const_iterator>);
private:
    Compare m_keyCompare;
    TreeNode* m_root = nullptr;
    std::size_t m_nodeCount = 0;
private:
    // node construction and destruction
    template<typename... Args>
    [[nodiscard]] TreeNode* constructNode(Args&&... args)
    {
        TreeNode* p = new TreeNode(std::forward<Args>(args)...);
        p->left = p->right = p->parent = nullptr;
        return p;
    }
    void destroyNode(TreeNode* node)
    {
        if (node)
        {
            delete node;
        }
    }
    // copy node and its children
    [[nodiscard]] TreeNode* copyNode(TreeNode* node)
    {
        if (node == nullptr)
        {
            return nullptr;
        }
        TreeNode* res = constructNode(node->data);
        res->left = copyNode(node->left);
        res->right = copyNode(node->right);
        if (res->left)
        {
            res->left->parent = res;
        }
        if (res->right)
        {
            res->right->parent = res;
        }
        return res;
    }
    // inner operations, make sure input node is not nullptr
    static Key& key(TreeNode* x)
    {
        static KeyOfValue keyOfValue;
        return keyOfValue(x->data);
    }
    TreeNode* leftMost(TreeNode* node) const
    {
        while (node->left)
        {
            node = node->left;
        }
        return node;
    }
    TreeNode* rightMost(TreeNode* node) const
    {
        while (node->right)
        {
            node = node->right;
        }
        return node;
    }
    TreeNode* successor(TreeNode* node) const
    {
        if (node->right)
        {
            return leftMost(node->right);
        }
        TreeNode* par = node->parent;
        while (par && node == par->right)
        {
            node = par;
            par = par->parent;
        }
        return par; // nullptr if node is the right most
    }
    TreeNode* predecessor(TreeNode* node) const
    {
        if (node == nullptr)
        {
            return rightMost(m_root);
        }
        if (node->left)
        {
            return rightMost(node->left);
        }
        TreeNode* par = node->parent;
        while (par && node == par->left)
        {
            node = par;
            par = par->parent;
        }
        return par; // nullptr if node is the left most
    }
    TreeNode* search(TreeNode* node, const Key& val) const
    {
        while (node)
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
        return nullptr;
    }
    // insert a new node, return existed node with same key or the input new node
    TreeNode* insertNode(TreeNode* newNode)
    {
        TreeNode* node = m_root;
        TreeNode* prev = nullptr;
        while (node)
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
        if (prev == nullptr) // root is nullptr, the tree is empty
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
        newNode->left = newNode->right = nullptr;
        m_root->parent = nullptr;   // insert process may change m_root, m_root->parent should keep nullptr all the time.
        m_nodeCount++;
        return newNode;
    }
    // auxiliary function: replace node with newNode, make sure node is not nullptr or nullptr
    void transplant(TreeNode* node, TreeNode* newNode)
    {
        if (node->parent == nullptr) // node is root
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
        if (newNode)
        {
            newNode->parent = node->parent;
        }
    }
    // remove specified node: make sure node is not nullptr or nullptr
    TreeNode* removeNode(TreeNode* node)
    {
        TreeNode* ret = successor(node);
        if (node->left == nullptr) // node has no left child, (include the case of no child)
        {
            transplant(node, node->right);
        }
        else if (node->right == nullptr) // node has no right child
        {
            transplant(node, node->left);
        }
        else // node has both left and right child
        {
            TreeNode* y = leftMost(node->right); // y has no left child for sure
            if (y->parent != node)
            {
                transplant(y, y->right);
                y->right = node->right;
                y->right->parent = y;
            }
            transplant(node, y);
            y->left = node->left;
            y->left->parent = y;
        }
        destroyNode(node);
        m_root->parent = nullptr; // remove process may change m_root, m_root->parent should keep nullptr all the time (include the case m_root is nullptr).
        m_nodeCount--;
        return ret;
    }
public:
    BsTree(Compare keyComp = Compare())
        : m_keyCompare(keyComp)
        , m_root(nullptr)
        , m_nodeCount(0)
    {
    }
    template<std::input_iterator InputIterator>
    BsTree(InputIterator first, InputIterator last, Compare keyComp = Compare())
        : m_keyCompare(keyComp)
        , m_root(nullptr)
        , m_nodeCount(0)
    {
        insert(first, last);
    }
    BsTree(const BsTree& other)
        : m_keyCompare(other.m_keyCompare)
        , m_root(nullptr)
        , m_nodeCount(0)
    {
        m_root = copyNode(other.m_root);
        m_nodeCount = other.m_nodeCount;
    }
    BsTree(BsTree&& other)
        : m_keyCompare(std::move(other.m_keyCompare))
        , m_root(other.m_root)
        , m_nodeCount(other.m_nodeCount)
    {
        other.m_root = nullptr;
        other.m_nodeCount = 0;
    }
    BsTree& operator=(const BsTree& other)
    {
        clear();
        m_root = copyNode(other.m_root);
        m_nodeCount = other.m_nodeCount;
        return *this;
    }
    BsTree& operator=(BsTree&& other)
    {
        clear();
        m_root = other.m_root;
        m_nodeCount = other.m_nodeCount;
        m_keyCompare = std::move(other.m_keyCompare);
        other.m_root = nullptr;
        other.m_nodeCount = 0;
        return *this;
    }
    ~BsTree()
    {
        clear();
    }
    // better option, record size
    std::size_t size() const
    {
        return m_nodeCount;
    }
    bool empty() const
    {
        return m_root == nullptr;
    }
    iterator begin()
    {
        return { this, m_root == nullptr ? nullptr : leftMost(m_root) };
    }
    iterator end()
    {
        return iterator(this, nullptr);
    }
    const_iterator begin() const
    {
        return { this, m_root == nullptr ? nullptr : leftMost(m_root) };
    }
    const_iterator end() const
    {
        return const_iterator(this, nullptr);
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
        if (m_root)
        {
            q.push(m_root);
        }
        while (!q.empty())
        {
            TreeNode* node = q.front();
            if (node->left)
            {
                q.push(node->left);
            }
            if (node->right)
            {
                q.push(node->right);
            }
            destroyNode(node);
            q.pop();
        }
        m_root = nullptr;
        m_nodeCount = 0;
    }
};
