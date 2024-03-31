#pragma once

#include <magic_enum.hpp>

#include <nese/basic_types.hpp>
#include <nese/cpu/addr_mode.hpp>

namespace nese::cpu::instruction {

enum class opcode;

constexpr string_view get_mnemonic(opcode opcode);
constexpr addr_mode get_addr_mode_from_name(opcode opcode);
constexpr byte_t get_operand_size(opcode opcode);

enum class opcode
{
    adc_immediate = 0x69,
    adc_zero_page = 0x65,
    adc_zero_page_x = 0x75,
    adc_absolute = 0x6D,
    adc_absolute_x = 0x7D,
    adc_absolute_y = 0x79,
    adc_indexed_indirect = 0x61,
    adc_indirect_indexed = 0x71,

    and_immediate = 0x29,
    and_zero_page = 0x25,
    and_zero_page_x = 0x35,
    and_absolute = 0x2D,
    and_absolute_x = 0x3D,
    and_absolute_y = 0x39,
    and_indexed_indirect = 0x21,
    and_indirect_indexed = 0x31,

    asl_accumulator = 0x0A,
    asl_zero_page = 0x06,
    asl_zero_page_x = 0x16,
    asl_absolute = 0x0E,
    asl_absolute_x = 0x1E,

    bcc_relative = 0x90,
    bcs_relative = 0xB0,
    beq_relative = 0xF0,
    bit_zero_page = 0x24,
    bit_absolute = 0x2C,
    bmi_relative = 0x30,
    bne_relative = 0xD0,
    bpl_relative = 0x10,
    brk_implied = 0x00,
    bvc_relative = 0x50,
    bvs_relative = 0x70,

    clc_implied = 0x18,
    cld_implied = 0xD8,
    cli_implied = 0x58,
    clv_implied = 0xB8,

    cmp_immediate = 0xC9,
    cmp_zero_page = 0xC5,
    cmp_zero_page_x = 0xD5,
    cmp_absolute = 0xCD,
    cmp_absolute_x = 0xDD,
    cmp_absolute_y = 0xD9,
    cmp_indexed_indirect = 0xC1,
    cmp_indirect_indexed = 0xD1,

    cpx_immediate = 0xE0,
    cpx_zero_page = 0xE4,
    cpx_absolute = 0xEC,

    cpy_immediate = 0xC0,
    cpy_zero_page = 0xC4,
    cpy_absolute = 0xCC,

    dec_zero_page = 0xC6,
    dec_zero_page_x = 0xD6,
    dec_absolute = 0xCE,
    dec_absolute_x = 0xDE,

    dex_implied = 0xCA,
    dey_implied = 0x88,

    eor_immediate = 0x49,
    eor_zero_page = 0x45,
    eor_zero_page_x = 0x55,
    eor_absolute = 0x4D,
    eor_absolute_x = 0x5D,
    eor_absolute_y = 0x59,
    eor_indexed_indirect = 0x41,
    eor_indirect_indexed = 0x51,

    inc_zero_page = 0xE6,
    inc_zero_page_x = 0xF6,
    inc_absolute = 0xEE,
    inc_absolute_x = 0xFE,

    inx_implied = 0xE8,
    iny_implied = 0xC8,

    jmp_absolute = 0x4C,
    jmp_indirect = 0x6C,

    jsr_absolute = 0x20,

    lda_immediate = 0xA9,
    lda_zero_page = 0xA5,
    lda_zero_page_x = 0xB5,
    lda_absolute = 0xAD,
    lda_absolute_x = 0xBD,
    lda_absolute_y = 0xB9,
    lda_indexed_indirect = 0xA1,
    lda_indirect_indexed = 0xB1,

    ldx_immediate = 0xA2,
    ldx_zero_page = 0xA6,
    ldx_zero_page_y = 0xB6,
    ldx_absolute = 0xAE,
    ldx_absolute_y = 0xBE,

    ldy_immediate = 0xA0,
    ldy_zero_page = 0xA4,
    ldy_zero_page_x = 0xB4,
    ldy_absolute = 0xAC,
    ldy_absolute_x = 0xBC,

    lsr_accumulator = 0x4A,
    lsr_zero_page = 0x46,
    lsr_zero_page_x = 0x56,
    lsr_absolute = 0x4E,
    lsr_absolute_x = 0x5E,

    nop_implied = 0xEA,

    ora_immediate = 0x09,
    ora_zero_page = 0x05,
    ora_zero_page_x = 0x15,
    ora_absolute = 0x0D,
    ora_absolute_x = 0x1D,
    ora_absolute_y = 0x19,
    ora_indexed_indirect = 0x01,
    ora_indirect_indexed = 0x11,

