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

#include <Blob.hpp>
#include <gtest/gtest.h>

//------------------------------------------------------------------------------

struct BlobData : public ::testing::Test
{
	BlobData()
		: elements(5)
		, bytes(elements * sizeof(int))
		, numbers(new int[elements]{1, 2, 3, 4, 5})
	{
	}

	const size_t elements;
	const size_t bytes;
	int* numbers;
};

//------------------------------------------------------------------------------

TEST(BlobView, default_construction_should_be_empty)
{
	BlobView view;
	EXPECT_TRUE(view.Empty());
	EXPECT_EQ(0, view.Size());
	EXPECT_EQ(nullptr, view.Data());
}

TEST(BlobView, nullptr_view_should_be_empty)
{
	BlobView view{nullptr, 123};
	EXPECT_TRUE(view.Empty());
	EXPECT_EQ(123, view.Size());
	EXPECT_EQ(nullptr, view.Data());
}

TEST(BlobView, zero_size_view_should_be_empty)
{
	int data = 0;
	BlobView view{&data, 0};
	EXPECT_TRUE(view.Empty());
	EXPECT_EQ(0, view.Size());
	EXPECT_EQ(&data, view.Data());
}

struct BlobViewData : public BlobData
{
	BlobViewData()
		: view{numbers, bytes}
	{
	}

	BlobView view;
};

TEST_F(BlobViewData, sub_view_returns_same_view)
{
	BlobView sub_view = view.SubView();
	EXPECT_EQ(view, sub_view);
	EXPECT_EQ(view.Data(), sub_view.Data());
	EXPECT_EQ(view.Size(), sub_view.Size());
}

TEST_F(BlobViewData, sub_view_at_end_returns_empty_view)
{
	BlobView sub_view = view.SubView(bytes);
	EXPECT_TRUE(sub_view.Empty());
	EXPECT_EQ(view.Data(bytes), sub_view.Data());
	EXPECT_EQ(0, sub_view.Size());
}

TEST_F(BlobViewData, sub_view_returns_smaller_view)
{
	BlobView sub_view = view.SubView(sizeof(int));
	EXPECT_EQ(4 * sizeof(int), sub_view.Size());
	EXPECT_EQ(view.Data(sizeof(int)), sub_view.Data());
}

TEST_F(BlobViewData, sub_view_returns_slices)
{
	constexpr size_t slice_size = sizeof(int) * 2;

	BlobView front_view = view.SubView(0, slice_size);
	EXPECT_EQ(slice_size, front_view.Size());
	EXPECT_EQ(view.Data(), front_view.Data());

	BlobView middle_view = view.SubView(slice_size, slice_size);
	EXPECT_EQ(slice_size, middle_view.Size());
	EXPECT_EQ(view.Data(slice_size), middle_view.Data());

	BlobView end_view = view.SubView(view.Size() - slice_size, slice_size);
	EXPECT_EQ(slice_size, end_view.Size());
	EXPECT_EQ(view.Data(view.Size() - slice_size), end_view.Data());
}

//------------------------------------------------------------------------------

TEST(BlobSpan, default_construction_should_be_empty)
{
	BlobSpan span;
	EXPECT_TRUE(span.Empty());
	EXPECT_EQ(0, span.Size());
	EXPECT_EQ(nullptr, span.Data());
}

TEST(BlobSpan, nullptr_span_should_be_empty)
{
	BlobSpan span{nullptr, 123};
	EXPECT_TRUE(span.Empty());
	EXPECT_EQ(123, span.Size());
	EXPECT_EQ(nullptr, span.Data());
}

TEST(BlobSpan, zero_size_span_should_be_empty)
{
	int data = 0;
	BlobSpan span{&data, 0};
	EXPECT_TRUE(span.Empty());
	EXPECT_EQ(0, span.Size());
	EXPECT_EQ(&data, span.Data());
}

struct BlobSpanData : public BlobData
{
	BlobSpanData()
		: span{numbers, bytes}
	{
	}

	BlobSpan span;
};

TEST_F(BlobSpanData, sub_view_returns_same_view)
{
	BlobSpan sub_span = span.SubSpan();
	EXPECT_EQ(span, sub_span);
	EXPECT_EQ(span.Data(), sub_span.Data());
	EXPECT_EQ(span.Size(), sub_span.Size());
}

TEST_F(BlobSpanData, sub_view_at_end_returns_empty_view)
{
	BlobSpan sub_span = span.SubSpan(bytes);
	EXPECT_TRUE(sub_span.Empty());
	EXPECT_EQ(span.Data(bytes), sub_span.Data());
	EXPECT_EQ(0, sub_span.Size());
}

