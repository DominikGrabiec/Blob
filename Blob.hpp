// MIT License

// Copyright (c) 2013-2023 Dominik Grabiec

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "Assert.hpp"

#include <memory>
#include <vector>
#include <span>
#include <array>


class BlobSpan;

//------------------------------------------------------------------------------

//
// Holds a read only view of a blob of memory
//
class BlobView
{
public:
	constexpr BlobView() noexcept;
	[[nodiscard]] constexpr BlobView(const void* data, size_t bytes) noexcept;

	template <typename T, typename A>
	[[nodiscard]] constexpr BlobView(const std::vector<T, A>& vector) noexcept;

	template <typename T, size_t N>
	[[nodiscard]] constexpr BlobView(const std::array<T, N>& array) noexcept;

	template <typename T, size_t N>
	[[nodiscard]] constexpr BlobView(const T (&array)[N]) noexcept;

	constexpr BlobView(const BlobView& other) noexcept = default;
	constexpr BlobView(const BlobSpan& other) noexcept;

	~BlobView() = default;

	BlobView& operator=(const BlobView& other) noexcept = default;

	friend void swap(BlobView& first, BlobView& second) noexcept;

	[[nodiscard]] constexpr bool Empty() const noexcept;
	[[nodiscard]] constexpr size_t Size() const noexcept;

	// Returns a const pointer to the data
	[[nodiscard]] constexpr const void* Data() const noexcept;

	// Returns a const pointer to the data at the offset
	[[nodiscard]] inline const void* Data(const size_t offset) const;

	// Returns a const pointer to offset as type T
	template <typename T>
	[[nodiscard]] inline const T* Pointer(const size_t offset = 0) const;

	// Returns a constant reference to an object at the specified offset
	template <typename T>
	[[nodiscard]] inline const T& As(const size_t offset = 0) const;

	// Returns a sub view of this view starting at an offset to the end of the buffer
	[[nodiscard]] inline BlobView SubView(size_t offset = 0) const;

	// Returns a sub view of this view starting at an offset and being of a limited size
	[[nodiscard]] inline BlobView SubView(size_t offset, size_t bytes) const;

	// Returns a slice of this view starting at offset begin (inclusive) and ending at offset end (exclusive)
	[[nodiscard]] inline BlobView Slice(size_t begin, size_t end) const noexcept;

	// Returns a std::span of type const T from a specified offset with count elements
	template <typename T>
	[[nodiscard]] inline std::span<const T> ArrayView(size_t offset, size_t count) const noexcept;

private:
	const void* pointer_;
	size_t size_; // bytes
};

//------------------------------------------------------------------------------

//
// Holds a read-write span of a blob of memory
//
class BlobSpan
{
public:
	constexpr BlobSpan() noexcept;
	[[nodiscard]] constexpr BlobSpan(void* data, size_t bytes) noexcept;

	template <typename T, typename A>
	[[nodiscard]] constexpr BlobSpan(std::vector<T, A>& vector) noexcept;

	template <typename T, size_t N>
	[[nodiscard]] constexpr BlobSpan(std::array<T, N>& array) noexcept;

	template <typename T, size_t N>
	[[nodiscard]] constexpr BlobSpan(T (&array)[N]) noexcept;

	BlobSpan(const BlobSpan& other) noexcept = default;
	BlobSpan(const BlobView& other) noexcept = delete;

	~BlobSpan() = default;

	BlobSpan& operator=(const BlobSpan& other) noexcept = default;
	BlobSpan& operator=(const BlobView& other) noexcept = delete;

	friend void swap(BlobSpan& first, BlobSpan& second) noexcept;

	[[nodiscard]] constexpr bool Empty() const noexcept;
	[[nodiscard]] constexpr size_t Size() const noexcept;

	// Returns a pointer to the data
	[[nodiscard]] constexpr void* Data() const noexcept;

	// Returns a pointer to the data at the offset
	[[nodiscard]] void* Data(const size_t offset) const;

	// Returns a pointer to offset as type T
	template <typename T>
	[[nodiscard]] T* Pointer(const size_t offset = 0) const;

	// Returns a reference to an object at the specified offset
	template <typename T>
	[[nodiscard]] T& As(const size_t offset = 0) const;

	// Returns a sub span of this span starting at an offset to the end of the buffer
	[[nodiscard]] BlobSpan SubSpan(size_t offset = 0) const;

	// Returns a sub span of this span starting at an offset and being of a limited size
	[[nodiscard]] BlobSpan SubSpan(size_t offset, size_t bytes) const;