    pha_implied = 0x48,
    php_implied = 0x08,
    pla_implied = 0x68,
    plp_implied = 0x28,

    rol_accumulator = 0x2A,
    rol_zero_page = 0x26,
    rol_zero_page_x = 0x36,
    rol_absolute = 0x2E,
    rol_absolute_x = 0x3E,

    ror_accumulator = 0x6A,
    ror_zero_page = 0x66,
    ror_zero_page_x = 0x76,
    ror_absolute = 0x6E,
    ror_absolute_x = 0x7E,

    rti_implied = 0x40,
    rts_implied = 0x60,

    sbc_immediate = 0xE9,
    sbc_zero_page = 0xE5,
    sbc_zero_page_x = 0xF5,
    sbc_absolute = 0xED,
    sbc_absolute_x = 0xFD,
    sbc_absolute_y = 0xF9,
    sbc_indexed_indirect = 0xE1,
    sbc_indirect_indexed = 0xF1,

    sec_implied = 0x38,
    sed_implied = 0xF8,
    sei_implied = 0x78,

    sta_zero_page = 0x85,
    sta_zero_page_x = 0x95,
    sta_absolute = 0x8D,
    sta_absolute_x = 0x9D,
    sta_absolute_y = 0x99,
    sta_indexed_indirect = 0x81,
    sta_indirect_indexed = 0x91,

    stx_zero_page = 0x86,
    stx_zero_page_y = 0x96,
    stx_absolute = 0x8E,

    sty_zero_page = 0x84,
    sty_zero_page_x = 0x94,
    sty_absolute = 0x8C,

    tax_implied = 0xAA,
    tay_implied = 0xA8,
    tsx_implied = 0xBA,
    txa_implied = 0x8A,
    txs_implied = 0x9A,
    tya_implied = 0x98
};
} // namespace nese::cpu::instruction

template<>
struct magic_enum::customize::enum_range<nese::cpu::instruction::opcode>
{
    static constexpr bool is_flags = false;

    static constexpr int min = 0x00;
    static constexpr int max = 0xFF;
};

namespace nese::cpu::instruction {

constexpr string_view get_mnemonic(opcode opcode)
{
    const string_view name = magic_enum::enum_name(opcode);
    const string_view::size_type idx = name.find_first_of('_');

    return name.substr(0, idx);
}

constexpr addr_mode get_addr_mode_from_name(opcode opcode)
{
    const string_view name = magic_enum::enum_name(opcode);
    const string_view::size_type idx = name.find_first_of('_');
    const string_view addr_mode = name.substr(idx + 1);

    if (addr_mode == "implied")
    {
        return addr_mode::implied;
    }

    if (addr_mode == "accumulator")
    {
        return addr_mode::accumulator;
    }

    if (addr_mode == "immediate")
    {
        return addr_mode::immediate;
    }

    if (addr_mode == "zero_page")
    {
        return addr_mode::zero_page;
    }

    if (addr_mode == "zero_page_x")
    {
        return addr_mode::zero_page_x;
    }

    if (addr_mode == "zero_page_y")
    {
        return addr_mode::zero_page_y;
    }

    if (addr_mode == "absolute")
    {
        return addr_mode::absolute;
    }

    if (addr_mode == "absolute_x")
    {
        return addr_mode::absolute_x;
    }

    if (addr_mode == "absolute_y")
    {
        return addr_mode::absolute_y;
    }

    if (addr_mode == "relative")
    {
        return addr_mode::relative;
    }

    if (addr_mode == "indirect")
    {
        return addr_mode::indirect;
    }

    if (addr_mode == "indexed_indirect")
    {
        return addr_mode::indexed_indirect;
    }

    if (addr_mode == "indirect_indexed")
    {
        return addr_mode::indirect_indexed;
    }

    NESE_ASSUME(false);
}

constexpr byte_t get_operand_size(opcode opcode)
{
    switch (get_addr_mode_from_name(opcode))
    {
    case addr_mode::implied:
    case addr_mode::accumulator:
        return 0;

    case addr_mode::relative:
    case addr_mode::immediate:
    case addr_mode::zero_page:
    case addr_mode::zero_page_x:
    case addr_mode::zero_page_y:
    case addr_mode::indexed_indirect:
    case addr_mode::indirect_indexed:
        return 1;

    case addr_mode::indirect:
    case addr_mode::absolute:
    case addr_mode::absolute_x:
    case addr_mode::absolute_y:
        return 2;
    }

    NESE_ASSUME(false);
}

constexpr auto format_as(opcode code)
{
    return static_cast<u8_t>(code);
}

} // namespace nese::cpu::instruction
