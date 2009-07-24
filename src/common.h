/**
 * Common
 */

#pragma once

// Common defines

// #define FORMAT_1US 1

// Types

typedef unsigned long long	u64_t;
typedef unsigned int		u32_t;
typedef unsigned short		u16_t;
typedef unsigned char		u8_t;
typedef signed long long	s64_t;
typedef signed int			s32_t;
typedef signed short		s16_t;
typedef signed char			s8_t;

typedef union {
	u8_t	value;
	/*
	struct {
		u8_t	d0;
		u8_t	d1;
		u8_t	d2;
		u8_t	d3;
	} comp;
	*/
} data_t;

// Chunk defines

#define CHUNK_WIDTH			256
#define CHUNK_HEIGHT		50
#define CHUNK_LENGTH		(CHUNK_WIDTH * CHUNK_HEIGHT)
#define CHUNK_SIZE			(CHUNK_WIDTH * CHUNK_HEIGHT * sizeof(data_t))
#define CHUNK_BORDER		4

// Defines

#define MAX_ID					255
#define CHUNK_STATE_FREE		253
#define CHUNK_STATE_FINISHED	254

// Structs

typedef enum {
	E_CHUNK_FREE		= 0x00,
	E_CHUNK_RECEIVING	= 0x01,
	E_CHUNK_WAIT		= 0x02,
	E_CHUNK_WORKING		= 0x03,
	E_CHUNK_READY		= 0x04,
	E_CHUNK_SENDING		= 0x05,
	
	E_CHUNK_LENGTH		= 0x06
} chunk_state_t;

typedef struct {
	data_t			data[CHUNK_LENGTH];
	u32_t			id;
	u32_t			x, y;
	u8_t			seq_id;
	u8_t			seq_index;
	u8_t			prog_id;		// cached value - lookup from (seq_id, seq_index)
	u8_t			param_id;		// cached value - lookup from (seq_id, seq_index)
	chunk_state_t	state				:8;
} chunk_t;

typedef struct {
	u32_t	buffer_free;
	u32_t	buffer_wait;
	u32_t	buffer_busy;
	u32_t	buffer_ready;
} status_t;

