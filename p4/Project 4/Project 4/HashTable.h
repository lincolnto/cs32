#ifndef HASHTABLE_INCLUDED
#define HASHTABLE_INCLUDED

template <typename KeyType, typename ValueType>
class HashTable
{
public:
	HashTable(unsigned int numBuckets, unsigned int capacity);
	~HashTable();
	bool isFull() const { return m_curCapacity == MAX_CAPACITY; }
	bool set(const KeyType&	key, const ValueType& value, bool permanent = false);
	bool get(const KeyType& key, ValueType& value)	const;
	bool touch(const KeyType& key);
	bool discard(KeyType& key, ValueType& value);
private:
	struct Node
	{
		KeyType key;
		ValueType value;
		Node* prev;
		Node* next;
		bool isPermanent;
	};
	int MAX_CAPACITY;
	int m_curCapacity;
	int m_numBuckets;
	int m_nonPermItems;
	Node** m_HashTable;
	Node** recent;
	void addToTop(Node* p) // Adds node to top of recents
	{
		if (!p->isPermanent) // Only add if node is not permanent
		{
			for (int k = m_nonPermItems; k != 0; k--) // Shift all recents to right
			{
				recent[k] = recent[k - 1];
			}
			recent[0] = p; // Add node to top of recently modified items
		}
	}


	//	We	prevent a	HashTable from	being	copied	or	assigned	by	declaring	the
	//	copy	constructor	and	assignment	operator	private	and	not	implementing	them.
	HashTable(const HashTable&);
	HashTable& operator=(const HashTable&);
};


template <typename KeyType, typename ValueType>
inline
HashTable<KeyType, ValueType>::HashTable(unsigned int numBuckets, unsigned int capacity)
{
	m_HashTable = new Node*[numBuckets]; // Allocate an array
	recent = new Node*[capacity];
	MAX_CAPACITY = capacity;
	m_curCapacity = 0;
	m_nonPermItems = 0;
	m_numBuckets = numBuckets;
	for (unsigned int k = 0; k < numBuckets; k++)
		m_HashTable[k] = nullptr;
}


template <typename KeyType, typename ValueType>
inline
HashTable<KeyType, ValueType>::~HashTable()
{
	//for (unsigned int k = 0; k != m_numBuckets; k++)
	//{
	//	if (m_HashTable[k].next == nullptr)
	//		delete m_HashTable[k];
	//	else
	//	{
	//		Node* p = m_HashTable[k].next;
	//		while (p != nullptr)
	//		{
	//			Node* prev = p;
	//			p = p.next;
	//			delete prev;
	//		}
	//		delete p;
	//		delete m_HashTable[k];
	//	}
	//}
	delete[] m_HashTable;
}

template <typename KeyType, typename ValueType>
inline
bool HashTable<KeyType, ValueType>::set(const KeyType& key, const ValueType& value, bool permanent = false)
{
	unsigned int computeHash(KeyType);
	unsigned int result = computeHash(key) % m_numBuckets;
	ValueType val;
	if (!get(key, val)) // Key is not in table
	{
		if (m_curCapacity == MAX_CAPACITY) // Hash table is full
			return false;
		if (m_HashTable[result] == nullptr) // No values currently stored in hash
		{
			m_HashTable[result] = new Node;
			Node* n = m_HashTable[result];
			n->key = key; n->value = value;
			n->next = nullptr; n->prev = nullptr;
			n->isPermanent = permanent;
			m_curCapacity++;
			if (!n->isPermanent)
			{
				m_nonPermItems++;
				addToTop(n);
			}
			return true;
		}
		else // Values stored with this hash value
		{
			Node *p = m_HashTable[result]->next;
			while (p != nullptr) // Find the last node with this hash
				p = p->next;
			p->next = new Node;
			Node* n = p->next;
			n->key = key; n->value = value;
			n->next = nullptr; n->prev = p;
			n->isPermanent = permanent;
			m_curCapacity++;
			if (!n->isPermanent)
			{
				m_nonPermItems++;
				addToTop(n);
			}
			return true;
		}

	}
	else // Key is already in table
	{
		touch(key);
		if (m_HashTable[result]->key == key) // Find the key and update value
			m_HashTable[result]->value = value;
		else
		{
			Node *p = m_HashTable[result]->next;
			while (p->key != key)
				p = p->next;
			p->value = value;
		}
		return true;
	}
}

template <typename KeyType, typename ValueType>
inline
bool HashTable<KeyType, ValueType>::get(const KeyType& key, ValueType& value) const
{
	unsigned int computeHash(KeyType);
	unsigned int result = computeHash(key) % m_numBuckets;
	if (m_HashTable[result] == nullptr)
	{
		return false;
	}
	if (m_HashTable[result]->next == nullptr) // Only one key at this hash
	{
		if (m_HashTable[result]->key == key)
		{
			value = m_HashTable[result]->value;
			return true;
		}
	}
	else // Check for all keys with same hash value
	{
		Node* p = m_HashTable[result];
		while (p != nullptr)
		{
			if (m_HashTable[result]->key == key)
			{
				value = m_HashTable[result]->value;
				return true;
			}
			p = p->next;
		}
	}
	return true;

}

template <typename KeyType, typename ValueType>
inline
bool HashTable<KeyType, ValueType>::touch(const KeyType& key)
{
	int keyPos = -1;
	Node *temp = nullptr;
	for (int k = 0; k < m_nonPermItems; k++) // Find the key 
	{
		if (recent[k]->key == key)
		{
			temp = recent[k];
			keyPos = k;
			break;
		}
	}
	if (temp == nullptr) // Key not found
		return false;
	if (keyPos > 0) // Key was in recents & is not already on top
	{
		for (int k = keyPos; k > 0; k--) // Shift all recents
		{
			recent[k] = recent[k - 1];
		}
		recent[0] = temp; // Place touched value on top
		return true;
	}
	return false;
}

template <typename KeyType, typename ValueType>
inline
bool HashTable<KeyType, ValueType>::discard(KeyType& key, ValueType& value)
{
	if (m_nonPermItems == 0)
		return false;
	Node* p = recent[m_nonPermItems];
	key = p->key;	value = p->value;
	if (p->prev != nullptr)	p->prev->next = p->next; // Fix linked list in hash table
	if (p->next != nullptr)	p->next->prev = p->prev;

	delete p;
	m_curCapacity--;
	m_nonPermItems--;
	return true;
}

// template <typename KeyType, typename ValueType>
// inline
// HashTable<KeyType, ValueType>::

#endif // HASHTABLE_INCLUDED //