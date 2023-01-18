#pragma once

#include <functional>
#include <iterator>
#include <memory>
#include <queue>
#include <type_traits>
#include <concepts>

namespace CLRS
{

template<typename Key, typename Priority, typename Value, typename KeyOfValue, typename PriorityOfValue,
         bool Multi = true, typename KeyCompare = std::less<Key>, typename PriorityCompare = std::less<Priority>>
class Treap
{
/*
Treap: provide attributes of both binary search tree and heap.
    binary search tree: use KeyOfValue get key
    heap: use PriorityOfValue get priority, (using std::less<> would cause greatest value as top/root)

Multi: support multiple same key or not.
*/
private:
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
    };

    template<bool isConst>
    struct TreapIterator
    {
        friend class Treap<Key, Priority, Value, KeyOfValue, PriorityOfValue, Multi, KeyCompare, PriorityCompare>;
    private:
        using BaseTreeType = Treap<Key, Priority, Value, KeyOfValue, PriorityOfValue, Multi, KeyCompare, PriorityCompare>;
        using TreeType = std::conditional_t<isConst, const BaseTreeType, BaseTreeType>;
        using IterReference = std::conditional_t<isConst, Value&, const Value&>;
        using IterPointer = std::conditional_t<isConst, Value*, const Value*>;
    public:
        TreapIterator(TreeType* _tree = nullptr, TreeNode* _node = nullptr) : tree(_tree), node(_node)
        {
        }
        TreapIterator(const TreapIterator& other) : tree(other.tree), node(other.node)
        {
        }
        TreapIterator(const TreapIterator<false>& other) requires (isConst) // convert iterator to const_iterator
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
        TreapIterator& operator++()
        {
            node = tree->successor(node);
            return *this;
        }
        TreapIterator operator++(int)
        {
            TreapIterator res(tree, node);
            ++*this;
            return res;
        }
        TreapIterator& operator--()
        {
            node = tree->predecessor(node);
            return *this;
        }
        TreapIterator operator--(int)
        {
            TreapIterator res(tree, node);
            --*this;
            return res;
        }
        bool operator==(const TreapIterator& other) const
        {
            return tree == other.tree && node == other.node;
        }
        TreapIterator& operator=(const TreapIterator& other)
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
    using iterator = TreapIterator<false>;
    using const_iterator = TreapIterator<true>;
    static_assert(std::bidirectional_iterator<iterator>);
    static_assert(std::bidirectional_iterator<const_iterator>);
private:
    KeyCompare m_keyCompare;
    PriorityCompare m_priorityCompare;
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
    using ReturnKeyType = std::conditional_t<std::is_reference_v<std::invoke_result_t<KeyOfValue, Value>>, const Key&, Key>;
    static ReturnKeyType key(TreeNode* x)
    {
        static KeyOfValue keyOfValue;
        return keyOfValue(x->data);
    }
    using ReturnPriorityType = std::conditional_t<std::is_reference_v<std::invoke_result_t<PriorityOfValue, Value>>, const Key&, Key>;
    static ReturnPriorityType priority(TreeNode* x)
    {
        static PriorityOfValue priorityOfValue;
        return priorityOfValue(x->data);
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
    // rotate
    // left rotate: make sure node is not nullptr and node has right child
    //  node  to   y
    //   \        /
    //    y      node
    void leftRotate(TreeNode* node)
    {
        TreeNode* y = node->right;
        node->right = y->left;
        if (y->left)
        {
            y->left->parent = node;
        }
        y->parent = node->parent;
        if (node->parent == nullptr) // node is root
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
    // right rotate: make sure node is not nullptr and node has left child
    //  node  to  y
    //  /          \_
    // y           node
    void rightRotate(TreeNode* node)
    {
        TreeNode* y = node->left;
        node->left = y->right;
        if (y->right)
        {
            y->right->parent = node;
        }
        y->parent = node->parent;
        if (node->parent == nullptr) // node is root
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
        insertFixUp(newNode);
        return newNode;
    }
    // fix up the attributes of treap after inserting: make sure node is not nullptr
    void insertFixUp(TreeNode* node)
    {
        while (node->parent && m_priorityCompare(priority(node->parent), priority(node)))
        {
            if (node == node->parent->left)
            {
                rightRotate(node->parent);
            }
            else
            {
                leftRotate(node->parent);
            }
        }
    }
    // auxiliary function: replace node with newNode, make sure node is not nullptr
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
    // remove specified node: make sure node is not nullptr
    TreeNode* removeNode(TreeNode* node)
    {
        TreeNode* ret = successor(node);
        // rotate node to leave, then remove it
        while (node->left && node->right)
        {
            if (m_priorityCompare(priority(node->left), priority(node->right)))
            {
                leftRotate(node);
            }
            else
            {
                rightRotate(node);
            }
        }
        if (node->left)
        {
            transplant(node, node->left);
        }
        else
        {
            transplant(node, node->right);
        }
        destroyNode(node);
        if (m_root)
        {
            m_root->parent = nullptr; // seems unnecessary
        }
        m_nodeCount--;
        return ret;
    }
public:
    Treap(KeyCompare keyComp = KeyCompare(), PriorityCompare priorityComp = PriorityCompare())
        : m_keyCompare(keyComp)
        , m_priorityCompare(priorityComp)
        , m_root(nullptr)
        , m_nodeCount(0)
    {
    }
    template<std::input_iterator InputIterator>
    Treap(InputIterator first, InputIterator last, KeyCompare keyComp = KeyCompare(), PriorityCompare priorityComp = PriorityCompare())
        : m_keyCompare(keyComp)
        , m_priorityCompare(priorityComp)
        , m_root(nullptr)
        , m_nodeCount(0)
    {
        insert(first, last);
    }
    Treap(const Treap& other)
        : m_keyCompare(other.m_keyCompare)
        , m_priorityCompare(other.m_priorityCompare)
        , m_root(nullptr)
        , m_nodeCount(0)
    {
        m_root = copyNode(other.m_root);
        m_nodeCount = other.m_nodeCount;
    }
    Treap(Treap&& other)
        : m_keyCompare(std::move(other.m_keyCompare))
        , m_priorityCompare(std::move(other.m_priorityCompare))
        , m_root(other.m_root)
        , m_nodeCount(other.m_nodeCount)
    {
        other.m_root = nullptr;
        other.m_nodeCount = 0;
    }
    Treap& operator=(const Treap& other)
    {
        clear();
        m_root = copyNode(other.m_root);
        m_nodeCount = other.m_nodeCount;
        return *this;
    }
    Treap& operator=(Treap&& other)
    {
        clear();
        m_root = other.m_root;
        m_nodeCount = other.m_nodeCount;
        other.m_root = nullptr;
        other.m_nodeCount = 0;
        return *this;
    }
    ~Treap()
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
    // heap features
    // top element (the root element)
    const Value& top() const
    {
        return m_root->data;
    }
    // pop top element
    void pop()
    {
        if (m_root)
        {
            removeNode(m_root);
        }
    }
};


} // namespace CLRS