	// Returns a slice of this span starting at offset begin (inclusive) and ending at offset end (exclusive)
	[[nodiscard]] inline BlobSpan Slice(size_t begin, size_t end) const noexcept;

	// Returns a std::span of type T from a specified offset with count elements
	template <typename T>
	[[nodiscard]] inline std::span<T> ArraySpan(size_t offset, size_t count) const noexcept;

private:
	void* pointer_;
	size_t size_; // bytes
};

//------------------------------------------------------------------------------

//
// Holds an owned blob of memory
// Note that the copy constructor is disabled and must use copy() function to create a copy
//
class Blob
{
public:
	constexpr Blob() noexcept;

	// Constructs an empty blob of the given size
	[[nodiscard]] explicit Blob(size_t size_in_bytes);

	// Constructs a blob by taking ownership of some memory, This can be potentially unsafe
	[[nodiscard]] inline Blob(void* data, size_t size);

	// Constructs a blob by taking ownership of a buffer
	template <typename T>
	[[nodiscard]] Blob(std::unique_ptr<T[]>&& buffer, size_t elements);

	Blob(const Blob& other) = delete;
	Blob(Blob&& other) noexcept;

	~Blob();

	Blob& operator=(const Blob& other) = delete;
	Blob& operator=(Blob&& other) noexcept;

	friend void swap(Blob& first, Blob& second) noexcept;

	// Reset the contents and free all allocated memory
	void Reset();

	// Release the memory owned by this blob
	[[nodiscard]] std::pair<void*, size_t> Release();

	// Creates a copy of the buffer
	[[nodiscard]] Blob Copy() const;

	// Clears the contained memory, setting it to 0
	void Clear() noexcept;

	[[nodiscard]] constexpr bool Empty() const noexcept;
	[[nodiscard]] constexpr size_t Size() const noexcept;

	// Returns a pointer to the data
	[[nodiscard]] void* Data() noexcept;
	[[nodiscard]] const void* Data() const noexcept;

	// Returns a pointer to the data at the offset
	[[nodiscard]] void* Data(const size_t offset);
	[[nodiscard]] const void* Data(const size_t offset) const;

	// Returns a pointer to offset as type T
	template <typename T>
	[[nodiscard]] T* Pointer(const size_t offset = 0);

	// Returns a pointer to offset as type T
	template <typename T>
	[[nodiscard]] const T* Pointer(const size_t offset = 0) const;

	// Returns a reference to an object at the specified offset
	template <typename T>
	[[nodiscard]] T& As(const size_t offset = 0);

	// Returns a reference to an object at the specified offset
	template <typename T>
	[[nodiscard]] const T& As(const size_t offset = 0) const;

	// Returns a view of this memory starting at an offset to the end of the buffer
	[[nodiscard]] BlobSpan Span(const size_t offset = 0);

	// Returns a view of this memory starting at an offset and being of a limited size
	[[nodiscard]] BlobSpan Span(const size_t offset, const size_t size);

	// Returns a constant view of this memory starting at an offset to the end of the buffer
	[[nodiscard]] BlobView View(const size_t offset = 0) const;

	// Returns a constant view of this memory starting at an offset and being of a limited size
	[[nodiscard]] BlobView View(const size_t offset, const size_t size) const;

	// Returns a std::span of type const T from a specified offset with count elements
	template <typename T>
	[[nodiscard]] inline std::span<const T> ArrayView(size_t offset, size_t count) const noexcept;

	// Returns a std::span of type T from a specified offset with count elements
	template <typename T>
	[[nodiscard]] inline std::span<T> ArraySpan(size_t offset, size_t count) noexcept;

private:
	uint8_t* data_;
	size_t size_;
};

//------------------------------------------------------------------------------

constexpr BlobView::BlobView() noexcept
	: pointer_(nullptr)
	, size_(0)
{
}

constexpr BlobView::BlobView(const void* data, size_t bytes) noexcept
	: pointer_(data)
	, size_(bytes)
{
}

template <typename T, typename A>
constexpr BlobView::BlobView(const std::vector<T, A>& vector) noexcept
	: BlobView(vector.Data(), vector.Size() * sizeof(T))
{
}

template <typename T, size_t N>
constexpr BlobView::BlobView(const std::array<T, N>& array) noexcept
	: BlobView(array.Data(), N * sizeof(T))
{
}

template <typename T, size_t N>
constexpr BlobView::BlobView(const T (&array)[N]) noexcept
	: BlobView(&array[0], N * sizeof(T))
{
}

