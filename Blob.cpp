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

#include "Blob.hpp"

Blob::Blob(size_t size_in_bytes)
	: data_(nullptr)
	, size_(size_in_bytes)
{
	ASSERT(size_in_bytes != 0);
	data_ = static_cast<uint8_t*>(std::malloc(size_));
}

Blob::Blob(Blob&& other) noexcept
	: data_(std::exchange(other.data_, nullptr))
	, size_(std::exchange(other.size_, 0))
{
}

Blob::~Blob()
{
	Reset();
}

Blob& Blob::operator=(Blob&& other) noexcept
{
	Reset();
	data_ = std::exchange(other.data_, nullptr);
	size_ = std::exchange(other.size_, 0);
	return *this;
}

void Blob::Reset()
{
	std::free(std::exchange(data_, nullptr));
	size_ = 0;
}

Blob Blob::Copy() const
{
	if (data_ != nullptr && size_ > 0)
	{
		Blob result(size_);
		std::memcpy(result.data_, data_, size_);
		return result;
	}
	return Blob();
}

void Blob::Clear() noexcept
{
	if (data_ != nullptr && size_ > 0)
	{
		std::memset(data_, 0, size_);
	}
}
