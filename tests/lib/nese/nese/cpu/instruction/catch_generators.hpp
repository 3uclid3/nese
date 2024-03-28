#pragma once

#include <catch2/generators/catch_generators.hpp>

#include <nese/basic_types.hpp>

// clang-format off
#define GENERATE_ADDR() \
           static_cast<nese::addr_t>( \
            GENERATE(0x0000, 0xFFFF, \
                     0x00FE, 0x01FE, \
                     0x7FFE, 0xFFFE, \
                     0x00FF, 0x0100))

#define GENERATE_NEGATIVE_BYTE() \
           static_cast<nese::byte_t>( \
            GENERATE(0x80, \
                     0x81, \
                     0xC0, \
                     0xFE, \
                     0xFF))

#define GENERATE_POSITIVE_BYTE() \
           static_cast<nese::byte_t>( \
            GENERATE(0x01, \
                     0x10, \
                     0x40, \
                     0x7E, \
                     0x7F))

#define GENERATE_ADDR_FOR_ZERO_PAGE() \
     GENERATE(table<addr_t, byte_t>({ \
                /* Simple zero-page tests */ \
                {0x0200, 0x00}, /* PC in a common code area, val_addr at zero page start */ \
                {0x0200, 0xFF}, /* PC in a common code area, val_addr at zero page end */ \
                /* Boundary conditions */ \
                {0x01FF, 0x00}, /* PC just before zero page, val_addr at zero page start */ \
                {0x0200, 0x01}, /* PC in a common code area, val_addr just into zero page */ \
                {0x01FF, 0xFF}, /* PC just before zero page, val_addr at zero page end */ \
                {0xF000, 0x80}, /* Higher PC value, val_addr in middle of zero page */ \
                /* Testing PC at various points */ \
                {0x0000, 0x02}, /* PC at start of memory, testing very early execution */ \
                {0x8000, 0x04}, /* PC in a typical ROM area, val_addr early in zero page */ \
                {0xFFFC, 0xFE}, /* PC at the very end of memory space */ \
                /* Varied val_addr values to test LDA, LDX, LDY behavior */ \
                {0x0300, 0x10}, /* Common code area, testing nonzero page value */ \
                {0x0400, 0x20}  /* Another common code area, testing nonzero page value */ \
            }))


#define GENERATE_ADDR_AND_OFFSET_FOR_ZERO_PAGE() \
     GENERATE(table<addr_t, byte_t, byte_t>({ \
                /* Offset within zero-page without wrap */ \
                {0x0200, 0x00, 0x01}, \
                {0x0200, 0x10, 0x0F}, \
                /* Offset causing wrap-around */ \
                {0x0200, 0xFE, 0x01}, \
                {0x0200, 0xFD, 0x03}, \
                /* Varied PC values with offset */ \
                {0x0000, 0x02, 0x01}, \
                {0x8000, 0x03, 0x02}, \
                {0xFFFC, 0xFD, 0x02} \
            }))

#define GENERATE_ADDR_FOR_ABSOLUTE() \
     GENERATE(table<addr_t, addr_t>({ \
                /* Absolute addressing tests in different memory regions */ \
                {0x0200, 0x0100}, /* PC in common code area, absolute address in lower memory */ \
                {0x0200, 0x8000}, /* PC in common code area, absolute address in upper memory */ \
                /* Boundary conditions */ \
                {0x01FF, 0x0000}, /* PC just before zero page, testing absolute address at memory start */ \
                {0x0200, 0xFFFF}, /* PC in common code area, testing absolute address at memory end */ \
                {0xF000, 0x4000}, /* Higher PC value, absolute address in the middle of memory */ \
                /* Testing PC at various points */ \
                {0x0000, 0x0200}, /* PC at start of memory, absolute address in common code area */ \
                {0x8000, 0x0300}, /* PC in a typical ROM area, absolute address in common code area */ \
                {0xFFFC, 0x0400}  /* PC at the very end of memory space, absolute address in common code area */ \
            }))

#define GENERATE_ADDR_AND_OFFSET_FOR_ABSOLUTE() \
     GENERATE(table<addr_t, addr_t, byte_t>({ \
                /* Conceptual tests with "offsets" for absolute addressing */ \
                {0x0200, 0x0100, 0x01}, /* Common code area, lower memory with a byte offset */ \
                {0x0200, 0x8000, 0x10}, /* Common code area, upper memory with a byte offset */ \
                /* Varied PC values with "offsets" */ \
                {0x0000, 0x0200, 0x20}, /* PC at start, absolute address in common code area with a byte offset */ \
                {0x8000, 0x0300, 0x30}, /* PC in ROM area, absolute address in common code area with a byte offset */ \
                {0xFFFC, 0x0400, 0x40}  /* PC at end, absolute address in common code area with a byte offset */ \
            }))

// clang-format on