constexpr BlobView::BlobView(const BlobSpan& other) noexcept
	: BlobView(other.Data(), other.Size())
{
}

constexpr bool BlobView::Empty() const noexcept
{
	return pointer_ == nullptr || size_ == 0;
}

constexpr size_t BlobView::Size() const noexcept
{
	return size_;
}

constexpr const void* BlobView::Data() const noexcept
{
	return pointer_;
}

inline const void* BlobView::Data(const size_t offset) const
{
	ASSERT(offset <= size_);
	return static_cast<const uint8_t*>(pointer_) + offset;
}

template <typename T>
inline const T* BlobView::Pointer(const size_t offset) const
{
	ASSERT(offset <= size_);
	return static_cast<const T*>(Data(offset));
}

template <typename T>
inline const T& BlobView::As(const size_t offset) const
{
	return *Pointer<T>(offset);
}

inline BlobView BlobView::SubView(size_t offset) const
{
	ASSERT(offset <= size_);
	return BlobView(Data(offset), size_ - offset);
}

inline BlobView BlobView::SubView(size_t offset, size_t bytes) const
{
	ASSERT(offset + bytes <= size_);
	return BlobView(Data(offset), bytes);
}

inline BlobView BlobView::Slice(size_t begin, size_t end) const noexcept
{
	ASSERT(begin <= size_);
	ASSERT(end <= size_);
	return BlobView(Data(begin), end - begin);
}

template <typename T>
inline std::span<const T> BlobView::ArrayView(size_t offset, size_t count) const noexcept
{
	ASSERT(offset + count * sizeof(T) <= size_);
	return std::span<const T>(Pointer<const T>(offset), count);
}

//------------------------------------------------------------------------------

constexpr BlobSpan::BlobSpan() noexcept
	: pointer_(nullptr)
	, size_(0)
{
}

constexpr BlobSpan::BlobSpan(void* data, size_t bytes) noexcept
	: pointer_(data)
	, size_(bytes)
{
}

template <typename T, typename A>
constexpr BlobSpan::BlobSpan(std::vector<T, A>& vector) noexcept
	: BlobSpan(vector.Data(), vector.Size() * sizeof(T))
{
}

template <typename T, size_t N>
constexpr BlobSpan::BlobSpan(std::array<T, N>& array) noexcept
	: BlobSpan(array.Data(), N * sizeof(T))
{
}

template <typename T, size_t N>
constexpr BlobSpan::BlobSpan(T (&array)[N]) noexcept
	: BlobSpan(&array[0], N * sizeof(T))
{
}

constexpr bool BlobSpan::Empty() const noexcept
{
	return pointer_ == nullptr || size_ == 0;
}

constexpr size_t BlobSpan::Size() const noexcept
{
	return size_;
}

constexpr void* BlobSpan::Data() const noexcept
{
	return pointer_;
}

inline void* BlobSpan::Data(const size_t offset) const
{
	ASSERT(offset <= size_);
	return static_cast<uint8_t*>(pointer_) + offset;
}

template <typename T>
inline T* BlobSpan::Pointer(const size_t offset) const
{
	ASSERT(offset <= size_);
	return static_cast<T*>(Data(offset));
}

template <typename T>
inline T& BlobSpan::As(const size_t offset) const
{
	return *Pointer<T>(offset);
}

inline BlobSpan BlobSpan::SubSpan(size_t offset) const
{
	ASSERT(offset <= size_);
	return BlobSpan(Data(offset), size_ - offset);
}

inline BlobSpan BlobSpan::SubSpan(size_t offset, size_t bytes) const
{
	ASSERT(offset + bytes <= size_);
	return BlobSpan(Data(offset), bytes);
}

inline BlobSpan BlobSpan::Slice(size_t begin, size_t end) const noexcept
{
	ASSERT(begin <= size_);
	ASSERT(end <= size_);
	return BlobSpan(Data(begin), end - begin);
}

template<typename T>
inline std::span<T> BlobSpan::ArraySpan(size_t offset, size_t count) const noexcept
{
	ASSERT(offset + count * sizeof(T) <= size_);
	return std::span<T>(Pointer<T>(offset), count);
}

//------------------------------------------------------------------------------

constexpr Blob::Blob() noexcept
	: data_(nullptr)
	, size_(0)
{
}

inline Blob::Blob(void* data, size_t size)
	: data_(static_cast<uint8_t*>(data))
	, size_(size)
{
	ASSERT(data != nullptr);
	ASSERT(size != 0);
}

