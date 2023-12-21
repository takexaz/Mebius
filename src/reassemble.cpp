#include <_reassemble.hpp>
#include <error.hpp>

#include <bit>
#include <format>

using namespace mebius;
using namespace mebius::reassemble;

mebius::reassemble::Reassembler::Reassembler(uint32_t address, size_t min_size) : _size(0)
{
	size_t size = 0;
	size_t eip = address;
	do {
		ZydisDisassembledInstruction inst;
		if (ZYAN_FAILED(ZydisDisassembleIntel(_MACHINE_MODE, 0, std::bit_cast<void*>(eip), 0x7fffffff, &inst))) {
			throw MebiusError("Failed to decode assembly.");
		}
		size += inst.info.length;
		eip += inst.info.length;

		auto& req = requests.emplace_back();
		if (ZYAN_FAILED(ZydisEncoderDecodedInstructionToEncoderRequest(&(inst.info), inst.operands, inst.info.operand_count_visible, &req))) {
			throw MebiusError("Failed to make request.");
		}
		switch (req.branch_type) {
		case ZYDIS_BRANCH_TYPE_SHORT:
			if (req.mnemonic == ZYDIS_MNEMONIC_JMP) {

				(req, eip);
			}
			else if (req.mnemonic == ZYDIS_MNEMONIC_LOOP
				|| req.mnemonic == ZYDIS_MNEMONIC_LOOPE
				|| req.mnemonic == ZYDIS_MNEMONIC_LOOPNE
				|| req.mnemonic == ZYDIS_MNEMONIC_JCXZ
				|| req.mnemonic == ZYDIS_MNEMONIC_JECXZ) {
				ConvertRel8OnlyOpcode(req, eip);
			}
			else {
				ConvertJccRel8(req, eip);
			}
			break;
		case ZYDIS_BRANCH_TYPE_NEAR:
			req.operands[0].imm.u += eip;
			break;
		default:
			break;
		}
	} while (size < min_size);
	_original_size = size;
	_size += size;
}

void mebius::reassemble::Reassembler::Reassemble(code_t* buffer, size_t bufferSize) {
	size_t size = 0;
	for (auto&& req : requests) {
		size_t length = bufferSize - size;
		ZyanStatus result;
		if (req.branch_type == ZYDIS_BRANCH_TYPE_SHORT) {
			result = ZydisEncoderEncodeInstruction(&req, &buffer[size], &length);
		}
		else {
			result = ZydisEncoderEncodeInstructionAbsolute(&req, &buffer[size], &length, std::bit_cast<uint32_t>(&buffer[size]));
		}
		if (ZYAN_FAILED(result)) {
			throw MebiusError(std::vformat("Failed to encode instruction.\nmnemonic = {:d}\nERROR_CODE: 0x{:08X}", std::make_format_args(static_cast<uint32_t>(req.mnemonic), result)));
		}
		size += length;
	}
}

void mebius::reassemble::Reassembler::ConvertJmpRel8(ZydisEncoderRequest& req, uint32_t eip) noexcept {
	// JMP rel8
	// ->
	// JMP rel32
	_size += 3;
	req.operand_size_hint = ZYDIS_OPERAND_SIZE_HINT_32;
	req.branch_type = ZYDIS_BRANCH_TYPE_NEAR;
	req.branch_width = ZYDIS_BRANCH_WIDTH_32;
	req.operands[0].imm.u += eip;
}

void mebius::reassemble::Reassembler::ConvertJccRel8(ZydisEncoderRequest& req, uint32_t eip) noexcept {
	// Jcc rel8
	// ->
	// Jcc rel32
	_size += 4;
	req.operand_size_hint = ZYDIS_OPERAND_SIZE_HINT_32;
	req.branch_type = ZYDIS_BRANCH_TYPE_NEAR;
	req.branch_width = ZYDIS_BRANCH_WIDTH_32;
	req.operands[0].imm.u += eip;
}

void mebius::reassemble::Reassembler::ConvertRel8OnlyOpcode(ZydisEncoderRequest& req, uint32_t eip) noexcept {
	// LOOP rel8 / LOOPE rel8 / LOOPNE rel8 / JCXZ rel8 / JECXZ rel8
	// ->
	// LOOP 0x5 / LOOPE 0x5 / LOOPNE 0x5 / JCXZ 0x5 / JECXZ 0x5
	// JMP rel32
	// JMP rel32
	_size += 10;
	auto& req2 = requests.emplace_back();
	req2.mnemonic = ZYDIS_MNEMONIC_JMP;
	req2.allowed_encodings = ZYDIS_ENCODABLE_ENCODING_LEGACY;
	req2.address_size_hint = ZYDIS_ADDRESS_SIZE_HINT_32;
	req2.operand_size_hint = ZYDIS_OPERAND_SIZE_HINT_32;
	req2.machine_mode = _MACHINE_MODE;
	req2.branch_type = ZYDIS_BRANCH_TYPE_NEAR;
	req2.branch_width = ZYDIS_BRANCH_WIDTH_32;
	req2.operand_count = 1;
	req2.operands[0].type = ZYDIS_OPERAND_TYPE_IMMEDIATE;
	req2.operands[0].imm.u = eip;
	auto& req3 = requests.emplace_back();
	req3.mnemonic = ZYDIS_MNEMONIC_JMP;
	req3.allowed_encodings = ZYDIS_ENCODABLE_ENCODING_LEGACY;
	req3.address_size_hint = ZYDIS_ADDRESS_SIZE_HINT_32;
	req3.operand_size_hint = ZYDIS_OPERAND_SIZE_HINT_32;
	req3.machine_mode = _MACHINE_MODE;
	req3.branch_type = ZYDIS_BRANCH_TYPE_NEAR;
	req3.branch_width = ZYDIS_BRANCH_WIDTH_32;
	req3.operand_count = 1;
	req3.operands[0].type = ZYDIS_OPERAND_TYPE_IMMEDIATE;
	req3.operands[0].imm.u = req.operands[0].imm.u + eip;
	req.operands[0].imm.u = 0x5;
}