TEST_F(BlobSpanData, sub_view_returns_smaller_view)
{
	BlobSpan sub_span = span.SubSpan(sizeof(int));
	EXPECT_EQ(4 * sizeof(int), sub_span.Size());
	EXPECT_EQ(span.Data(sizeof(int)), sub_span.Data());
}

TEST_F(BlobSpanData, sub_view_returns_slices)
{
	constexpr size_t slice_size = sizeof(int) * 2;

	BlobSpan front_span = span.SubSpan(0, slice_size);
	EXPECT_EQ(slice_size, front_span.Size());
	EXPECT_EQ(span.Data(), front_span.Data());

	BlobSpan middle_span = span.SubSpan(slice_size, slice_size);
	EXPECT_EQ(slice_size, middle_span.Size());
	EXPECT_EQ(span.Data(slice_size), middle_span.Data());

	BlobSpan end_span = span.SubSpan(span.Size() - slice_size, slice_size);
	EXPECT_EQ(slice_size, end_span.Size());
	EXPECT_EQ(span.Data(span.Size() - slice_size), end_span.Data());
}

//------------------------------------------------------------------------------

TEST(Blob, default_construction_should_be_empty)
{
	Blob blob;
	EXPECT_TRUE(blob.Empty());
	EXPECT_EQ(0, blob.Size());
	EXPECT_EQ(nullptr, blob.Data());
}

TEST(Blob, construct_with_specific_byte_size)
{
	const size_t bytes = 12;

	Blob blob(bytes);

	EXPECT_FALSE(blob.Empty());
	EXPECT_EQ(bytes, blob.Size());
	EXPECT_NE(nullptr, blob.Data());
}

TEST(Blob, construct_with_moved_unique_uint8_t_array)
{
	const size_t elements = 10;
	auto buffer = std::make_unique<uint8_t[]>(elements);
	const void* const data = buffer.get();

	Blob blob(std::move(buffer), elements);

	EXPECT_EQ(elements, blob.Size());
	EXPECT_EQ(data, blob.Data());
	EXPECT_EQ(nullptr, buffer.get());
}

TEST(Blob, construct_with_moved_unique_uint64_t_array)
{
	const size_t elements = 8;
	auto buffer = std::make_unique<uint64_t[]>(elements);
	const void* const data = buffer.get();

	Blob blob(std::move(buffer), elements);

	EXPECT_EQ(elements * sizeof(uint64_t), blob.Size());
	EXPECT_EQ(data, blob.Data());
	EXPECT_EQ(nullptr, buffer.get());
}

TEST(Blob, construct_using_pointer_and_size)
{
	const size_t elements = 5;
	const size_t bytes = elements * sizeof(int);
	auto buffer = new int[elements];

	Blob blob(buffer, bytes);

	EXPECT_EQ(bytes, blob.Size());
	EXPECT_EQ(buffer, blob.Data());
}

TEST(Blob, release_makes_blob_empty)
{
	Blob blob(12);
	EXPECT_FALSE(blob.Empty());

	const void* blob_pointer = blob.Data();
	const auto blob_size = blob.Size();

	auto pair = blob.Release();

	EXPECT_TRUE(blob.Empty());
	EXPECT_EQ(nullptr, blob.Data());
	EXPECT_EQ(0, blob.Size());

	EXPECT_EQ(blob_pointer, pair.first);
	EXPECT_EQ(blob_size, pair.second);

	std::free(pair.first);
}

TEST(Blob, reset_makes_blob_empty)
{
	Blob blob(12);
	blob.Reset();

	EXPECT_TRUE(blob.Empty());
}

TEST(Blob, swap)
{
	Blob blob_a(4);
	void* buffer_a = blob_a.Data();
	Blob blob_b(7);
	void* buffer_b = blob_b.Data();

	swap(blob_a, blob_b);

	EXPECT_EQ(buffer_a, blob_b.Data());
	EXPECT_EQ(4, blob_b.Size());
	EXPECT_EQ(buffer_b, blob_a.Data());
	EXPECT_EQ(7, blob_a.Size());
}

struct BlobDataFixture : public BlobData
{
	BlobDataFixture()
		: blob(numbers, bytes)
	{
	}

	Blob blob;
};

TEST_F(BlobDataFixture, view_returns_view_of_whole_blob)
{
	BlobView view = blob.View();
	EXPECT_EQ(view.Data(), view.Data());
	EXPECT_EQ(view.Size(), view.Size());
}