template <typename T>
inline Blob::Blob(std::unique_ptr<T[]>&& buffer, size_t elements)
	: Blob(static_cast<void*>(buffer.release()), sizeof(T) * elements)
{
}

constexpr bool Blob::Empty() const noexcept
{
	return data_ == nullptr || size_ == 0;
}

constexpr size_t Blob::Size() const noexcept
{
	return size_;
}

inline std::pair<void*, size_t> Blob::Release()
{
	auto result = std::make_pair(data_, size_);
	data_ = nullptr;
	size_ = 0;
	return result;
}

inline void* Blob::Data() noexcept
{
	return data_;
}

inline const void* Blob::Data() const noexcept
{
	return data_;
}

inline void* Blob::Data(const size_t offset)
{
	ASSERT(data_ != nullptr);
	ASSERT(offset <= size_);
	return data_ + offset;
}

inline const void* Blob::Data(const size_t offset) const
{
	ASSERT(data_ != nullptr);
	ASSERT(offset <= size_);
	return data_ + offset;
}

template <typename T>
inline T* Blob::Pointer(const size_t offset)
{
	ASSERT(offset <= size_);
	return reinterpret_cast<T*>(Data(offset));
}

template <typename T>
inline const T* Blob::Pointer(const size_t offset) const
{
	ASSERT(offset <= size_);
	return reinterpret_cast<const T*>(Data(offset));
}

template <typename T>
inline T& Blob::As(const size_t offset)
{
	return *Pointer<T>(offset);
}

template <typename T>
inline const T& Blob::As(const size_t offset) const
{
	return *Pointer<T>(offset);
}

inline BlobSpan Blob::Span(const size_t offset)
{
	return BlobSpan(Data(offset), size_ - offset);
}

inline BlobSpan Blob::Span(const size_t offset, const size_t size)
{
	ASSERT(offset + size <= size_);
	return BlobSpan(Data(offset), size);
}

inline BlobView Blob::View(const size_t offset) const
{
	return BlobView(Data(offset), size_ - offset);
}

inline BlobView Blob::View(const size_t offset, const size_t size) const
{
	ASSERT(offset + size <= size_);
	return BlobView(Data(offset), size);
}

template<typename T>
inline std::span<const T> Blob::ArrayView(size_t offset, size_t count) const noexcept
{
	ASSERT(offset + count * sizeof(T) <= size_);
	return std::span<const T>(Pointer<const T>(offset), count);
}

template<typename T>
inline std::span<T> Blob::ArraySpan(size_t offset, size_t count) noexcept
{
	ASSERT(offset + count * sizeof(T) <= size_);
	return std::span<T>(Pointer<T>(offset), count);
}

//------------------------------------------------------------------------------

inline void swap(BlobView& first, BlobView& second) noexcept
{
	using std::swap;
	swap(first.pointer_, second.pointer_);
	swap(first.size_, second.size_);
}

inline void swap(BlobSpan& first, BlobSpan& second) noexcept
{
	using std::swap;
	swap(first.pointer_, second.pointer_);
	swap(first.size_, second.size_);
}

inline void swap(Blob& first, Blob& second) noexcept
{
	using std::swap;
	swap(first.data_, second.data_);
	swap(first.size_, second.size_);
}

constexpr bool operator==(const BlobView& left, const BlobView& right) noexcept
{
	return left.Data() == right.Data() && left.Size() == right.Size();
}

constexpr bool operator!=(const BlobView& left, const BlobView& right) noexcept
{
	return left.Data() != right.Data() || left.Size() != right.Size();
}

constexpr bool operator==(const BlobView& first, const BlobSpan& second) noexcept
{
	return first.Data() == second.Data() && first.Size() == second.Size();
}

constexpr bool operator!=(const BlobView& first, const BlobSpan& second) noexcept
{
	return first.Data() != second.Data() || first.Size() != second.Size();
}

constexpr bool operator==(const BlobSpan& left, const BlobSpan& right) noexcept
{
	return left.Data() == right.Data() && left.Size() == right.Size();
}

constexpr bool operator!=(const BlobSpan& left, const BlobSpan& right) noexcept
{
	return left.Data() != right.Data() || left.Size() != right.Size();
}

constexpr bool operator==(const BlobSpan& first, const BlobView& second) noexcept
{
	return first.Data() == second.Data() && first.Size() == second.Size();
}

constexpr bool operator!=(const BlobSpan& first, const BlobView& second) noexcept
{
	return first.Data() != second.Data() || first.Size() != second.Size();
}

//------------------------------------------------------------------------------
