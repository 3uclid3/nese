#include <nese/cpu/cpu_stack_offset.hpp>
#include <nese/utility/assert.hpp>

namespace nese::v2 {

constexpr bool is_page_crossing(addr_t addr, addr_t new_addr)
{
    const addr_t page_start = addr & 0xFF00;
    const addr_t new_page_start = new_addr & 0xFF00;

    return page_start != new_page_start;
}

constexpr bool is_zero(byte_t value)
{
    return value == 0;
}

constexpr bool is_negative(byte_t value)
{
    return value & 0x80;
}

constexpr bool is_overflow(byte_t value)
{
    return value & 0x40;
}

constexpr bool is_sign_overflow(byte_t old_byte, byte_t new_byte, byte_t byte)
{
    return (old_byte & 0x80) == (byte & 0x80) && (old_byte & 0x80) != (new_byte & 0x80);
}

template<typename BusT>
consteval typename cpu<BusT>::instruction_callback_table cpu<BusT>::instruction_callback_table::create()
{
    instruction_callback_table table{};

    table[cpu_opcode::adc_immediate] = &cpu::instruction_adc<cpu_addr_mode::immediate>;
    table[cpu_opcode::adc_zero_page] = &cpu::instruction_adc<cpu_addr_mode::zero_page>;
    table[cpu_opcode::adc_zero_page_x] = &cpu::instruction_adc<cpu_addr_mode::zero_page_x>;
    table[cpu_opcode::adc_absolute] = &cpu::instruction_adc<cpu_addr_mode::absolute>;
    table[cpu_opcode::adc_absolute_x] = &cpu::instruction_adc<cpu_addr_mode::absolute_x>;
    table[cpu_opcode::adc_absolute_y] = &cpu::instruction_adc<cpu_addr_mode::absolute_y>;
    table[cpu_opcode::adc_indexed_indirect] = &cpu::instruction_adc<cpu_addr_mode::indexed_indirect>;
    table[cpu_opcode::adc_indirect_indexed] = &cpu::instruction_adc<cpu_addr_mode::indirect_indexed>;

    table[cpu_opcode::and_immediate] = &cpu::instruction_and<cpu_addr_mode::immediate>;
    table[cpu_opcode::and_zero_page] = &cpu::instruction_and<cpu_addr_mode::zero_page>;
    table[cpu_opcode::and_zero_page_x] = &cpu::instruction_and<cpu_addr_mode::zero_page_x>;
    table[cpu_opcode::and_absolute] = &cpu::instruction_and<cpu_addr_mode::absolute>;
    table[cpu_opcode::and_absolute_x] = &cpu::instruction_and<cpu_addr_mode::absolute_x>;
    table[cpu_opcode::and_absolute_y] = &cpu::instruction_and<cpu_addr_mode::absolute_y>;
    table[cpu_opcode::and_indexed_indirect] = &cpu::instruction_and<cpu_addr_mode::indexed_indirect>;
    table[cpu_opcode::and_indirect_indexed] = &cpu::instruction_and<cpu_addr_mode::indirect_indexed>;

    table[cpu_opcode::asl_accumulator] = &cpu::instruction_asl<cpu_addr_mode::accumulator>;
    table[cpu_opcode::asl_zero_page] = &cpu::instruction_asl<cpu_addr_mode::zero_page>;
    table[cpu_opcode::asl_zero_page_x] = &cpu::instruction_asl<cpu_addr_mode::zero_page_x>;
    table[cpu_opcode::asl_absolute] = &cpu::instruction_asl<cpu_addr_mode::absolute>;
    table[cpu_opcode::asl_absolute_x] = &cpu::instruction_asl<cpu_addr_mode::absolute_x>;

    table[cpu_opcode::bcc_relative] = &cpu::instruction_bcc<cpu_addr_mode::relative>;
    table[cpu_opcode::bcs_relative] = &cpu::instruction_bcs<cpu_addr_mode::relative>;
    table[cpu_opcode::beq_relative] = &cpu::instruction_beq<cpu_addr_mode::relative>;
    table[cpu_opcode::bmi_relative] = &cpu::instruction_bmi<cpu_addr_mode::relative>;
    table[cpu_opcode::bne_relative] = &cpu::instruction_bne<cpu_addr_mode::relative>;
    table[cpu_opcode::bpl_relative] = &cpu::instruction_bpl<cpu_addr_mode::relative>;
    table[cpu_opcode::bvc_relative] = &cpu::instruction_bvc<cpu_addr_mode::relative>;
    table[cpu_opcode::bvs_relative] = &cpu::instruction_bvs<cpu_addr_mode::relative>;

    table[cpu_opcode::bit_zero_page] = &cpu::instruction_bit<cpu_addr_mode::zero_page>;
    table[cpu_opcode::bit_absolute] = &cpu::instruction_bit<cpu_addr_mode::absolute>;

    table[cpu_opcode::brk_implied] = &cpu::instruction_brk<cpu_addr_mode::implied>;

    table[cpu_opcode::clc_implied] = &cpu::instruction_clc<cpu_addr_mode::implied>;
    table[cpu_opcode::cld_implied] = &cpu::instruction_cld<cpu_addr_mode::implied>;
    table[cpu_opcode::cli_implied] = &cpu::instruction_cli<cpu_addr_mode::implied>;
    table[cpu_opcode::clv_implied] = &cpu::instruction_clv<cpu_addr_mode::implied>;

    table[cpu_opcode::cmp_immediate] = &cpu::instruction_cmp<cpu_addr_mode::immediate>;
    table[cpu_opcode::cmp_zero_page] = &cpu::instruction_cmp<cpu_addr_mode::zero_page>;
    table[cpu_opcode::cmp_zero_page_x] = &cpu::instruction_cmp<cpu_addr_mode::zero_page_x>;
    table[cpu_opcode::cmp_absolute] = &cpu::instruction_cmp<cpu_addr_mode::absolute>;
    table[cpu_opcode::cmp_absolute_x] = &cpu::instruction_cmp<cpu_addr_mode::absolute_x>;
    table[cpu_opcode::cmp_absolute_y] = &cpu::instruction_cmp<cpu_addr_mode::absolute_y>;
    table[cpu_opcode::cmp_indexed_indirect] = &cpu::instruction_cmp<cpu_addr_mode::indexed_indirect>;
    table[cpu_opcode::cmp_indirect_indexed] = &cpu::instruction_cmp<cpu_addr_mode::indirect_indexed>;

    table[cpu_opcode::cpx_immediate] = &cpu::instruction_cpx<cpu_addr_mode::immediate>;
    table[cpu_opcode::cpx_zero_page] = &cpu::instruction_cpx<cpu_addr_mode::zero_page>;
    table[cpu_opcode::cpx_absolute] = &cpu::instruction_cpx<cpu_addr_mode::absolute>;

    table[cpu_opcode::cpy_immediate] = &cpu::instruction_cpy<cpu_addr_mode::immediate>;
    table[cpu_opcode::cpy_zero_page] = &cpu::instruction_cpy<cpu_addr_mode::zero_page>;
    table[cpu_opcode::cpy_absolute] = &cpu::instruction_cpy<cpu_addr_mode::absolute>;

    table[cpu_opcode::dec_zero_page] = &cpu::instruction_dec<cpu_addr_mode::zero_page>;
    table[cpu_opcode::dec_zero_page_x] = &cpu::instruction_dec<cpu_addr_mode::zero_page_x>;
    table[cpu_opcode::dec_absolute] = &cpu::instruction_dec<cpu_addr_mode::absolute>;
    table[cpu_opcode::dec_absolute_x] = &cpu::instruction_dec<cpu_addr_mode::absolute_x>;

    table[cpu_opcode::dex_implied] = &cpu::instruction_dex<cpu_addr_mode::implied>;
    table[cpu_opcode::dey_implied] = &cpu::instruction_dey<cpu_addr_mode::implied>;

    table[cpu_opcode::eor_immediate] = &cpu::instruction_eor<cpu_addr_mode::immediate>;
    table[cpu_opcode::eor_zero_page] = &cpu::instruction_eor<cpu_addr_mode::zero_page>;
    table[cpu_opcode::eor_zero_page_x] = &cpu::instruction_eor<cpu_addr_mode::zero_page_x>;
    table[cpu_opcode::eor_absolute] = &cpu::instruction_eor<cpu_addr_mode::absolute>;
    table[cpu_opcode::eor_absolute_x] = &cpu::instruction_eor<cpu_addr_mode::absolute_x>;
    table[cpu_opcode::eor_absolute_y] = &cpu::instruction_eor<cpu_addr_mode::absolute_y>;
    table[cpu_opcode::eor_indexed_indirect] = &cpu::instruction_eor<cpu_addr_mode::indexed_indirect>;
    table[cpu_opcode::eor_indirect_indexed] = &cpu::instruction_eor<cpu_addr_mode::indirect_indexed>;

    table[cpu_opcode::inc_zero_page] = &cpu::instruction_inc<cpu_addr_mode::zero_page>;
    table[cpu_opcode::inc_zero_page_x] = &cpu::instruction_inc<cpu_addr_mode::zero_page_x>;
    table[cpu_opcode::inc_absolute] = &cpu::instruction_inc<cpu_addr_mode::absolute>;
    table[cpu_opcode::inc_absolute_x] = &cpu::instruction_inc<cpu_addr_mode::absolute_x>;

    table[cpu_opcode::inx_implied] = &cpu::instruction_inx<cpu_addr_mode::implied>;
    table[cpu_opcode::iny_implied] = &cpu::instruction_iny<cpu_addr_mode::implied>;

    table[cpu_opcode::jmp_absolute] = &cpu::instruction_jmp<cpu_addr_mode::absolute>;
    table[cpu_opcode::jmp_indirect] = &cpu::instruction_jmp<cpu_addr_mode::indirect>;
    table[cpu_opcode::jsr_absolute] = &cpu::instruction_jsr<cpu_addr_mode::absolute>;

    table[cpu_opcode::lda_immediate] = &cpu::instruction_lda<cpu_addr_mode::immediate>;
    table[cpu_opcode::lda_zero_page] = &cpu::instruction_lda<cpu_addr_mode::zero_page>;
    table[cpu_opcode::lda_zero_page_x] = &cpu::instruction_lda<cpu_addr_mode::zero_page_x>;
    table[cpu_opcode::lda_absolute] = &cpu::instruction_lda<cpu_addr_mode::absolute>;
    table[cpu_opcode::lda_absolute_x] = &cpu::instruction_lda<cpu_addr_mode::absolute_x>;
    table[cpu_opcode::lda_absolute_y] = &cpu::instruction_lda<cpu_addr_mode::absolute_y>;
    table[cpu_opcode::lda_indexed_indirect] = &cpu::instruction_lda<cpu_addr_mode::indexed_indirect>;
    table[cpu_opcode::lda_indirect_indexed] = &cpu::instruction_lda<cpu_addr_mode::indirect_indexed>;

    table[cpu_opcode::ldx_immediate] = &cpu::instruction_ldx<cpu_addr_mode::immediate>;
    table[cpu_opcode::ldx_zero_page] = &cpu::instruction_ldx<cpu_addr_mode::zero_page>;
    table[cpu_opcode::ldx_zero_page_y] = &cpu::instruction_ldx<cpu_addr_mode::zero_page_y>;
    table[cpu_opcode::ldx_absolute] = &cpu::instruction_ldx<cpu_addr_mode::absolute>;
    table[cpu_opcode::ldx_absolute_y] = &cpu::instruction_ldx<cpu_addr_mode::absolute_y>;

    table[cpu_opcode::ldy_immediate] = &cpu::instruction_ldy<cpu_addr_mode::immediate>;
    table[cpu_opcode::ldy_zero_page] = &cpu::instruction_ldy<cpu_addr_mode::zero_page>;
    table[cpu_opcode::ldy_zero_page_x] = &cpu::instruction_ldy<cpu_addr_mode::zero_page_x>;
    table[cpu_opcode::ldy_absolute] = &cpu::instruction_ldy<cpu_addr_mode::absolute>;
    table[cpu_opcode::ldy_absolute_x] = &cpu::instruction_ldy<cpu_addr_mode::absolute_x>;

    table[cpu_opcode::lsr_accumulator] = &cpu::instruction_lsr<cpu_addr_mode::accumulator>;
    table[cpu_opcode::lsr_zero_page] = &cpu::instruction_lsr<cpu_addr_mode::zero_page>;
    table[cpu_opcode::lsr_zero_page_x] = &cpu::instruction_lsr<cpu_addr_mode::zero_page_x>;
    table[cpu_opcode::lsr_absolute] = &cpu::instruction_lsr<cpu_addr_mode::absolute>;
    table[cpu_opcode::lsr_absolute_x] = &cpu::instruction_lsr<cpu_addr_mode::absolute_x>;

    table[cpu_opcode::nop_implied] = &cpu::instruction_nop<cpu_addr_mode::implied>;

    table[cpu_opcode::ora_immediate] = &cpu::instruction_ora<cpu_addr_mode::immediate>;
    table[cpu_opcode::ora_zero_page] = &cpu::instruction_ora<cpu_addr_mode::zero_page>;
    table[cpu_opcode::ora_zero_page_x] = &cpu::instruction_ora<cpu_addr_mode::zero_page_x>;
    table[cpu_opcode::ora_absolute] = &cpu::instruction_ora<cpu_addr_mode::absolute>;
    table[cpu_opcode::ora_absolute_x] = &cpu::instruction_ora<cpu_addr_mode::absolute_x>;
    table[cpu_opcode::ora_absolute_y] = &cpu::instruction_ora<cpu_addr_mode::absolute_y>;
    table[cpu_opcode::ora_indexed_indirect] = &cpu::instruction_ora<cpu_addr_mode::indexed_indirect>;
    table[cpu_opcode::ora_indirect_indexed] = &cpu::instruction_ora<cpu_addr_mode::indirect_indexed>;

    table[cpu_opcode::pha_implied] = &cpu::instruction_pha<cpu_addr_mode::implied>;
    table[cpu_opcode::php_implied] = &cpu::instruction_php<cpu_addr_mode::implied>;

    table[cpu_opcode::pla_implied] = &cpu::instruction_pla<cpu_addr_mode::implied>;
    table[cpu_opcode::plp_implied] = &cpu::instruction_plp<cpu_addr_mode::implied>;

    table[cpu_opcode::rti_implied] = &cpu::instruction_rti<cpu_addr_mode::implied>;
    table[cpu_opcode::rts_implied] = &cpu::instruction_rts<cpu_addr_mode::implied>;

    table[cpu_opcode::rol_accumulator] = &cpu::instruction_rol<cpu_addr_mode::accumulator>;
    table[cpu_opcode::rol_zero_page] = &cpu::instruction_rol<cpu_addr_mode::zero_page>;
    table[cpu_opcode::rol_zero_page_x] = &cpu::instruction_rol<cpu_addr_mode::zero_page_x>;
    table[cpu_opcode::rol_absolute] = &cpu::instruction_rol<cpu_addr_mode::absolute>;
    table[cpu_opcode::rol_absolute_x] = &cpu::instruction_rol<cpu_addr_mode::absolute_x>;

    table[cpu_opcode::ror_accumulator] = &cpu::instruction_ror<cpu_addr_mode::accumulator>;
    table[cpu_opcode::ror_zero_page] = &cpu::instruction_ror<cpu_addr_mode::zero_page>;
    table[cpu_opcode::ror_zero_page_x] = &cpu::instruction_ror<cpu_addr_mode::zero_page_x>;
    table[cpu_opcode::ror_absolute] = &cpu::instruction_ror<cpu_addr_mode::absolute>;
    table[cpu_opcode::ror_absolute_x] = &cpu::instruction_ror<cpu_addr_mode::absolute_x>;

    table[cpu_opcode::sbc_immediate] = &cpu::instruction_sbc<cpu_addr_mode::immediate>;
    table[cpu_opcode::sbc_zero_page] = &cpu::instruction_sbc<cpu_addr_mode::zero_page>;
    table[cpu_opcode::sbc_zero_page_x] = &cpu::instruction_sbc<cpu_addr_mode::zero_page_x>;
    table[cpu_opcode::sbc_absolute] = &cpu::instruction_sbc<cpu_addr_mode::absolute>;
    table[cpu_opcode::sbc_absolute_x] = &cpu::instruction_sbc<cpu_addr_mode::absolute_x>;
    table[cpu_opcode::sbc_absolute_y] = &cpu::instruction_sbc<cpu_addr_mode::absolute_y>;
    table[cpu_opcode::sbc_indexed_indirect] = &cpu::instruction_sbc<cpu_addr_mode::indexed_indirect>;
    table[cpu_opcode::sbc_indirect_indexed] = &cpu::instruction_sbc<cpu_addr_mode::indirect_indexed>;

    table[cpu_opcode::sec_implied] = &cpu::instruction_sec<cpu_addr_mode::implied>;
    table[cpu_opcode::sed_implied] = &cpu::instruction_sed<cpu_addr_mode::implied>;
    table[cpu_opcode::sei_implied] = &cpu::instruction_sei<cpu_addr_mode::implied>;

    table[cpu_opcode::sta_zero_page] = &cpu::instruction_sta<cpu_addr_mode::zero_page>;
    table[cpu_opcode::sta_zero_page_x] = &cpu::instruction_sta<cpu_addr_mode::zero_page_x>;
    table[cpu_opcode::sta_absolute] = &cpu::instruction_sta<cpu_addr_mode::absolute>;
    table[cpu_opcode::sta_absolute_x] = &cpu::instruction_sta<cpu_addr_mode::absolute_x>;
    table[cpu_opcode::sta_absolute_y] = &cpu::instruction_sta<cpu_addr_mode::absolute_y>;
    table[cpu_opcode::sta_indexed_indirect] = &cpu::instruction_sta<cpu_addr_mode::indexed_indirect>;
    table[cpu_opcode::sta_indirect_indexed] = &cpu::instruction_sta<cpu_addr_mode::indirect_indexed>;

    table[cpu_opcode::stx_zero_page] = &cpu::instruction_stx<cpu_addr_mode::zero_page>;
    table[cpu_opcode::stx_zero_page_y] = &cpu::instruction_stx<cpu_addr_mode::zero_page_y>;
    table[cpu_opcode::stx_absolute] = &cpu::instruction_stx<cpu_addr_mode::absolute>;

    table[cpu_opcode::sty_zero_page] = &cpu::instruction_sty<cpu_addr_mode::zero_page>;
    table[cpu_opcode::sty_zero_page_x] = &cpu::instruction_sty<cpu_addr_mode::zero_page_x>;
    table[cpu_opcode::sty_absolute] = &cpu::instruction_sty<cpu_addr_mode::absolute>;

    table[cpu_opcode::tax_implied] = &cpu::instruction_tax<cpu_addr_mode::implied>;
    table[cpu_opcode::tay_implied] = &cpu::instruction_tay<cpu_addr_mode::implied>;
    table[cpu_opcode::tsx_implied] = &cpu::instruction_tsx<cpu_addr_mode::implied>;
    table[cpu_opcode::txa_implied] = &cpu::instruction_txa<cpu_addr_mode::implied>;
    table[cpu_opcode::txs_implied] = &cpu::instruction_txs<cpu_addr_mode::implied>;
    table[cpu_opcode::tya_implied] = &cpu::instruction_tya<cpu_addr_mode::implied>;

#if NESE_UNOFFICIAL_INSTRUCTIONS_ENABLED
    table[cpu_opcode::dcp_zero_page_unofficial] = &cpu::instruction_dcp<cpu_addr_mode::zero_page>;
    table[cpu_opcode::dcp_zero_page_x_unofficial] = &cpu::instruction_dcp<cpu_addr_mode::zero_page_x>;
    table[cpu_opcode::dcp_absolute_unofficial] = &cpu::instruction_dcp<cpu_addr_mode::absolute>;
    table[cpu_opcode::dcp_absolute_x_unofficial] = &cpu::instruction_dcp<cpu_addr_mode::absolute_x>;
    table[cpu_opcode::dcp_absolute_y_unofficial] = &cpu::instruction_dcp<cpu_addr_mode::absolute_y>;
    table[cpu_opcode::dcp_indexed_indirect_unofficial] = &cpu::instruction_dcp<cpu_addr_mode::indexed_indirect>;
    table[cpu_opcode::dcp_indirect_indexed_unofficial] = &cpu::instruction_dcp<cpu_addr_mode::indirect_indexed>;

    table[cpu_opcode::isb_zero_page_unofficial] = &cpu::instruction_isb<cpu_addr_mode::zero_page>;
    table[cpu_opcode::isb_zero_page_x_unofficial] = &cpu::instruction_isb<cpu_addr_mode::zero_page_x>;
    table[cpu_opcode::isb_absolute_unofficial] = &cpu::instruction_isb<cpu_addr_mode::absolute>;
    table[cpu_opcode::isb_absolute_x_unofficial] = &cpu::instruction_isb<cpu_addr_mode::absolute_x>;
    table[cpu_opcode::isb_absolute_y_unofficial] = &cpu::instruction_isb<cpu_addr_mode::absolute_y>;
    table[cpu_opcode::isb_indexed_indirect_unofficial] = &cpu::instruction_isb<cpu_addr_mode::indexed_indirect>;
    table[cpu_opcode::isb_indirect_indexed_unofficial] = &cpu::instruction_isb<cpu_addr_mode::indirect_indexed>;

    table[cpu_opcode::lax_zero_page_unofficial] = &cpu::instruction_lax<cpu_addr_mode::zero_page>;
    table[cpu_opcode::lax_zero_page_y_unofficial] = &cpu::instruction_lax<cpu_addr_mode::zero_page_y>;
    table[cpu_opcode::lax_absolute_unofficial] = &cpu::instruction_lax<cpu_addr_mode::absolute>;
    table[cpu_opcode::lax_absolute_y_unofficial] = &cpu::instruction_lax<cpu_addr_mode::absolute_y>;
    table[cpu_opcode::lax_indexed_indirect_unofficial] = &cpu::instruction_lax<cpu_addr_mode::indexed_indirect>;
    table[cpu_opcode::lax_indirect_indexed_unofficial] = &cpu::instruction_lax<cpu_addr_mode::indirect_indexed>;

    table[cpu_opcode::nop_immediate_unofficial_80] = &cpu::instruction_nop<cpu_addr_mode::immediate>;
    table[cpu_opcode::nop_implied_unofficial_1A] = &cpu::instruction_nop<cpu_addr_mode::implied>;
    table[cpu_opcode::nop_implied_unofficial_3A] = &cpu::instruction_nop<cpu_addr_mode::implied>;
    table[cpu_opcode::nop_implied_unofficial_5A] = &cpu::instruction_nop<cpu_addr_mode::implied>;
    table[cpu_opcode::nop_implied_unofficial_7A] = &cpu::instruction_nop<cpu_addr_mode::implied>;
    table[cpu_opcode::nop_implied_unofficial_DA] = &cpu::instruction_nop<cpu_addr_mode::implied>;
    table[cpu_opcode::nop_implied_unofficial_FA] = &cpu::instruction_nop<cpu_addr_mode::implied>;
    table[cpu_opcode::nop_zero_page_unofficial_04] = &cpu::instruction_nop<cpu_addr_mode::zero_page>;
    table[cpu_opcode::nop_zero_page_unofficial_44] = &cpu::instruction_nop<cpu_addr_mode::zero_page>;
    table[cpu_opcode::nop_zero_page_unofficial_64] = &cpu::instruction_nop<cpu_addr_mode::zero_page>;
    table[cpu_opcode::nop_zero_page_x_unofficial_14] = &cpu::instruction_nop<cpu_addr_mode::zero_page_x>;
    table[cpu_opcode::nop_zero_page_x_unofficial_34] = &cpu::instruction_nop<cpu_addr_mode::zero_page_x>;
    table[cpu_opcode::nop_zero_page_x_unofficial_54] = &cpu::instruction_nop<cpu_addr_mode::zero_page_x>;
    table[cpu_opcode::nop_zero_page_x_unofficial_74] = &cpu::instruction_nop<cpu_addr_mode::zero_page_x>;
    table[cpu_opcode::nop_zero_page_x_unofficial_D4] = &cpu::instruction_nop<cpu_addr_mode::zero_page_x>;
    table[cpu_opcode::nop_zero_page_x_unofficial_F4] = &cpu::instruction_nop<cpu_addr_mode::zero_page_x>;
    table[cpu_opcode::nop_absolute_unofficial_0C] = &cpu::instruction_nop<cpu_addr_mode::absolute>;
    table[cpu_opcode::nop_absolute_x_unofficial_1C] = &cpu::instruction_nop<cpu_addr_mode::absolute_x>;
    table[cpu_opcode::nop_absolute_x_unofficial_3C] = &cpu::instruction_nop<cpu_addr_mode::absolute_x>;
    table[cpu_opcode::nop_absolute_x_unofficial_5C] = &cpu::instruction_nop<cpu_addr_mode::absolute_x>;
    table[cpu_opcode::nop_absolute_x_unofficial_7C] = &cpu::instruction_nop<cpu_addr_mode::absolute_x>;
    table[cpu_opcode::nop_absolute_x_unofficial_DC] = &cpu::instruction_nop<cpu_addr_mode::absolute_x>;
    table[cpu_opcode::nop_absolute_x_unofficial_FC] = &cpu::instruction_nop<cpu_addr_mode::absolute_x>;

    table[cpu_opcode::rla_zero_page_unofficial] = &cpu::instruction_rla<cpu_addr_mode::zero_page>;
    table[cpu_opcode::rla_zero_page_x_unofficial] = &cpu::instruction_rla<cpu_addr_mode::zero_page_x>;
    table[cpu_opcode::rla_absolute_unofficial] = &cpu::instruction_rla<cpu_addr_mode::absolute>;
    table[cpu_opcode::rla_absolute_x_unofficial] = &cpu::instruction_rla<cpu_addr_mode::absolute_x>;
    table[cpu_opcode::rla_absolute_y_unofficial] = &cpu::instruction_rla<cpu_addr_mode::absolute_y>;
    table[cpu_opcode::rla_indexed_indirect_unofficial] = &cpu::instruction_rla<cpu_addr_mode::indexed_indirect>;
    table[cpu_opcode::rla_indirect_indexed_unofficial] = &cpu::instruction_rla<cpu_addr_mode::indirect_indexed>;

    table[cpu_opcode::rra_zero_page_unofficial] = &cpu::instruction_rra<cpu_addr_mode::zero_page>;
    table[cpu_opcode::rra_zero_page_x_unofficial] = &cpu::instruction_rra<cpu_addr_mode::zero_page_x>;
    table[cpu_opcode::rra_absolute_unofficial] = &cpu::instruction_rra<cpu_addr_mode::absolute>;
    table[cpu_opcode::rra_absolute_x_unofficial] = &cpu::instruction_rra<cpu_addr_mode::absolute_x>;
    table[cpu_opcode::rra_absolute_y_unofficial] = &cpu::instruction_rra<cpu_addr_mode::absolute_y>;
    table[cpu_opcode::rra_indexed_indirect_unofficial] = &cpu::instruction_rra<cpu_addr_mode::indexed_indirect>;
    table[cpu_opcode::rra_indirect_indexed_unofficial] = &cpu::instruction_rra<cpu_addr_mode::indirect_indexed>;

    table[cpu_opcode::sbc_immediate_unofficial] = &cpu::instruction_sbc<cpu_addr_mode::immediate>;

    table[cpu_opcode::sax_zero_page_unofficial] = &cpu::instruction_sax<cpu_addr_mode::zero_page>;
    table[cpu_opcode::sax_zero_page_y_unofficial] = &cpu::instruction_sax<cpu_addr_mode::zero_page_y>;
    table[cpu_opcode::sax_absolute_unofficial] = &cpu::instruction_sax<cpu_addr_mode::absolute>;
    table[cpu_opcode::sax_indexed_indirect_unofficial] = &cpu::instruction_sax<cpu_addr_mode::indexed_indirect>;

    table[cpu_opcode::slo_zero_page_unofficial] = &cpu::instruction_slo<cpu_addr_mode::zero_page>;
    table[cpu_opcode::slo_zero_page_x_unofficial] = &cpu::instruction_slo<cpu_addr_mode::zero_page_x>;
    table[cpu_opcode::slo_absolute_unofficial] = &cpu::instruction_slo<cpu_addr_mode::absolute>;
    table[cpu_opcode::slo_absolute_x_unofficial] = &cpu::instruction_slo<cpu_addr_mode::absolute_x>;
    table[cpu_opcode::slo_absolute_y_unofficial] = &cpu::instruction_slo<cpu_addr_mode::absolute_y>;
    table[cpu_opcode::slo_indexed_indirect_unofficial] = &cpu::instruction_slo<cpu_addr_mode::indexed_indirect>;
    table[cpu_opcode::slo_indirect_indexed_unofficial] = &cpu::instruction_slo<cpu_addr_mode::indirect_indexed>;

    table[cpu_opcode::sre_zero_page_unofficial] = &cpu::instruction_sre<cpu_addr_mode::zero_page>;
    table[cpu_opcode::sre_zero_page_x_unofficial] = &cpu::instruction_sre<cpu_addr_mode::zero_page_x>;
    table[cpu_opcode::sre_absolute_unofficial] = &cpu::instruction_sre<cpu_addr_mode::absolute>;
    table[cpu_opcode::sre_absolute_x_unofficial] = &cpu::instruction_sre<cpu_addr_mode::absolute_x>;
    table[cpu_opcode::sre_absolute_y_unofficial] = &cpu::instruction_sre<cpu_addr_mode::absolute_y>;
    table[cpu_opcode::sre_indexed_indirect_unofficial] = &cpu::instruction_sre<cpu_addr_mode::indexed_indirect>;
    table[cpu_opcode::sre_indirect_indexed_unofficial] = &cpu::instruction_sre<cpu_addr_mode::indirect_indexed>;
#endif // NESE_UNOFFICIAL_INSTRUCTIONS_ENABLED

    return table;
}

template<typename BusT>
cpu<BusT>::cpu(BusT& bus)
    : _bus(bus)
{
}

template<typename BusT>
void cpu<BusT>::reset()
{
}

template<typename BusT>
void cpu<BusT>::irq()
{
}

template<typename BusT>
void cpu<BusT>::nmi()
{
}

template<typename BusT>
bool cpu<BusT>::step()
{
    const auto opcode = static_cast<cpu_opcode>(decode());
    const auto instruction = _instructions[opcode];

    if (instruction == nullptr) [[unlikely]]
    {
        NESE_ERROR("[cpu] Unimplemented instruction {:02X}", opcode);
        return false;
    }

    ((*this).*instruction)();

    return true;
}

template<typename BusT>
const cpu_state& cpu<BusT>::get_state() const
{
    return _state;
}

template<typename BusT>
cpu_state& cpu<BusT>::get_state()
{
    return _state;
}

template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::add_with_carry(byte_t value)
{
    const byte_t old_byte = a();

    // FF+FF+1=FF -> need to detect such case by checking for overflow in each step
    // However, for FF+80+1=80 -> the sign bit got "rescued" so we should just check final result
    byte_t new_byte = a() + value;
    bool bit7_overflow = new_byte < old_byte;
    const byte_t new_byte_pre_carry = new_byte;

    new_byte += is_status_set(cpu_status::carry) ? 1 : 0;
    bit7_overflow = bit7_overflow || new_byte < new_byte_pre_carry;

    a() = new_byte;

    set_status(cpu_status::overflow, is_sign_overflow(old_byte, a(), value));
    set_status(cpu_status::carry, bit7_overflow);
    set_status(cpu_status::zero, is_zero(a()));
    set_status(cpu_status::negative, is_negative(a()));
}

template<typename BusT>
void cpu<BusT>::branch(bool condition)
{
    const addr_t initial_pc = pc();
    const s8_t byte = static_cast<s8_t>(decode());

    if (condition)
    {
        // if branch succeeds ++
        step_cycle(1);
        pc() += byte;

        // if crossing to a new page ++
        if (is_page_crossing(initial_pc, pc()))
        {
            step_cycle(1);
        }
    }

    step_cycle(2);
}

template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::compare(byte_t to)
{
    bool page_crossing = false;
    const word_t operand = decode_operand<AddrModeT>(page_crossing);
    const byte_t byte = read_operand<AddrModeT>(operand);
    const byte_t diff = to - byte;

    set_status(cpu_status::carry, to >= byte);
    set_status(cpu_status::zero, is_zero(diff));
    set_status(cpu_status::negative, is_negative(diff));

    step_cycle(get_addr_mode_cycle_cost<AddrModeT>(page_crossing));
}

template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::load(byte_t& value)
{
    bool page_crossing{false};
    const word_t operand = decode_operand<AddrModeT>(page_crossing);

    value = read_operand<AddrModeT>(operand);

    set_status(cpu_status::zero, is_zero(value));
    set_status(cpu_status::negative, is_negative(value));
    step_cycle(get_addr_mode_cycle_cost<AddrModeT>(page_crossing));
}

template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::store(byte_t value)
{
    const word_t operand = decode_operand<AddrModeT>();

    write(operand, value);
}

// ADC (Add with Carry):
// Adds a memory value and the carry flag to the accumulator, affecting flags for carry, zero, overflow, and negative.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_adc()
{
    bool page_crossing{false};
    const addr_t addr = decode_operand<AddrModeT>(page_crossing);
    const byte_t byte = read_operand<AddrModeT>(addr);

    add_with_carry<AddrModeT>(byte);

    page_crossing = true;
    step_cycle(get_addr_mode_cycle_cost<AddrModeT>(page_crossing));
}

// AND (Logical AND):
// Performs a bitwise AND on the accumulator and a memory value, affecting the zero and negative flags.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_and()
{
    bool page_crossing{false};
    const addr_t addr = decode_operand<AddrModeT>(page_crossing);
    const byte_t byte = read_operand<AddrModeT>(addr);

    a() &= byte;

    set_status(cpu_status::zero, is_zero(a()));
    set_status(cpu_status::negative, is_negative(a()));

    step_cycle(get_addr_mode_cycle_cost<AddrModeT>(page_crossing));
}

// ASL (Arithmetic Shift Left):
// Shifts all bits of the accumulator or a memory location one bit to the left, setting the carry flag with the last bit's value and affecting the zero and negative flags.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_asl()
{
    bool page_crossing{false};
    const word_t operand = decode_operand<AddrModeT>(page_crossing);
    const byte_t value = read_operand<AddrModeT>(operand);
    const byte_t new_value = static_cast<byte_t>(value << 1);

    write_operand<AddrModeT>(operand, new_value);

    set_status(cpu_status::carry, value & 0x80);
    set_status(cpu_status::zero, is_zero(new_value));
    set_status(cpu_status::negative, is_negative(new_value));

    step_cycle(get_addr_mode_cycle_cost<AddrModeT>(page_crossing));
}

// BCC (Branch if Carry Clear):
// If the carry flag is clear, it adds the relative displacement to the program counter to branch to a new location.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_bcc()
{
    branch(is_status_clear(cpu_status::carry));
}

// BCS (Branch if Carry Set):
// If the carry flag is set, it adds the relative displacement to the program counter to branch to a new location.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_bcs()
{
    branch(is_status_set(cpu_status::carry));
}

// BEQ (Branch if Equal):
// If the zero flag is set, adds the relative displacement to the program counter to branch to a new location.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_beq()
{
    branch(is_status_set(cpu_status::zero));
}

// BIT (Bit Test):
// Tests bits in memory with the accumulator, affecting the zero, negative, and overflow flags.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_bit()
{
    bool page_crossing = false;
    const word_t operand = decode_operand<AddrModeT>(page_crossing);
    const byte_t byte = read_operand<AddrModeT>(operand);
    const byte_t new_byte = byte & a();

    set_status(cpu_status::zero, is_zero(new_byte));
    set_status(cpu_status::negative, is_negative(byte));
    set_status(cpu_status::overflow, is_overflow(byte));

    step_cycle(get_addr_mode_cycle_cost<AddrModeT>(page_crossing));
}

// BMI (Branch if Minus):
// If the negative flag is set, it adds the relative displacement to the program counter to branch to a new location.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_bmi()
{
    branch(is_status_set(cpu_status::negative));
}

// BNE (Branch if Not Equal):
// If the zero flag is clear, adds the relative displacement to the program counter to branch to a new location.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_bne()
{
    branch(is_status_clear(cpu_status::zero));
}

// BPL (Branch if Positive):
// If the negative flag is clear, it adds the relative displacement to the program counter to branch to a new location.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_bpl()
{
    branch(is_status_clear(cpu_status::negative));
}

// BRK (Branch if Positive):
// If the negative flag is clear, it adds the relative displacement to the program counter to branch to a new location.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_brk()
{
    step_cycle(7);
}

// BVC (Branch if Overflow Clear):
// If the overflow flag is clear, it adds the relative displacement to the program counter to branch to a new location.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_bvc()
{
    branch(is_status_clear(cpu_status::overflow));
}

// BVS (Branch if Overflow Set):
// If the overflow flag is set, it adds the relative displacement to the program counter to branch to a new location.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_bvs()
{
    branch(is_status_set(cpu_status::overflow));
}

// CLC (Clear Carry Flag):
// Clears the carry flag to 0.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_clc()
{
    clear_status(cpu_status::carry);

    step_cycle(2);
}

// CLD (Clear Decimal Mode):
// Clears the decimal mode flag, affecting how ADC and SBC instructions work.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_cld()
{
    clear_status(cpu_status::decimal);

    step_cycle(2);
}

// CLI (Clear Interrupt Disable):
// Clears the interrupt disable flag, allowing interrupts.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_cli()
{
    clear_status(cpu_status::interrupt);

    step_cycle(2);
}

// CLV (Clear Overflow Flag):
// Clears the overflow flag to 0, affecting subsequent arithmetic and branch instructions.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_clv()
{
    clear_status(cpu_status::overflow);

    step_cycle(2);
}

// CMP (Compare Accumulator):
// Compares the accumulator with a memory value, setting flags based on the subtraction result (carry, zero, and negative flags).
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_cmp()
{
    compare<AddrModeT>(_state.registers.a);
}

// CPX (Compare X Register):
// Compares the X register with a memory value, setting flags based on the subtraction result (carry, zero, and negative flags).
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_cpx()
{
    compare<AddrModeT>(_state.registers.x);
}

// CPY (Compare Y Register):
// Compares the Y register with a memory value, setting flags based on the subtraction result (carry, zero, and negative flags).
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_cpy()
{
    compare<AddrModeT>(_state.registers.y);
}

// DEC (Decrement Memory):
// Decrements the value at a specified memory location by one, setting the zero and negative flags based on the result.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_dec()
{
    const addr_t addr = decode_operand_addr<AddrModeT>();
    const byte_t value = read(addr);
    const byte_t new_value = value - 1;

    write(addr, new_value);

    set_status(cpu_status::zero, is_zero(new_value));
    set_status(cpu_status::negative, is_negative(new_value));

    switch (AddrModeT)
    {
    case cpu_addr_mode::zero_page:
        step_cycle(5);
        break;

    case cpu_addr_mode::zero_page_x:
    case cpu_addr_mode::absolute:
        step_cycle(6);
        break;

    case cpu_addr_mode::absolute_x:
        step_cycle(7);
        break;

    default:
        NESE_ASSERT(false);
    }
}

// DEX (Decrement X Register):
// Decreases the value in the X register by one, affecting the zero and negative flags.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_dex()
{
    --x();

    set_status(cpu_status::zero, is_zero(x()));
    set_status(cpu_status::negative, is_negative(x()));
    step_cycle(2);
}

// DEY (Decrement Y Register):
// Decreases the value in the Y register by one, affecting the zero and negative flags.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_dey()
{
    --y();

    set_status(cpu_status::zero, is_zero(y()));
    set_status(cpu_status::negative, is_negative(y()));
    step_cycle(2);
}

// EOR (Exclusive OR):
// Performs a bitwise exclusive OR between the accumulator and a memory value, affecting the zero and negative flags.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_eor()
{
    bool page_crossing{false};
    const addr_t addr = decode_operand<AddrModeT>(page_crossing);
    const byte_t byte = read_operand<AddrModeT>(addr);

    a() ^= byte;

    set_status(cpu_status::zero, is_zero(a()));
    set_status(cpu_status::negative, is_negative(a()));

    step_cycle(get_addr_mode_cycle_cost<AddrModeT>(page_crossing));
}

// INC (Increment Memory):
// Increments the value at a specified memory location by one, setting the zero and negative flags based on the result.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_inc()
{
    const addr_t addr = decode_operand_addr<AddrModeT>();
    const byte_t value = read(addr);
    const byte_t new_value = value + 1;

    write(addr, new_value);

    set_status(cpu_status::zero, is_zero(new_value));
    set_status(cpu_status::negative, is_negative(new_value));

    switch (AddrModeT)
    {
    case cpu_addr_mode::zero_page:
        step_cycle(5);
        break;

    case cpu_addr_mode::zero_page_x:
    case cpu_addr_mode::absolute:
        step_cycle(6);
        break;

    case cpu_addr_mode::absolute_x:
        step_cycle(7);
        break;

    default:
        NESE_ASSERT(false);
    }
}

// INX (Increment Register):
// Increases a register by one, affecting the zero and negative flags.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_inx()
{
    ++x();

    set_status(cpu_status::zero, is_zero(x()));
    set_status(cpu_status::negative, is_negative(x()));
    step_cycle(2);
}

// INX (Increment X Register):
// Increases the X register by one, affecting the zero and negative flags.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_iny()
{
    ++y();

    set_status(cpu_status::zero, is_zero(y()));
    set_status(cpu_status::negative, is_negative(y()));
    step_cycle(2);
}

// JMP (Jump):
// Sets the program counter to the address specified by the operand, effectively jumping to a new code location.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_jmp()
{
    addr_t new_addr;

    if constexpr (AddrModeT == cpu_addr_mode::indirect)
    {
        const addr_t addr = decodew();

        if ((addr & 0xff) == 0xff)
        {
            // Account for JMP hardware bug
            // http://wiki.nesdev.com/w/index.php/Errata
            new_addr = read(addr) + static_cast<addr_t>(static_cast<addr_t>(read(addr & 0xff00)) << 8);
        }
        else
        {
            new_addr = readw(addr);
        }
    }
    else
    {
        new_addr = decode_operand_addr<AddrModeT>();
    }

    if (new_addr == pc() - 1)
    {
        // TODO Infinite loop
    }

    pc() = new_addr;
    step_cycle(3);
}

// JSR (Jump to Subroutine):
// Pushes the address (minus one) of the next operation on to the stack and sets the program counter to the target address, for subroutine calls.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_jsr()
{
    // we push the actual return address -1, which is the current place (before decoding the 16-bit addr) + 1
    pushw(pc() + 1);

    pc() = decode_operand_addr<AddrModeT>();
    step_cycle(cpu_cycle_t(6));
}

// LDA (Load Accumulator):
// Loads a value into the accumulator from memory or an immediate value, affecting the zero and negative flags.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_lda()
{
    load<AddrModeT>(a());
}

// LDX (Load X Register):
// Loads a value into the X register from memory or an immediate value, affecting the zero and negative flags.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_ldx()
{
    load<AddrModeT>(x());
}

// LDY (Load Y Register):
// Loads a value into the Y register from memory or an immediate value, affecting the zero and negative flags.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_ldy()
{
    load<AddrModeT>(y());
}

// LSR (Logical Shift Right):
// Shifts all bits of the accumulator or a memory location one bit to the right, setting the carry flag with the first bit's value and affecting the zero and negative flags.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_lsr()
{
    bool page_crossing{false};
    const word_t operand = decode_operand<AddrModeT>(page_crossing);
    const byte_t value = read_operand<AddrModeT>(operand);
    const byte_t new_value = static_cast<byte_t>(value >> 1);

    write_operand<AddrModeT>(operand, new_value);

    set_status(cpu_status::carry, value & 0x1);
    set_status(cpu_status::zero, is_zero(new_value));
    set_status(cpu_status::negative, false); // never negative
    // set_status(cpu_status::negative, is_negative(new_value));

    step_cycle(get_addr_mode_cycle_cost<AddrModeT>(page_crossing));
}

// NOP (No Operation):
// Performs no operation and is used for timing adjustments and code alignment.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_nop()
{
    if constexpr (AddrModeT != cpu_addr_mode::implied)
    {
        [[maybe_unused]] const word_t operand = decode_operand<AddrModeT>();

        step_cycle(get_addr_mode_cycle_cost<AddrModeT>());
    }
    else
    {
        step_cycle(2);
    }
}

// ORA (Logical Inclusive OR):
// Performs a bitwise OR between the accumulator and a memory value, affecting the zero and negative flags.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_ora()
{
    bool page_crossing{false};
    const addr_t addr = decode_operand<AddrModeT>(page_crossing);
    const byte_t byte = read_operand<AddrModeT>(addr);

    a() |= byte;

    set_status(cpu_status::zero, is_zero(a()));
    set_status(cpu_status::negative, is_negative(a()));

    step_cycle(get_addr_mode_cycle_cost<AddrModeT>(page_crossing));
}

// PHA (Push Accumulator):
// Pushes a copy of the accumulator onto the stack.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_pha()
{
    push(a());

    step_cycle(3);
}

// PHP (Push Processor Status):
// Pushes a copy of the status flags onto the stack.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_php()
{
    // http://wiki.nesdev.com/w/index.php/cpu_status_behavior
    // Set bit 5 and 4 to 1 when copy status into from PHP
    push(status() | 0x30);

    step_cycle(3);
}

// PLA (Pull Accumulator):
// Pulls a byte from the stack into the accumulator, affecting the zero and negative flags.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_pla()
{
    a() = pop();

    set_status(cpu_status::zero, is_zero(a()));
    set_status(cpu_status::negative, is_negative(a()));

    step_cycle(4);
}

// PLP (Pull Processor Status):
// Pulls the processor status flags from the stack.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_plp()
{
    // http://wiki.nesdev.com/w/index.php/cpu_status_behavior
    // Bit 5 and 4 are ignored when pulled from stack - which means they are preserved
    // @TODO - Nintendulator actually always sets bit 5, not sure which one is correct
    status() = (pop() & 0xef) | (status() & 0x10) | 0x20;

    step_cycle(4);
}

// RTI (Return from Interrupt):
// Restores the CPU's state from the stack, including the program counter and processor flags, to conclude an interrupt service routine.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_rti()
{
    instruction_plp<AddrModeT>();

    pc() = popw();

    step_cycle(2);
}

// RTS (Return from Subroutine):
// Pulls the program counter (plus one) from the stack, returning from a subroutine.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_rts()
{
    pc() = popw() + 1;

    step_cycle(6);
}

// ROL (Rotate Left):
// Rotates all bits of the accumulator or a memory location one bit to the left, including the carry flag, affecting the carry, zero, and negative flags.template<addr_mode AddrModeT>
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_rol()
{
    bool page_crossing{false};
    const word_t operand = decode_operand<AddrModeT>(page_crossing);
    const byte_t value = read_operand<AddrModeT>(operand);
    const byte_t carry_mask = is_status_set(cpu_status::carry) ? 0x01 : 0x00;
    const byte_t new_value = value << 1 | carry_mask;

    write_operand<AddrModeT>(operand, new_value);

    set_status(cpu_status::carry, value & 0x80);
    set_status(cpu_status::zero, is_zero(new_value));
    set_status(cpu_status::negative, is_negative(new_value));

    step_cycle(get_addr_mode_cycle_cost<AddrModeT>(page_crossing));
}

// ROR (Rotate Right):
// Rotates all bits of the accumulator or a memory location one bit to the right, including the carry flag, affecting the carry, zero, and negative flags.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_ror()
{
    bool page_crossing{false};
    const word_t operand = decode_operand<AddrModeT>();
    const byte_t value = read_operand<AddrModeT>(operand);
    const byte_t carry_mask = is_status_set(cpu_status::carry) ? 0x80 : 0x00;
    const byte_t new_value = value >> 1 | carry_mask;

    write_operand<AddrModeT>(operand, new_value);

    set_status(cpu_status::carry, value & 0x1);
    set_status(cpu_status::zero, is_zero(new_value));
    set_status(cpu_status::negative, is_negative(new_value));

    step_cycle(get_addr_mode_cycle_cost<AddrModeT>(page_crossing));
}

// SBC (Subtract with Carry):
// Subtracts a memory value and the carry flag from the accumulator, affecting flags for carry, zero, overflow, and negative.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_sbc()
{
    bool page_crossing{false};
    const addr_t addr = decode_operand<AddrModeT>(page_crossing);
    const byte_t byte = read_operand<AddrModeT>(addr);

    add_with_carry<AddrModeT>(~byte);

    step_cycle(get_addr_mode_cycle_cost<AddrModeT>(page_crossing));
}

// SEC (Set Carry Flag):
// Sets the carry flag to 1.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_sec()
{
    set_status(cpu_status::carry);

    step_cycle(2);
}

// SED (Set Decimal Mode):
// Sets the decimal mode flag, affecting how ADC and SBC instructions work.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_sed()
{
    set_status(cpu_status::decimal);

    step_cycle(2);
}

// SEI (Set Interrupt Disable):
// Sets the interrupt disable flag, preventing interrupts.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_sei()
{
    set_status(cpu_status::interrupt);

    step_cycle(2);
}

// STA (Store Accumulator):
// Stores the value in the accumulator into a specific location in memory.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_sta()
{
    store<AddrModeT>(a());

    step_cycle(get_addr_mode_cycle_cost<AddrModeT>(true));
}

// STX (Store X Register):
// Stores the value in the X register into a specified memory location.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_stx()
{
    store<AddrModeT>(x());

    step_cycle(get_addr_mode_cycle_cost<AddrModeT>());
}

// STY (Store Y Register):
// Stores the value in the Y register into a specified memory location.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_sty()
{
    store<AddrModeT>(y());

    step_cycle(get_addr_mode_cycle_cost<AddrModeT>());
}

// TAX (Transfer Accumulator to X):
// Transfers the value in the accumulator to the X register, affecting the zero and negative flags.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_tax()
{
    x() = a();

    set_status(cpu_status::zero, is_zero(x()));
    set_status(cpu_status::negative, is_negative(x()));

    step_cycle(2);
}

// TAY (Transfer Accumulator to Y):
// Transfers the value in the accumulator to the Y register, affecting the zero and negative flags.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_tay()
{
    y() = a();

    set_status(cpu_status::zero, is_zero(y()));
    set_status(cpu_status::negative, is_negative(y()));

    step_cycle(2);
}

// TSX (Transfer Stack Pointer to X):
// Transfers the current stack pointer value to the X register, affecting the zero and negative flags.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_tsx()
{
    x() = sp();

    set_status(cpu_status::zero, is_zero(x()));
    set_status(cpu_status::negative, is_negative(x()));

    step_cycle(2);
}

// TXA (Transfer X to Accumulator):
// Transfers the value in the X register to the accumulator, affecting the zero and negative flags.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_txa()
{
    a() = x();

    set_status(cpu_status::zero, is_zero(a()));
    set_status(cpu_status::negative, is_negative(a()));

    step_cycle(2);
}

// TXS (Transfer X to Stack Pointer):
// Transfers the value in the X register to the stack pointer. Note that this instruction does not affect any flags.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_txs()
{
    sp() = x();

    step_cycle(2);
}

// TYA (Transfer Y to Accumulator):
// Transfers the value in the Y register to the accumulator, affecting the zero and negative flags.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_tya()
{
    a() = y();

    set_status(cpu_status::zero, is_zero(a()));
    set_status(cpu_status::negative, is_negative(a()));

    step_cycle(2);
}

#if NESE_UNOFFICIAL_INSTRUCTIONS_ENABLED
// DCP (Decrement Memory then Compare with Accumulator):
// Decrements a memory location and then compares the result with the accumulator, setting the zero, carry, and negative flags based on the subtraction result.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_dcp()
{
    bool page_crossing{false};
    const addr_t addr = decode_operand<AddrModeT>(page_crossing);
    const byte_t value = read_operand<AddrModeT>(addr);
    const byte_t new_value = value - 1;

    write_operand<AddrModeT>(addr, new_value);

    const byte_t diff = a() - new_value;

    set_status(cpu_status::carry, a() >= new_value);
    set_status(cpu_status::zero, is_zero(diff));
    set_status(cpu_status::negative, is_negative(diff));

    step_cycle(get_addr_mode_cycle_cost<AddrModeT>(page_crossing) + cpu_cycle_t(2));
}

// ISB (Increment Memory then Subtract with Borrow):
// Increments the value at a memory location, then subtracts it from the accumulator with borrow, affecting the carry, zero, negative, and overflow flags.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_isb()
{
    bool page_crossing{false};
    const addr_t addr = decode_operand<AddrModeT>(page_crossing);
    const byte_t value = read_operand<AddrModeT>(addr);
    const byte_t new_value = value + 1;

    write_operand<AddrModeT>(addr, new_value);

    add_with_carry<AddrModeT>(~new_value);

    step_cycle(get_addr_mode_cycle_cost<AddrModeT>(page_crossing) + cpu_cycle_t(2));
}

// LAX (Load Accumulator and X):
// Loads both the accumulator and the X register with the same memory content, updating the zero and negative flags based on the value loaded.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_lax()
{
    bool page_crossing{false};
    const addr_t addr = decode_operand<AddrModeT>(page_crossing);
    const byte_t value = read_operand<AddrModeT>(addr);

    a() = value;
    x() = value;

    set_status(cpu_status::zero, is_zero(value));
    set_status(cpu_status::negative, is_negative(value));

    step_cycle(get_addr_mode_cycle_cost<AddrModeT>(page_crossing));
}

// RLA (Rotate Left then AND):
// Rotates a memory location or the accumulator left, then ANDs the result with the accumulator, affecting the carry, zero, and negative flags.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_rla()
{
    bool page_crossing{false};
    const word_t operand = decode_operand<AddrModeT>(page_crossing);
    const byte_t value = read_operand<AddrModeT>(operand);
    const byte_t carry_mask = is_status_set(cpu_status::carry) ? 0x01 : 0x00;
    const byte_t new_value = value << 1 | carry_mask;

    write_operand<AddrModeT>(operand, new_value);

    a() &= new_value;

    set_status(cpu_status::carry, (value & 0x80) != 0);
    set_status(cpu_status::zero, is_zero(a()));
    set_status(cpu_status::negative, is_negative(a()));

    step_cycle(get_addr_mode_cycle_cost<AddrModeT>(page_crossing) + cpu_cycle_t(2));
}

// RRA (Rotate Right then Add):
// Rotates a memory location or the accumulator right, then adds the result to the accumulator with carry, affecting the carry, zero, and negative flags.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_rra()
{
    bool page_crossing{false};
    const word_t operand = decode_operand<AddrModeT>(page_crossing);
    const byte_t value = read_operand<AddrModeT>(operand);
    const byte_t carry_mask = is_status_set(cpu_status::carry) ? 0x80 : 0x00;
    const byte_t new_value = value >> 1 | carry_mask;

    write_operand<AddrModeT>(operand, new_value);

    set_status(cpu_status::carry, value & 0x1);

    add_with_carry<AddrModeT>(new_value);

    step_cycle(get_addr_mode_cycle_cost<AddrModeT>(page_crossing) + cpu_cycle_t(2));
}

// SAX (Store Accumulator and X):
// Stores the bitwise AND of the accumulator and the X register to memory, without affecting any flags.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_sax()
{
    bool page_crossing{false};
    const addr_t addr = decode_operand<AddrModeT>(page_crossing);

    write_operand<AddrModeT>(addr, static_cast<byte_t>(a() & x()));

    step_cycle(get_addr_mode_cycle_cost<AddrModeT>(page_crossing));
}

// SLO (Shift Left then Logical OR):
// Shifts the value in memory one bit to the left (ASL) and then performs an OR operation with the accumulator, affecting the zero, negative, and carry flags.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_slo()
{
    bool page_crossing{false};
    const addr_t addr = decode_operand<AddrModeT>(page_crossing);
    const byte_t value = read_operand<AddrModeT>(addr);
    const byte_t new_value = static_cast<byte_t>(value << 1);

    write_operand<AddrModeT>(addr, new_value);

    a() |= new_value;

    set_status(cpu_status::carry, is_negative(value));
    set_status(cpu_status::zero, is_zero(a()));
    set_status(cpu_status::negative, is_negative(a()));

    step_cycle(get_addr_mode_cycle_cost<AddrModeT>(page_crossing) + cpu_cycle_t(2));
}

// SRE (Shift Right then Exclusive OR):
// Shifts a memory location or the accumulator right, then XORs the result with the accumulator, affecting the carry, zero, and negative flags.
template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::instruction_sre()
{
    bool page_crossing{false};
    const addr_t addr = decode_operand<AddrModeT>(page_crossing);
    const byte_t value = read_operand<AddrModeT>(addr);
    const byte_t new_value = static_cast<byte_t>(value >> 1);

    write_operand<AddrModeT>(addr, new_value);

    a() ^= new_value;

    set_status(cpu_status::carry, value & 0x1);
    set_status(cpu_status::zero, is_zero(a()));
    set_status(cpu_status::negative, is_negative(a()));

    step_cycle(get_addr_mode_cycle_cost<AddrModeT>(page_crossing) + cpu_cycle_t(2));
}
#endif

template<typename BusT>
byte_t& cpu<BusT>::a()
{
    return _state.registers.a;
}

template<typename BusT>
byte_t& cpu<BusT>::x()
{
    return _state.registers.x;
}

template<typename BusT>
byte_t& cpu<BusT>::y()
{
    return _state.registers.y;
}

template<typename BusT>
byte_t& cpu<BusT>::sp()
{
    return _state.registers.sp;
}

template<typename BusT>
word_t& cpu<BusT>::pc()
{
    return _state.registers.pc;
}

template<typename BusT>
byte_t& cpu<BusT>::status()
{
    return _state.registers.status;
}

template<typename BusT>
void cpu<BusT>::step_cycle(u64_t cycle)
{
    step_cycle(cpu_cycle_t(cycle));
}

template<typename BusT>
void cpu<BusT>::step_cycle(cpu_cycle_t cycle)
{
    _state.cycle += cycle;
}

template<typename BusT>
bool cpu<BusT>::is_status_set(cpu_status status) const
{
    return _state.registers.is_status_set(status);
}

template<typename BusT>
bool cpu<BusT>::is_status_clear(cpu_status status) const
{
    return _state.registers.is_status_clear(status);
}

template<typename BusT>
void cpu<BusT>::set_status(cpu_status status, bool value)
{
    _state.registers.set_status(status, value);
}

template<typename BusT>
void cpu<BusT>::set_status(cpu_status status)
{
    _state.registers.set_status(status);
}

template<typename BusT>
void cpu<BusT>::clear_status(cpu_status status)
{
    _state.registers.clear_status(status);
}

template<typename BusT>
byte_t cpu<BusT>::read(addr_t addr)
{
    return _bus.get().read(addr);
}

template<typename BusT>
word_t cpu<BusT>::readw(addr_t addr)
{
    return _bus.get().read_word(addr);
}

template<typename BusT>
void cpu<BusT>::write(addr_t addr, byte_t value)
{
    _bus.get().write(addr, value);
}

template<typename BusT>
void cpu<BusT>::writew(addr_t addr, word_t value)
{
    _bus.get().write_word(addr, value);
}

template<typename BusT>
byte_t cpu<BusT>::pop()
{
    ++sp();

    return read(sp() + cpu_stack_offset);
}

template<typename BusT>
word_t cpu<BusT>::popw()
{
    const byte_t lo = pop();
    const byte_t hi = pop();

    return static_cast<word_t>(hi << 8) + lo;
}

template<typename BusT>
void cpu<BusT>::push(byte_t value)
{
    write(sp() + cpu_stack_offset, value);

    --sp();
}

template<typename BusT>
void cpu<BusT>::pushw(word_t value)
{
    push(value >> 8);
    push(value & 0xff);
}

template<typename BusT>
byte_t cpu<BusT>::decode()
{
    return read(pc()++);
}

template<typename BusT>
word_t cpu<BusT>::decodew()
{
    const word_t decoded = readw(pc());

    pc() += 2;

    return decoded;
}

template<typename BusT>
template<cpu_addr_mode AddrModeT>
word_t cpu<BusT>::decode_operand()
{
    bool page_crossing{false};
    return decode_operand<AddrModeT>(page_crossing);
}

template<typename BusT>
template<cpu_addr_mode AddrModeT>
word_t cpu<BusT>::decode_operand(bool& page_crossing)
{
    if constexpr (AddrModeT == cpu_addr_mode::accumulator)
    {
        return 0;
    }

    if constexpr (AddrModeT == cpu_addr_mode::immediate)
    {
        return decode();
    }

    return decode_operand_addr<AddrModeT>(page_crossing);
}

template<typename BusT>
template<cpu_addr_mode AddrModeT>
addr_t cpu<BusT>::decode_operand_addr()
{
    bool page_crossing{false};
    return decode_operand_addr<AddrModeT>(page_crossing);
}

template<typename BusT>
template<cpu_addr_mode AddrModeT>
addr_t cpu<BusT>::decode_operand_addr(bool& page_crossing [[maybe_unused]])
{
    if constexpr (AddrModeT == cpu_addr_mode::zero_page)
    {
        return decode();
    }

    if constexpr (AddrModeT == cpu_addr_mode::zero_page_x)
    {
        return decode() + x() & 0xFF;
    }

    if constexpr (AddrModeT == cpu_addr_mode::zero_page_y)
    {
        return decode() + y() & 0xFF;
    }

    if constexpr (AddrModeT == cpu_addr_mode::absolute)
    {
        return decodew();
    }

    if constexpr (AddrModeT == cpu_addr_mode::absolute_x)
    {
        return decodew() + x();
    }

    if constexpr (AddrModeT == cpu_addr_mode::absolute_y)
    {
        return decodew() + y();
    }

    if constexpr (AddrModeT == cpu_addr_mode::indexed_indirect)
    {
        const byte_t addr = decode();

        const byte_t lo = read((addr + x()) & 0xff);
        const byte_t hi = read((addr + x() + 1) & 0xff);

        return static_cast<addr_t>(lo) + static_cast<addr_t>(static_cast<addr_t>(hi) << 8);
    }

    if constexpr (AddrModeT == cpu_addr_mode::indirect_indexed)
    {
        const byte_t addr = decode();

        const byte_t lo = read(addr);
        const byte_t hi = read((addr + 1) & 0xFF);

        const addr_t new_addr = static_cast<addr_t>(lo) + static_cast<addr_t>(static_cast<addr_t>(hi) << 8) + y();

        page_crossing = is_page_crossing(addr, new_addr);

        return new_addr;
    }

    NESE_ASSUME(false);
}

template<typename BusT>
template<cpu_addr_mode AddrModeT>
byte_t cpu<BusT>::read_operand(word_t operand)
{
    if constexpr (AddrModeT == cpu_addr_mode::accumulator)
    {
        return a();
    }

    if constexpr (AddrModeT == cpu_addr_mode::immediate)
    {
        return static_cast<byte_t>(operand);
    }

    return read(operand);
}

template<typename BusT>
template<cpu_addr_mode AddrModeT>
void cpu<BusT>::write_operand(word_t operand, byte_t value)
{
    if constexpr (AddrModeT == cpu_addr_mode::accumulator)
    {
        a() = value;
    }
    else
    {
        write(operand, value);
    }
}

template<typename BusT>
template<cpu_addr_mode AddrModeT>
constexpr cpu_cycle_t cpu<BusT>::get_addr_mode_cycle_cost(bool page_crossing)
{
    switch (AddrModeT)
    {
    case cpu_addr_mode::accumulator:
    case cpu_addr_mode::immediate:
        return cpu_cycle_t(2);

    case cpu_addr_mode::zero_page:
        return cpu_cycle_t(3);

    case cpu_addr_mode::zero_page_x:
    case cpu_addr_mode::zero_page_y:
    case cpu_addr_mode::absolute:
        return cpu_cycle_t(4);

    case cpu_addr_mode::absolute_x:
    case cpu_addr_mode::absolute_y:
        return cpu_cycle_t(page_crossing ? 5 : 4);

    case cpu_addr_mode::indexed_indirect:
        return cpu_cycle_t(6);

    case cpu_addr_mode::indirect_indexed:
        return cpu_cycle_t(page_crossing ? 6 : 5);

    default:
        NESE_ASSERT(false);
        return cpu_cycle_t(0);
    }
}

} // namespace nese::v2