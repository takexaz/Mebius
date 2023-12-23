#include <_alloc.hpp>

#include <Windows.h>

#include <cassert>
#include <format>

using namespace mebius;
using namespace mebius::alloc;

template <>
mebius::alloc::SmallPool::Pool() {
	void* ptr = VirtualAlloc(nullptr, _MEM_PAGE_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (ptr == nullptr) {
		throw MebiusError("Can't alloc rwx small pool.");
	}
	SmallPoolNode* ns = new(ptr) SmallPoolNode[ALLOC_SMALL_NODE_NUM];
	for (size_t i = 0; i < ALLOC_SMALL_NODE_NUM - 1; i++) {
		ns[i].next = &ns[i + 1];
	}
	ns[ALLOC_SMALL_NODE_NUM - 1].next = nullptr;
	this->nodes = std::move(ns);
	this->top = &(this->nodes)[0];
}

template <>
mebius::alloc::SmallPool::~Pool() noexcept {
	if (this->nodes != nullptr) {
		(this->nodes)->~SmallPoolNode();
		VirtualFree(this->nodes, 0, MEM_RELEASE);
	}
}

template <>
mebius::alloc::LargePool::Pool() {
	void* ptr = VirtualAlloc(nullptr, _MEM_PAGE_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (ptr == nullptr) {
		throw MebiusError("Can't alloc rwx large pool.");
	}
	LargePoolNode* ns = new(ptr) LargePoolNode[ALLOC_LARGE_NODE_NUM];
	for (size_t i = 0; i < ALLOC_LARGE_NODE_NUM - 1; i++) {
		ns[i].next = &ns[i + 1];
	}
	ns[ALLOC_LARGE_NODE_NUM - 1].next = nullptr;
	this->nodes = std::move(ns);
	this->top = &(this->nodes)[0];
}

template <>
mebius::alloc::LargePool::~Pool() noexcept {
	(this->nodes)->~LargePoolNode();
	VirtualFree(this->nodes, 0, MEM_RELEASE);
}


template<size_t _Size>
inline mebius::alloc::internal::Pool<_Size>::Pool()
{
	ShowErrorDialog("Unspecialized pool constructor shouldn't be called.");
}


code_t* mebius::alloc::CodeAllocator::Allocate(size_t size) noexcept {
	code_t* mem = nullptr;
	if (size <= _MEM_SMALL_NODE_SIZE) {
		mem = AllocateFromSmallPool();
	}
	else if (size <= _MEM_LARGE_NODE_SIZE) {
		mem = AllocateFromLargePool();
	}
	else {
		ShowErrorDialog(std::vformat("The size allocator required ({:d}) is bigger than {:d}.", std::make_format_args(size, _MEM_LARGE_NODE_SIZE)).c_str());
	}
	return mem;
}

inline code_t* mebius::alloc::CodeAllocator::AllocateFromSmallPool() noexcept
{
	code_t* mem = nullptr;
	for (auto& p : _small_pools) {
		if (!p.IsFull()) {
			mem = AllocFromPool(p);
			break;
		}
	}
	// ‘Ssmall poolŒÍŠ‰Žž
	if (mem == nullptr) {
		try {
			auto& p = AppendSmallPool();
			mem = AllocFromPool(p);
		}
		catch (const MebiusError& e) {
			ShowErrorDialog(e.what());
		}
	}
	assert(mem != nullptr);
	return mem;
}

inline code_t* mebius::alloc::CodeAllocator::AllocateFromLargePool() noexcept
{
	code_t* mem = nullptr;
	for (auto& p : _large_pools) {
		if (!p.IsFull()) {
			mem = AllocFromPool(p);
			break;
		}
	}
	// ‘Slarge poolŒÍŠ‰Žž
	if (mem == nullptr) {
		try {
			auto& p = AppendLargePool();
			mem = AllocFromPool(p);
		}
		catch (const MebiusError& e) {
			ShowErrorDialog(e.what());
		}
	}
	assert(mem != nullptr);
	return mem;
}

void mebius::alloc::CodeAllocator::DeAllocate(code_t* ptr) noexcept {
	MEMORY_BASIC_INFORMATION info = {};
	if (!VirtualQuery(ptr, &info, _MEM_PAGE_SIZE)) {
		return;
	}

	if (ptr != nullptr && info.State != MEM_FREE) {
		auto it = used.find(ptr);
		if (it != used.end()) {
			it->second.Free(ptr);
			used.erase(it);
		}
	}
}
