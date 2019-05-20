#pragma once
#include <vector>
namespace BLUE_BERRY {

template<typename _Key, typename _Value, typename _HashKey = std::hash<_Key> >
class LockFreeMap
{
public:
	typedef _Key key_type;
	typedef _Value value_type;
	typedef std::pair<_Key, _Value> entry_type;

	// hash map bucket
	struct _Bucket
	{
		enum class _State
		{
			EMPTY,		// empty
			VALID,		// valid
			LOCKED,		// locked 
			INVALID,	// invalid
		};
		std::atomic<_State> _state;
		entry_type _entry;

		_Bucket() :_state(_State::EMPTY) {}
	};


	struct _SubMap
	{
		std::vector<_Bucket> _map;
		std::atomic<std::size_t> _validCount;

		_SubMap(std::size_t capacity_) : _map(capacity_), _validCount(0) {}

		_Bucket& get(std::size_t index_) { return _map[index_]; }
		const _Bucket& get(std::size_t index_) const { return _map[index_]; }

		void increValidCount() { _validCount.fetch_add(1, std::memory_order_relaxed); }

	};


	LockFreeMap()
	{
	}

	~LockFreeMap()
	{
	}

private:
	
	_HashKey _keyHash;


	std::vector<_SubMap> _map;
};


}