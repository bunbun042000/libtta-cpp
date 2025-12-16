/*
The ttaplugins-winamp project.
Copyright (C) 2005-2026 Yamagata Fumihiro

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <cstdlib>
#include <memory>

#include "libtta.h"
#include "tta_encoder_extend.h"

#define MUL_FRAME_TIME(x) (256 * (x) / 245) // = x * FRAME_TIME

void writer_start(TTA_fifo* s);
TTAuint32 write_tta_header(TTA_fifo* s, TTA_info* info);
__forceinline void flush_bit_cache(TTA_fifo* s);
void writer_done(TTA_fifo* s);
void writer_skip_bytes(TTA_fifo* s, TTAuint32 size);

void tta::tta_encoder_extend::init_set_info_for_memory(TTA_info* info, TTAuint64 pos) {
	bool isAllocateSeektable = false;

	// check for supported formats
	if (info->format > 2 ||
		info->bps < MIN_BPS ||
		info->bps > MAX_BPS ||
		info->nch > MAX_NCH)
		throw tta::tta_exception::tta_exception(TTA_FORMAT_ERROR);

	// set start position if required
	if (pos && fifo.io->seek(fifo.io, (TTAint64)pos) < 0)
		throw tta::tta_exception::tta_exception(TTA_SEEK_ERROR);

	writer_start(&fifo);
	pos += write_tta_header(&fifo, info);

	offset = pos; // size of headers
	format = info->format;
	depth = (info->bps + 7) / 8;
	flen_std = MUL_FRAME_TIME(info->sps);
	flen_last = info->samples % flen_std;
	frames = info->samples / flen_std + (flen_last ? 1 : 0);
	if (!flen_last) flen_last = flen_std;
	rate = 0;

	// allocate memory for seek table data
	if (nullptr == seek_table)
	{
		seek_table = (TTAuint64*)_aligned_malloc(frames * sizeof(TTAuint64), 16);
		isAllocateSeektable = true;
	}
	else
	{
		// Do nothing
	}

	if (nullptr == seek_table)
	{
		throw tta::tta_exception::tta_exception(TTA_MEMORY_ERROR);
	}
	else if (isAllocateSeektable)
	{
		memset(seek_table, 0, frames * sizeof(TTAuint64));
	}
	else
	{
		// Do nothing
	}

	if (info->samples != MAX_SAMPLES)
	{
		writer_skip_bytes(&fifo, (frames + 1) * sizeof(TTAuint64));
		m_header_and_seektable_offset = offset + (frames + 1) * sizeof(TTAuint64);
	}
	else
	{
		m_header_and_seektable_offset = offset;
	}


	encoder_last = encoder + info->nch - 1;
	shift_bits = (4 - depth) << 3;

	frame_init(0);
}

void tta::tta_encoder_extend::preliminaryFinish(void)
{
	flush_bit_cache(&fifo);
	seek_table[fnum++] = fifo.count;
	writer_done(&fifo);
}

void tta::tta_encoder_extend::flushFifo(void)
{
	writer_done(&fifo);
}
