#pragma once

#ifndef NESE_UNOFFICIAL_INSTRUCTIONS_ENABLED
#define NESE_UNOFFICIAL_INSTRUCTIONS_ENABLED 1
#endif

namespace nese::cpu::instruction {

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
    tya_implied = 0x98,

#if NESE_UNOFFICIAL_INSTRUCTIONS_ENABLED
    // Unofficial instructions
    nop_immediate_unofficial_80 = 0x80,

    nop_implied_unofficial_1A = 0x1A,
    nop_implied_unofficial_3A = 0x3A,
    nop_implied_unofficial_5A = 0x5A,
    nop_implied_unofficial_7A = 0x7A,
    nop_implied_unofficial_DA = 0xDA,
    nop_implied_unofficial_FA = 0xFA,

    nop_zero_page_unofficial_04 = 0x04,
    nop_zero_page_unofficial_44 = 0x44,
    nop_zero_page_unofficial_64 = 0x64,

    nop_absolute_unofficial_0C = 0x0C,

    nop_zero_page_x_unofficial_14 = 0x14,
    nop_zero_page_x_unofficial_34 = 0x34,
    nop_zero_page_x_unofficial_54 = 0x54,
    nop_zero_page_x_unofficial_74 = 0x74,
    nop_zero_page_x_unofficial_D4 = 0xD4,
    nop_zero_page_x_unofficial_F4 = 0xF4,

    nop_absolute_x_unofficial_1C = 0x1C,
    nop_absolute_x_unofficial_3C = 0x3C,
    nop_absolute_x_unofficial_5C = 0x5C,
    nop_absolute_x_unofficial_7C = 0x7C,
    nop_absolute_x_unofficial_DC = 0xDC,
    nop_absolute_x_unofficial_FC = 0xFC,

#endif // NESE_UNOFFICIAL_INSTRUCTIONS_ENABLED
};

} // namespace nese::cpu::instruction