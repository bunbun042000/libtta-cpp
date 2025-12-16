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

#ifndef TTA_ENCODER_EXTEND_H_INCLUDED
#define TTA_ENCODER_EXTEND_H_INCLUDED

#include "libtta.h"

static const TTAuint64 MAX_SAMPLES = 4294967295;

namespace tta
{
	class TTA_EXTERN_API tta_encoder_extend : public tta::tta_encoder
	{
	public:
		using tta::tta_encoder::tta_encoder;

		TTAuint64 getHeaderOffset(void) const { return offset; };
		TTAuint64 getHeaderAndSeekTableOffset(void) const { return m_header_and_seektable_offset; };
		void init_set_info_for_memory(TTA_info* info, TTAuint64 pos);
		void preliminaryFinish(void);
		void flushFifo(void);

	protected:
		TTAuint64 m_header_and_seektable_offset = 0;	// data start position (header and seektable size, bytes)
	};

}

#endif // #ifndef TTA_ENCODER_EXTEND_H_INCLUDED