TEST_F(BlobDataFixture, view_at_end_returns_empty_view)
{
	BlobView view = blob.View(bytes);
	EXPECT_TRUE(view.Empty());
	EXPECT_EQ(blob.Data(bytes), view.Data());
	EXPECT_EQ(0, view.Size());
}

TEST_F(BlobDataFixture, view_returns_smaller_view)
{
	BlobView view = blob.View(sizeof(int));
	EXPECT_EQ(4 * sizeof(int), view.Size());
	EXPECT_EQ(blob.Data(sizeof(int)), view.Data());
}

TEST_F(BlobDataFixture, view_returns_slices)
{
	constexpr size_t slice_size = sizeof(int) * 2;

	BlobView front_view = blob.View(0, slice_size);
	EXPECT_EQ(slice_size, front_view.Size());
	EXPECT_EQ(blob.Data(), front_view.Data());

	BlobView middle_view = blob.View(slice_size, slice_size);
	EXPECT_EQ(slice_size, middle_view.Size());
	EXPECT_EQ(blob.Data(slice_size), middle_view.Data());

	BlobView end_view = blob.View(blob.Size() - slice_size, slice_size);
	EXPECT_EQ(slice_size, end_view.Size());
	EXPECT_EQ(blob.Data(blob.Size() - slice_size), end_view.Data());
}

TEST_F(BlobDataFixture, span_returns_span_of_whole_blob)
{
	BlobSpan span = blob.Span();
	EXPECT_EQ(blob.Data(), span.Data());
	EXPECT_EQ(blob.Size(), span.Size());
}

TEST_F(BlobDataFixture, span_at_end_returns_empty_span)
{
	BlobSpan span = blob.Span(bytes);
	EXPECT_TRUE(span.Empty());
	EXPECT_EQ(blob.Data(bytes), span.Data());
	EXPECT_EQ(0, span.Size());
}

TEST_F(BlobDataFixture, span_returns_smaller_span)
{
	BlobSpan span = blob.Span(sizeof(int));
	EXPECT_EQ(4 * sizeof(int), span.Size());
	EXPECT_EQ(blob.Data(sizeof(int)), span.Data());
}

TEST_F(BlobDataFixture, span_returns_slices)
{
	constexpr size_t slice_size = sizeof(int) * 2;

	BlobSpan front_span = blob.Span(0, slice_size);
	EXPECT_EQ(slice_size, front_span.Size());
	EXPECT_EQ(blob.Data(), front_span.Data());

	BlobSpan middle_span = blob.Span(slice_size, slice_size);
	EXPECT_EQ(slice_size, middle_span.Size());
	EXPECT_EQ(blob.Data(slice_size), middle_span.Data());

	BlobSpan end_span = blob.Span(blob.Size() - slice_size, slice_size);
	EXPECT_EQ(slice_size, end_span.Size());
	EXPECT_EQ(blob.Data(blob.Size() - slice_size), end_span.Data());
}


//------------------------------------------------------------------------------

template <typename BlobType>
struct BlobAccessing : public BlobData
{
	BlobAccessing()
		: blob{numbers, bytes}
	{
	}

	BlobType blob;
};

using BlobTypes = ::testing::Types<Blob, const Blob, const BlobView, const BlobSpan>;
TYPED_TEST_SUITE(BlobAccessing, BlobTypes);

TYPED_TEST(BlobAccessing, treat_as_native_type)
{
	EXPECT_EQ(1, this->blob.As<int>());
	EXPECT_EQ(2, this->blob.As<int>(sizeof(int)));
	EXPECT_EQ(5, this->blob.As<int>(4 * sizeof(int)));
}

TYPED_TEST(BlobAccessing, treat_as_smaller_type)
{
	EXPECT_EQ(static_cast<short>(1), this->blob.As<short>());
	EXPECT_EQ(static_cast<short>(0), this->blob.As<short>(sizeof(short)));
}

TYPED_TEST(BlobAccessing, treat_as_larger_type)
{
	EXPECT_EQ(0x0000'0002'0000'0001ull, this->blob.As<uint64_t>());
}

TYPED_TEST(BlobAccessing, get_typed_pointer)
{
	EXPECT_EQ(this->blob.Data(), this->blob.Pointer<int>());
	EXPECT_EQ(this->blob.Data(4), this->blob.Pointer<int>(4));
}

//------------------------------------------------------------------------------
