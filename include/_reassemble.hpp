#pragma once

#include <zydis/Zydis.h>

#include <cstdint>
#include <deque>

namespace mebius {
	using code_t = uint8_t;

	namespace reassemble {
		constexpr static inline ZydisMachineMode _MACHINE_MODE = ZYDIS_MACHINE_MODE_LONG_COMPAT_32;

		class Reassembler {
		public:
			Reassembler(uint32_t address, size_t min_size);
			void Reassemble(code_t* buffer, size_t bufferSize);

			inline size_t GetSize() const noexcept {
				return _size;
			}

			inline size_t GetOriginalSize() const noexcept {
				return _original_size;
			}

		private:
			std::deque<ZydisEncoderRequest> requests;
			size_t _size;
			size_t _original_size;

			void ConvertJmpRel8(ZydisEncoderRequest& req, uint32_t eip) noexcept;
			void ConvertJccRel8(ZydisEncoderRequest& req, uint32_t eip) noexcept;
			void ConvertRel8OnlyOpcode(ZydisEncoderRequest& req, uint32_t eip) noexcept;
		};
	}
}