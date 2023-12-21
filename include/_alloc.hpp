#pragma once

#include <error.hpp>

#include <array>
#include <bit>
#include <cstdint>
#include <deque>
#include <memory>
#include <unordered_map>

namespace mebius {
	using code_t = uint8_t;
	namespace alloc {
		constexpr static inline size_t _MEM_ALIGNMENT_SIZE = 0x10;
		constexpr static inline size_t _MEM_PAGE_SIZE = 0x1000;
		constexpr static inline size_t _MEM_INIT_CODE_SIZE = _MEM_PAGE_SIZE;

		constexpr static inline size_t _MEM_SMALL_NODE_SIZE = 0x10;
		constexpr static inline size_t _MEM_LARGE_NODE_SIZE = 0x60;

		namespace internal {
			class PoolBase {
			public:
				virtual code_t* Alloc() noexcept = 0;
				virtual void Free(code_t* node) noexcept = 0;
				virtual ~PoolBase() = default;
			};

			template <size_t _Size>
			class PoolNode {
			public:
				union {
					std::array<code_t, _Size> code;
					PoolNode<_Size>* next;
				};
			};

			using SmallPoolNode = PoolNode<_MEM_SMALL_NODE_SIZE>;
			using LargePoolNode = PoolNode<_MEM_LARGE_NODE_SIZE>;
			constexpr static inline size_t ALLOC_SMALL_NODE_NUM = _MEM_PAGE_SIZE / sizeof(SmallPoolNode);
			constexpr static inline size_t ALLOC_LARGE_NODE_NUM = _MEM_PAGE_SIZE / sizeof(LargePoolNode);

			template <size_t _Size>
			class Pool : public PoolBase {
			public:
				Pool();
				~Pool() noexcept override;

				Pool(const Pool&) = delete;
				Pool& operator=(const Pool&) = delete;

				Pool(Pool&& pool) noexcept : nodes(pool.nodes), top(pool.top) {
					pool.nodes = nullptr;
					pool.top = nullptr;
				}
				Pool& operator=(Pool&& pool) noexcept {
					this->nodes = std::move(pool.nodes);
					this->top = std::move(pool.top);
					pool.nodes = nullptr;
					pool.top = nullptr;
				}


				inline bool IsFull() const noexcept {
					return top == nullptr;
				}

				inline code_t* Alloc() noexcept override {
					auto& ret = top->code;
					top = top->next;
					return ret.data();
				}

				inline void Free(code_t* ptr) noexcept override {
					if (ptr != nullptr) {
						auto node = std::bit_cast<PoolNode<_Size>*>(ptr);
						node->next = top;
						top = node;
					}
				}

			private:
				PoolNode<_Size>* nodes;
				PoolNode<_Size>* top;
			};
		}

		using SmallPool = internal::Pool<sizeof(internal::SmallPoolNode)>;
		using LargePool = internal::Pool<sizeof(internal::LargePoolNode)>;

		class CodeAllocator {
		public:
			static CodeAllocator& GetInstance() noexcept {
				static CodeAllocator instance{};
				return instance;
			}
			CodeAllocator(const CodeAllocator&) = delete;
			CodeAllocator& operator=(const CodeAllocator&) = delete;
			CodeAllocator(CodeAllocator&&) = delete;
			CodeAllocator& operator=(CodeAllocator&&) = delete;

			[[nodiscard]] code_t* Allocate(size_t size) noexcept;
			void DeAllocate(code_t* ptr) noexcept;

		private:
			std::unordered_map<code_t*, internal::PoolBase&> used{};
			std::deque<SmallPool> _small_pools{};
			std::deque<LargePool> _large_pools{};

			CodeAllocator() noexcept {
				try {
					AppendSmallPool();
					AppendLargePool();
				}
				catch (const MebiusError& e) {
					ShowErrorDialog(e.what());
				}
			}
			~CodeAllocator() noexcept = default;

			inline SmallPool& AppendSmallPool() noexcept {
				return _small_pools.emplace_front();
			}
			inline LargePool& AppendLargePool() noexcept {
				return _large_pools.emplace_front();
			}

			inline code_t* AllocateFromSmallPool() noexcept;
			inline code_t* AllocateFromLargePool() noexcept;

			inline code_t* AllocFromPool(internal::PoolBase& pool) noexcept {
				return std::get<0>(used.emplace(pool.Alloc(), pool))->first;
			}
		};
	}
}
