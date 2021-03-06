/*
 * AlignedWriter.h
 *
 *  Created on: Mar 18, 2015
 *      Author: Collin
 */

#ifndef BUFFER_UTILS_ALIGNED_WRITER_H_INCLUDED
#define BUFFER_UTILS_ALIGNED_WRITER_H_INCLUDED

#include <functional>
#include <cstddef>
#include <cstring>

#include <glm/glm.hpp>

#include <boost/fusion/include/for_each.hpp>

template <typename UnsignedType>
constexpr UnsignedType roundToMultiple(UnsignedType num, UnsignedType factor) noexcept
{
	return factor * ((num + factor - 1) / factor);
}

struct AlignedWriterLayoutSTD140
{
	template <typename T>
	struct Type
	{
		static constexpr std::size_t getAlignment() noexcept {return sizeof(T);}
		static constexpr std::size_t getSize() noexcept {return sizeof(T);}
	};

	template <typename T, int i>
	struct Type<T[i]>
	{
		static constexpr std::size_t getAlignment() noexcept {return roundToMultiple(Type<T>::getSize(), Type<glm::vec4>::getSize());}
		static constexpr std::size_t getSize() noexcept {return i * getAlignment();}
	};

	template <typename T, glm::precision P>
	struct Type<glm::tvec2<T, P>>
	{
		static constexpr std::size_t getAlignment() noexcept {return 2 * Type<T>::getAlignment();}
		static constexpr std::size_t getSize() noexcept {return 2 * Type<T>::getSize();}
	};

	template <typename T, glm::precision P>
	struct Type<glm::tvec3<T, P>>
	{
		static constexpr std::size_t getAlignment() noexcept {return 4 * Type<T>::getAlignment();}
		static constexpr std::size_t getSize() noexcept {return 4 * Type<T>::getSize();}
	};

	template <typename T, glm::precision P>
	struct Type<glm::tvec4<T, P>>
	{
		static constexpr std::size_t getAlignment() noexcept {return 4 * Type<T>::getAlignment();}
		static constexpr std::size_t getSize() noexcept {return 4 * Type<T>::getSize();}
	};

	template <typename T, glm::precision P>
	struct Type<glm::tmat4x4<T, P>> : public Type<typename glm::tmat4x4<T, P>::col_type[4]> {};

	template <typename T, glm::precision P>
	struct Type<glm::tmat3x3<T, P>> : public Type<typename glm::tmat3x3<T, P>::col_type[3]> {};

	template <typename T>
	static constexpr std::size_t getAlignment() noexcept
	{
		return Type<T>::getAlignment();
	}
	template <typename T>
	static constexpr std::size_t getSize() noexcept
	{
		return Type<T>::getSize();
	}
	template <typename T>
	static constexpr std::size_t getArrayElementAlignment() noexcept
	{
		return Type<T>::getAlignment();
	}
	template <typename T>
	static constexpr std::size_t getArrayElementSize() noexcept
	{
		return roundToMultiple(Type<T>::getSize(), Type<glm::vec4>::getSize());
	}
};

class AlignedWriter
{
protected:
	void *bufferBase;
	void *bufferHead;

	void alignBuffer(std::uintptr_t alignment) noexcept
	{
		auto bufferInt = reinterpret_cast<std::uintptr_t>(bufferHead);
		bufferInt = roundToMultiple(bufferInt, alignment);
		bufferHead = reinterpret_cast<void*>(bufferInt);
	}
	void incrementBuffer(std::uintptr_t distance) noexcept
	{
		auto bufferInt = reinterpret_cast<std::uintptr_t>(bufferHead);
		bufferInt += distance;
		bufferHead = reinterpret_cast<void*>(bufferInt);
	}

	template <typename T>
	static const void* getTypePtr(const T& t) noexcept
	{
		return &t;
	}

	template <typename Layout, typename T>
	void writeUnit(const T& unit) noexcept
	{
		alignBuffer(Layout::template getAlignment<T>());
		memcpy(bufferHead, getTypePtr(unit), sizeof(unit));
		incrementBuffer(Layout::template getSize<T>());
	}

	template <typename Layout, typename T, int i>
	void writeUnit(const T (&unit)[i]) noexcept
	{
		for(std::size_t j = 0; j < i; ++j)
		{
			alignBuffer(Layout::template getArrayElementAlignment<T>());
			memcpy(bufferHead, getTypePtr(unit[j]), sizeof(unit[j]));
			incrementBuffer(Layout::template getArrayElementSize<T>());
		}
	}

	template <typename Layout, typename T, glm::precision P>
	void writeUnit(const glm::tmat4x4<T, P>& unit) noexcept
	{
		writeUnit<Layout>(*static_cast<const glm::tvec4<T, P>(*)[4]>(static_cast<const void*>(&unit[0])));	// Ugly type cast hacks
	}

	template <typename Layout>
	struct FusionCallable
	{
		AlignedWriter& w;

		FusionCallable(AlignedWriter& w_in) noexcept: w(w_in) {}

		template <typename T>
		void operator()(const T& t) const noexcept
		{
			w.writeUnit<Layout>(t);
		}
	};

public:
	AlignedWriter(void *buffer_in) noexcept: bufferBase(buffer_in), bufferHead(buffer_in) {}

	void reset() noexcept {bufferHead = bufferBase;}
	void setOffset(std::uintptr_t off) noexcept {reset(); incrementBuffer(off);}
	std::uintptr_t getOffset() noexcept {return (std::uintptr_t) bufferHead - (std::uintptr_t) bufferBase;}

	template <typename Layout, typename RandomAccessSequence>
	void write(const RandomAccessSequence& seq) noexcept
	{
		boost::fusion::for_each(seq, FusionCallable<Layout>(*this));
	}
};

#endif /* BUFFER_UTILS_ALIGNED_WRITER_H_INCLUDED */
