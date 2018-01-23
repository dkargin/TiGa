#pragma once
//\addtogroup Containers
//@{

/// Intrusive tree node
/**
 * 	Each node can have several children.
 *	Children are stored in double-linked list
 */
template<class Type> class TreeNode
{
protected:
	typedef TreeNode<Type> node_type;	///< Defines node type
	Type * parent;						///< Pointer to parent node
	Type * next;						///< Pointer to next node
	Type * prev;						///< Pointer to previous node
	Type * head;						///< Pointer to head child nodes
	Type * tail;						///< Pointer to tail child nodes

	/// Called when node is attached
	virtual void onAttach( Type * object ) = 0;
	/// Called when node is detached
	virtual void onDetach( Type * object ) = 0;
public:

	/// Constructor
	TreeNode()
	{
		parent = NULL;
		next = NULL;
		prev = NULL;
		head = NULL;
		tail = NULL;
	}

	/// Get pointer to target type
	virtual Type * getTargetType() = 0;

	/// Disconnect self from tree
	void orphan_me()
	{
		if( parent != NULL )
			parent->removeChild(this);
	}

	/// Check if there are any chilren
	bool hasChild(const node_type * child) const
	{
		return child && child->parent == this;
	}

	/// Attach child node
	/** newChild is detached first
	 */
	void attach(Type * newChild)
	{
		newChild->orphan_me();
		if( head == NULL)
		{
			head = newChild;
			tail = newChild;
			newChild->prev = NULL;
			newChild->next = NULL;
		}
		else
		{
			tail->next = newChild;
			newChild->prev = tail;
			newChild->next = NULL;
			tail = newChild;
		}
		newChild->parent = static_cast<Type*>(this);
		onAttach(static_cast<Type*>(newChild));
	}

	/// Removes specified child.
	void removeChild(node_type * child)
	{
		assert( hasChild(child) );
		if( child->prev != head )
			child->prev->next = child->next;
		else
			head = child->next;

		if( child->next != tail )
			child->next->prev = child->prev;
		else
			tail = child->prev;
		child->next = NULL;
		child->prev = NULL;
		child->parent = NULL;
		onDetach(static_cast<Type*>(child));
	}

	/// Detaches from tree
	/** Detaches node from existing tree
	 */
	virtual void detach(node_type * child)
	{
		removeChild(child);
	}

	/// Destructor
	virtual ~TreeNode()
	{
		orphan_me();
	}

	/// Const iterator to iterate through immediate children nodes
	class const_iterator
	{
		protected:
		const Type * container;	///< Pointer to container
		const Type * current;		///< Pointer to current node
	public:
		typedef const_iterator iterator_type;	///< own iterator type
		/// Constructor
		const_iterator(const Type * container_, const Type * current_)
		{
			container = container_;
			current = current_;
		}
		/// Copy constructor
		const_iterator(const const_iterator &it)
		{
			container = it.container;
			current = it.current;
		}
		/// Check if iterators are equal
		bool operator == ( const iterator_type & it) const
		{
			return container == it.container && current == it.current;
		}
		/// Check if iterators are not equal
		bool operator != ( const iterator_type & it) const
		{
			return container != it.container || current != it.current;
		}
		/// Dereferencing operator
		const Type * operator->()
		{
			return current;
		}
		/// Prefix increment. Moves to the next element
		iterator_type & operator++()
		{
			assert(current != NULL);
			current = current->next;
			return *this;
		}
		/// Postfix increment
		iterator_type operator++(int)	// postfix
		{
			assert(current != NULL);
			const_iterator result(*this);
			current = current->next;
			return result;
		}
	};

	/// Iterator to iterate through immediate children nodes
	class iterator
	{
	protected:
		Type * container;	///< pointer to the container
		Type * current;		///< pointer to current element
	public:
		typedef iterator iterator_type;		///< own iterator type
		/// Constructor
		iterator(Type * container_, Type * current_)
		{
			container = container_;
			current = current_;
		}
		/// Copy constructor
		iterator(const const_iterator &it)
		{
			container = it.container;
			current = it.current;
		}
		/// Check if iterators are equal
		bool operator == ( const iterator_type & it) const
		{
			return container == it.container && current == it.current;
		}
		/// Check if iterators are different
		bool operator != ( const iterator_type & it) const
		{
			return container != it.container || current != it.current;
		}
		/// Dereferencing operator
		Type * operator->()
		{
			return current;
		}
		/// Prefix increment
		iterator_type & operator++()	// prefix
		{
			assert(current != NULL);
			current = current->next;
			return *this;
		}
		/// Postfix increment
		iterator_type operator++(int)	// postfix
		{
			assert(current != NULL);
			const_iterator result(*this);
			current = current->next;
			return result;
		}
	};

	/// Get const iterator to the first child
	const_iterator begin() const
	{
		return const_iterator(static_cast<const Type*>(this), head);
	}

	/// Get const iterator to the last child
	const_iterator end() const
	{
		return const_iterator(static_cast<const Type*>(this), NULL);
	}

	/// Get iterator to the first child
	iterator begin()
	{
		return iterator(static_cast<Type*>(this),head);
	}
	/// Get iterator to the last child
	iterator end()
	{
		return iterator(static_cast<Type*>(this), NULL);
	}
};
